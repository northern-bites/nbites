
#include <vector>
#include <boost/shared_ptr.hpp>

#include "MotionSwitchboard.h"

using namespace std;
using namespace boost;

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
	  scriptedProvider(1/50.,sensors), // HOW SHOULD WE PASS FRAME_LENGTH??? HACK!
	  headProvider(1/50.0f,sensors),
	  curProvider(&scriptedProvider),
	  nextProvider(&scriptedProvider),
      curGait(NULL),
      nextGait(&DEFAULT_PARAMETERS),
      nextJoints(Kinematics::NUM_JOINTS, 0.0f),
      //nextJoints(sensors->getBodyAngles()),
	  running(false),
      newJoints(false)
{

    //Allow safe access to the next joints
    pthread_mutex_init(&next_joints_mutex, NULL);
    pthread_cond_init(&calc_new_joints_cond,NULL);

#ifdef DEBUG_JOINTS_OUTPUT
    initDebugLogs();
#endif

    //Very Important, ensure that we have selected a default walk parameter set
    sendMotionCommand(new GaitCommand(DEFAULT_P));

//     //build the sit down routine
//     vector<float> * sitDownPos = new vector<float>(sitDownAngles,
//                                                    sitDownAngles+NUM_JOINTS);
//     sitDown = new BodyJointCommand(4.0f,sitDownPos,Kinematics::INTERPOLATION_LINEAR);

//     //the getup routine waits for the walk engine to be inited
//     //Build the get up routine
//     vector<float> * initPos
//         = DEFAULT_PARAMETERS.getWalkStance();
// 	getUp = new BodyJointCommand(5.0f,
//                                  initPos,
//                                  Kinematics::INTERPOLATION_LINEAR);

// 	vector<float>* headJoints1 = new vector<float>(2,M_PI/2);
// 	hjc = new HeadJointCommand(2.0f,
// 							   headJoints1,
// 							   Kinematics::INTERPOLATION_LINEAR);

// 	vector<float>* headJoints2 = new vector<float>(2,0.0f);
// 	hjc2 = new HeadJointCommand(3.0f,
// 								headJoints2,
// 								Kinematics::INTERPOLATION_LINEAR);
// 	vector<float>* headJoints3 = new vector<float>(2,-M_PI/8);
// 	hjc3 = new HeadJointCommand(2.0f,
// 								headJoints3,
// 								Kinematics::INTERPOLATION_LINEAR);



// 	vector<float>* bodyJoints3 = new vector<float>(4,0.0f);//M_PI/4);
// 	command3 = new BodyJointCommand(2.0f, RARM_CHAIN,
// 									bodyJoints3,
// 									Kinematics::INTERPOLATION_LINEAR);

// 	bodyJoints = new vector<float>(20,0.0f);
// 	command = new BodyJointCommand(5.0f,
// 								   bodyJoints,
//                                    Kinematics::INTERPOLATION_LINEAR);

// 	bodyJoints2 = new vector<float>(20,-M_PI/6);
// 	command2 = new BodyJointCommand(5.0f,
// 									bodyJoints2,
// 									Kinematics::INTERPOLATION_LINEAR);

    //We cannot read the sensor values until the run method
    //so we must ensure that no one reads them until we get a chance
    //to initialize them correctly
    pthread_mutex_lock(&next_joints_mutex);
}

MotionSwitchboard::~MotionSwitchboard() {
    pthread_mutex_destroy(&next_joints_mutex);
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

// 	headProvider.enqueue(hjc);

	//nextProvider = reinterpret_cast <MotionProvider *>( &scriptedProvider);
	//sendMotionCommand(command3);
	//sendMotionCommand(hjc3);
// 	headProvider.enqueue(hjc3);
    //sendMotionCommand(getUp);

    running = true;

}


