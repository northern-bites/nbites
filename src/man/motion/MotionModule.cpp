#include "MotionModule.h"

#include <fstream>

namespace man
{
namespace motion
{
MotionModule::MotionModule()
    : jointsOutput_(base()),
      stiffnessOutput_(base()),
      odometryOutput_(base()),
      motionStatusOutput_(base()),
      curProvider(&nullBodyProvider),
      nextProvider(&nullBodyProvider),
      curHeadProvider(&nullHeadProvider),
      nextHeadProvider(&nullHeadProvider),
      nextJoints(std::vector<float>(Kinematics::NUM_JOINTS, 0.0f)),
      nextStiffnesses(std::vector<float>(Kinematics::NUM_JOINTS, 0.0f)),
      lastJoints(std::vector<float>(Kinematics::NUM_JOINTS, 0.0f)),
      frameCount(0),
      running(true),
      newJoints(false),
      newInputJoints(false),
      readyToSend(false),
      noWalkTransitionCommand(true)
{

}

MotionModule::~MotionModule()
{
}

void MotionModule::start()
{
    running = true;
}

void MotionModule::stop()
{
    running = false;
}

void MotionModule::run_()
{
    // (1) Before anything else happens, it is important to
    //     retrieve the correct current joint angles.
    jointsInput_.latch();
    inertialsInput_.latch();
    fsrInput_.latch();
    stiffnessInput_.latch();
    bodyCommandInput_.latch();
    headCommandInput_.latch();
    requestInput_.latch();

    sensorAngles    = toJointAngles(jointsInput_.message());

    newInputJoints = false;

    // (2) If motion is enabled, perform a single iteration
    //     of the main motion loop.
    if(running)
    {
        // (3) Do any necessary preprocessing of joint angles
        //     then actually compute next joints and
        //     stiffnesses.
        realityCheckJoints();

        processMotionInput();

        preProcess();
        processJoints();
        processStiffness();
        bool active = postProcess();

       // (4) Send newly computed joints and stiffnesses to
        //     the joint enactor module.
        if(active)
            setJointsAndStiffness();

        // (5) Get the latest odometry measurements and update
        //     the messages that we output.
        updateOdometry();
        updateStatus();

        newInputJoints = false;
        frameCount++;
    }
}

void MotionModule::resetOdometry()
{
    walkProvider.resetOdometry();
}

void MotionModule::preProcess()
{
    preProcessHead();
    preProcessBody();
}

void MotionModule::processJoints()
{
    processHeadJoints();
    processBodyJoints();
    safetyCheckJoints();
}

/**
 * Method to process remaining stiffness requests
 * Technically this could be handled by another provider, but there isn't
 * too much too it:
 */
void MotionModule::processStiffness()
{
    if(curHeadProvider->isActive())
    {
        const std::vector<float> headStiffnesses =
            curHeadProvider->getChainStiffnesses(Kinematics::HEAD_CHAIN);

        for(unsigned int i = 0; i < Kinematics::HEAD_JOINTS; i++)
        {
            nextStiffnesses[Kinematics::HEAD_YAW + i] = headStiffnesses.at(i);
        }
    }

    if(curProvider->isActive())
    {
        const std::vector<float> llegStiffnesses =
            curProvider->getChainStiffnesses(Kinematics::LLEG_CHAIN);

        const std::vector<float> rlegStiffnesses =
            curProvider->getChainStiffnesses(Kinematics::RLEG_CHAIN);

        const std::vector<float> rarmStiffnesses =
            curProvider->getChainStiffnesses(Kinematics::RARM_CHAIN);

        const std::vector<float> larmStiffnesses =
            curProvider->getChainStiffnesses(Kinematics::LARM_CHAIN);

        for(unsigned int i = 0; i < Kinematics::LEG_JOINTS; i ++){
            nextStiffnesses[Kinematics::L_HIP_YAW_PITCH + i] = llegStiffnesses.at(i);
            nextStiffnesses[Kinematics::R_HIP_YAW_PITCH + i] = rlegStiffnesses.at(i);
        }

        for(unsigned int i = 0; i < Kinematics::ARM_JOINTS; i ++){
            nextStiffnesses[Kinematics::L_SHOULDER_PITCH + i] = larmStiffnesses.at(i);
            nextStiffnesses[Kinematics::R_SHOULDER_PITCH + i] = rarmStiffnesses.at(i);
        }
    }

    std::vector<float>::iterator i = nextStiffnesses.begin();
    for (; i != nextStiffnesses.end(); ++i) {
        if (*i < MotionConstants::MIN_STIFFNESS){
            *i = MotionConstants::NO_STIFFNESS;
        } else {
            *i = NBMath::clip(*i,
                              MotionConstants::MIN_STIFFNESS,
                              MotionConstants::MAX_STIFFNESS);
        }
    }
}

bool MotionModule::postProcess()
{
    newJoints = true;

    //Make sure that if the current provider just became inactive,
    //and we have the next provider ready, then we want to swap to ensure
    //that we never have an inactive provider when an active one is potentially
    //ready to take over:
    if (curProvider != nextProvider && !curProvider->isActive())
    {
        swapBodyProvider();
    }

    if (curHeadProvider != nextHeadProvider && !curHeadProvider->isActive())
    {
        swapHeadProvider();
    }

    // Update sensors with the correct support foot because it may have
    // changed this frame.
    // TODO: This can be improved by keeping a local copy of the SupportFoot
    //       so that we only update sensors when there has been a change.
    //       The overhead of the mutex shouldn't be that high though.
    //sensors->setSupportFoot(curProvider->getSupportFoot());

    //return if one of the enactors is active
    return curProvider->isActive()  || curHeadProvider->isActive();
}

void MotionModule::processBodyJoints()
{
    if (curProvider->isActive())
    {
        //TODO: move this
        //let the walk engine know if it's in use or in standby
        if (curProvider != &walkProvider)
        {
            walkProvider.setStandby(true);
            //"fake" calculate - this is just for the sensor computation
            walkProvider.calculateNextJointsAndStiffnesses(
                sensorAngles, inertialsInput_.message(), fsrInput_.message());
            curProvider->calculateNextJointsAndStiffnesses(
                sensorAngles, inertialsInput_.message(), fsrInput_.message());
        }
        else
        {
            walkProvider.setStandby(false);
            walkProvider.calculateNextJointsAndStiffnesses(
                sensorAngles, inertialsInput_.message(), fsrInput_.message());
        }

        const std::vector<float> llegJoints =
            curProvider->getChainJoints(Kinematics::LLEG_CHAIN);
        const std::vector<float> rlegJoints =
            curProvider->getChainJoints(Kinematics::RLEG_CHAIN);
        const std::vector<float> rarmJoints =
            curProvider->getChainJoints(Kinematics::RARM_CHAIN);
        const std::vector<float> larmJoints =
            curProvider->getChainJoints(Kinematics::LARM_CHAIN);

        //copy and clip joints for safety
        for(unsigned int i = 0; i < Kinematics::LEG_JOINTS; i ++)
        {
            nextJoints[Kinematics::R_HIP_YAW_PITCH + i] =
                NBMath::clip(rlegJoints.at(i),
                             Kinematics::RIGHT_LEG_BOUNDS[i][0],
                             Kinematics::RIGHT_LEG_BOUNDS[i][1]);

            nextJoints[Kinematics::L_HIP_YAW_PITCH + i]
                = NBMath::clip(llegJoints.at(i),
                               Kinematics::LEFT_LEG_BOUNDS[i][0],
                               Kinematics::LEFT_LEG_BOUNDS[i][1]);
        }

        for(unsigned int i = 0; i < Kinematics::ARM_JOINTS; i ++)
        {
            nextJoints[Kinematics::L_SHOULDER_PITCH + i] =
                NBMath::clip(larmJoints.at(i),
                             Kinematics::LEFT_ARM_BOUNDS[i][0],
                             Kinematics::LEFT_ARM_BOUNDS[i][1]);

            nextJoints[Kinematics::R_SHOULDER_PITCH + i] =
                NBMath::clip(rarmJoints.at(i),
                             Kinematics::RIGHT_ARM_BOUNDS[i][0],
                             Kinematics::RIGHT_ARM_BOUNDS[i][1]);
        }
    }
}

void MotionModule::processHeadJoints()
{
    if (curHeadProvider->isActive())
    {
        curHeadProvider->calculateNextJointsAndStiffnesses(
            sensorAngles,
            inertialsInput_.message(),
            fsrInput_.message());
        std::vector<float> headJoints =
            curHeadProvider->getChainJoints(Kinematics::HEAD_CHAIN);

        clipHeadJoints(headJoints);

        for(unsigned int i = Kinematics::FIRST_HEAD_JOINT;
            i < Kinematics::FIRST_HEAD_JOINT + Kinematics::HEAD_JOINTS;
            ++i)
        {
            nextJoints[i] = headJoints.at(i);
        }
    }
}

void MotionModule::processMotionInput()
{
    // (1) Process Behavior requests.
    if(lastRequest != requestInput_.message().timestamp())
    {
        lastRequest = requestInput_.message().timestamp();

        if (requestInput_.message().stop_body())
        {
            stopBodyMoves();
        }
        if (requestInput_.message().stop_head())
        {
            stopHeadMoves();
        }
        if (requestInput_.message().reset_odometry())
        {
            resetOdometry();
        }
        if (requestInput_.message().remove_stiffness())
        {
            // Don't set gainsOn to false or else we won't freeze from behaviors
            // Side effect is that if behaviors tells motion to freeze,
            // we won't be able to enable gains with a button. Oh well
            if (gainsOn)
            {
                sendMotionCommand(FreezeCommand::ptr(new FreezeCommand()));
            }
        }
        if (requestInput_.message().enable_stiffness())
        {
            // Set gains on so that if our button got pressed we will
            // freeze again next frame.
            if (!stiffnessInput_.message().remove())
            {
                sendMotionCommand(UnfreezeCommand::ptr(new UnfreezeCommand()));
                gainsOn = true;
            }
        }
    }

    // (2) Guardian checks.
    if(stiffnessInput_.message().remove() && gainsOn)
    {
        gainsOn = false;
        sendMotionCommand(FreezeCommand::ptr(new FreezeCommand()));
        return;
    }
    if(!stiffnessInput_.message().remove() && !gainsOn)
    {
        gainsOn = true;
        sendMotionCommand(UnfreezeCommand::ptr(new UnfreezeCommand()));
        return;
    }

    // (3) Disallow further commands if we turned gains off.
    if (!gainsOn)
        return;



    // (4) Process body commands.
    if(lastBodyCommand != bodyCommandInput_.message().timestamp())
    {
        lastBodyCommand = bodyCommandInput_.message().timestamp();

        if (bodyCommandInput_.message().type() ==
            messages::MotionCommand::DESTINATION_WALK)
        {
            sendMotionCommand(bodyCommandInput_.message().dest());
        }
        else if (bodyCommandInput_.message().type() ==
                 messages::MotionCommand::WALK_COMMAND)
        {
            sendMotionCommand(bodyCommandInput_.message().speed());
        }
        else if (bodyCommandInput_.message().type() ==
                 messages::MotionCommand::SCRIPTED_MOVE)
        {
            // Send an unfreeze command so we can stand up.
            sendMotionCommand(UnfreezeCommand::ptr(new UnfreezeCommand()));
            sendMotionCommand(bodyCommandInput_.message().script());
        }
    }

    // (5) Process head commands.
    if(lastHeadCommand != headCommandInput_.message().timestamp())
    {
        lastHeadCommand = headCommandInput_.message().timestamp();

        if (headCommandInput_.message().type() ==
            messages::HeadMotionCommand::POS_HEAD_COMMAND)
        {
            sendMotionCommand(headCommandInput_.message().pos_command());
        }
        else if (headCommandInput_.message().type() ==
                 messages::HeadMotionCommand::SCRIPTED_HEAD_COMMAND)
        {
            sendMotionCommand(headCommandInput_.message().scripted_command());
        }
    }
}

void MotionModule::preProcessBody()
{
    if (curProvider != &nullBodyProvider &&
        nextProvider == &nullBodyProvider)
    {
        scriptedProvider.hardReset();
        walkProvider.hardReset();
    }

    //determine the curProvider, and do any necessary swapping
    if (curProvider != nextProvider)
    {
        if (!curProvider->isStopping())
        {
            curProvider->requestStop();
        }

        if (!curProvider->isActive())
        {
            swapBodyProvider();
        }
    }
}

void MotionModule::preProcessHead()
{
    if(curHeadProvider != &nullHeadProvider &&
       nextHeadProvider == &nullHeadProvider)
    {
        headProvider.hardReset();
        swapHeadProvider();
        // Skip other checks since we are performing a hard
        // reset.
        return;
    }

    // determine the curHeadProvider, and do any necessary swapping
    if(curHeadProvider != nextHeadProvider)
    {
        if(!curHeadProvider->isStopping())
        {
            curHeadProvider->requestStop();
        }

        if (!curHeadProvider->isActive())
        {
            swapHeadProvider();
        }
    }
}

void MotionModule::clipHeadJoints(std::vector<float>& joints)
{
    float yaw   = (float)fabs(joints[Kinematics::HEAD_YAW]);
    float pitch = joints[Kinematics::HEAD_PITCH];

    if (yaw < 0.5f)
    {
        if (pitch > 0.46)
        {
            pitch = 0.46f;
        }
    }

    else if (yaw < 1.0f)
    {
        if (pitch > 0.4f)
        {
            pitch = 0.4f;
        }
    }

    else if (yaw < 1.32f)
    {
        if (pitch > 0.42f)
        {
            pitch = 0.42f;
        }
    }

    else if (yaw < 1.57f)
    {
        //if (pitch > -0.2f)
        //{
        //    pitch = -0.2f;
        //}
        if (pitch > 0.2f)
        {
            pitch = 0.2f;
        }
    }

    else if (yaw >= 1.57f)
    {
        //if (pitch > -0.3f)
        //{
        //    pitch = -0.3f;
        //}
        if (pitch > 0.2f)
        {
            pitch = 0.2f;
        }
    }

    joints[Kinematics::HEAD_PITCH] = pitch;
}

void MotionModule::safetyCheckJoints()
{
    for (unsigned int i = 0; i < Kinematics::NUM_JOINTS; i++)
    {
        //We need to clip angles twice. Why? Because the sensor values are between
        //20 and 40 ms old, so we can't strictly use the sensor reports to clip
        // the velocity.
        //We also can't just use the internaly held motion angles because these
        // could be out of sync with reality, and thus allow us to send bad
        // commands.
        //As a balance, we clip both with respect to sensor readings which we
        //ASSUME are 40 ms old (even if they are newer), AND we clip with respect
        //to the internally held motion command angles, which ensures that we
        //aren't sending commands which are in general too fast for the motors.
        //For the sensor angles, we clip with TWICE the max speed.

        const float allowedMotionDiffInRad = Kinematics::jointsMaxVelNoLoad[i];
        const float allowedSensorDiffInRad = allowedMotionDiffInRad*6.0f;
       // std::cout << "nj before: " << nextJoints[i] << std::endl;

       //  //considering checking which clip is more restrictive each frame and
       //  //only applying it
       //  nextJoints[i] = NBMath::clip(nextJoints[i],
       //                               lastJoints[i] - allowedMotionDiffInRad,
       //                               lastJoints[i] + allowedMotionDiffInRad);

       //  std::cout << "(nj, nj - amdir, lj + amdir) = "
       //            << "(" << nextJoints[i] << ", "
       //            << ", " << lastJoints[i] - allowedMotionDiffInRad
       //            << ", " << lastJoints[i] + allowedMotionDiffInRad
       //            << std::endl;

       //  nextJoints[i] = NBMath::clip(nextJoints[i],
       //                               sensorAngles[i] - allowedSensorDiffInRad,
       //                               sensorAngles[i] + allowedSensorDiffInRad);

       //  std::cout << "(nj, sa - asdir, sa + asdir) = "
       //            << "(" << nextJoints[i] << ", "
       //            << ", " << sensorAngles[i] - allowedSensorDiffInRad
       //            << ", " << sensorAngles[i] + allowedSensorDiffInRad
       //            << std::endl;

       //  std::cout << "nj after: " << nextJoints[i] << std::endl;
       //  std::cout << std::endl;

        lastJoints[i] = nextJoints[i];
    }
}

/**
 * Method handles switching providers. Also handles any special action
 * required when switching between providers
 */
void MotionModule::swapBodyProvider()
{
    std::vector<BodyJointCommand::ptr> transitions;
    std::string old_provider = curProvider->getName();

    switch(nextProvider->getType())
    {
    case WALK_PROVIDER:
        //WARNING THIS COULD CAUSE INFINITE LOOP IF SWITCHBOARD IS BROKEN!
        //TODO/HACK: Since we overwrite Joint angles in realityCheck
        //we may want to ensure that a gaitTranstition command is only run
        // ONCE (Maybe twice?), instead of doing this forever.
        //The potential symptoms of such a bug would be jittering when standing
        //We need to ensure we are in the correct gait before walking
        if(noWalkTransitionCommand){//only enqueue one
            noWalkTransitionCommand = false;
            transitions = generateNextBodyProviderTransitions();

            if(transitions.size() >= 1){
                for(unsigned int i = 0; i< transitions.size(); i++){
                    scriptedProvider.setCommand(transitions[i]);
                }
                curProvider = static_cast<MotionProvider * >(&scriptedProvider);
                break;
            }
        }
        curProvider = nextProvider;
        break;

    case NULL_PROVIDER:
    case SCRIPTED_PROVIDER:
    case HEAD_PROVIDER:
    default:
        noWalkTransitionCommand = true;
        curProvider = nextProvider;
    }
}

void MotionModule::swapHeadProvider()
{
    switch(nextHeadProvider->getType())
    {
    case HEAD_PROVIDER:
    default:
        curHeadProvider = nextHeadProvider;
    }
}

const std::vector<float> MotionModule::getNextStiffness() const
{
    std::vector<float> result(nextStiffnesses);
    return result;
}

void MotionModule::signalNextFrame()
{
    newInputJoints = true;
}

/**
 * Checks to ensure that the current MotionBodyAngles are close enough to
 * what the sensors are reporting. If they are very different,
 * then the bad value is replaced
 */
int MotionModule::realityCheckJoints(){
//          static const float joint_override_thresh = 0.12f;//radians
//          static const float head_joint_override_thresh = 0.3f;//need diff for head

//          int changed = 0;
//          sensorAngles = sensors->getBodyAngles();
//          vector<float> motionAngles = sensors->getMotionBodyAngles();

//          //HEAD ANGLES - handled separately to avoid trouble in HeadProvider
//          for(unsigned int i = 0; i < HEAD_JOINTS; i++){
//          if (fabs(sensorAngles[i] - motionAngles[i]) >
//              head_joint_override_thresh){
// #ifdef DEBUG_SWITCHBOARD_DISCREPANCIES
//              cout << "RealityCheck discrepancy: "<<endl
//               << "    Joint "<<i << " is off from sensors by"<<sensorAngles[i] - motionAngles[i]<<endl;
// #endif
//              nextJoints[i] = motionAngles[i] = sensorAngles[i];
//              changed += 1;
//          }
//          }

//          //BODY ANGLES
//          for(unsigned int i = HEAD_JOINTS -1; i < NUM_JOINTS; i++){
//          if (fabs(sensorAngles[i] - motionAngles[i]) > joint_override_thresh){
// #ifdef DEBUG_SWITCHBOARD_DISCREPANCIES
//             cout << "RealityCheck discrepancy: "<<endl
//               << "    Joint "<<i << " is off from sensors by"<<sensorAngles[i] - motionAngles[i]<<endl;
// #endif
//              nextJoints[i] = motionAngles[i] = sensorAngles[i];
//              changed += 1;
//          }
//          }
//          if(changed != 0)
//          sensors->setMotionBodyAngles(motionAngles);
//          return changed;
    // @todo !! figure this out!
    return 0;
}

void MotionModule::sendMotionCommand(const WalkCommand::ptr command)
{
    nextProvider = &walkProvider;
    walkProvider.setCommand(command);
}

// TESTED by EJ. Don't fuck with unless I'm told.
/*
 * Given a vector of percentages for set speeds
 */
void MotionModule::sendMotionCommand(messages::WalkCommand command)
{
    nextProvider = &walkProvider;
    WalkCommand::ptr walkCommand(
        new WalkCommand(
            command.x_percent(),
            command.y_percent(),
            command.h_percent())
        );
    walkProvider.setCommand(walkCommand);
}

void MotionModule::sendMotionCommand(const BodyJointCommand::ptr command)
{
    noWalkTransitionCommand = true;
    nextProvider = &scriptedProvider;
    scriptedProvider.setCommand(command);
}

void MotionModule::sendMotionCommand(const std::vector<BodyJointCommand::ptr> commands)
{
    noWalkTransitionCommand = true;
    nextProvider = &scriptedProvider;
    for(std::vector<BodyJointCommand::ptr>::const_iterator iter = commands.begin();
        iter != commands.end();
        iter++){
        scriptedProvider.setCommand(*iter);
    }
}

// TESTED by EJ. DO NOT FUCK with any of this without telling me.
void MotionModule::sendMotionCommand(messages::ScriptedMove script)
{
    // Create a command for every Body Joint Command
    for (int i = 0; i < script.command_size(); i++)
    {
        std::vector<float> angles(26, 0.f);
        std::vector<float> stiffness(26, 0.f);

        // Script angles are given in degrees, convert to Radians
        angles[0] = TO_RAD * script.command(i).angles().l_shoulder_pitch();
        angles[1] = TO_RAD * script.command(i).angles().l_shoulder_roll();
        angles[2] = TO_RAD * script.command(i).angles().l_elbow_yaw();
        angles[3] = TO_RAD * script.command(i).angles().l_elbow_roll();
        angles[4] = TO_RAD * script.command(i).angles().l_hip_yaw_pitch();
        angles[5] = TO_RAD * script.command(i).angles().l_hip_roll();
        angles[6] = TO_RAD * script.command(i).angles().l_hip_pitch();
        angles[7] = TO_RAD * script.command(i).angles().l_knee_pitch();
        angles[8] = TO_RAD * script.command(i).angles().l_ankle_pitch();
        angles[9] = TO_RAD * script.command(i).angles().l_ankle_roll();
        angles[10] = TO_RAD * script.command(i).angles().r_hip_yaw_pitch();
        angles[11] = TO_RAD * script.command(i).angles().r_hip_roll();
        angles[12] = TO_RAD * script.command(i).angles().r_hip_pitch();
        angles[13] = TO_RAD * script.command(i).angles().r_knee_pitch();
        angles[14] = TO_RAD * script.command(i).angles().r_ankle_pitch();
        angles[15] = TO_RAD * script.command(i).angles().r_ankle_roll();
        angles[16] = TO_RAD * script.command(i).angles().r_shoulder_pitch();
        angles[17] = TO_RAD * script.command(i).angles().r_shoulder_roll();
        angles[18] = TO_RAD * script.command(i).angles().r_elbow_yaw();
        angles[19] = TO_RAD * script.command(i).angles().r_elbow_roll();

        // Stiffness given as gains, can take direct
        stiffness[0] = script.command(i).stiffness().head_yaw();
        stiffness[1] = script.command(i).stiffness().head_pitch();
        stiffness[2] = script.command(i).stiffness().l_shoulder_pitch();
        stiffness[3] = script.command(i).stiffness().l_shoulder_roll();
        stiffness[4] = script.command(i).stiffness().l_elbow_yaw();
        stiffness[5] = script.command(i).stiffness().l_elbow_roll();
        stiffness[6] = script.command(i).stiffness().l_hip_yaw_pitch();
        stiffness[7] = script.command(i).stiffness().l_hip_roll();
        stiffness[8] = script.command(i).stiffness().l_hip_pitch();
        stiffness[9] = script.command(i).stiffness().l_knee_pitch();
        stiffness[10] = script.command(i).stiffness().l_ankle_pitch();
        stiffness[11] = script.command(i).stiffness().l_ankle_roll();
        stiffness[12] = script.command(i).stiffness().r_hip_yaw_pitch();
        stiffness[13] = script.command(i).stiffness().r_hip_roll();
        stiffness[14] = script.command(i).stiffness().r_hip_pitch();
        stiffness[15] = script.command(i).stiffness().r_knee_pitch();
        stiffness[16] = script.command(i).stiffness().r_ankle_pitch();
        stiffness[17] = script.command(i).stiffness().r_ankle_roll();
        stiffness[18] = script.command(i).stiffness().r_shoulder_pitch();
        stiffness[19] = script.command(i).stiffness().r_shoulder_roll();
        stiffness[20] = script.command(i).stiffness().r_elbow_yaw();
        stiffness[21] = script.command(i).stiffness().r_elbow_roll();

        // Interpolation is set per command
        Kinematics::InterpolationType interType = Kinematics::INTERPOLATION_SMOOTH;
        if(script.command(i).interpolation() == 1)
            interType = Kinematics::INTERPOLATION_LINEAR;

        // create the BJC and set it
        motion::BodyJointCommand::ptr newCommand(
            new motion::BodyJointCommand(
                script.command(i).time(),
                angles,
                stiffness,
                interType)
            );

        noWalkTransitionCommand = true;
        nextProvider = &scriptedProvider;
        scriptedProvider.setCommand(newCommand);
    }
}

void MotionModule::sendMotionCommand(const SetHeadCommand::ptr command)
{
    nextHeadProvider = &headProvider;
    headProvider.setCommand(command);
}

void MotionModule::sendMotionCommand(const messages::PositionHeadCommand& command)
{
    nextHeadProvider = &headProvider;
    if (command.max_speed_yaw() == -1 || command.max_speed_pitch() == -1)
    {
        SetHeadCommand::ptr setHeadCommand(
            new SetHeadCommand(TO_RAD * command.head_yaw(),
                               TO_RAD * command.head_pitch()
                )
            );
        headProvider.setCommand(setHeadCommand);
    }
    else
    {
        SetHeadCommand::ptr setHeadCommand(
            new SetHeadCommand(TO_RAD * command.head_yaw(),
                               TO_RAD * command.head_pitch(),
                               command.max_speed_yaw(),
                               command.max_speed_pitch()
                )
            );
        headProvider.setCommand(setHeadCommand);
    }
}

void MotionModule::sendMotionCommand(const HeadJointCommand::ptr command)
{
    nextHeadProvider = &headProvider;
    headProvider.setCommand(command);
}

void MotionModule::sendMotionCommand(const messages::ScriptedHeadCommand script)
{
    nextHeadProvider = &headProvider;
    // Create a command for every Body Joint Command
    for (int i = 0; i < script.command_size(); i++)
    {
        std::vector<float> angles(26, 0.f);
        std::vector<float> stiffness(26, 0.f);

        // populate vectors
        angles[0] = TO_RAD * script.command(i).angles().head_yaw();
        angles[1] = TO_RAD * script.command(i).angles().head_pitch();

        stiffness[0] = script.command(i).stiffness().head_yaw();
        stiffness[1] = script.command(i).stiffness().head_pitch();
        stiffness[2] = script.command(i).stiffness().l_shoulder_pitch();
        stiffness[3] = script.command(i).stiffness().l_shoulder_roll();
        stiffness[4] = script.command(i).stiffness().l_elbow_yaw();
        stiffness[5] = script.command(i).stiffness().l_elbow_roll();
        stiffness[6] = script.command(i).stiffness().l_hip_yaw_pitch();
        stiffness[7] = script.command(i).stiffness().l_hip_roll();
        stiffness[8] = script.command(i).stiffness().l_hip_pitch();
        stiffness[9] = script.command(i).stiffness().l_knee_pitch();
        stiffness[10] = script.command(i).stiffness().l_ankle_pitch();
        stiffness[11] = script.command(i).stiffness().l_ankle_roll();
        stiffness[12] = script.command(i).stiffness().r_hip_yaw_pitch();
        stiffness[13] = script.command(i).stiffness().r_hip_roll();
        stiffness[14] = script.command(i).stiffness().r_hip_pitch();
        stiffness[15] = script.command(i).stiffness().r_knee_pitch();
        stiffness[16] = script.command(i).stiffness().r_ankle_pitch();
        stiffness[17] = script.command(i).stiffness().r_ankle_roll();
        stiffness[18] = script.command(i).stiffness().r_shoulder_pitch();
        stiffness[19] = script.command(i).stiffness().r_shoulder_roll();
        stiffness[20] = script.command(i).stiffness().r_elbow_yaw();
        stiffness[21] = script.command(i).stiffness().r_elbow_roll();



        Kinematics::InterpolationType interType = Kinematics::INTERPOLATION_SMOOTH;
        if(script.command(i).interpolation() == 1)
            interType = Kinematics::INTERPOLATION_LINEAR;

        // create the HJC and set it
        motion::HeadJointCommand::ptr newCommand(
            new motion::HeadJointCommand(
                (script.command(i).time()),
                angles,
                stiffness,
                interType)
            );

        headProvider.setCommand(newCommand);
    }
}

void MotionModule::sendMotionCommand(const FreezeCommand::ptr command)
{
    nextProvider = &nullBodyProvider;
    nextHeadProvider = &nullHeadProvider;

    nullHeadProvider.setCommand(command);
    nullBodyProvider.setCommand(command);
}

void MotionModule::sendMotionCommand(const UnfreezeCommand::ptr command)
{
    if(curHeadProvider == &nullHeadProvider)
    {
        nullHeadProvider.setCommand(command);
    }
    if(curProvider == &nullBodyProvider)
    {
        nullBodyProvider.setCommand(command);
    }
}

void MotionModule::sendMotionCommand(const StepCommand::ptr command)
{
    nextProvider = &walkProvider;
    walkProvider.setCommand(command);
}

void MotionModule::sendMotionCommand(const DestinationCommand::ptr command)
{
    nextProvider = &walkProvider;
    walkProvider.setCommand(command);
}


// TESTED by EJ, works appropriately. Don't fuck with unless I'm told.
/*
 * Given a DestinationWalk proto,
 * Rel x and y given in cm, rel h in degrees
 * If gain is defined in the proto then it also sets the speed
 * to that gain, else does .5 by default (half speed)
 */
void MotionModule::sendMotionCommand(messages::DestinationWalk command)
{
    // Message is coming from behaviors in centimeters and degrees
    // StepCommands take millimeters and radians so Convert!
    float relX = command.rel_x() * CM_TO_MM;
    float relY = command.rel_y() * CM_TO_MM;
    float relH = command.rel_h() * TO_RAD;

    // For now go at half speed for odometry walk
    // @TODO major refactoring on all dis shit. lets make it hot
    float DEFAULT_SPEED = .5f;
    float gain = DEFAULT_SPEED;
    if(command.gain() > 0.f)
        gain = command.gain();

    nextProvider = &walkProvider;
    StepCommand::ptr newCommand(
        new StepCommand(
            relX,
            relY,
            relH,
            gain)
        );
    walkProvider.setCommand(newCommand);
}

std::vector<BodyJointCommand::ptr> MotionModule::generateNextBodyProviderTransitions()
{
    std::vector<BodyJointCommand::ptr> commands;

    std::vector<float> providerJoints = nextProvider->getInitialStance();

    if (providerJoints.size() == 0)
        return commands;

    float max_change = -M_PI_FLOAT*10.0f;

    //ignore the first chain since it's the head one
    for (unsigned i = 0; i < Kinematics::NUM_BODY_JOINTS; i++) {
        max_change = (float)std::max((double)max_change, fabs(sensorAngles[i + Kinematics::HEAD_JOINTS] - providerJoints[i]));
    }

    // this is the max we allow, not the max the hardware can do
    const float  MAX_RAD_PER_SEC =  M_PI_FLOAT*0.3f;
    float time = max_change/MAX_RAD_PER_SEC;

    if(time <= MOTION_FRAME_LENGTH_S){
        return commands;
    }

    //larm: (0.,90.,0.,0.)
    //rarm: (0.,-90.,0.,0.)
    float larm_angles[] = {0.9f, 0.3f,0.0f,0.0f};
    float rarm_angles[] = {0.9f,-0.3f,0.0f,0.0f};

    std::vector<float> safe_larm(larm_angles, &larm_angles[Kinematics::ARM_JOINTS]);
    std::vector<float> safe_rarm(rarm_angles, &rarm_angles[Kinematics::ARM_JOINTS]);

    // HACK @joho get gait stiffness params. nextGait.maxStiffness
    std::vector<float> stiffness(Kinematics::NUM_JOINTS, 0.75f);
    std::vector<float> stiffness2(Kinematics::NUM_JOINTS, 0.75f);

    std::vector<float> empty(0);
    if (time > MOTION_FRAME_LENGTH_S * 30){
        commands.push_back(
            BodyJointCommand::ptr (
                new BodyJointCommand(0.5f,safe_larm, empty,empty,safe_rarm,
                                     stiffness,
                                     Kinematics::INTERPOLATION_SMOOTH)) );
    }

    commands.push_back(
        BodyJointCommand::ptr (
            new BodyJointCommand(time, providerJoints, stiffness2,
                                 Kinematics::INTERPOLATION_SMOOTH))  );

    return commands;
}

void MotionModule::setJointsAndStiffness()
{
    using namespace Kinematics;

    portals::Message<messages::JointAngles> newJoints(0);

    // Head angles.
    newJoints.get()->set_head_yaw(nextJoints[HEAD_YAW]);
    newJoints.get()->set_head_pitch(nextJoints[HEAD_PITCH]);

    // Left arm angles.
    newJoints.get()->set_l_shoulder_pitch(nextJoints[L_SHOULDER_PITCH]);
    newJoints.get()->set_l_shoulder_roll(nextJoints[L_SHOULDER_ROLL]);
    newJoints.get()->set_l_elbow_yaw(nextJoints[L_ELBOW_YAW]);
    newJoints.get()->set_l_elbow_roll(nextJoints[L_ELBOW_ROLL]);

    // Right arm angles.
    newJoints.get()->set_r_shoulder_pitch(nextJoints[R_SHOULDER_PITCH]);
    newJoints.get()->set_r_shoulder_roll(nextJoints[R_SHOULDER_ROLL]);
    newJoints.get()->set_r_elbow_yaw(nextJoints[R_ELBOW_YAW]);
    newJoints.get()->set_r_elbow_roll(nextJoints[R_ELBOW_ROLL]);

    // Pelvis angles.
    newJoints.get()->set_l_hip_yaw_pitch(nextJoints[L_HIP_YAW_PITCH]);
    newJoints.get()->set_r_hip_yaw_pitch(nextJoints[R_HIP_YAW_PITCH]);

    // Left leg angles.
    newJoints.get()->set_l_hip_roll(nextJoints[L_HIP_ROLL]);
    newJoints.get()->set_l_hip_pitch(nextJoints[L_HIP_PITCH]);
    newJoints.get()->set_l_knee_pitch(nextJoints[L_KNEE_PITCH]);
    newJoints.get()->set_l_ankle_pitch(nextJoints[L_ANKLE_PITCH]);
    newJoints.get()->set_l_ankle_roll(nextJoints[L_ANKLE_ROLL]);

    // Right leg angles.
    newJoints.get()->set_r_hip_roll(nextJoints[R_HIP_ROLL]);
    newJoints.get()->set_r_hip_pitch(nextJoints[R_HIP_PITCH]);
    newJoints.get()->set_r_knee_pitch(nextJoints[R_KNEE_PITCH]);
    newJoints.get()->set_r_ankle_pitch(nextJoints[R_ANKLE_PITCH]);
    newJoints.get()->set_r_ankle_roll(nextJoints[R_ANKLE_ROLL]);

    jointsOutput_.setMessage(newJoints);

    portals::Message<messages::JointAngles> newStiffness(0);

    // Head stiffness.
    newStiffness.get()->set_head_yaw(nextStiffnesses[HEAD_YAW]);
    newStiffness.get()->set_head_pitch(nextStiffnesses[HEAD_PITCH]);

    // Left arm angles.
    newStiffness.get()->set_l_shoulder_pitch(nextStiffnesses[L_SHOULDER_PITCH]);
    newStiffness.get()->set_l_shoulder_roll(nextStiffnesses[L_SHOULDER_ROLL]);
    newStiffness.get()->set_l_elbow_yaw(nextStiffnesses[L_ELBOW_YAW]);
    newStiffness.get()->set_l_elbow_roll(nextStiffnesses[L_ELBOW_ROLL]);

    // Right arm angles.
    newStiffness.get()->set_r_shoulder_pitch(nextStiffnesses[R_SHOULDER_PITCH]);
    newStiffness.get()->set_r_shoulder_roll(nextStiffnesses[R_SHOULDER_ROLL]);
    newStiffness.get()->set_r_elbow_yaw(nextStiffnesses[R_ELBOW_YAW]);
    newStiffness.get()->set_r_elbow_roll(nextStiffnesses[R_ELBOW_ROLL]);

    // Pelvis angles.
    newStiffness.get()->set_l_hip_yaw_pitch(nextStiffnesses[L_HIP_YAW_PITCH]);
    newStiffness.get()->set_r_hip_yaw_pitch(nextStiffnesses[R_HIP_YAW_PITCH]);

    // Left leg angles.
    newStiffness.get()->set_l_hip_roll(nextStiffnesses[L_HIP_ROLL]);
    newStiffness.get()->set_l_hip_pitch(nextStiffnesses[L_HIP_PITCH]);
    newStiffness.get()->set_l_knee_pitch(nextStiffnesses[L_KNEE_PITCH]);
    newStiffness.get()->set_l_ankle_pitch(nextStiffnesses[L_ANKLE_PITCH]);
    newStiffness.get()->set_l_ankle_roll(nextStiffnesses[L_ANKLE_ROLL]);

    // Right leg angles.
    newStiffness.get()->set_r_hip_roll(nextStiffnesses[R_HIP_ROLL]);
    newStiffness.get()->set_r_hip_pitch(nextStiffnesses[R_HIP_PITCH]);
    newStiffness.get()->set_r_knee_pitch(nextStiffnesses[R_KNEE_PITCH]);
    newStiffness.get()->set_r_ankle_pitch(nextStiffnesses[R_ANKLE_PITCH]);
    newStiffness.get()->set_r_ankle_roll(nextStiffnesses[R_ANKLE_ROLL]);

    stiffnessOutput_.setMessage(newStiffness);
}

messages::JointAngles MotionModule::genJointCommand(float headYaw, float headPitch,
                                                    float lShoulderPitch, float lShoulderRoll,
                                                    float lElbowYaw, float lElbowRoll,
                                                    float lWristYaw, float lHand,
                                                    float rShoulderPitch, float rShoulderRoll,
                                                    float rElbowYaw, float rElbowRoll,
                                                    float rWristYaw,float rHand,
                                                    float lHipYawPitch, float rHipYawPitch,
                                                    float lHipRoll, float lHipPitch,
                                                    float lKneePitch, float lAnklePitch, float lAnkleRoll,
                                                    float rHipRoll, float rHipPitch,
                                                    float rKneePitch, float rAnklePitch, float rAnkleRoll)
{
    messages::JointAngles newMessage;
    newMessage.set_head_yaw(headYaw);
    newMessage.set_head_pitch(headPitch);

    newMessage.set_l_shoulder_pitch(lShoulderPitch);
    newMessage.set_l_shoulder_roll(lShoulderRoll);
    newMessage.set_l_elbow_yaw(lElbowYaw);
    newMessage.set_l_elbow_roll(lElbowRoll);
    newMessage.set_l_wrist_yaw(lWristYaw);
    newMessage.set_l_hand(lHand);

    newMessage.set_r_shoulder_pitch(rShoulderPitch);
    newMessage.set_r_shoulder_roll(rShoulderRoll);
    newMessage.set_r_elbow_yaw(rElbowYaw);
    newMessage.set_r_elbow_roll(rElbowRoll);
    newMessage.set_r_wrist_yaw(rWristYaw);
    newMessage.set_r_hand(rHand);

    newMessage.set_l_hip_yaw_pitch(lHipYawPitch);
    newMessage.set_r_hip_yaw_pitch(rHipYawPitch);

    newMessage.set_l_hip_roll(lHipRoll);
    newMessage.set_l_hip_pitch(lHipPitch);
    newMessage.set_l_knee_pitch(lKneePitch);
    newMessage.set_l_ankle_pitch(lAnklePitch);
    newMessage.set_l_ankle_roll(lAnkleRoll);

    newMessage.set_r_hip_roll(rHipRoll);
    newMessage.set_r_hip_pitch(rHipPitch);
    newMessage.set_r_knee_pitch(rKneePitch);
    newMessage.set_r_ankle_pitch(rAnklePitch);
    newMessage.set_r_ankle_roll(rAnkleRoll);

    return newMessage;
}

std::vector<BodyJointCommand::ptr> MotionModule::readScriptedSequence(
    const std::string& file)
{
    std::fstream fileStream;
    fileStream.open(file.c_str(), std::fstream::in);

    std::vector<BodyJointCommand::ptr> scriptedSequence;

    std::vector<float> angles(Kinematics::NUM_JOINTS, 0.0f);
    std::vector<float> stiffness(Kinematics::NUM_JOINTS, 0.0f);

    int numCommands = 0;
    float time = 0.0f;
    fileStream >> numCommands;

    std::cout << "Reading a sequence of " << numCommands
              << " commands." << std::endl;

    for(unsigned int c = 0; (int)c < numCommands; ++c)
    {
        // (1) Get joint angles, excluding the head joints.
        //     (we are only concerned with body joints and
        //     stiffnesses).
        std::cout << "Angles: ";
        for(unsigned int i = 0; i < Kinematics::NUM_JOINTS-2; ++i)
        {
            fileStream >> angles[i];
            std::cout << angles[i] << " ";
        }
        std::cout << std::endl;

        std::cout << "Stiffness: ";
        // (2) Get joint stiffness.
        for(unsigned int i = 0; i < Kinematics::NUM_JOINTS; ++i)
        {
            fileStream >> stiffness[i];
            std::cout << stiffness[i] << " ";
        }
        std::cout << std::endl;

        fileStream >> time;
        std::cout << "Time: " << time << std::endl;
        std::cout << std::endl;

        BodyJointCommand::ptr bjc(
            new BodyJointCommand(
                time,
                angles,
                stiffness,
                Kinematics::INTERPOLATION_SMOOTH
                )
            );

        scriptedSequence.push_back(bjc);
    }

    fileStream.close();

    return scriptedSequence;
}

void MotionModule::updateOdometry()
{
    walkProvider.getOdometryUpdate(odometryOutput_);
}

void MotionModule::updateStatus()
{
    portals::Message<messages::MotionStatus> status(0);

    status.get()->set_standing(isStanding());
    status.get()->set_body_is_active(isBodyActive());
    status.get()->set_walk_is_active(isWalkActive());
    status.get()->set_head_is_active(isHeadActive());
    status.get()->set_calibrated(calibrated());

    motionStatusOutput_.setMessage(status);
}

} // namespace motion
} // namespace man
