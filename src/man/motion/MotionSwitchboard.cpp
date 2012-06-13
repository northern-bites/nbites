
#include <vector>
using namespace std;

#include <boost/shared_ptr.hpp>
using namespace boost;

#include "MotionSwitchboard.h"
#include "NBMatrixMath.h"
using namespace Kinematics;
using namespace NBMath;
#define DEBUG_SWITCHBOARD

MotionSwitchboard::MotionSwitchboard(boost::shared_ptr<Sensors> s,
                                     boost::shared_ptr<NaoPose> pose,
                                     MemoryMotion::ptr mMotion)
    : sensors(s),
      walkProvider(sensors, pose),
      scriptedProvider(sensors),
      headProvider(sensors, pose),
      nullHeadProvider(sensors),
      nullBodyProvider(sensors),
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
      newInputJoints(false),
      readyToSend(false),
      noWalkTransitionCommand(true),
      memoryProvider(&MotionSwitchboard::updateMemory, this, mMotion)
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
    if (!newInputJoints) {
        pthread_cond_wait(&calc_new_joints_cond, &calc_new_joints_mutex);
    }
    newInputJoints = false;
    pthread_mutex_unlock(&calc_new_joints_mutex);

    while(running) {
        PROF_ENTER(P_SWITCHBOARD);
        realityCheckJoints();

        preProcess();
        processJoints();
        processStiffness();
        bool active  = postProcess();

        memoryProvider.updateMemory();

        if(active)
        {
            readyToSend = true;
#ifdef DEBUG_JOINTS_OUTPUT
            updateDebugLogs();
#endif
        }

        pthread_mutex_lock(&calc_new_joints_mutex);
        if (!newInputJoints) {
            pthread_cond_wait(&calc_new_joints_cond, &calc_new_joints_mutex);
        }
        newInputJoints = false;
        pthread_mutex_unlock(&calc_new_joints_mutex);
        frameCount++;
        PROF_EXIT(P_SWITCHBOARD);
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
    pthread_mutex_lock(&stiffness_mutex);

    if(curHeadProvider->isActive()){
        const vector <float > headStiffnesses =
            curHeadProvider->getChainStiffnesses(HEAD_CHAIN);

        for(unsigned int i = 0; i < HEAD_JOINTS; i ++){
            nextStiffnesses[HEAD_YAW + i] = headStiffnesses.at(i);
        }
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

        for(unsigned int i = 0; i < LEG_JOINTS; i ++){
            nextStiffnesses[L_HIP_YAW_PITCH + i] = llegStiffnesses.at(i);
            nextStiffnesses[R_HIP_YAW_PITCH + i] = rlegStiffnesses.at(i);
        }

        for(unsigned int i = 0; i < ARM_JOINTS; i ++){
            nextStiffnesses[L_SHOULDER_PITCH + i] = larmStiffnesses.at(i);
            nextStiffnesses[R_SHOULDER_PITCH + i] = rarmStiffnesses.at(i);
        }
    }

    vector<float>::iterator i = nextStiffnesses.begin();
    for (; i != nextStiffnesses.end(); ++i) {
        if (*i < MotionConstants::MIN_STIFFNESS){
            *i = MotionConstants::NO_STIFFNESS;
        } else {
            *i = NBMath::clip(*i,
                              MotionConstants::MIN_STIFFNESS,
                              MotionConstants::MAX_STIFFNESS);
        }
    }
    pthread_mutex_unlock(&stiffness_mutex);
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
    }

    if (curHeadProvider != nextHeadProvider)
    {
        if (!curHeadProvider->isStopping()) {
            #ifdef DEBUG_SWITCHBOARD
            cout << "Requesting stop on "<< *curHeadProvider <<endl;
            #endif
            curHeadProvider->requestStop();
        }

        if (!curHeadProvider->isActive()) {
            swapHeadProvider();
        }
    }
}

