
#include <vector>
using namespace std;

#include <boost/shared_ptr.hpp>
using namespace boost;

#include "MotionSwitchboard.h"
#include "NBMatrixMath.h"
using namespace Kinematics;
using namespace NBMath;
//#define DEBUG_SWITCHBOARD

const float MotionSwitchboard::sitDownAngles[NUM_BODY_JOINTS] =
{1.57f,0.0f,-1.13f,-1.0f,
 0.0f,0.0f,-0.96f,2.18f,
 -1.22f,0.0f,0.0f,0.0f,
 -0.96f,2.18f,-1.22f,0.0f,
 1.57f,0.0f,1.13f,1.01f};


MotionSwitchboard::MotionSwitchboard(shared_ptr<Sensors> s,
                                     shared_ptr<Profiler> p,
                                     shared_ptr<NaoPose> pose)
    : sensors(s),
      profiler(p),
      walkProvider(sensors, pose, p),
      scriptedProvider(sensors, p),
      headProvider(sensors, p),
      coordHeadProvider(sensors, p, pose),
      nullHeadProvider(sensors, p),
      nullBodyProvider(sensors, p),
      curProvider(&nullBodyProvider),
      nextProvider(&nullBodyProvider),
      curHeadProvider(&nullHeadProvider),
      nextHeadProvider(&nullHeadProvider),
      sensorAngles(s->getBodyAngles()),
      nextJoints(sensorAngles),
      nextStiffnesses(vector<float>(NUM_JOINTS,0.0f)),
      lastJoints(sensorAngles),
      running(false),
      newJoints(false),
      readyToSend(false),
      noWalkTransitionCommand(true)
{

    //Allow safe access to the next joints
    pthread_mutex_init(&next_joints_mutex, NULL);
    pthread_mutex_init(&next_provider_mutex, NULL);
    pthread_mutex_init(&calc_new_joints_mutex, NULL);
    pthread_mutex_init(&stiffness_mutex, NULL);
    pthread_cond_init(&calc_new_joints_cond,NULL);

#ifdef DEBUG_JOINTS_OUTPUT
    initDebugLogs();
#endif

    boost::shared_ptr<FreezeCommand> paralyze
        = boost::shared_ptr<FreezeCommand>(new FreezeCommand());

    nullBodyProvider.setCommand(paralyze);
    nullHeadProvider.setCommand(paralyze);

    //Very Important, ensure that we have selected a default walk parameter set
    boost::shared_ptr<Gait> defaultGait(new Gait(DEFAULT_GAIT));

    sendMotionCommand(defaultGait);
}

MotionSwitchboard::~MotionSwitchboard() {
    pthread_mutex_destroy(&next_joints_mutex);
    pthread_mutex_destroy(&next_provider_mutex);
    pthread_mutex_destroy(&calc_new_joints_mutex);
    pthread_mutex_destroy(&stiffness_mutex);
    pthread_cond_destroy(&calc_new_joints_cond);

#ifdef DEBUG_JOINTS_OUTPUT
    closeDebugLogs();
#endif
}


void MotionSwitchboard::start() {

#ifdef DEBUG_INITIALIZATION
    cout << "Switchboard::initializing" << endl;
    cout << "  creating threads" << endl;
#endif
    fflush(stdout);

    running = true;
}


void MotionSwitchboard::stop() {
    running = false;
    cout << "Switchboard signaled to stop" <<endl;
    //signal to end waiting in the run method,
    pthread_mutex_lock(&calc_new_joints_mutex);
    pthread_cond_signal(&calc_new_joints_cond);
    pthread_mutex_unlock(&calc_new_joints_mutex);
}


/**
 * The switchboard run method is continuously looping. At each iteration
 * it grabs the appropriate joints from the designated provider, and
 * then copies them into place so an enactor can send them to the low level.
 * This threaed then 'hangs' until the enactor signals it has read the current
 * values. (This signaling is actually done in the getNextJoints method in
 * this class)
 *
 * Potential problems: If the processing for the next joints
 * takes too long, the enactor will send old joints.
 */
