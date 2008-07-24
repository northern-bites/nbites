
#ifndef _MotionCore_h_DEFINED
#define _MotionCore_h_DEFINED

/**
 * MotionCore is the central place for motion decisions. MotionInterface will
 * acts as an intermediary between python and this class. Currently MotionCore
 * is a wrapper around Aldebaran library calls, but may eventually be our own
 * implementation.
 *
 * TODO:
 *   - set priority of the motion thread to be the highest possible.
 *   - move all the thread stuff into Motion.cpp
 *   - set up the use of chainTimeRemaining and chainTaskIDs
 *   - Consolidate all the types of Commands into a single header file
 *   - Possibly create an individual mutex for each queue.
 *   - Figure out how we're going to estimate odometry in WalkCommands
 *   - Remember that setBodyStiffness is called in the constructor and is given
 *     a full second to set it.
 */

#include <vector>
#include <pthread.h>

// Aldebaran library includes
#include "almotionproxy.h"

// NBites header includes
#include "Sensors.h"
#include "BodyJointCommand.h"
#include "HeadJointCommand.h"
#include "HeadScanCommand.h"
#include "WalkCommand.h"
#include "Kinematics.h"
using namespace Kinematics;

class MotionCore
{
 public:
  static const AL::ALMotionProxy::INTERPOLATION_TYPE SMOOTH_INTERPOLATION =
    AL::ALMotionProxy::INTERPOLATION_SMOOTH;
  static const AL::ALMotionProxy::INTERPOLATION_TYPE LINEAR_INTERPOLATION =
    AL::ALMotionProxy::INTERPOLATION_LINEAR;
  static const AL::ALMotionProxy::INTERPOLATION_TYPE DEFAULT_INTERPOLATION =
    SMOOTH_INTERPOLATION;

  public:
    MotionCore(Sensors *s);
    virtual ~MotionCore(void);

    void setNextWalkCommand (const WalkCommand *command) {
      pthread_mutex_lock(&motion_mutex);
      if (nextWalkCommand)
	delete nextWalkCommand;
      nextWalkCommand = command;
      pthread_mutex_unlock(&motion_mutex);
    }
    void enqueue (const BodyJointCommand *command) {
      pthread_mutex_lock(&motion_mutex);
      bodyQueue.push_back(command);
      pthread_mutex_unlock(&motion_mutex);
    }
    void enqueue (const HeadJointCommand *command) {
      pthread_mutex_lock(&motion_mutex);
      //cout << "Got a head command: " << endl;
      //cout << "   " << (command->getJoints())->at(0) << "  "  << (command->getJoints())->at(1) << endl;
      headQueue.push_back(command);
      pthread_mutex_unlock(&motion_mutex);
    }
    void enqueue (const HeadScanCommand *command) {
      pthread_mutex_lock(&motion_mutex);
      headScanQueue.push_back(command);
      pthread_mutex_unlock(&motion_mutex);
    }
    void enqueueSequence(std::vector<BodyJointCommand*> &seq);

    /*
    std::vector<float> getChainAnglesTo(int chainID, float x, float y, float z,
                                        int jointMask, float HYPAngle);
    std::vector<float> getLastOdometry (void);

    void holdChain   (int chainID, int time);
    bool isQueueEmpty(void);

    void setHead  (float yaw, float pitch);
    void setMotion(float x, float y, float h, int numStep);
    */
    // ALMotionProxy wrapped methods
    void setWalkConfig ( float pMaxStepLength, float pMaxStepHeight,
			 float pMaxStepSide, float pMaxStepTurn,
			 float pZmpOffsetX, float pZmpOffsetY);
    void setWalkArmsConfig ( float pShoulderMedian, float pShoulderAmplitude,
			     float pElbowMedian, float pElbowAmplitude);
    void setWalkExtraConfig( float pLHipRollBacklashCompensator,
			     float pRHipRollBacklashCompensator,
			     float pHipHeight , float pTorsoYOrientation);
    void setWalkParameters( const WalkParameters& params);

    void setSupportMode( int pSupportMode);
    int getSupportMode();
    void setBalanceMode( int pBalanceMode);
    int getBalanceMode();

    void stopHeadMoves();
    void stopBodyMoves();

    int postGotoCom(float pX, float pY, float pZ, float pTime, int pType) {
      motionProxy->postGotoCom(pX, pY, pZ, pTime, pType);
    }
    int postGotoTorsoOrientation(float pX, float pY, float pTime, int pType) {
      motionProxy->postGotoTorsoOrientation(pX, pY, pTime, pType);
    }

    bool isWalkActive() { return motionProxy->walkIsActive(); }
    void setBodyStiffness(float percentStiffness,
			  float time) {
      motionProxy->setBodyStiffness(percentStiffness,time); }

    const vector<float> getOdometry();
    const float getHeadSpeed();

    bool isRunning() { return running; }    
    void run    ();
    void runStep();
    void start  ();
    void stop   ();


    void updateSensorsWithMotion();
 protected:
    void processCommands();
    void updateOdometry();
    void updateHeadSpeed();

 private: // Constants
    static const int NO_TASK = -1;
    static const int MOTION_FRAME_RATE = 50;
    static const float MOTION_FRAME_LENGTH_uS = // in microseconds
      // 1 second * 1000 ms/s * 1000 us/ms
      1.0f * 1000.0f * 1000.0f / MOTION_FRAME_RATE;
    static const float MOTION_FRAME_LENGTH_S = // in seconds
      // 1 second * 1000 ms/s * 1000 us/ms
      1.0f / MOTION_FRAME_RATE;

    static const vector<float> NOT_MOVING;

    // Constants concerned with the get up and sit down command. There are 20
    // joints in the whole body.
    static const float GET_UP_TIME = 3.0f;
    static const float SIT_DOWN_TIME = 3.0f;
    static const float GET_UP_BODY_JOINTS[NUM_BODY_JOINTS];
    static const float SIT_DOWN_BODY_JOINTS[NUM_BODY_JOINTS];

    static const BodyJointCommand GET_UP;
    static const BodyJointCommand SIT_DOWN;

  private:
    static void* runThread(void *motion);

  private:
    Sensors *sensors;

    std::list<const BodyJointCommand*> bodyQueue;
    std::list<const HeadJointCommand*> headQueue;
    std::list<const HeadScanCommand*> headScanQueue;
    WalkCommand const *nextWalkCommand;

    // Pointer to the Aldebaran motion proxy
    AL::ALMotionProxy *motionProxy;
    bool walkIsStopping;

    bool running;  // is the motion thread running

    // we remember how much time is left to complete the actions queued up
    // for each chain.
    float chainTimeRemaining[NUM_CHAINS];
    // When we 'post' each command to a chain, we get back a taskID from
    // ALMotion, so that we can later stop that action. This array holds the
    // most recently queued commands for each chain, so we can cancel all
    // body movements if need be.
    int chainTaskIDs[NUM_CHAINS];
    // Since we preemptiely enqueue commands shortly before the previous one is
    // done (in order to avoid slight jitters and pauses in motor movement), we
    // need to store a second round of chainTaskIDs
    int preemptiveChainTaskIDs[NUM_CHAINS];

    //Odometry information and localization
    vector<float> latestOdometry; //built up odometry info since last read
    vector<float> currentWalkVector; //current displacement in one motion frame
    bool odometryRead; //keep track of if our loc updates have been read yet
    float lastHeadYaw;
    float lastHeadPitch;
    float headSpeed;

    pthread_t motion_thread;
    pthread_mutex_t motion_mutex;
    pthread_mutex_t odometry_mutex;
    pthread_mutex_t head_speed_mutex;
};

#endif
