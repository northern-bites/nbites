#include "MotionSwitchboard.h"
MotionSwitchboard::MotionSwitchboard(Sensors *s)
    : sensors(s),
      walkProvider(),
	  scriptedProvider(1/50.,sensors), // HOW SHOULD WE PASS FRAME_LENGTH??? HACK!
	  headProvider(1/50.0f,sensors),
      nextJoints(Kinematics::NUM_JOINTS, 0.0f),
      //nextJoints(sensors->getBodyAngles()),
	  running(false)
{

    //Allow safe access to the next joints
    pthread_mutex_init(&next_joints_mutex, NULL);
    pthread_cond_init(&calc_new_joints_cond,NULL);

	vector<float>* headJoints1 = new vector<float>(2,M_PI/2);
	hjc = new HeadJointCommand(2.0f,
							   headJoints1,
							   Kinematics::INTERPOLATION_LINEAR);

	vector<float>* headJoints2 = new vector<float>(2,0.0f);
	hjc2 = new HeadJointCommand(3.0f,
								headJoints2,
								Kinematics::INTERPOLATION_LINEAR);
	vector<float>* headJoints3 = new vector<float>(2,-M_PI/2);
	hjc3 = new HeadJointCommand(10.0f,
								headJoints3,
								Kinematics::INTERPOLATION_LINEAR);



	vector<float>* bodyJoints3 = new vector<float>(4,0.0f);//M_PI/4);
	command3 = new BodyJointCommand(2.0f, RARM_CHAIN,
									bodyJoints3,
									Kinematics::INTERPOLATION_LINEAR);

	bodyJoints = new vector<float>(20,0.0f);
	command = new BodyJointCommand(5.0f,
								   bodyJoints,
                                   Kinematics::INTERPOLATION_LINEAR);

	bodyJoints2 = new vector<float>(20,-M_PI/6);
	command2 = new BodyJointCommand(5.0f,
									bodyJoints2,
									Kinematics::INTERPOLATION_LINEAR);

    //We cannot read the sensor values until the run method
    //so we must ensure that no one reads them until we get a chance
    //to initialize them correctly
    pthread_mutex_lock(&next_joints_mutex);
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

// 	headProvider.enqueue(hjc);
// 	scriptedProvider.enqueue(command);
// 	scriptedProvider.enqueue(command2);
//	scriptedProvider.enqueue(command3);
// 	headProvider.enqueue(hjc2);
// 	headProvider.enqueue(hjc3);

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


    //Build the get up routine
    // vector<float> initPos = walkProvider.getWalkStance();
//     cout <<"InitPos:"<< initPos.size() <<endl;
//     for(int i = 0; i <20; i++)
//         cout << "initPos["<<i<<"]: " << initPos[i];

    while(running) {

        if(fcount == 1){
            //hack to help keep from falling over in the simulator
            usleep(2*1000*1000);
        }

        cout << "Switchboard stepping" <<endl;

        //At the beginning of each frame, we need to update the sensor values
        //that are tied to
        //sensors->setBodyAngles(nextJoints); // WATCH THIS LINE!! IS IT RIGHT?

        // Calculate the next joints and get them
		headProvider.calculateNextJoints();
		// get headJoints from headProvider
		vector <float > headJoints = headProvider.getChainJoints(HEAD_CHAIN);
        //Just switch this line to decide which provider we should use
        MotionProvider * curProvider =
//            reinterpret_cast<MotionProvider *>( &
            reinterpret_cast <MotionProvider *>( &scriptedProvider);
        curProvider->calculateNextJoints();

        vector <float > llegJoints = curProvider->getChainJoints(LLEG_CHAIN);
        vector <float > rlegJoints = curProvider->getChainJoints(RLEG_CHAIN);
		vector <float > rarmJoints = curProvider->getChainJoints(RARM_CHAIN);
		vector <float > larmJoints= curProvider->getChainJoints(LARM_CHAIN);

		//Copy the new values into place, and wait to be signaled.
        pthread_mutex_lock(&next_joints_mutex);
        //by default, set the angles to what they are sensed to be
        // nextJoints = sensors->getBodyAngles();

        //note: isActive has been implemented nowhere, will return false
        if (headProvider.isActive())
            for(unsigned int i = 0; i < HEAD_JOINTS;i++){
                nextJoints[HEAD_YAW + i] = headJoints.at(i);
            }
        if (curProvider->isActive()){
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
        }else{
            cout << "Skipping bodyprovider" <<endl;
        }

        pthread_cond_wait(&calc_new_joints_cond, &next_joints_mutex);
        pthread_mutex_unlock(&next_joints_mutex);
        fcount++;

    }
}


const vector <float> MotionSwitchboard::getNextJoints() {
    cout << "GetNextJoints" <<endl;
    pthread_mutex_lock(&next_joints_mutex);
    cout << "GetNextJoints locked" <<endl;
    const vector <float> vec(nextJoints);
    pthread_cond_signal(&calc_new_joints_cond);
    pthread_mutex_unlock(&next_joints_mutex);


    return vec;
}