void MotionSwitchboard::preProcessBody()
{
    if (curProvider != &nullBodyProvider &&
        nextProvider == &nullBodyProvider)
    {
            #ifdef DEBUG_SWITCHBOARD
            cout << "Hard reset on "<< *curProvider <<endl;
            #endif
        scriptedProvider.hardReset();
        walkProvider.hardReset();
    }

    //determine the curProvider, and do any necessary swapping
    if (curProvider != nextProvider)
    {
        if (!curProvider->isStopping()) {
            #ifdef DEBUG_SWITCHBOARD
            cout << "Requesting stop on "<< *curProvider <<endl;
            #endif
            curProvider->requestStop();
        }

        if (!curProvider->isActive()) {
            swapBodyProvider();
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
                #ifdef DEBUG_SWITCHBOARD
                cout << "Switched to " << *curProvider
                     << " to transition to " << *nextProvider << endl;
                #endif
                break;
            }
        }
        curProvider = nextProvider;
        #ifdef DEBUG_SWITCHBOARD
            cout << "Switched to " << *curProvider << " from "<< old_provider << endl;
        #endif
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
    if(!newJoints && readyToSend){
        cout << "An enactor is grabbing old joints from switchboard."
             <<" Must have missed a frame!" <<endl;
    }
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
    newInputJoints = true;
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

void MotionSwitchboard::sendMotionCommand(const Gait::ptr command){
    //Don't request to switch providers when we get a gait command
    walkProvider.setCommand(command);
}
void MotionSwitchboard::sendMotionCommand(const WalkCommand::ptr command){
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
void MotionSwitchboard::sendMotionCommand(const CoordHeadCommand::ptr command){
    pthread_mutex_lock(&next_provider_mutex);
    nextHeadProvider = &headProvider;
    headProvider.setCommand(command);
    pthread_mutex_unlock(&next_provider_mutex);

}
void MotionSwitchboard::sendMotionCommand(const HeadJointCommand::ptr command){
    pthread_mutex_lock(&next_provider_mutex);
    nextHeadProvider = &headProvider;
    headProvider.setCommand(command);
    pthread_mutex_unlock(&next_provider_mutex);

}
void MotionSwitchboard::sendMotionCommand(const FreezeCommand::ptr command){
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
void MotionSwitchboard::sendMotionCommand(const UnfreezeCommand::ptr command){
    pthread_mutex_lock(&next_provider_mutex);
    if(curHeadProvider == &nullHeadProvider){
        nullHeadProvider.setCommand(command);
    }
    if(curProvider == &nullBodyProvider){
        nullBodyProvider.setCommand(command);
    }
    pthread_mutex_unlock(&next_provider_mutex);
}

void MotionSwitchboard::sendMotionCommand(const StepCommand::ptr command){
    pthread_mutex_lock(&next_provider_mutex);
    nextProvider = &walkProvider;
    walkProvider.setCommand(command);
    pthread_mutex_unlock(&next_provider_mutex);
}

void MotionSwitchboard::sendMotionCommand(const DestinationCommand::ptr command){
    pthread_mutex_lock(&next_provider_mutex);
    nextProvider = &walkProvider;
    walkProvider.setCommand(command);
    pthread_mutex_unlock(&next_provider_mutex);
}

void MotionSwitchboard::updateMemory(MemoryMotion::ptr mMotion) const {

    proto::Motion* proto_motion = mMotion->get();

    proto_motion->set_current_body_provider(curProvider->getName());
    proto_motion->set_next_body_provider(nextProvider->getName());

    proto::RobotLocation* odometry = proto_motion->mutable_odometry();

    MotionModel motionOdometry = getOdometryUpdate();
    odometry->set_h(motionOdometry.theta);
    odometry->set_x(motionOdometry.x);
    odometry->set_y(motionOdometry.y);

    walkProvider.update(proto_motion->mutable_walk_provider());

    proto::ScriptedProvider* scripted_provider = proto_motion->mutable_scripted_provider();
    scripted_provider->set_active(scriptedProvider.isActive());
    scripted_provider->set_stopping(scriptedProvider.isStopping());
}

//vector<float> MotionSwitchboard::getBodyJointsFromProvider(MotionProvider* provider) {
//
//    vector<float> joints(NUM_JOINTS, 0);
//
//    const vector <float > llegJoints = provider->getChainJoints(LLEG_CHAIN);
//    const vector <float > rlegJoints = provider->getChainJoints(RLEG_CHAIN);
//    const vector <float > rarmJoints = provider->getChainJoints(RARM_CHAIN);
//    const vector <float > larmJoints = provider->getChainJoints(LARM_CHAIN);
//
//    for(unsigned int i = 0; i < LEG_JOINTS; i ++)
//    {
//        joints[R_HIP_YAW_PITCH + i] = rlegJoints.at(i);
//        joints[L_HIP_YAW_PITCH + i] = llegJoints.at(i);
//    }
//
//    for(unsigned int i = 0; i < ARM_JOINTS; i ++)
//    {
//        joints[L_SHOULDER_PITCH + i] = larmJoints.at(i);
//        joints[R_SHOULDER_PITCH + i] = rarmJoints.at(i);
//    }
//
//    return joints;
//}

vector<BodyJointCommand::ptr> MotionSwitchboard::generateNextBodyProviderTransitions() {

    vector<BodyJointCommand::ptr> commands;

    vector<float> providerJoints = nextProvider->getInitialStance();
    vector<float> curJoints = sensors->getMotionBodyAngles();

    if (providerJoints.size() == 0)
        return commands;

    float max_change = -M_PI_FLOAT*10.0f;

    //ignore the first chain since it's the head one
    for (unsigned i = 0; i < Kinematics::NUM_BODY_JOINTS; i++) {
        max_change = max(max_change, fabs(curJoints[i + Kinematics::HEAD_JOINTS] - providerJoints[i]));
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

    vector<float>safe_larm(larm_angles, &larm_angles[ARM_JOINTS]);
    vector<float> safe_rarm(rarm_angles, &rarm_angles[ARM_JOINTS]);

    // HACK @joho get gait stiffness params. nextGait.maxStiffness
    vector<float> stiffness(Kinematics::NUM_JOINTS, 0.75f);
    vector<float> stiffness2(Kinematics::NUM_JOINTS, 0.75f);

    vector<float> empty(0);
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
