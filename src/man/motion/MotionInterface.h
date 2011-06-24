
// This file is part of Man, a robotic perception, locomotion, and
// team strategy application created by the Northern Bites RoboCup
// team of Bowdoin College in Brunswick, Maine, for the Aldebaran
// Nao robot.
//
// Man is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Man is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.

#ifndef _MotionInterface_h_DEFINED
#define _MotionInterface_h_DEFINED

#include <queue>
#include <vector>

#include "Kinematics.h"
#include "WalkCommand.h"
#include "BodyJointCommand.h"
#include "HeadJointCommand.h"
#include "MotionSwitchboard.h"
#include "WalkingConstants.h"
#include "StepCommand.h"
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

    void setNextWalkCommand(const WalkCommand::ptr command);
    void sendStepCommand(const StepCommand::ptr command);
    void sendDestCommand(const DestinationCommand::ptr command);
    void enqueue(const BodyJointCommand::ptr command);
    void enqueue(const HeadJointCommand::ptr command);
    inline bool isWalkActive() {return switchboard->isWalkActive();}
    inline bool isHeadActive(){return switchboard->isHeadActive();}
    inline bool isBodyActive(){return switchboard->isBodyActive();}
    void setGait(const Gait::ptr command);
    void setHead(const SetHeadCommand::ptr command);
    void coordHead(const CoordHeadCommand::ptr command);
    void stopBodyMoves();
    void stopHeadMoves();
    void resetWalkProvider();
    void resetScriptedProvider();
    void sendFreezeCommand(const FreezeCommand::ptr command);
    void sendFreezeCommand(const UnfreezeCommand::ptr command);

    //For noggin
    MotionModel getOdometryUpdate(){
        return switchboard->getOdometryUpdate();
    }

    int postGotoCom(float pX, float pY, float pZ,
                    float pTime, int pType) const{
        return DUMMY_I;
    }
    int postGotoTorsoOrientation(float pX, float pY,
                                 float pTime, int pType) const {
        return DUMMY_I;
    }

    float getHeadSpeed() const ;

    void setBodyStiffness(float percentStiffness, float time) const {
    }
    void setHead(float time, float yaw, float pitch,
                 Kinematics::InterpolationType type) const { }
    void setWalkConfig ( float pMaxStepLength, float pMaxStepHeight,
			 float pMaxStepSide, float pMaxStepTurn,
			 float pZmpOffsetX, float pZmpOffsetY) const ;
    void setWalkArmsConfig ( float pShoulderMedian, float pShoulderAmplitude,
			     float pElbowMedian, float pElbowAmplitude) const ;
    void setWalkExtraConfig( float pLHipRollBacklashCompensator,
			     float pRHipRollBacklashCompensator,
			     float pHipHeight , float pTorsoYOrientation) const;

    void setSupportMode( int pSupportMode ) const ;
    int getSupportMode() const;
    void setBalanceMode( int pBalanceMode ) const ;
    int getBalanceMode() const;

    int getFrameCount() const { return switchboard->getFrameCount(); }

  private:
    MotionSwitchboard *switchboard;
};

#endif

