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
int BHWalkProvider::frameCount = 0;
using namespace boost;

// TODO make this consistent with new walk
const float BHWalkProvider::INITIAL_BODY_POSE_ANGLES[] =
{
        1.57f, 0.17f, -1.57f, -0.05f,
        0.0f, 0.0f, -0.44f, 0.98f, -0.55f, 0.0f,
        0.0f, 0.0f, -0.44f, 0.98f, -0.55f, 0.0f,
        1.57f, -0.17f, 1.57f, 0.05f
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
    : MotionProvider(WALK_PROVIDER), requestedToStop(false), tryingToWalk(false), kicking(false)
{
	// Setup Walk Engine Configuation Parameters
	ModuleBase::config_path = common::paths::NAO_CONFIG_DIR;

    // Setup the blackboard (used by bhuman to pass data around modules)
	blackboard = new Blackboard;

    // Setup the walk engine & kick engine
	walkingEngine = new WalkingEngine;
    kickEngine = new KickEngine;
    hardReset();
}

BHWalkProvider::~BHWalkProvider()
{
    delete blackboard;
    delete kickEngine;
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

    // If our calibration became bad (as decided by the walkingEngine,
    // reset. We will wait until we're recalibrated to walk.
    // NOTE currentMotionType and requestMotionType are of type MotionType enum 
    // defined in WalkingEngine.h
    if (walkingEngine->currentMotionType == 0 && tryingToWalk &&
        walkingEngine->instability.getAverageFloat() > 20.f && calibrated())
    {
        walkingEngine->inertiaSensorCalibrator->reset();
        walkingEngine->instability.init();
    }
    assert(JointDataBH::numOfJoints == Kinematics::NUM_JOINTS);

    if (standby) {
        tryingToWalk = false;

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
        tryingToWalk = false;

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
            tryingToWalk = true;

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
                tryingToWalk = false;

                MotionRequestBH motionRequest;
                motionRequest.motion = MotionRequestBH::stand;

                walkingEngine->theMotionRequestBH = motionRequest;
            }

        } else {
        if (currentCommand.get() && currentCommand->getType() == MotionConstants::WALK) {
            tryingToWalk = true;

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
            tryingToWalk = true;

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
                else if (command->kickType == 3){
                    motionRequest.walkRequest.kickType = WalkRequest::right;
                }
                else if (command->kickType == 4){
                    motionRequest.walkRequest.kickType = WalkRequest::diagonalLeft;
                }
                else {
                    motionRequest.walkRequest.kickType = WalkRequest::diagonalRight;
                }
            }

            walkingEngine->theMotionRequestBH = motionRequest;
        }
        else if(currentCommand.get() && currentCommand->getType() == MotionConstants::KICK)
        {
            //std::cout << "Kick Command" << std::endl;
            kickCommand = boost::shared_static_cast<KickCommand>(currentCommand);

            // Only set kicking to true once
            if (kickCommand->timeStamp != kickIndex) {
                std::cout << "Sent" << std::endl;
                kickIndex = kickCommand->timeStamp;
                kickOut = KickEngineOutput();
                kicking = true;
            }
            else if (!kicking) { // Ignore the command if we've finished kicking
                std::cout << "!kicking" << std::endl;
                stand();
            }


        }
        //TODO make special command for stand
        if (!currentCommand.get()) {
            tryingToWalk = false;

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
    bh_sensors.data[SensorDataBH::gyroZ] = sensorInertials.gyr_z(); // NOTE not currently used by BH

    bh_sensors.data[SensorDataBH::accX] = sensorInertials.acc_x();
    bh_sensors.data[SensorDataBH::accY] = sensorInertials.acc_y();
    bh_sensors.data[SensorDataBH::accZ] = sensorInertials.acc_z();

    bh_sensors.data[SensorDataBH::angleX] = sensorInertials.angle_x();
    bh_sensors.data[SensorDataBH::angleY] = sensorInertials.angle_y();
    bh_sensors.data[SensorDataBH::angleZ] = sensorInertials.angle_z();

    bh_sensors.data[SensorDataBH::fsrLFL] = sensorFSRs.lfl();
    bh_sensors.data[SensorDataBH::fsrLFR] = sensorFSRs.lfr();
    bh_sensors.data[SensorDataBH::fsrLBL] = sensorFSRs.lrl();
    bh_sensors.data[SensorDataBH::fsrLBR] = sensorFSRs.lrr();

    bh_sensors.data[SensorDataBH::fsrRFL] = sensorFSRs.rfl();
    bh_sensors.data[SensorDataBH::fsrLFR] = sensorFSRs.lfr();
    bh_sensors.data[SensorDataBH::fsrLBL] = sensorFSRs.lrl();
    bh_sensors.data[SensorDataBH::fsrLBR] = sensorFSRs.lrr();

    const float* angles = NULL;
    const int* hardness = NULL;


    if (kicking) {
        //std::cout << "Kicking!" << std::endl;;
        MotionRequestBH motionRequest;
        motionRequest.motion = MotionRequestBH::kick;
        if (kickCommand->kickType == 0) {
            motionRequest.kickRequest.kickMotionType = KickRequest::getKickMotionFromName("kickForward");
            motionRequest.kickRequest.mirror = true;
        }
        else if (kickCommand->kickType == 1) {
            motionRequest.kickRequest.kickMotionType = KickRequest::getKickMotionFromName("kickForward");
        }
        else {
            std::cout << "Kick unknown to BHWalkProvider requested. Defaulting." << std::endl;
            motionRequest.kickRequest.kickMotionType = KickRequest::getKickMotionFromName("kickForward");
        }

        walkingEngine->theMotionRequestBH = motionRequest;
        kickEngine->theMotionRequestBH = motionRequest;

        WalkingEngineStandOutputBH standout;
        walkingEngine->update(walkOutput);
        walkingEngine->update(standout);

        kickEngine->theWalkingEngineStandOutputBH = standout;
        kickEngine->update(kickOut);

//        angles = (kickOut.angles);
        float kickRatio = kickEngine->theMotionSelectionBH.ratios[MotionRequestBH::kick];
        if (kickEngine->theMotionSelectionBH.ratios[MotionRequestBH::walk] > 0) {
            interpolate((JointRequestBH)kickOut, (JointRequestBH)walkOutput, kickRatio);
            angles = request.angles;
        }
        else {
            angles = kickOut.angles;
        }


        // Kick Engine doesn't seem to be setting stiffnesses..
        hardness = (ON_STIFF_ARRAY);
        if (kickOut.isLeavingPossible) {
            std::cout << "DONE KICKING" << std::endl;
            kicking = false;
            stand();
        }
    }
    else {
        //std::cout << "Walking" << std::endl;
        walkingEngine->update(walkOutput);
        kickEngine->update(kickOut);

        float walkRatio = walkingEngine->theMotionSelectionBH.ratios[MotionRequestBH::stand];

        // Make sure that we smoothly transition into standing
        if (walkingEngine->theMotionSelectionBH.ratios[MotionRequestBH::kick] > 0) {
            interpolate((JointRequestBH)kickOut, (JointRequestBH)walkOutput, walkRatio);
            angles = request.angles;
        }
        else {
            angles = (walkOutput.angles);
        }
        hardness = (walkOutput.jointHardness.hardness);
    }

    // Update justMotionKicked so that we do not motion kick multiple times in a row
    if (walkOutput.executedWalk.kickType != WalkRequest::none) { // if we succesfully motion kicked
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
            chain_angles.push_back(angles[nb_joint_order[j]] * walkingEngine->theJointCalibrationBH.joints[nb_joint_order[j]].sign - walkingEngine->theJointCalibrationBH.joints[nb_joint_order[j]].offset);
            if (hardness[nb_joint_order[j]] == 0) {
                std::cout << "NO_STIFF" << std::endl;
                chain_hardness.push_back(MotionConstants::NO_STIFFNESS);
            } else {
                chain_hardness.push_back(hardness[nb_joint_order[j]] / 100.f);
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


void BHWalkProvider::interpolate(const JointRequestBH& from, const JointRequestBH& to, float toRatio)
{
    if (toRatio > 1.0f) printf("WHY?\n\n\n\n\n\n\n\n\n\n\n\n\n");
  for(int i = 0; i < JointDataBH::numOfJoints; ++i)
  {

    float f = from.angles[i];
    float t = to.angles[i];

    if(t == JointDataBH::ignore && f == JointDataBH::ignore)
      continue;

    if(t == JointDataBH::ignore)
      t = request.angles[i];
    if(f == JointDataBH::ignore)
      f = request.angles[i];

    int fHardness = f != JointDataBH::off ? from.jointHardness.hardness[i] : 0;
    int tHardness = t != JointDataBH::off ? to.jointHardness.hardness[i] : 0;

    if(t == JointDataBH::off || t == JointDataBH::ignore)
      t = request.angles[i];
    if(f == JointDataBH::off || f == JointDataBH::ignore)
      f = request.angles[i];
    // if(request.angles[i] == JointDataBH::off || request.angles[i] == JointDataBH::ignore)
    //   target.angles[i] = request.angles[i];

    ASSERT(request.angles[i] != JointDataBH::off && request.angles[i] != JointDataBH::ignore);
    ASSERT(t != JointDataBH::off && t != JointDataBH::ignore);
    ASSERT(f != JointDataBH::off && f != JointDataBH::ignore);

    request.angles[i] = toRatio * t + (1 - toRatio) * f;
    // if(interpolateHardness)
    //   target.jointHardness.hardness[i] += int(-fromRatio * float(tHardness) + fromRatio * float(fHardness));
    // else
    //   target.jointHardness.hardness[i] = tHardness;
  }
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

void BHWalkProvider::setCommand(const KickCommand::ptr command) {
    currentCommand = command;

    active();
}

bool BHWalkProvider::calibrated() const {
    return walkingEngine->theInertiaSensorDataBH.calibrated;
}

bool BHWalkProvider::upright() const {
    if (!isWalkActive() && walkingEngine->theFallDownStateBH.state == FallDownStateBH::onGround) {
        //std::cout << "[INERT DEBUG] Walk is not active, and falldownstate is on the ground, but we are returning upright anyways..." << std::endl;
        return true;
    }

    return walkingEngine->theFallDownStateBH.state == FallDownStateBH::upright || walkingEngine->theFallDownStateBH.state == FallDownStateBH::undefined || walkingEngine->theFallDownStateBH.state == FallDownStateBH::staggering;
//     return walkingEngine->theFallDownStateBH.state == FallDownStateBH::upright;
}

float BHWalkProvider::leftHandSpeed() const {
    return walkingEngine->leftHandSpeed;
}

float BHWalkProvider::rightHandSpeed() const {
    return walkingEngine->rightHandSpeed;
}

}
}