void MotionSwitchboard::run() {
    frameCount = 0;

    //IMPORTANT Before anything else happens we need to put the correct
    //angles into sensors->motionBodyAngles:
    sensors->setMotionBodyAngles(sensors->getBodyAngles());

    pthread_mutex_lock(&calc_new_joints_mutex);
    pthread_cond_wait(&calc_new_joints_cond, &calc_new_joints_mutex);
    pthread_mutex_unlock(&calc_new_joints_mutex);

    while(running) {
        PROF_ENTER(profiler, P_SWITCHBOARD);
        realityCheckJoints();

        preProcess();
        processJoints();
        processStiffness();
        bool active  = postProcess();
        PROF_EXIT(profiler, P_SWITCHBOARD);

        if(active)
        {
            readyToSend = true;
#ifdef DEBUG_JOINTS_OUTPUT
            updateDebugLogs();
#endif
        }

        pthread_mutex_lock(&calc_new_joints_mutex);
        pthread_cond_wait(&calc_new_joints_cond, &calc_new_joints_mutex);
        pthread_mutex_unlock(&calc_new_joints_mutex);
        frameCount++;
    }
    cout << "Switchboard run has exited" <<endl;
}

void MotionSwitchboard::preProcess()
{
    pthread_mutex_lock(&next_provider_mutex);

    preProcessHead();
    preProcessBody();

    pthread_mutex_unlock(&next_provider_mutex);
}

void MotionSwitchboard::processJoints()
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
void MotionSwitchboard::processStiffness(){
    try{
        if(curHeadProvider->isActive()){
            const vector <float > headStiffnesses =
                curHeadProvider->getChainStiffnesses(HEAD_CHAIN);

            pthread_mutex_lock(&stiffness_mutex);

            for(unsigned int i = 0; i < HEAD_JOINTS; i ++){
                nextStiffnesses[HEAD_YAW + i] = headStiffnesses.at(i);
            }

            pthread_mutex_unlock(&stiffness_mutex);
        }

        if(curProvider->isActive()){
            const vector <float > llegStiffnesses =
                curProvider->getChainStiffnesses(LLEG_CHAIN);

            const vector <float > rlegStiffnesses =
                curProvider->getChainStiffnesses(RLEG_CHAIN);

            const vector <float > rarmStiffnesses =
                curProvider->getChainStiffnesses(RARM_CHAIN);

            const vector <float > larmStiffnesses =
                curProvider->getChainStiffnesses(LARM_CHAIN);

            pthread_mutex_lock(&stiffness_mutex);

            for(unsigned int i = 0; i < LEG_JOINTS; i ++){
                nextStiffnesses[L_HIP_YAW_PITCH + i] = llegStiffnesses.at(i);
                nextStiffnesses[R_HIP_YAW_PITCH + i] = rlegStiffnesses.at(i);
            }

            for(unsigned int i = 0; i < ARM_JOINTS; i ++){
                nextStiffnesses[L_SHOULDER_PITCH + i] = larmStiffnesses.at(i);
                nextStiffnesses[R_SHOULDER_PITCH + i] = rarmStiffnesses.at(i);
            }

            pthread_mutex_unlock(&stiffness_mutex);
        }
    }

    catch(std::out_of_range & e){
        cout << "Out of range exception caught in processStiffness"<< e.what()<<endl;
        exit(0);
    }
}


bool MotionSwitchboard::postProcess(){
    pthread_mutex_lock(&next_provider_mutex);

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
    sensors->setSupportFoot(curProvider->getSupportFoot());

    pthread_mutex_unlock(&next_provider_mutex);

    //return if one of the enactors is active
    return curProvider->isActive() || curHeadProvider->isActive();
}


