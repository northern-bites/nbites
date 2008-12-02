
#ifndef _MotionInterface_h_DEFINED
#define _MotionInterface_h_DEFINED

#include <queue>

#include "Kinematics.h"
#include "MotionCore.h"
#include "WalkCommand.h"
#include "BodyJointCommand.h"
#include "MotionSwitchboard.h"

/**
 * MotionInterface stores motion commands until
 * sendToMotion() is run.
 *
 */

class MotionInterface
{
  public:
    MotionInterface(MotionSwitchboard *_switchboard)
        : switchboard(_switchboard) {}
    MotionInterface(MotionCore *_core) : core(_core) {}
    virtual ~MotionInterface() {}

    //interface calls
    void setNextWalkCommand(const WalkCommand *command);
    void enqueue(const BodyJointCommand *command);
    void enqueue(const HeadJointCommand *command);
    void enqueue(const HeadScanCommand *command);
    inline bool isWalkActive() { return core->isWalkActive(); }

    void stopBodyMoves();
    void stopHeadMoves();

    int postGotoCom(float pX, float pY, float pZ, float pTime, int pType) {
      return core->postGotoCom(pX, pY, pZ, pTime, pType);
    }
    int postGotoTorsoOrientation(float pX, float pY, float pTime, int pType) {
      return core->postGotoTorsoOrientation(pX, pY, pTime, pType);
    }

    float getHeadSpeed();

    void setBodyStiffness(float percentStiffness, float time) {
      core->setBodyStiffness(percentStiffness,time);
    }
    void setHead(float time, float yaw, float pitch,
                 Kinematics::InterpolationType type) { }
    void setWalkConfig ( float pMaxStepLength, float pMaxStepHeight,
			 float pMaxStepSide, float pMaxStepTurn,
			 float pZmpOffsetX, float pZmpOffsetY);
    void setWalkArmsConfig ( float pShoulderMedian, float pShoulderAmplitude,
			     float pElbowMedian, float pElbowAmplitude);
    void setWalkExtraConfig( float pLHipRollBacklashCompensator,
			     float pRHipRollBacklashCompensator,
			     float pHipHeight , float pTorsoYOrientation);
    void setWalkParameters( const WalkParameters& param);

    void setSupportMode( int pSupportMode );
    int getSupportMode();
    void setBalanceMode( int pBalanceMode );
    int getBalanceMode();

    //void sendToMotion();
    
  private:
    MotionCore *core;
    MotionSwitchboard *switchboard;
    /*
    bool shouldStopHead;
    bool shouldStopBody;

    std::queue <const WalkCommand*> actionCommands;
    std::queue <const BodyJointCommand*> jointCommands;
    */
};

#endif

