#include "HeadProvider.h"

using namespace Kinematics;

namespace man
{
namespace motion
{

HeadProvider::HeadProvider()
    : MotionProvider(HEAD_PROVIDER),
      chopper(),
      nextJoints(),
      latestJointAngles(Kinematics::NUM_JOINTS, 0.0f),
      currChoppedCommand(new ChoppedCommand()),
      headCommandQueue(),
      curMode(SCRIPTED),
      yawDest(0.0f),
      pitchDest(0.0f),
      lastYawDest(0.0f),
      lastPitchDest(0.0f),
      pitchMaxSpeed(0.0f),
      yawMaxSpeed(0.0f),
      headSetStiffness(0.6f)
{
}

HeadProvider::~HeadProvider()
{
}

void HeadProvider::requestStopFirstInstance()
{
    // Finish motion or stop immediately?
    //For the head, we will stop immediately:
    stopScripted();
    stopSet();
    setActive();
}

void HeadProvider::hardReset()
{
    stopScripted();
    stopSet();
    setActive();
}

void HeadProvider::calculateNextJointsAndStiffnesses(
    std::vector<float>&            sensorAngles,
    const messages::InertialState& sensorInertials,
    const messages::FSR&           sensorFSRs
    )
{
    latestJointAngles = sensorAngles;

    switch(curMode)
    {
    case SCRIPTED:
        scriptedMode(sensorAngles);
        break;
    case SET:
        setMode();
        break;
    }
    setActive();
}

//Method called during the 'SET' Mode
void HeadProvider::setMode()
{
    //Maximum head movement is Rad/motion frame (6 deg/20ms from AL docs)
    const float MAX_HEAD_VEL = 6.0f*TO_RAD;/* ** *///we don't use this...

    //Calculate how much we can move toward the goal
    const float yawChangeTarget = NBMath::clip(yawDest - lastYawDest,
                                               - yawMaxSpeed,
                                               yawMaxSpeed);
    const float pitchChangeTarget = NBMath::clip(pitchDest - lastPitchDest,
                                                 -pitchMaxSpeed,
                                                 pitchMaxSpeed);

    //update memory for next  run
    lastYawDest = lastYawDest+yawChangeTarget;
    lastPitchDest = lastPitchDest +pitchChangeTarget;


    //update the chain angles
    std::vector<float> newChainAngles;
    newChainAngles.push_back(lastYawDest);
    newChainAngles.push_back(lastPitchDest);
    setNextChainJoints(HEAD_CHAIN,newChainAngles);

    std::vector<float> head_gains(HEAD_JOINTS, headSetStiffness);
    //Return the stiffnesses for each joint
    setNextChainStiffnesses(HEAD_CHAIN, head_gains);
}

void HeadProvider::scriptedMode(std::vector<float>& sensorAngles)
{
    if ( currChoppedCommand->isDone() )
        setNextHeadCommand(sensorAngles);

    if (!currChoppedCommand->isDone() ) {
        currChoppedCommand->nextFrame();
        setNextChainJoints( HEAD_CHAIN,
                            currChoppedCommand->getNextJoints(HEAD_CHAIN) );
        setNextChainStiffnesses( Kinematics::HEAD_CHAIN,
                                 currChoppedCommand->getStiffness(
                                     Kinematics::HEAD_CHAIN) );

    }
    else {
        setNextChainJoints( HEAD_CHAIN, getCurrentHeads(sensorAngles) );
        setNextChainStiffnesses( Kinematics::HEAD_CHAIN,
                                 std::vector<float>(HEAD_JOINTS, 0.0f) );
    }
}

void HeadProvider::setCommand(const SetHeadCommand::ptr command)
{
    transitionTo(SET);
    yawDest = command->getYaw();
    pitchDest = command->getPitch();
    yawMaxSpeed = command->getMaxSpeedYaw();
    pitchMaxSpeed = command->getMaxSpeedPitch();

    /* ** *///debugging speed clipping (should probably stay in some form)
    yawMaxSpeed = NBMath::clip(yawMaxSpeed,
                               0,
                               Kinematics::jointsMaxVelNominal
                               [Kinematics::HEAD_YAW]*.1f);
    pitchMaxSpeed = NBMath::clip(pitchMaxSpeed,
                                 0,
                                 Kinematics::jointsMaxVelNominal
                                 [Kinematics::HEAD_PITCH]*.1f);

    setActive();

    currHeadCommand = command;
}

void HeadProvider::setCommand(const HeadJointCommand::ptr command)
{
    transitionTo(SCRIPTED);
    headCommandQueue.push(command);
    setActive();
}

void HeadProvider::enqueueSequence(std::vector<HeadJointCommand::ptr> &seq)
{
    // Take in vec of commands and enqueue them all
    std::vector<HeadJointCommand::ptr>::iterator i;
    for (i = seq.begin(); i != seq.end(); ++i)
        setCommand(*i);
}

void HeadProvider::setNextHeadCommand(std::vector<float>& sensorAngles)
{
    if ( !headCommandQueue.empty() ) {
        currChoppedCommand = chopper.chopCommand(
            headCommandQueue.front(),
            sensorAngles);
        headCommandQueue.pop();
    }
}

std::vector<float> HeadProvider::getCurrentHeads(std::vector<float>& sensorAngles)
{
    std::vector<float> currentHeads(HEAD_JOINTS);

    for(unsigned int i = 0; i < HEAD_JOINTS; i++)
    {
        currentHeads[i] = sensorAngles[i];
    }

    return currentHeads;
}

void HeadProvider::setActive()
{
    isDone() ? inactive() : active();
}


bool HeadProvider::isDone()
{
    bool setDone, scriptedDone;
    switch(curMode){
    case SET:
        setDone = ((yawDest == lastYawDest)
                   && (pitchDest == lastPitchDest));
        if (setDone && currHeadCommand) {
            currHeadCommand->finishedExecuting();
        }
        return setDone;
        break;
    case SCRIPTED:
        scriptedDone = (currChoppedCommand->isDone()
                        && headCommandQueue.empty());
        return scriptedDone;
        break;
    default:
        return true;
    }
}

void HeadProvider::stopScripted()
{
    while(!headCommandQueue.empty())
    {
        headCommandQueue.pop();
    }

    currChoppedCommand = ChoppedCommand::ptr(new ChoppedCommand());
}

void HeadProvider::stopSet()
{
    //set the target to our current location.
    yawDest = lastYawDest;
    pitchDest = lastPitchDest;
}

void HeadProvider::transitionTo(HeadMode newMode)
{
    if(newMode != curMode)
    {
        switch(newMode){
        case SCRIPTED:
            stopScripted();
            break;
        case SET:
            lastYawDest = latestJointAngles[0];
            lastPitchDest = latestJointAngles[1];
            break;
        }
        curMode = newMode;
    }
    else
    {
        // No transition required.
    }
}

} // namespace motion
} // namespace man