void MotionSwitchboard::processHeadJoints()
{
#ifdef DEBUG_SWITCHBOARD
    static bool switchedHeadToInactive = true;
#endif

    if (curHeadProvider->isActive())
    {
        // Calculate the next joints and get them
        curHeadProvider->calculateNextJointsAndStiffnesses();

        // get headJoints from headProvider
        vector <float > headJoints = curHeadProvider->getChainJoints(HEAD_CHAIN);

        clipHeadJoints(headJoints);

        for(unsigned int i = FIRST_HEAD_JOINT;
            i < FIRST_HEAD_JOINT + HEAD_JOINTS; i++)
        {
            nextJoints[i] = headJoints.at(i);
        }

#ifdef DEBUG_SWITCHBOARD
        switchedHeadToInactive = false;
#endif
    }

#ifdef DEBUG_SWITCHBOARD
    else
    {
        if (!switchedHeadToInactive)
        {
            cout << *curHeadProvider << " is inactive" <<endl;
        }

        switchedHeadToInactive = true;
    }
#endif
}

void MotionSwitchboard::processBodyJoints()
{
#ifdef DEBUG_SWITCHBOARD
    static bool switchedToInactive;
#endif
    if (curProvider->isActive())
    {
        //Request new joints
        curProvider->calculateNextJointsAndStiffnesses();
        const vector <float > llegJoints = curProvider->getChainJoints(LLEG_CHAIN);
        const vector <float > rlegJoints = curProvider->getChainJoints(RLEG_CHAIN);
        const vector <float > rarmJoints = curProvider->getChainJoints(RARM_CHAIN);

        const vector <float > larmJoints = curProvider->getChainJoints(LARM_CHAIN);

        //Copy the new values into place, and wait to be signaled.
        pthread_mutex_lock(&next_joints_mutex);

        for(unsigned int i = 0; i < LEG_JOINTS; i ++)
        {
            nextJoints[R_HIP_YAW_PITCH + i] = rlegJoints.at(i);
            nextJoints[L_HIP_YAW_PITCH + i] = llegJoints.at(i);
        }

        for(unsigned int i = 0; i < ARM_JOINTS; i ++)
        {
            nextJoints[L_SHOULDER_PITCH + i] = larmJoints.at(i);
            nextJoints[R_SHOULDER_PITCH + i] = rarmJoints.at(i);
        }

        pthread_mutex_unlock(&next_joints_mutex);

#ifdef DEBUG_SWITCHBOARD
        switchedToInactive = false;
#endif
    }

#ifdef DEBUG_SWITCHBOARD
    else
    {
        if (!switchedToInactive)
        {
            cout << *curProvider << " is inactive" <<endl;
        }

        switchedToInactive = true;
    }
#endif
}

void MotionSwitchboard::preProcessHead()
{
    if (curHeadProvider != &nullHeadProvider &&
        nextHeadProvider == &nullHeadProvider)
    {
        headProvider.hardReset();
        coordHeadProvider.hardReset();
    }

    if (curHeadProvider != nextHeadProvider)
    {
        if (!curHeadProvider->isActive())
        {
            swapHeadProvider();
        }

        if (!curHeadProvider->isStopping())
        {
#ifdef DEBUG_SWITCHBOARD
            cout << "Requesting stop on "<< *curHeadProvider <<endl;
#endif
            curHeadProvider->requestStop();
        }
    }
}

void MotionSwitchboard::preProcessBody()
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
        if (!curProvider->isActive())
        {
            swapBodyProvider();
        }

        if (!curProvider->isStopping()){
#ifdef DEBUG_SWITCHBOARD
            cout << "Requesting stop on "<< *curProvider <<endl;
#endif
            curProvider->requestStop();
        }
    }
}

