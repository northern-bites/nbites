
#ifndef _MotionInterface_h_DEFINED
#define _MotionInterface_h_DEFINED

#include <queue>
#include <vector>

#include "Kinematics.h"
#include "WalkCommand.h"
#include "BodyJointCommand.h"
#include "MotionSwitchboard.h"
#include "WalkingConstants.h"
#define DUMMY_F 0.0f
#define DUMMY_I 0

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
    virtual ~MotionInterface() {}

    //depricated interface calls
    void setNextWalkCommand(const WalkCommand *command);
    void enqueue(const BodyJointCommand *command);
    void enqueue(const HeadJointCommand *command);
    inline bool isWalkActive() { return true; }
    void setGait(const GaitCommand *command);
    void setHead(const SetHeadCommand * command);

    void stopBodyMoves();
    void stopHeadMoves();


    int postGotoCom(float pX, float pY, float pZ, float pTime, int pType) {
        return DUMMY_I;
    }
    int postGotoTorsoOrientation(float pX, float pY, float pTime, int pType) {
        return DUMMY_I;
    }

    float getHeadSpeed();

    void setBodyStiffness(float percentStiffness, float time) {
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

    void setSupportMode( int pSupportMode );
    int getSupportMode();
    void setBalanceMode( int pBalanceMode );
    int getBalanceMode();

  private:
    MotionSwitchboard *switchboard;
};

#endif

