#include "BHWalkProvider.h"
#include "Profiler.h"
#include "Common.h"
#include "NaoPaths.h"

#include <cassert>
#include <string>
#include <iostream>

// BH
#include "bhuman.h"

namespace man
{
namespace motion
{

using namespace boost;

// TODO make this consistent with new walk
const float BHWalkProvider::INITIAL_BODY_POSE_ANGLES[] =
{
        1.57f, 0.18f, -1.56f, -0.18f,
        0.0f, 0.0f, -0.39f, 0.76f, -0.37f, 0.0f,
        0.0f, 0.0f, -0.39f, 0.76f, -0.37f, 0.0f,
        1.57f, -0.18f, 1.43f, 0.23f
};

/**
 * Since the NBites use a different order for the joints, we use this
 * array to convert between Kinematics::JointName and BHuman's JointDataBH::Joint
 *
 * So the JointDataBH::Joint values in this array are arranged in the order
 * the NBites need them
 */
static const JointDataBH::Joint nb_joint_order[] = {
        JointDataBH::HeadYaw,
        JointDataBH::HeadPitch,
        JointDataBH::LShoulderPitch,
        JointDataBH::LShoulderRoll,
        JointDataBH::LElbowYaw,
        JointDataBH::LElbowRoll,
        JointDataBH::LHipYawPitch,
        JointDataBH::LHipRoll,
        JointDataBH::LHipPitch,
        JointDataBH::LKneePitch,
        JointDataBH::LAnklePitch,
        JointDataBH::LAnkleRoll,
        JointDataBH::RHipYawPitch,
        JointDataBH::RHipRoll,
        JointDataBH::RHipPitch,
        JointDataBH::RKneePitch,
        JointDataBH::RAnklePitch,
        JointDataBH::RAnkleRoll,
        JointDataBH::RShoulderPitch,
        JointDataBH::RShoulderRoll,
        JointDataBH::RElbowYaw,
        JointDataBH::RElbowRoll
};

BHWalkProvider::BHWalkProvider()
    : MotionProvider(WALK_PROVIDER), requestedToStop(false)
{
	// Setup Walk Engine Configuation Parameters
	ModuleBase::config_path = common::paths::NAO_CONFIG_DIR;

    // Setup the blackboard (used by bhuman to pass data around modules)
	blackboard = new Blackboard;

    // Setup the walk engine
	walkingEngine = new WalkingEngine;
    hardReset();
}

BHWalkProvider::~BHWalkProvider()
{
    delete blackboard;
    delete walkingEngine;
}

void BHWalkProvider::requestStopFirstInstance()
{
    requestedToStop = true;
}

bool hasLargerMagnitude(float x, float y) {
    if (y > 0.0f)
        return x > y;
    if (y < 0.0f)
        return x < y;
    return true; // Considers values of 0.0f as always smaller in magnitude than anything
}

// Return true if p1 has "passed" p2 (has components values that either have a magnitude
// larger than the corresponding magnitude p2 within the same sign)
bool hasPassed(const Pose2DBH& p1, const Pose2DBH& p2) {

    return (hasLargerMagnitude(p1.rotation, p2.rotation) &&
            hasLargerMagnitude(p1.translation.x, p2.translation.x) &&
            hasLargerMagnitude(p1.translation.y, p2.translation.y));
}

/**
 * This method converts the NBites sensor and joint input to
 * input suitable for the (BH) B-Human walk, and similarly the output
 * and then interprets the walk engine output
 *
 * Main differences:
 * The BH joint data is in a different order and is transformed by sign and
 * offset to make calculation easier
 */
void BHWalkProvider::calculateNextJointsAndStiffnesses(
    std::vector<float>&            sensorAngles,
    std::vector<float>&            sensorCurrents,
    const messages::InertialState& sensorInertials,
    const messages::FSR&           sensorFSRs
    ) 
{

    PROF_ENTER(P_WALK);

    // TODO reimplement or not necessary any more?
    // If our calibration became bad (as decided by the walkingEngine,
    // reset. We will wait until we're recalibrated to walk.
    // if (walkingEngine->shouldReset && calibrated())
    // if (calibrated())
    // {
    //     std::cout << "We are stuck! Recalibrating." << std::endl;
    //     hardReset();
    // }

    assert(JointDataBH::numOfJoints == Kinematics::NUM_JOINTS);

    if (standby) {
        MotionRequestBH motionRequest;
        motionRequest.motion = MotionRequestBH::specialAction;

        // TODO why are we sitting and what does standby mean?
        // Special action requests keep bhwalk from recalibrating
        motionRequest.specialActionRequest.specialAction = SpecialActionRequest::sitDown;
        walkingEngine->theMotionRequestBH = motionRequest;

        // TODO anything that's not in the walk is marked as unstable
        walkingEngine->theMotionInfoBH = MotionInfoBH();
        walkingEngine->theMotionInfoBH.isMotionStable = false;

    } else {
    // TODO VERY UGLY -- refactor this please
    if (requestedToStop || !isActive()) {
        MotionRequestBH motionRequest;

        // TODO why are we sitting?
        motionRequest.motion = MotionRequestBH::specialAction;
        motionRequest.specialActionRequest.specialAction = SpecialActionRequest::sitDown;

        walkingEngine->theMotionRequestBH = motionRequest;

        currentCommand = MotionCommand::ptr();

    } else {
        // If we're not calibrated, wait until we are calibrated to walk
        if (!calibrated())
        {
            MotionRequestBH motionRequest;
            motionRequest.motion = MotionRequestBH::stand;

            walkingEngine->theMotionRequestBH = motionRequest;
        } else if (currentCommand.get() && currentCommand->getType() == MotionConstants::STEP) {
            StepCommand::ptr command = boost::shared_static_cast<StepCommand>(currentCommand);

            Pose2DBH deltaOdometry = walkingEngine->theOdometryDataBH - startOdometry;
            Pose2DBH absoluteTarget(command->theta_rads, command->x_mms, command->y_mms);

            Pose2DBH relativeTarget = absoluteTarget - (deltaOdometry + walkingEngine->upcomingOdometryOffset);

            if (!hasPassed(deltaOdometry + walkingEngine->upcomingOdometryOffset, absoluteTarget)) {
                MotionRequestBH motionRequest;
                motionRequest.motion = MotionRequestBH::walk;

                motionRequest.walkRequest.mode = WalkRequest::targetMode;

                motionRequest.walkRequest.speed.rotation = command->gain;
                motionRequest.walkRequest.speed.translation.x = command->gain;
                motionRequest.walkRequest.speed.translation.y = command->gain;

                motionRequest.walkRequest.target = relativeTarget;

                walkingEngine->theMotionRequestBH = motionRequest;

            } else {
                currentCommand = MotionCommand::ptr();
            }

        } else {
        if (currentCommand.get() && currentCommand->getType() == MotionConstants::WALK) {
            WalkCommand::ptr command = boost::shared_static_cast<WalkCommand>(currentCommand);

            MotionRequestBH motionRequest;
            motionRequest.motion = MotionRequestBH::walk;

            motionRequest.walkRequest.mode = WalkRequest::percentageSpeedMode;

            motionRequest.walkRequest.speed.rotation = command->theta_percent;
            motionRequest.walkRequest.speed.translation.x = command->x_percent;
            motionRequest.walkRequest.speed.translation.y = command->y_percent;

            walkingEngine->theMotionRequestBH = motionRequest;
        } else {
        if (currentCommand.get() && currentCommand->getType() == MotionConstants::DESTINATION) {
            DestinationCommand::ptr command = boost::shared_static_cast<DestinationCommand>(currentCommand);

            MotionRequestBH motionRequest;
            motionRequest.motion = MotionRequestBH::walk;

            motionRequest.walkRequest.mode = WalkRequest::targetMode;

            motionRequest.walkRequest.speed.rotation = command->gain;
            motionRequest.walkRequest.speed.translation.x = command->gain;
            motionRequest.walkRequest.speed.translation.y = command->gain;

            motionRequest.walkRequest.target.rotation = command->theta_rads;
            motionRequest.walkRequest.target.translation.x = command->x_mm;
            motionRequest.walkRequest.target.translation.y = command->y_mm;

            // Let's do some motion kicking!
            if (command->motionKick && !justMotionKicked) {
                if (command->kickType == 0) {
                    motionRequest.walkRequest.kickType = WalkRequest::sidewardsLeft;
                }
                else if (command->kickType == 1) {
                    motionRequest.walkRequest.kickType = WalkRequest::sidewardsRight;
                }
                else if (command->kickType == 2) {
                    motionRequest.walkRequest.kickType = WalkRequest::left;
                }
                else {
                    motionRequest.walkRequest.kickType = WalkRequest::right;
                }
            }

            walkingEngine->theMotionRequestBH = motionRequest;
        }
        //TODO make special command for stand
        if (!currentCommand.get()) {
            MotionRequestBH motionRequest;
            motionRequest.motion = MotionRequestBH::stand;

            walkingEngine->theMotionRequestBH = motionRequest;
        }
        }
        }
    }
    }

    // We do not copy temperatures because they are unused
    JointDataBH& bh_joint_data = walkingEngine->theJointDataBH;

    for (int i = 0; i < JointDataBH::numOfJoints; i++)
    {
        bh_joint_data.angles[nb_joint_order[i]] = sensorAngles[i];
    }

    SensorDataBH& bh_sensors = walkingEngine->theSensorDataBH;

    for (int i = 0; i < JointDataBH::numOfJoints; i++)
    {
        bh_sensors.currents[nb_joint_order[i]] = sensorCurrents[i];
    }

    bh_sensors.data[SensorDataBH::gyroX] = sensorInertials.gyr_x();
    bh_sensors.data[SensorDataBH::gyroY] = sensorInertials.gyr_y();

    bh_sensors.data[SensorDataBH::accX] = sensorInertials.acc_x();
    bh_sensors.data[SensorDataBH::accY] = sensorInertials.acc_y();
    bh_sensors.data[SensorDataBH::accZ] = sensorInertials.acc_z();

    bh_sensors.data[SensorDataBH::angleX] = sensorInertials.angle_x();
    bh_sensors.data[SensorDataBH::angleY] = sensorInertials.angle_y();

    bh_sensors.data[SensorDataBH::fsrLFL] = sensorFSRs.lfl();
    bh_sensors.data[SensorDataBH::fsrLFR] = sensorFSRs.lfr();
    bh_sensors.data[SensorDataBH::fsrLBL] = sensorFSRs.lrl();
    bh_sensors.data[SensorDataBH::fsrLBR] = sensorFSRs.lrr();

    bh_sensors.data[SensorDataBH::fsrRFL] = sensorFSRs.rfl();
    bh_sensors.data[SensorDataBH::fsrLFR] = sensorFSRs.lfr();
    bh_sensors.data[SensorDataBH::fsrLBL] = sensorFSRs.lrl();
    bh_sensors.data[SensorDataBH::fsrLBR] = sensorFSRs.lrr();

    WalkingEngineOutputBH output;
    walkingEngine->update(output);

    // Update justMotionKicked so that we do not motion kick multiple times in a row
    if (output.executedWalk.kickType != WalkRequest::none) { // if we succesfully motion kicked
        justMotionKicked = true;
    }
    else if (walkingEngine->theMotionRequestBH.walkRequest.mode != WalkRequest::targetMode || // else if we are no longer attempting to motion kick
             !boost::shared_static_cast<DestinationCommand>(currentCommand)->motionKick) {
        justMotionKicked = false;
    }

    // Ignore the first chain since it's the head
    for (unsigned i = 1; i < Kinematics::NUM_CHAINS; i++) {
        std::vector<float> chain_angles;
        std::vector<float> chain_hardness;
        for (unsigned j = Kinematics::chain_first_joint[i];
                     j <= Kinematics::chain_last_joint[i]; j++) {
            chain_angles.push_back(output.angles[nb_joint_order[j]] * walkingEngine->theJointCalibrationBH.joints[nb_joint_order[j]].sign - walkingEngine->theJointCalibrationBH.joints[nb_joint_order[j]].offset);
            if (output.jointHardness.hardness[nb_joint_order[j]] == 0) {
                chain_hardness.push_back(MotionConstants::NO_STIFFNESS);
            } else {
                chain_hardness.push_back(output.jointHardness.hardness[nb_joint_order[j]] / 100.f);
            }

        }
        this->setNextChainJoints((Kinematics::ChainID) i, chain_angles);
        this->setNextChainStiffnesses((Kinematics::ChainID) i, chain_hardness);
    }

    // We only really leave when we do a sweet move, so request a special action
    if (walkingEngine->theMotionSelectionBH.targetMotion == MotionRequestBH::specialAction
            && requestedToStop) {

        inactive();
        requestedToStop = false;
        // Reset odometry - this allows the walk to not "freak out" when we come back
        // from other providers
        walkingEngine->theOdometryDataBH = OdometryDataBH();
    }

    PROF_EXIT(P_WALK);
}

bool BHWalkProvider::isStanding() const {
    return walkingEngine->theMotionRequestBH.motion == MotionRequestBH::stand;
}

bool BHWalkProvider::isWalkActive() const {
    return !(isStanding() && walkingEngine->theWalkingEngineOutputBH.isLeavingPossible) && isActive();
}

void BHWalkProvider::stand() {
    currentCommand = MotionCommand::ptr();
    active();
}

void BHWalkProvider::getOdometryUpdate(portals::OutPortal<messages::RobotLocation>& out) const
{
    portals::Message<messages::RobotLocation> odometryData(0);
    odometryData.get()->set_x(walkingEngine->theOdometryDataBH.translation.x
                              * MM_TO_CM);
    odometryData.get()->set_y(walkingEngine->theOdometryDataBH.translation.y
                              * MM_TO_CM);
    odometryData.get()->set_h(walkingEngine->theOdometryDataBH.rotation);

    out.setMessage(odometryData);
}

void BHWalkProvider::hardReset() {
    inactive();

    // Reset odometry
    walkingEngine->theOdometryDataBH = OdometryDataBH();

    MotionRequestBH motionRequest;
    motionRequest.motion = MotionRequestBH::specialAction;

    // TODO is this the same?
    motionRequest.specialActionRequest.specialAction = SpecialActionRequest::sitDown;
    currentCommand = MotionCommand::ptr();

    walkingEngine->inertiaSensorCalibrator->reset();

    requestedToStop = false;
}

void BHWalkProvider::resetOdometry() {
    walkingEngine->theOdometryDataBH = OdometryDataBH();
}

void BHWalkProvider::setCommand(const WalkCommand::ptr command) {

    if (command->theta_percent == 0 && command->x_percent == 0 && command->y_percent == 0) {
        this->stand();
        return;
    }

    currentCommand = command;

    active();
}

void BHWalkProvider::setCommand(const StepCommand::ptr command) {
    MotionRequestBH motionRequest;
    motionRequest.motion = MotionRequestBH::walk;
    walkingEngine->theMotionRequestBH = motionRequest;

    startOdometry = walkingEngine->theOdometryDataBH;
    currentCommand = command;

    active();
}

void BHWalkProvider::setCommand(const DestinationCommand::ptr command) {

    currentCommand = command;

    active();
}

bool BHWalkProvider::calibrated() const {
    return walkingEngine->theInertiaSensorDataBH.calibrated;
}

float BHWalkProvider::leftHandSpeed() const {
    return walkingEngine->leftHandSpeed;
}

float BHWalkProvider::rightHandSpeed() const {
    return walkingEngine->rightHandSpeed;
}

}
}
