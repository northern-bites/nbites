/**
 * Walk commands take parameters in the following units:
 *   - Centimeter/second for Distance, Degrees/second for rotation.
 */
#ifndef _WalkCommand_h_DEFINED
#define _WalkCommand_h_DEFINED

// Ours
#include "MotionConstants.h"
#include "MotionCommand.h"

class WalkCommand : public MotionCommand
{
public:
    WalkCommand(float _x_mms, float _y_mms, float _theta_rads)
        : MotionCommand(MotionConstants::WALK),
          x_mms(_x_mms),y_mms(_y_mms),theta_rads(_theta_rads)
        { setChainList(); }

  virtual ~WalkCommand() {}

public:
//    WalkParameters params;
    const float x_mms;    //cm/second
    const float y_mms;    //cm/second
    const float theta_rads; //deg/second
private:
	virtual void setChainList() {
        chainList.assign(MotionConstants::WALK_CHAINS,
                         MotionConstants::WALK_CHAINS +
                         MotionConstants::WALK_NUM_CHAINS);
    }

public:
    friend std::ostream& operator<< (std::ostream &o, const WalkCommand &w)
        {
            return o << "WalkCommand("
                     << w.x_mms << "," << w.y_mms << "," << w.theta_rads
                     << ") ";
        }

};

#endif
