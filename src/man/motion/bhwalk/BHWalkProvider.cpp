
#include "BHWalkProvider.h"

#include <cassert>
#include <string>

#include "NullStream.h"
#include "bhuman.h"

namespace man {
namespace motion {

using namespace boost;
using namespace std;

//#define DEBUG_BHWALK
#ifdef DEBUG_BHWALK
#define bhwalk_out std::cout
#else
#define bhwalk_out (*NullStream::NullInstance())
#endif

const float BHWalkProvider::INITIAL_BODY_POSE_ANGLES[] =
{
        1.57f, 0.18f, -1.56f, -0.18f,
        0.0f, 0.0f, -0.39f, 0.76f, -0.37f, 0.0f,
        0.0f, 0.0f, -0.39f, 0.76f, -0.37f, 0.0f,
        1.57f, -0.18f, -1.43f, 0.23f
};

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
        MotionProvider(WALK_PROVIDER), requestedToStop(false),
        sensors(s) {
}

void BHWalkProvider::requestStopFirstInstance() {
    this->stand();
    requestedToStop = true;
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
    JointData& bh_joint_data = walkingEngine.theJointData;
    vector<float> nb_joint_data = sensors->getBodyAngles();

    for (int i = 0; i < JointData::numOfJoints; i++) {
        bh_joint_data.angles[nb_joint_order[i]] = nb_joint_data[i];
    }

    SensorData& bh_sensors = walkingEngine.theSensorData;

    Inertial nb_raw_inertial = sensors->getUnfilteredInertial();

    bh_sensors.data[SensorData::gyroX] = nb_raw_inertial.gyrX;
    bh_sensors.data[SensorData::gyroY] = nb_raw_inertial.gyrY;

    bh_sensors.data[SensorData::accX] = nb_raw_inertial.accX;
    bh_sensors.data[SensorData::accY] = nb_raw_inertial.accY;
    bh_sensors.data[SensorData::accZ] = nb_raw_inertial.accZ;

    bh_sensors.data[SensorData::angleX] = nb_raw_inertial.angleX;
    bh_sensors.data[SensorData::angleY] = nb_raw_inertial.angleY;

    FSR nb_fsr_l = sensors->getLeftFootFSR();

    bh_sensors.data[SensorData::fsrLFL] = nb_fsr_l.frontLeft;
    bh_sensors.data[SensorData::fsrLFR] = nb_fsr_l.frontRight;
    bh_sensors.data[SensorData::fsrLBL] = nb_fsr_l.rearLeft;
    bh_sensors.data[SensorData::fsrLBR] = nb_fsr_l.rearRight;

    FSR nb_fsr_r = sensors->getRightFootFSR();

    bh_sensors.data[SensorData::fsrRFL] = nb_fsr_r.frontLeft;
    bh_sensors.data[SensorData::fsrLFR] = nb_fsr_r.frontRight;
    bh_sensors.data[SensorData::fsrLBL] = nb_fsr_r.rearLeft;
    bh_sensors.data[SensorData::fsrLBR] = nb_fsr_r.rearRight;

    walkingEngine.update();

    //ignore the first chain since it's the head one
    for (unsigned i = 1; i < Kinematics::NUM_CHAINS; i++) {
        vector<float> chain_angles;
        vector<float> chain_hardness;
        for (unsigned j = Kinematics::chain_first_joint[i];
                     j <= Kinematics::chain_last_joint[i]; j++) {
            //position angle
            chain_angles.push_back(walkingEngine.joint_angles[nb_joint_order[j]]);
            //hardness
            if (walkingEngine.joint_hardnesses[nb_joint_order[j]] == 0) {
                chain_hardness.push_back(MotionConstants::NO_STIFFNESS);
            } else {
                chain_hardness.push_back(walkingEngine.joint_hardnesses[nb_joint_order[j]]);
            }

        }
        this->setNextChainJoints((Kinematics::ChainID) i, chain_angles);
        this->setNextChainStiffnesses((Kinematics::ChainID) i, chain_hardness);
    }

    if (requestedToStop && walkingEngine.walkingEngineOutput.isLeavingPossible) {
        inactive();
        requestedToStop = false;
    }
}

void BHWalkProvider::stand() {
    bhwalk_out << "BHWalk stand requested" << endl;
    walkingEngine.theMotionRequest.motion = MotionRequest::stand;
    walkingEngine.theMotionRequest.walkRequest = WalkRequest();
    active();
}

MotionModel BHWalkProvider::getOdometryUpdate() {
    return MotionModel(walkingEngine.theOdometryData.translation.x * MM_TO_CM,
                       walkingEngine.theOdometryData.translation.y * MM_TO_CM,
                       walkingEngine.theOdometryData.rotation);
}

void BHWalkProvider::hardReset() {
    stand();
}

void BHWalkProvider::setCommand(const WalkCommand::ptr command) {
    WalkRequest* walkRequest = &(walkingEngine.theMotionRequest.walkRequest);
    walkRequest->mode = WalkRequest::speedMode;

    if (command->theta_rads == 0 && command->x_mms == 0 && command->y_mms == 0) {
        this->stand();
        return;
    }

    walkingEngine.theMotionRequest.motion = MotionRequest::walk;

    walkRequest->speed.rotation = command->theta_rads;
    walkRequest->speed.translation.x = command->x_mms;
    walkRequest->speed.translation.y = command->y_mms;

    bhwalk_out << "BHWalk speed walk requested with command ";
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

    bhwalk_out << "BHWalk destination walk requested with command ";
    bhwalk_out << *(command.get()) << endl;

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
