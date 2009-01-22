#include "MotionSwitchboard.h"
MotionSwitchboard::MotionSwitchboard(Sensors *s)
    : sensors(s),
      walkProvider(),
	  scriptedProvider(1/50.,sensors), // HOW SHOULD WE PASS FRAME_LENGTH??? HACK!
	  headProvider(1/50.0f,sensors),
      nextJoints(Kinematics::NUM_JOINTS, 0.0f),
	  running(false)
{

    //Allow safe access to the next joints
    pthread_mutex_init(&next_joints_mutex, NULL);
    pthread_cond_init(&calc_new_joints_cond,NULL);

	vector<float>* headJoints1 = new vector<float>(2,90.0f);
	hjc = new HeadJointCommand(2.0f,
							   headJoints1,
							   Kinematics::INTERPOLATION_LINEAR);

	vector<float>* headJoints2 = new vector<float>(2,0.0f);
	hjc2 = new HeadJointCommand(3.0f,
								headJoints2,
								Kinematics::INTERPOLATION_LINEAR);
	vector<float>* headJoints3 = new vector<float>(2,-90.0f);
	hjc3 = new HeadJointCommand(10.0f,
								headJoints3,
								Kinematics::INTERPOLATION_LINEAR);



	vector<float>* bodyJoints3 = new vector<float>(4,45.0f);
	command3 = new BodyJointCommand(5.0f, LARM_CHAIN,
									bodyJoints3,
									Kinematics::INTERPOLATION_LINEAR);

	bodyJoints = new vector<float>(20,30.0f);
	command = new BodyJointCommand(10.0f,
								   bodyJoints,
							   Kinematics::INTERPOLATION_LINEAR);

	bodyJoints2 = new vector<float>(20,-30.0f);
	command2 = new BodyJointCommand(10.0f,
									bodyJoints2,
									Kinematics::INTERPOLATION_LINEAR);

}

MotionSwitchboard::~MotionSwitchboard() {
    pthread_mutex_destroy(&next_joints_mutex);
}


void MotionSwitchboard::start() {

#ifdef DEBUG_INITIALIZATION
    cout << "Switchboard::initializing" << endl;
    cout << "  creating threads" << endl;
#endif
    fflush(stdout);

	headProvider.enqueue(hjc);
	scriptedProvider.enqueue(command);
	scriptedProvider.enqueue(command2);
	scriptedProvider.enqueue(command3);
	headProvider.enqueue(hjc2);
	headProvider.enqueue(hjc3);

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
    while(running) {

        if(fcount == 1){
            //hack to help keep from falling over in the simulator
            usleep(2*1000*1000);
        }

        // Calculate the next joints and get them

		// get headJoints from headProvider
		vector <float > headJoints = headProvider.getChainJoints(HEAD_CHAIN);
        //Just switch this line to decide which provider we should use
        MotionProvider * curProvider =
            reinterpret_cast<MotionProvider *>( &walkProvider);
        //dynamic_cast <MotionProvider *>( &scriptedProvider);
        curProvider->calculateNextJoints();

        vector <float > llegJoints = curProvider->getChainJoints(LLEG_CHAIN);
        vector <float > rlegJoints = curProvider->getChainJoints(RLEG_CHAIN);
		vector <float > rarmJoints = curProvider->getChainJoints(RARM_CHAIN);
		vector <float > larmJoints= curProvider->getChainJoints(LARM_CHAIN);

        //Copy the new values into place, and wait to be signaled.
        pthread_mutex_lock(&next_joints_mutex);

		for(unsigned int i = 0; i < HEAD_JOINTS;i++){
			nextJoints[HEAD_YAW + i] = headJoints.at(i);
		}
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

        sensors->setBodyAngles(nextJoints); // WATCH THIS LINE!! IS IT RIGHT?
        pthread_cond_wait(&calc_new_joints_cond, &next_joints_mutex);
        pthread_mutex_unlock(&next_joints_mutex);
        fcount++;

    }
}


const vector <float> MotionSwitchboard::getNextJoints() {
    pthread_mutex_lock(&next_joints_mutex);

    const vector <float> vec(nextJoints);

    pthread_cond_signal(&calc_new_joints_cond);

    pthread_mutex_unlock(&next_joints_mutex);


    return vec;
}

