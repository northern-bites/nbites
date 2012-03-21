
#include "BHWalkProvider.h"

#include <cassert>
#include <string>

#include "Tools/Streams/InStreams.h"

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
        sensors(s),
        walkingEngine(&robotModel, &torsoMatrix, &robotDimensions, &massCalibration) {

    InConfigMap massesStream(BH_CONFIG_DIR + "/masses.cfg");
    if (massesStream.exists()) {
        massesStream >> massCalibration;
        cout << massCalibration.masses[2].mass << endl;
    } else {
        cout << "Could not find masses.cfg!" << endl;
    }

    InConfigMap robotDimStream(BH_CONFIG_DIR + "/robotDimensions.cfg");
    if (robotDimStream.exists()) {
        robotDimStream >> robotDimensions;
    } else {
        cout << "Could not find robotDims.cfg!" << endl;
    }

    InConfigMap jointCalibrateStream(BH_CONFIG_DIR + "/jointCalibration.cfg");
    if (jointCalibrateStream.exists()) {
        jointCalibrateStream >> jointCalibration;
    } else {
        cout << "Could not find jointCalibration.cfg!" << endl;
    }

    // load parameters from config file
    InConfigMap walkingCfgStream(BH_CONFIG_DIR + "/walking.cfg", InConfigMap::VERBOSE);
    if(walkingCfgStream.exists()) {
        walkingCfgStream >> walkingEngine.p;
    } else {
          cout << "Could not find walking.cfg!" << endl;
    //    InConfigMap stream("walking.cfg");
    //    if(stream.exists())
    //      stream >> p;
    //    else
          walkingEngine.p.computeContants();
      }
}

/**
 * This method converts the NBites sensor and joint input to
 * input suitable for the (BH) B-Human walk, and similarly the output
 *
 * Main differences:
 * * The BH joint data is in a different order;
 * * Some BH joint angles have different signs (found in the jointCalibration)
 */
void BHWalkProvider::calculateNextJointsAndStiffnesses() {

    assert(JointData::numOfJoints == Kinematics::NUM_JOINTS);

    vector<float> nbjointData = sensors->getMotionBodyAngles();

    JointData bhjointData;

    for (int i = 0; i < JointData::numOfJoints; i++) {
        bhjointData.angles[nb_joint_order[i]] =
              nbjointData[i] * jointCalibration.joints[nb_joint_order[i]].sign;
    }

    robotModel.setJointData(bhjointData, robotDimensions, massCalibration);

    Inertial inertial = sensors->getInertial();

    torsoMatrixProvider.update(torsoMatrix, robotModel, robotDimensions,
                               inertial.angleX, inertial.angleY);

    walkingEngine.update(walkOutput);

    //ignore the first chain since it's the head one
    for (unsigned i = 1; i < Kinematics::NUM_CHAINS; i++) {
        vector<float> chain_angles;
        vector<float> chain_hardness;
        for (unsigned j = Kinematics::chain_first_joint[i];
                     j <= Kinematics::chain_last_joint[i]; j++) {

            chain_angles.push_back(walkOutput.angles[nb_joint_order[j]]
                                   * jointCalibration.joints[nb_joint_order[j]].sign);

            if (walkOutput.angles[nb_joint_order[j]] == JointData::off) {
                chain_hardness.push_back(MotionConstants::NO_STIFFNESS);
            } else {
                chain_hardness.push_back(0.75f); //TODO: get them from jointHardness.cfg
            }
        }
        this->setNextChainJoints((Kinematics::ChainID) i, chain_angles);
        this->setNextChainStiffnesses((Kinematics::ChainID) i, chain_hardness);
    }

    if (walkOutput.positionInWalkCycle == 1.0f) {
        inactive();
    } else {
        active();
    }
}

MotionModel BHWalkProvider::getOdometryUpdate() {
    return MotionModel();
}

void BHWalkProvider::hardReset() {
    inactive();
    walkingEngine.motionRequest.motion = MotionRequest::specialAction;
}

void BHWalkProvider::setCommand(const WalkCommand::ptr command) {
    WalkRequest* walkRequest = &(walkingEngine.motionRequest.walkRequest);
    walkRequest->mode = WalkRequest::speedMode;

    walkRequest->speed.rotation = command->theta_rads;
    walkRequest->speed.translation.x = command->x_mms;
    walkRequest->speed.translation.y = command->y_mms;

    walkingEngine.motionRequest.motion = MotionRequest::walk;

    bhwalk_out << *(command.get());

    active();
}

void BHWalkProvider::setCommand(const StepCommand::ptr command) {

}

void BHWalkProvider::setCommand(const DestinationCommand::ptr command) {
    WalkRequest* walkRequest = &(walkingEngine.motionRequest.walkRequest);
    walkRequest->mode = WalkRequest::targetMode;

    walkRequest->speed.rotation = command->gain;
    walkRequest->speed.translation.x = command->gain;
    walkRequest->speed.translation.y = command->gain;

    walkRequest->target.rotation = command->theta_rads;
    walkRequest->target.translation.x = command->x_mm;
    walkRequest->target.translation.y = command->y_mm;

    walkingEngine.motionRequest.motion = MotionRequest::walk;

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
