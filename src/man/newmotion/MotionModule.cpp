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
    boost::shared_ptr<FreezeCommand> paralyze
        = boost::shared_ptr<FreezeCommand>(new FreezeCommand());
}

MotionModule::~MotionModule()
{

}

void MotionModule::start()
{
    std::cout << "(MotionModule) Starting motion." << std::endl;
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
    bodyCommandInput_.latch();

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

        // Josh and Wils are gonna try this. Does it work? Does it blend??
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
    using namespace Kinematics;
    // if(curHeadProvider->isActive()){
    //      const vector <float > headStiffnesses =
    //          curHeadProvider->getChainStiffnesses(HEAD_CHAIN);

    //      for(unsigned int i = 0; i < HEAD_JOINTS; i ++){
    //          nextStiffnesses[HEAD_YAW + i] = headStiffnesses.at(i);
    //      }
    // }

    if(curProvider->isActive()){
        const std::vector<float> llegStiffnesses =
            curProvider->getChainStiffnesses(LLEG_CHAIN);

        const std::vector<float> rlegStiffnesses =
            curProvider->getChainStiffnesses(RLEG_CHAIN);

        const std::vector<float> rarmStiffnesses =
            curProvider->getChainStiffnesses(RARM_CHAIN);

        const std::vector<float> larmStiffnesses =
            curProvider->getChainStiffnesses(LARM_CHAIN);

        for(unsigned int i = 0; i < LEG_JOINTS; i ++){
            nextStiffnesses[L_HIP_YAW_PITCH + i] = llegStiffnesses.at(i);
            nextStiffnesses[R_HIP_YAW_PITCH + i] = rlegStiffnesses.at(i);
        }

        for(unsigned int i = 0; i < ARM_JOINTS; i ++){
            nextStiffnesses[L_SHOULDER_PITCH + i] = larmStiffnesses.at(i);
            nextStiffnesses[R_SHOULDER_PITCH + i] = rarmStiffnesses.at(i);
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
        std::cout << "postprocess swap" << std::endl;
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
    using namespace Kinematics;

    if (curProvider->isActive())
    {
        //TODO: move this
        //let the walk engine know if it's in use or in standby
        if (curProvider != &walkProvider) {
            walkProvider.setStandby(true);
            //"fake" calculate - this is just for the sensor computation
            walkProvider.calculateNextJointsAndStiffnesses(
                sensorAngles, inertialsInput_.message(), fsrInput_.message());
            curProvider->calculateNextJointsAndStiffnesses(
                sensorAngles, inertialsInput_.message(), fsrInput_.message());
        } else {
            walkProvider.setStandby(false);
            walkProvider.calculateNextJointsAndStiffnesses(
                sensorAngles, inertialsInput_.message(), fsrInput_.message());
        }

        const std::vector<float> llegJoints = curProvider->getChainJoints(LLEG_CHAIN);
        const std::vector<float> rlegJoints = curProvider->getChainJoints(RLEG_CHAIN);
        const std::vector<float> rarmJoints = curProvider->getChainJoints(RARM_CHAIN);
        const std::vector<float> larmJoints = curProvider->getChainJoints(LARM_CHAIN);

        //copy and clip joints for safety
        for(unsigned int i = 0; i < LEG_JOINTS; i ++)
        {
            nextJoints[R_HIP_YAW_PITCH + i] = NBMath::clip(rlegJoints.at(i),
                                                           RIGHT_LEG_BOUNDS[i][0], RIGHT_LEG_BOUNDS[i][1]);

            nextJoints[L_HIP_YAW_PITCH + i] = NBMath::clip(llegJoints.at(i),
                                                           LEFT_LEG_BOUNDS[i][0], LEFT_LEG_BOUNDS[i][1]);
        }

        for(unsigned int i = 0; i < ARM_JOINTS; i ++)
        {
            nextJoints[L_SHOULDER_PITCH + i] = NBMath::clip(larmJoints.at(i),
                                                            LEFT_ARM_BOUNDS[i][0], LEFT_ARM_BOUNDS[i][1]);
            nextJoints[R_SHOULDER_PITCH + i] = NBMath::clip(rarmJoints.at(i),
                                                            RIGHT_ARM_BOUNDS[i][0], RIGHT_ARM_BOUNDS[i][1]);
        }
    }
}

// written as a stopgap measure by Josh Z 4/3/2013
void MotionModule::processHeadJoints()
{
    using namespace Kinematics;

    if (curHeadProvider->isActive())
    {
        const std::vector<float> headJoints = curHeadProvider->getChainJoints(HEAD_CHAIN);

        nextJoints[HEAD_YAW]   = boundHeadYaw(headJoints.at(0),headJoints.at(1));
        nextJoints[HEAD_PITCH] = headJoints.at(1);
    }
}

void MotionModule::processMotionInput()
{
    // (1) First process body commands.
    if(!bodyCommandInput_.message().processed_by_motion())
    {
        std::cout << "MESSAGE" << bodyCommandInput_.message().dest().rel_x() << " "
                  << bodyCommandInput_.message().dest().rel_y() << " "
                  << bodyCommandInput_.message().dest().rel_h() << " "<< std::endl;

        // Is this a destination walk request?
        if (bodyCommandInput_.message().type() == messages::MotionCommand::DESTINATION_WALK){
            sendMotionCommand(bodyCommandInput_.message().dest());
            const_cast<messages::MotionCommand&>(bodyCommandInput_.message()).set_processed_by_motion(true);
        }
        // Walk request?
        else if (bodyCommandInput_.message().type() == messages::MotionCommand::WALK_COMMAND){
            sendMotionCommand(bodyCommandInput_.message().speed());
            const_cast<messages::MotionCommand&>(bodyCommandInput_.message()).set_processed_by_motion(true);
        }
        // Sweet Move request?
        else if (bodyCommandInput_.message().type() == messages::MotionCommand::SCRIPTED_MOVE){
            sendMotionCommand(bodyCommandInput_.message().script());
            const_cast<messages::MotionCommand&>(bodyCommandInput_.message()).set_processed_by_motion(true);
        }
        std::cout << "POST!! " << bodyCommandInput_.message().processed_by_motion() << std::endl;
    }

    // (2) Process head commands.
    if(!headCommandInput_.message().processed_by_motion())
    {
        if(headCommandInput_.message().type() == messages::HeadMotionCommand::SET_HEAD_COMMAND)
        {
            sendMotionCommand(headCommandInput_.message().set_command());
            const_cast<messages::HeadMotionCommand&>(headCommandInput_.message()).set_processed_by_motion(true);
        }
        else if(headCommandInput_.message().type() == messages::HeadMotionCommand::SCRIPTED_HEAD_COMMAND)
        {
            sendMotionCommand(headCommandInput_.message().scripted_command());
            const_cast<messages::HeadMotionCommand&>(headCommandInput_.message()).set_processed_by_motion(true);
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
        if (!curProvider->isStopping()) {
            curProvider->requestStop();
        }

        if (!curProvider->isActive()) {
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
    using namespace Kinematics;

    float yaw = (float)fabs(joints[HEAD_YAW]);
    float pitch = joints[HEAD_PITCH];

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

    joints[HEAD_PITCH] = pitch;
}

void MotionModule::safetyCheckJoints()
{
    using namespace Kinematics;

    for (unsigned int i = 0; i < NUM_JOINTS; i++)
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

        const float allowedMotionDiffInRad = jointsMaxVelNoLoad[i];
        const float allowedSensorDiffInRad = allowedMotionDiffInRad*6.0f;

        //considering checking which clip is more restrictive each frame and
        //only applying it
        nextJoints[i] = NBMath::clip(nextJoints[i],
                                     lastJoints[i] - allowedMotionDiffInRad,
                                     lastJoints[i] + allowedMotionDiffInRad);

        nextJoints[i] = NBMath::clip(nextJoints[i],
                                     sensorAngles[i] - allowedSensorDiffInRad,
                                     sensorAngles[i] + allowedSensorDiffInRad);

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
    std::cout << "(MotionModule) Last provider: "
              << old_provider << "." << std::endl;

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


void MotionModule::sendMotionCommand(messages::ScriptedMove script)
{
    // Create a command for every Body Joint Command
    for (int i = 0; i < script.commands_size(); i++)
    {
        std::vector<float> angles(26, 0.f);
        std::vector<float> stiffness(26, 0.f);

        // populate vectors
        angles[0] = script.commands(i).angles().l_shoulder_pitch();
        angles[1] = script.commands(i).angles().l_shoulder_roll();
        angles[2] = script.commands(i).angles().l_elbow_yaw();
        angles[3] = script.commands(i).angles().l_elbow_roll();
        angles[4] = script.commands(i).angles().l_hip_yaw_pitch();
        angles[5] = script.commands(i).angles().l_hip_roll();
        angles[6] = script.commands(i).angles().l_hip_pitch();
        angles[7] = script.commands(i).angles().l_knee_pitch();
        angles[8] = script.commands(i).angles().l_ankle_pitch();
        angles[9] = script.commands(i).angles().l_ankle_roll();
        angles[10] = script.commands(i).angles().r_hip_yaw_pitch();
        angles[11] = script.commands(i).angles().r_hip_roll();
        angles[12] = script.commands(i).angles().r_hip_pitch();
        angles[13] = script.commands(i).angles().r_knee_pitch();
        angles[14] = script.commands(i).angles().r_ankle_pitch();
        angles[15] = script.commands(i).angles().r_ankle_roll();
        angles[16] = script.commands(i).angles().r_shoulder_pitch();
        angles[17] = script.commands(i).angles().r_shoulder_roll();
        angles[18] = script.commands(i).angles().r_elbow_yaw();
        angles[19] = script.commands(i).angles().r_elbow_roll();

        stiffness[0] = script.commands(i).stiffness().head_yaw();
        stiffness[1] = script.commands(i).stiffness().head_pitch();
        stiffness[2] = script.commands(i).stiffness().l_shoulder_pitch();
        stiffness[3] = script.commands(i).stiffness().l_shoulder_roll();
        stiffness[4] = script.commands(i).stiffness().l_elbow_yaw();
        stiffness[5] = script.commands(i).stiffness().l_elbow_roll();
        stiffness[6] = script.commands(i).stiffness().l_hip_yaw_pitch();
        stiffness[7] = script.commands(i).stiffness().l_hip_roll();
        stiffness[8] = script.commands(i).stiffness().l_hip_pitch();
        stiffness[9] = script.commands(i).stiffness().l_knee_pitch();
        stiffness[10] = script.commands(i).stiffness().l_ankle_pitch();
        stiffness[11] = script.commands(i).stiffness().l_ankle_roll();
        stiffness[12] = script.commands(i).stiffness().r_hip_yaw_pitch();
        stiffness[13] = script.commands(i).stiffness().r_hip_roll();
        stiffness[14] = script.commands(i).stiffness().r_hip_pitch();
        stiffness[15] = script.commands(i).stiffness().r_knee_pitch();
        stiffness[16] = script.commands(i).stiffness().r_ankle_pitch();
        stiffness[17] = script.commands(i).stiffness().r_ankle_roll();
        stiffness[18] = script.commands(i).stiffness().r_shoulder_pitch();
        stiffness[19] = script.commands(i).stiffness().r_shoulder_roll();
        stiffness[20] = script.commands(i).stiffness().r_elbow_yaw();
        stiffness[21] = script.commands(i).stiffness().r_elbow_roll();



        // Interpolation is set for the entire script, not per command
        Kinematics::InterpolationType interType = Kinematics::INTERPOLATION_SMOOTH;
        if(script.interpolation() == 1)
            interType = Kinematics::INTERPOLATION_LINEAR;

        // create the BJC and set it
        motion::BodyJointCommand::ptr newCommand(
            new motion::BodyJointCommand(
                // Time is set for the entire script, not per command
                (script.time()),
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

void MotionModule::sendMotionCommand(const messages::SetHeadCommand& command)
{
    nextHeadProvider = &headProvider;
    if (command.max_speed_yaw == -1 || command.max_speed_pitch == -1)
    {
        SetHeadCommand::ptr setHeadCommand(
            new SetHeadCommand(command.head_yaw(),
                               command.head_pitch()
                )
            );
    }
    else
    {
        SetHeadCommand::ptr setHeadCommand(
            new SetHeadCommand(command.head_yaw(),
                               command.head_pitch(),
                               command.max_speed_yaw(),
                               command.max_speed_pitch()
                )
            );
    }
    headProvider.setCommand(setHeadCommand);
}

void MotionModule::sendMotionCommand(const HeadJointCommand::ptr command)
{
    nextHeadProvider = &headProvider;
    headProvider.setCommand(command);
}

void MotionModule::sendMotionCommand(const messages::ScriptedHeadCommand& script)
{
    nextHeadProvider = &headProvider;
    // Create a command for every Body Joint Command
    for (int i = 0; i < script.commands_size(); i++)
    {
        std::vector<float> angles(26, 0.f);
        std::vector<float> stiffness(26, 0.f);

        // populate vectors
        angles[0] = script.commands(i).angles().head_yaw();
        angles[1] = script.commands(i).angles().head_pitch();

        stiffness[0] = script.commands(i).stiffness().head_yaw();
        stiffness[1] = script.commands(i).stiffness().head_pitch();
        stiffness[2] = script.commands(i).stiffness().l_shoulder_pitch();
        stiffness[3] = script.commands(i).stiffness().l_shoulder_roll();
        stiffness[4] = script.commands(i).stiffness().l_elbow_yaw();
        stiffness[5] = script.commands(i).stiffness().l_elbow_roll();
        stiffness[6] = script.commands(i).stiffness().l_hip_yaw_pitch();
        stiffness[7] = script.commands(i).stiffness().l_hip_roll();
        stiffness[8] = script.commands(i).stiffness().l_hip_pitch();
        stiffness[9] = script.commands(i).stiffness().l_knee_pitch();
        stiffness[10] = script.commands(i).stiffness().l_ankle_pitch();
        stiffness[11] = script.commands(i).stiffness().l_ankle_roll();
        stiffness[12] = script.commands(i).stiffness().r_hip_yaw_pitch();
        stiffness[13] = script.commands(i).stiffness().r_hip_roll();
        stiffness[14] = script.commands(i).stiffness().r_hip_pitch();
        stiffness[15] = script.commands(i).stiffness().r_knee_pitch();
        stiffness[16] = script.commands(i).stiffness().r_ankle_pitch();
        stiffness[17] = script.commands(i).stiffness().r_ankle_roll();
        stiffness[18] = script.commands(i).stiffness().r_shoulder_pitch();
        stiffness[19] = script.commands(i).stiffness().r_shoulder_roll();
        stiffness[20] = script.commands(i).stiffness().r_elbow_yaw();
        stiffness[21] = script.commands(i).stiffness().r_elbow_roll();



        Kinematics::InterpolationType interType = Kinematics::INTERPOLATION_SMOOTH;
        if(script.commands(i).interpolation() == 1)
            interType = Kinematics::INTERPOLATION_LINEAR;

        // create the HJC and set it
        motion::HeadJointCommand::ptr newCommand(
            new motion::HeadJointCommand(
                (script.commands(i).time()),
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
    if(curHeadProvider == &nullHeadProvider){
        nullHeadProvider.setCommand(command);
    }
    if(curProvider == &nullBodyProvider){
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

void MotionModule::sendMotionCommand(messages::DestinationWalk command)
{
    nextProvider = &walkProvider;
    DestinationCommand::ptr newCommand(
        new DestinationCommand(
            command.rel_x(),
            command.rel_y(),
            command.rel_h()
            )
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

} // namespace motion
} // namespace man