void MotionSwitchboard::stop() {
    running = false;
    //signal to end waiting in the run method,
    pthread_mutex_lock(&next_joints_mutex);
    pthread_cond_signal(&calc_new_joints_cond);
    pthread_mutex_unlock(&next_joints_mutex);
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

    //Initialize the next_joints to the sensors' angles:
    //Note that the mutex remains locked since the constructor,
    //until we set these angles here, since otherwise the
    //enactor can steal bad values before we have a chance to
    //correct them
    //Kind of a hack-ish
    nextJoints = sensors->getBodyAngles();
    pthread_cond_wait(&calc_new_joints_cond, &next_joints_mutex);
    pthread_mutex_unlock(&next_joints_mutex);


    while(running) {

        bool active  = processProviders();

#ifdef DEBUG_JOINTS_OUTPUT
        if(active)
            updateDebugLogs();
#endif
        pthread_mutex_lock(&next_joints_mutex);
        pthread_cond_wait(&calc_new_joints_cond, &next_joints_mutex);
        pthread_mutex_unlock(&next_joints_mutex);
        fcount++;

    }
}

int MotionSwitchboard::processProviders(){
    //determine the curProvider, and do any necessary swapping
	if (curProvider != nextProvider && !curProvider->isActive()) {

        swapBodyProvider();

#ifdef DEBUG_SWITCHBOARD
        cout << "Switched to " << *curProvider << endl;
#endif
	}
	if (curProvider != nextProvider && !curProvider->isStopping()){
#ifdef DEBUG_SWITCHBOARD
		cout << "Requesting stop on "<< *curProvider <<endl;
#endif
        curProvider->requestStop();
    }
    if (headProvider.isActive()) {
		// Calculate the next joints and get them
		headProvider.calculateNextJoints();
		// get headJoints from headProvider
		vector <float > headJoints = headProvider.getChainJoints(HEAD_CHAIN);

        for(unsigned int i = 0; i < HEAD_JOINTS;i++){
            nextJoints[HEAD_YAW + i] = headJoints.at(i);
        }
	}

#ifdef DEBUG_SWITCHBOARD
    static bool switchedToInactive;
#endif
    if (curProvider->isActive()){
		//Request new joints
		curProvider->calculateNextJoints();
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

    //return if one of the enactors 
    return curProvider->isActive() ||  headProvider.isActive();

}

/**
 * Method handles switching providers. Also handles any special action
 * required when switching between providers
 */
void MotionSwitchboard::swapBodyProvider(){
    BodyJointCommand * gaitSwitch = NULL;
    switch(nextProvider->getType()){
    case WALK_PROVIDER:
        //WARNING THIS COULD CAUSE INFINITE LOOP IF SWITCHBOAR IS BROKEN!
        //We need to ensure we are in the correct gait before walking
        gaitSwitch = walkProvider.getGaitTransitionCommand();
        if(gaitSwitch->getDuration() >= 0.02f){
            scriptedProvider.setCommand(gaitSwitch);
            curProvider = static_cast<MotionProvider * >(&scriptedProvider);
            break;
        }
    case SCRIPTED_PROVIDER:
    case HEAD_PROVIDER:
    default:
        curProvider = nextProvider;
    }

}

const vector <float> MotionSwitchboard::getNextJoints() {
    pthread_mutex_lock(&next_joints_mutex);
    if(!newJoints){
        cout << "An enactor is grabbing old joints from switchboard."
             <<" Must have missed a frame!" <<endl;
    }
    const vector <float> vec(nextJoints);
    newJoints =false;
    pthread_cond_signal(&calc_new_joints_cond);
    pthread_mutex_unlock(&next_joints_mutex);

    return vec;
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

void MotionSwitchboard::sendMotionCommand(const GaitCommand *command){
    nextProvider = &walkProvider;
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
void MotionSwitchboard::sendMotionCommand(const HeadJointCommand *command){
    // headProvider is NEVER the nextProvider. NEVER.
    headProvider.setCommand(command);
}
