
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


MotionSwitchboard::MotionSwitchboard(shared_ptr<Sensors> s)
    : sensors(s),
      walkProvider(sensors),
	  scriptedProvider(1/50.0f,sensors), // HOW SHOULD WE PASS FRAME_LENGTH??? HACK!
	  headProvider(1/50.0f,sensors),
	  curProvider(&scriptedProvider),
	  nextProvider(&scriptedProvider),
      curGait(NULL),
      nextGait(&DEFAULT_PARAMETERS),
      nextJoints(s->getBodyAngles()),
      nextStiffness(vector<float>(NUM_JOINTS,0.0f)),
	  running(false),
      newJoints(false),
      newStiffness(false),newStiffnessCommandSent(false),
      readyToSend(false),
      noWalkTransitionCommand(true)

{

    //Allow safe access to the next joints
    pthread_mutex_init(&next_joints_mutex, NULL);
    pthread_mutex_init(&calc_new_joints_mutex, NULL);
    pthread_mutex_init(&stiffness_mutex, NULL);
    pthread_cond_init(&calc_new_joints_cond,NULL);

#ifdef DEBUG_JOINTS_OUTPUT
    initDebugLogs();
#endif

    //temp
    scriptedProvider.setStiffness(0.0f);
    headProvider.setStiffness(0.0f);
    walkProvider.setStiffness(0.0f);

    //Very Important, ensure that we have selected a default walk parameter set
    boost::shared_ptr<GaitCommand>  defaultGait(new GaitCommand(DEFAULT_P));

    sendMotionCommand(defaultGait);

}

