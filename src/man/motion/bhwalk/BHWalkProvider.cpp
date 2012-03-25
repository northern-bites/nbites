
#include "BHWalkProvider.h"

#include <cassert>
#include <string>

#include "bhuman.h"

namespace man {
namespace motion {

using namespace boost;
using namespace std;

#define DEBUG_BHWALK
#ifdef DEBUG_BHWALK
#define bhwalk_out std::cout
#else
#define bhwalk_out (*NullStream::NullInstance())
#endif

/**
 * Since the NBites use a different order for the joints, we use this
 * array to convert between Kinematics::JointName and BHuman's JointData::Joint
 *
 * So the JointData::Joint values in this array are arranged in the order
 * the NBites need them
 */
static const JointData::Joint nb_joint_order[] = {
        JointData::HeadYaw,
        JointData::HeadPitch,
        JointData::LShoulderPitch,
        JointData::LShoulderRoll,
        JointData::LElbowYaw,
        JointData::LElbowRoll,
        JointData::LHipYawPitch,
        JointData::LHipRoll,
        JointData::LHipPitch,
        JointData::LKneePitch,
        JointData::LAnklePitch,
        JointData::LAnkleRoll,
        JointData::RHipYawPitch,
        JointData::RHipRoll,
        JointData::RHipPitch,
        JointData::RKneePitch,
        JointData::RAnklePitch,
        JointData::RAnkleRoll,
        JointData::RShoulderPitch,
        JointData::RShoulderRoll,
        JointData::RElbowYaw,
        JointData::RElbowRoll
};

BHWalkProvider::BHWalkProvider(shared_ptr<Sensors> s, boost::shared_ptr<NaoPose> p) :
        MotionProvider(WALK_PROVIDER),
        sensors(s) {
}

void BHWalkProvider::requestStopFirstInstance() {
    this->stand();
}

/**
 * This method converts the NBites sensor and joint input to
 * input suitable for the (BH) B-Human walk, and similarly the output
 * and then interprets the walk engine output
 *
 * Main differences:
 * * The BH joint data is in a different order;
 */
void BHWalkProvider::calculateNextJointsAndStiffnesses() {

    assert(JointData::numOfJoints == Kinematics::NUM_JOINTS);

    //We only copy joint position, and not temperatures or currents
    //Note: temperatures are unused, and currents are used by the GroundContactDetector
    //which is not used right now
    float* bh_input_sensors = walkingEngine.naoProvider.sensors;

    vector<float> nb_joint_data = sensors->getMotionBodyAngles();

    for (int i = 0; i < JointData::numOfJoints; i++) {
        bh_input_sensors[3*nb_joint_order[i]] = nb_joint_data[i];
    }

    Inertial nb_raw_inertial = sensors->getUnfilteredInertial();

    bh_input_sensors[gyroXSensor] = nb_raw_inertial.gyrX;
    bh_input_sensors[gyroYSensor] = nb_raw_inertial.gyrY;

    bh_input_sensors[accXSensor] = nb_raw_inertial.accX;
    bh_input_sensors[accYSensor] = nb_raw_inertial.accY;
    bh_input_sensors[accZSensor] = nb_raw_inertial.accZ;

    FSR nb_fsr_l = sensors->getLeftFootFSR();

    bh_input_sensors[lFSRFrontLeftSensor] = nb_fsr_l.frontLeft;
    bh_input_sensors[lFSRFrontRightSensor] = nb_fsr_l.frontRight;
    bh_input_sensors[lFSRRearLeftSensor] = nb_fsr_l.rearLeft;
    bh_input_sensors[lFSRRearRightSensor] = nb_fsr_l.rearRight;

    FSR nb_fsr_r = sensors->getRightFootFSR();

    bh_input_sensors[rFSRFrontLeftSensor] = nb_fsr_r.frontLeft;
    bh_input_sensors[rFSRFrontRightSensor] = nb_fsr_r.frontRight;
    bh_input_sensors[rFSRRearLeftSensor] = nb_fsr_r.rearLeft;
    bh_input_sensors[rFSRRearRightSensor] = nb_fsr_r.rearRight;

    walkingEngine.update();

    //ignore the first chain since it's the head one
    for (unsigned i = 1; i < Kinematics::NUM_CHAINS; i++) {
        vector<float> chain_angles;
        vector<float> chain_hardness;
        for (unsigned j = Kinematics::chain_first_joint[i];
                     j <= Kinematics::chain_last_joint[i]; j++) {
            //position angle
            chain_angles.push_back(walkingEngine.naoProvider.actuators[nb_joint_order[j]]);
            //hardness
            int hardness_index = nb_joint_order[j] + lbhNumOfPositionActuatorIds;
            if (walkingEngine.naoProvider.actuators[hardness_index] == 0) {
                chain_hardness.push_back(MotionConstants::NO_STIFFNESS);
            } else {
                chain_hardness.push_back(walkingEngine.naoProvider.actuators[hardness_index]);
            }

        }
        this->setNextChainJoints((Kinematics::ChainID) i, chain_angles);
        this->setNextChainStiffnesses((Kinematics::ChainID) i, chain_hardness);
    }

    if (walkingEngine.walkingEngineOutput.positionInWalkCycle == 1.0f) {
        stand();
    }
}

void BHWalkProvider::stand() {
    walkingEngine.theMotionRequest.motion = MotionRequest::stand;
    walkingEngine.theMotionRequest.walkRequest = WalkRequest();
    inactive();
}

MotionModel BHWalkProvider::getOdometryUpdate() {
    return MotionModel();
}

void BHWalkProvider::hardReset() {
    stand();
}

void BHWalkProvider::setCommand(const WalkCommand::ptr command) {
    WalkRequest* walkRequest = &(walkingEngine.theMotionRequest.walkRequest);
    walkRequest->mode = WalkRequest::speedMode;

    walkRequest->speed.rotation = command->theta_rads;
    walkRequest->speed.translation.x = command->x_mms;
    walkRequest->speed.translation.y = command->y_mms;

    walkingEngine.theMotionRequest.motion = MotionRequest::walk;

    bhwalk_out << *(command.get());

    active();
}

void BHWalkProvider::setCommand(const StepCommand::ptr command) {

}

void BHWalkProvider::setCommand(const DestinationCommand::ptr command) {
    WalkRequest* walkRequest = &(walkingEngine.theMotionRequest.walkRequest);
    walkRequest->mode = WalkRequest::targetMode;

    walkRequest->speed.rotation = command->gain;
    walkRequest->speed.translation.x = command->gain;
    walkRequest->speed.translation.y = command->gain;

    walkRequest->target.rotation = command->theta_rads;
    walkRequest->target.translation.x = command->x_mm;
    walkRequest->target.translation.y = command->y_mm;

    walkingEngine.theMotionRequest.motion = MotionRequest::walk;

    bhwalk_out << *(command.get());

    active();
}

const SupportFoot BHWalkProvider::getSupportFoot() const {
    if (walkingEngine.getSupportLeg() == 0) {//TODO: WalkingEngine::left) {
        return LEFT_SUPPORT;
    } else {
        return RIGHT_SUPPORT;
    }
}

}
}