void MotionSwitchboard::clipHeadJoints(vector<float>& joints)
{
    float yaw = fabs(joints[HEAD_YAW]);
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
        if (pitch > -0.2f)
        {
            pitch = -0.2f;
        }
    }

    else if (yaw >= 1.57f)
    {
        if (pitch > -0.3f)
        {
            pitch = -0.3f;
        }
    }

    joints[HEAD_PITCH] = pitch;
}

void MotionSwitchboard::safetyCheckJoints()
{
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
void MotionSwitchboard::swapBodyProvider(){
    std::vector<BodyJointCommand::ptr> gaitSwitches;
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
            gaitSwitches = walkProvider.getGaitTransitionCommand();
            if(gaitSwitches.size() >= 1){
                for(unsigned int i = 0; i< gaitSwitches.size(); i++){
                    scriptedProvider.setCommand(gaitSwitches[i]);
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
#ifdef DEBUG_SWITCHBOARD
    cout << "Switched to " << *curProvider << " from "<<old_provider<< endl;
#endif
}

void MotionSwitchboard::swapHeadProvider(){
    switch(nextHeadProvider->getType())
    {
    case HEAD_PROVIDER:
    default:
        curHeadProvider = nextHeadProvider;
    }
}

const vector <float> MotionSwitchboard::getNextJoints() const {
    pthread_mutex_lock(&next_joints_mutex);
#ifndef WEBOTS_BACKEND
#ifdef DEBUG_SWITCHBOARD
    if(!newJoints && readyToSend){
        cout << "An enactor is grabbing old joints from switchboard."
             <<" Must have missed a frame!" <<endl;
    }
#endif
#endif
    const vector <float> vec(nextJoints);
    newJoints = false;

    pthread_mutex_unlock(&next_joints_mutex);

    return vec;
}

const vector<float>  MotionSwitchboard::getNextStiffness() const{
    pthread_mutex_lock(&stiffness_mutex);
    vector<float> result(nextStiffnesses);
    pthread_mutex_unlock(&stiffness_mutex);
    return result;
}

void MotionSwitchboard::signalNextFrame(){
    pthread_mutex_lock(&calc_new_joints_mutex);
    pthread_cond_signal(&calc_new_joints_cond);
    pthread_mutex_unlock(&calc_new_joints_mutex);

}


/**
 * Checks to ensure that the current MotionBodyAngles are close enough to
 * what the sensors are reporting. If they are very different,
 * then the bad value is replaced
 */
int MotionSwitchboard::realityCheckJoints(){
    static const float joint_override_thresh = 0.12f;//radians
    static const float head_joint_override_thresh = 0.3f;//need diff for head

    int changed = 0;
    sensorAngles = sensors->getBodyAngles();
    vector<float> motionAngles = sensors->getMotionBodyAngles();

    //HEAD ANGLES - handled separately to avoid trouble in HeadProvider
    for(unsigned int i = 0; i < HEAD_JOINTS; i++){
        if (fabs(sensorAngles[i] - motionAngles[i]) >
            head_joint_override_thresh){
#ifdef DEBUG_SWITCHBOARD_DISCREPANCIES
            cout << "RealityCheck discrepancy: "<<endl
                 << "    Joint "<<i << " is off from sensors by"<<sensorAngles[i] - motionAngles[i]<<endl;
#endif
            nextJoints[i] = motionAngles[i] = sensorAngles[i];
            changed += 1;
        }
    }

    //BODY ANGLES
    for(unsigned int i = HEAD_JOINTS -1; i < NUM_JOINTS; i++){
        if (fabs(sensorAngles[i] - motionAngles[i]) > joint_override_thresh){
#ifdef DEBUG_SWITCHBOARD_DISCREPANCIES
            cout << "RealityCheck discrepancy: "<<endl
                 << "    Joint "<<i << " is off from sensors by"<<sensorAngles[i] - motionAngles[i]<<endl;
#endif
            nextJoints[i] = motionAngles[i] = sensorAngles[i];
            changed += 1;
        }
    }
    if(changed != 0)
        sensors->setMotionBodyAngles(motionAngles);
    return changed;
}

#ifdef DEBUG_JOINTS_OUTPUT
void MotionSwitchboard::initDebugLogs(){
    joints_log = fopen("/tmp/joints_log.xls","w");
    fprintf(joints_log,"time\t"
            "HEAD_YAW\t"
            "HEAD_PITCH\t"
            "L_SHOULDER_PITCH\t"
            "L_SHOULDER_ROLL\t"
            "L_ELBOW_YAW\t"
            "L_ELBOW_ROLL\t"
            "L_HIP_YAW_PITCH\t"
            "L_HIP_ROLL\t"
            "L_HIP_PITCH\t"
            "L_KNEE_PITCH\t"
            "L_ANKLE_PITCH\t"
            "L_ANKLE_ROLL\t"
            "R_HIP_YAW_PITCH\t"
            "R_HIP_ROLL\t"
            "R_HIP_PITCH\t"
            "R_KNEE_PITCH\t"
            "R_ANKLE_PITCH\t"
            "R_ANKLE_ROLL\t"
            "R_SHOULDER_PITCH\t"
            "R_SHOULDER_ROLL\t"
            "R_ELBOW_YAW\t"
            "R_ELBOW_ROLL\t\n");

    stiffness_log = fopen("/tmp/stiff_log.xls","w");
    fprintf(stiffness_log,"time\t"
            "HEAD_YAW\t"
            "HEAD_PITCH\t"
            "L_SHOULDER_PITCH\t"
            "L_SHOULDER_ROLL\t"
            "L_ELBOW_YAW\t"
            "L_ELBOW_ROLL\t"
            "L_HIP_YAW_PITCH\t"
            "L_HIP_ROLL\t"
            "L_HIP_PITCH\t"
            "L_KNEE_PITCH\t"
            "L_ANKLE_PITCH\t"
            "L_ANKLE_ROLL\t"
            "R_HIP_YAW_PITCH\t"
            "R_HIP_ROLL\t"
            "R_HIP_PITCH\t"
            "R_KNEE_PITCH\t"
            "R_ANKLE_PITCH\t"
            "R_ANKLE_ROLL\t"
            "R_SHOULDER_PITCH\t"
            "R_SHOULDER_ROLL\t"
            "R_ELBOW_YAW\t"
            "R_ELBOW_ROLL\t\n");

    effector_log = fopen("/tmp/effector_log.xls","w");
    fprintf(effector_log,"time\t"
            "HEAD_CHAIN_X\t"
            "HEAD_CHAIN_Y\t"
            "HEAD_CHAIN_Z\t"
            "LARM_CHAIN_X\t"
            "LARM_CHAIN_Y\t"
            "LARM_CHAIN_Z\t"
            "LLEG_CHAIN_X\t"
            "LLEG_CHAIN_Y\t"
            "LLEG_CHAIN_Z\t"
            "RLEG_CHAIN_X\t"
            "RLEG_CHAIN_Y\t"
            "RLEG_CHAIN_Z\t"
            "RARM_CHAIN_X\t"
            "RARM_CHAIN_Y\t"
            "RARM_CHAIN_Z\t\n"
        );
}
void MotionSwitchboard::closeDebugLogs(){
    fclose(joints_log);
    fclose(stiffness_log);
    fclose(effector_log);
}
void MotionSwitchboard::updateDebugLogs(){
    static float time = 0.0f;

    pthread_mutex_lock(&next_joints_mutex);
    //print joints:
    fprintf(joints_log, "%f\t",time);
    for(unsigned int i = 0; i < NUM_JOINTS; i++)
        fprintf(joints_log, "%f\t",nextJoints[i]);
    fprintf(joints_log, "\n");

    //known bug TODO: joint order is still reverse in Kinematics!!

    fprintf(effector_log, "%f\t",time);
    int index  =0;
    for(int chain = HEAD_CHAIN; chain <= RARM_CHAIN; chain++){
        ufvector3 dest = Kinematics::forwardKinematics((ChainID)chain,
                                                       &nextJoints[index]);
        fprintf(effector_log,"%f\t%f\t%f\t",dest(0),dest(1),dest(2));
        index += chain_lengths[chain];
    }
    fprintf(effector_log,"\n");
    pthread_mutex_unlock(&next_joints_mutex);

    //Log the stiffnesses as well
    pthread_mutex_lock(&stiffness_mutex);
    fprintf(stiffness_log, "%f\t",time);
    for(unsigned int i = 0; i < NUM_JOINTS; i++)
        fprintf(stiffness_log, "%f\t",nextStiffnesses[i]);
    fprintf(stiffness_log, "\n");
    pthread_mutex_unlock(&stiffness_mutex);


    time += 0.05f;
}
#endif

void MotionSwitchboard::sendMotionCommand(const boost::shared_ptr<Gait> command){
    //Don't request to switch providers when we get a gait command
    walkProvider.setCommand(command);
}
void MotionSwitchboard::sendMotionCommand(const WalkCommand *command){
    pthread_mutex_lock(&next_provider_mutex);
    nextProvider = &walkProvider;
    walkProvider.setCommand(command);
    pthread_mutex_unlock(&next_provider_mutex);

}
void MotionSwitchboard::sendMotionCommand(const BodyJointCommand::ptr command){
    pthread_mutex_lock(&next_provider_mutex);
    noWalkTransitionCommand = true;
    nextProvider = &scriptedProvider;
    scriptedProvider.setCommand(command);
    pthread_mutex_unlock(&next_provider_mutex);

}
void MotionSwitchboard::sendMotionCommand(const SetHeadCommand::ptr command){
    pthread_mutex_lock(&next_provider_mutex);
    nextHeadProvider = &headProvider;
    headProvider.setCommand(command);
    pthread_mutex_unlock(&next_provider_mutex);

}
void MotionSwitchboard::sendMotionCommand(const CoordHeadCommand * command){
    pthread_mutex_lock(&next_provider_mutex);
    nextHeadProvider = &coordHeadProvider;
    coordHeadProvider.setCommand(command);
    pthread_mutex_unlock(&next_provider_mutex);

}
void MotionSwitchboard::sendMotionCommand(const HeadJointCommand::ptr command){
    pthread_mutex_lock(&next_provider_mutex);
    nextHeadProvider = &headProvider;
    headProvider.setCommand(command);
    pthread_mutex_unlock(&next_provider_mutex);

}
void
MotionSwitchboard::sendMotionCommand(const shared_ptr<FreezeCommand> command){
    pthread_mutex_lock(&next_provider_mutex);
    nextProvider = &nullBodyProvider;
    nextHeadProvider = &nullHeadProvider;

    nullHeadProvider.setCommand(command);
    nullBodyProvider.setCommand(command);
    pthread_mutex_unlock(&next_provider_mutex);

#ifdef DEBUG_SWITCHBOARD
    cout << "Switched to " << *curProvider << endl;
#endif

}
void
MotionSwitchboard::sendMotionCommand(const shared_ptr<UnfreezeCommand> command){
    pthread_mutex_lock(&next_provider_mutex);
    if(curHeadProvider == &nullHeadProvider){
        nullHeadProvider.setCommand(command);
    }
    if(curProvider == &nullBodyProvider){
        nullBodyProvider.setCommand(command);
    }
    pthread_mutex_unlock(&next_provider_mutex);
}

void
MotionSwitchboard::sendMotionCommand(const shared_ptr<StepCommand> command){
    pthread_mutex_lock(&next_provider_mutex);
    nextProvider = &walkProvider;
    walkProvider.setCommand(command);
    pthread_mutex_unlock(&next_provider_mutex);
}