MotionSwitchboard::~MotionSwitchboard() {
    pthread_mutex_destroy(&next_joints_mutex);
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
 * This threaed then ``hangs'' until the enactor signals it has read the current
 * values. (This signaling is actually done in the getNextJoints method in
 * this class)
 *
 * Potential problems: If the processing for the next joints
 * takes too long, the enactor will send old joints.
 */
void MotionSwitchboard::run() {
    static int fcount = 0;

    //IMPORTANT Before anything else happens we need to put the correct
    //angles into sensors->motionBodyAngles:
    sensors->setMotionBodyAngles(sensors->getBodyAngles());
    cout << "Switchboard is here..." <<endl;
    pthread_mutex_lock(&calc_new_joints_mutex);
    pthread_cond_wait(&calc_new_joints_cond, &calc_new_joints_mutex);
    pthread_mutex_unlock(&calc_new_joints_mutex);

    while(running) {
        realityCheckJoints();
        processStiffness();
        bool active  = processProviders();


#ifdef DEBUG_JOINTS_OUTPUT
        if(active)
            updateDebugLogs();
#endif
        if(active)
            readyToSend = true;
        pthread_mutex_lock(&calc_new_joints_mutex);
        pthread_cond_wait(&calc_new_joints_cond, &calc_new_joints_mutex);
        pthread_mutex_unlock(&calc_new_joints_mutex);
        fcount++;

    }
    cout << "Switchboard run has exited" <<endl;
}

int MotionSwitchboard::processProviders(){


    //determine the curProvider, and do any necessary swapping
	if (curProvider != nextProvider && !curProvider->isActive()) {

        swapBodyProvider();


	}
	if (curProvider != nextProvider && !curProvider->isStopping()){
#ifdef DEBUG_SWITCHBOARD
		cout << "Requesting stop on "<< *curProvider <<endl;
#endif
        curProvider->requestStop();
    }
#ifdef DEBUG_SWITCHBOARD
    static bool switchedHeadToInactive = true;
#endif
    if (headProvider.isActive()) {
		// Calculate the next joints and get them
		headProvider.calculateNextJointsAndStiffnesses();
		// get headJoints from headProvider
		vector <float > headJoints = headProvider.getChainJoints(HEAD_CHAIN);

        for(unsigned int i = 0; i < HEAD_JOINTS;i++){
            nextJoints[HEAD_YAW + i] = headJoints.at(i);
        }
#ifdef DEBUG_SWITCHBOARD
        switchedHeadToInactive = false;
#endif

    }else{
#ifdef DEBUG_SWITCHBOARD
        if (!switchedHeadToInactive)
            cout << headProvider << " is inactive" <<endl;
        switchedHeadToInactive = true;
#endif
    }

#ifdef DEBUG_SWITCHBOARD
    static bool switchedToInactive;
#endif
    if (curProvider->isActive()){
		//Request new joints
		curProvider->calculateNextJointsAndStiffnesses();
		const vector <float > llegJoints = curProvider->getChainJoints(LLEG_CHAIN);
		const vector <float > rlegJoints = curProvider->getChainJoints(RLEG_CHAIN);
		const vector <float > rarmJoints = curProvider->getChainJoints(RARM_CHAIN);

		const vector <float > larmJoints = curProvider->getChainJoints(LARM_CHAIN);

		//Copy the new values into place, and wait to be signaled.
		pthread_mutex_lock(&next_joints_mutex);


        for(unsigned int i = 0; i < LEG_JOINTS; i ++){
            nextJoints[L_HIP_YAW_PITCH + i] = llegJoints.at(i);
        }
        for(unsigned int i = 0; i < LEG_JOINTS; i ++){
            nextJoints[R_HIP_YAW_PITCH + i] = rlegJoints.at(i);
        }
        for(unsigned int i = 0; i < ARM_JOINTS; i ++){
            nextJoints[L_SHOULDER_PITCH + i] = larmJoints.at(i);
        }
        for(unsigned int i = 0; i < ARM_JOINTS; i ++){
            nextJoints[R_SHOULDER_PITCH + i] = rarmJoints.at(i);
        }
        pthread_mutex_unlock(&next_joints_mutex);

#ifdef DEBUG_SWITCHBOARD
        switchedToInactive = false;
#endif

    }else{
#ifdef DEBUG_SWITCHBOARD
        if (!switchedToInactive)
            cout << *curProvider << " is inactive" <<endl;
        switchedToInactive = true;
#endif
    }
    newJoints = true;

    //Make sure that if the current provider just became inactive,
    //and we have the next provider ready, then we want to swap to ensure
    //that we never have an inactive provider when an active one is potentially
    //ready to take over:
	if (curProvider != nextProvider && !curProvider->isActive()) {
        swapBodyProvider();
	}

    // Update sensors with the correct support foot because it may have
    // changed this frame.
    // TODO: This can be improved by keeping a local copy of the SupportFoot
    //       so that we only update sensors when there has been a change.
    //       The overhead of the mutex shouldn't be that high though.
    sensors->setSupportFoot(curProvider->getSupportFoot());

    //return if one of the enactors 
    return curProvider->isActive() ||  headProvider.isActive();

}

/**
 * Method handles switching providers. Also handles any special action
 * required when switching between providers
 */
void MotionSwitchboard::swapBodyProvider(){
    std::vector<BodyJointCommand *> gaitSwitches;
    switch(nextProvider->getType()){
    case WALK_PROVIDER:
        //WARNING THIS COULD CAUSE INFINITE LOOP IF SWITCHBOAR IS BROKEN!
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
    case SCRIPTED_PROVIDER:
    case HEAD_PROVIDER:
    default:
        noWalkTransitionCommand = true;
        curProvider = nextProvider;
#ifdef DEBUG_SWITCHBOARD
        cout << "Switched to " << *curProvider << endl;
#endif
    }

}

const vector <float> MotionSwitchboard::getNextJoints() const {
    pthread_mutex_lock(&next_joints_mutex);
    if(!newJoints && readyToSend){
        cout << "An enactor is grabbing old joints from switchboard."
             <<" Must have missed a frame!" <<endl;
    }
    const vector <float> vec(nextJoints);
    newJoints = false;

    pthread_mutex_unlock(&next_joints_mutex);

    return vec;
}

const bool MotionSwitchboard::hasNewStiffness() const {
    pthread_mutex_lock(&stiffness_mutex);
    bool result(newStiffness);
    pthread_mutex_unlock(&stiffness_mutex);
    return result;
}

const vector<float>  MotionSwitchboard::getNextStiffness() const{
    pthread_mutex_lock(&stiffness_mutex);
    vector<float> result(nextStiffness);
    newStiffness = false;
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
    vector<float> sensorAngles = sensors->getBodyAngles();
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

    time += 0.05f;
}
#endif

void MotionSwitchboard::sendMotionCommand(const boost::shared_ptr<GaitCommand> command){
    //Don't request to switch providers when we get a gait command
    //nextProvider = &walkProvider;
    walkProvider.setCommand(command);
}
void MotionSwitchboard::sendMotionCommand(const WalkCommand *command){
    nextProvider = &walkProvider;
    walkProvider.setCommand(command);
}
void MotionSwitchboard::sendMotionCommand(const BodyJointCommand *command){
    nextProvider = &scriptedProvider;
    scriptedProvider.setCommand(command);
}
void MotionSwitchboard::sendMotionCommand(const SetHeadCommand * command){
    // headProvider is NEVER the nextProvider. NEVER.
    headProvider.setCommand(command);
}
void MotionSwitchboard::sendMotionCommand(const HeadJointCommand *command){
    // headProvider is NEVER the nextProvider. NEVER.
    headProvider.setCommand(command);
}

void MotionSwitchboard::sendMotionCommand(boost::shared_ptr<StiffnessCommand> command){
    pthread_mutex_lock(&stiffness_mutex);
    nextStiffnessCommand = command;
    newStiffnessCommandSent = true;
    pthread_mutex_unlock(&stiffness_mutex);

}

/**
 * Method to process remaining stiffness requests
 * Technically this could be handled by another provider, but there isn't
 * too much too it:
 */
int MotionSwitchboard::processStiffness(){
    int changed  = 0;
    if(!newStiffnessCommandSent)
        return changed;

    pthread_mutex_lock(&stiffness_mutex);
    //For each command, we look at each chain and see if there is anything
    //that needs to be done
    for(unsigned int i = HEAD_CHAIN; i <=RARM_CHAIN; i++){
        const vector <float> stiffnesses =
            nextStiffnessCommand->getChainStiffness((ChainID) i);

        //Skip empty chains
        if(stiffnesses.size() == 0){
            continue;
        }
        //Each chain has several joints we need to update
        //Ignore unchanged values or when the value is explicitly
        //set to NOT_SET
        for(unsigned int j = 0; j < chain_lengths[i]; j ++){
            if (nextStiffness[chain_first_joint[i] + j]
                != stiffnesses.at(j) &&
                stiffnesses.at(j) != StiffnessCommand::NOT_SET){
                nextStiffness[chain_first_joint[i] + j] =
                    stiffnesses.at(j);
                changed += 1; //flag when a value is changed
            }
        }

    }

    newStiffnessCommandSent = false;
    if(changed){
        newStiffness = true;
    }


    pthread_mutex_unlock(&stiffness_mutex);

    return changed;
}
