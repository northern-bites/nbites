/**
 * Walk commands take parameters in the following units:
 *   - Centimeter/second for Distance, Degrees/second for rotation.
 */
#pragma once

#include <boost/shared_ptr.hpp>

#include "MotionConstants.h"
#include "MotionCommand.h"

namespace man
{
namespace motion
{
class WalkCommand : public MotionCommand
{
public:
    typedef boost::shared_ptr<WalkCommand> ptr;

    WalkCommand(float x_percent, float y_percent, float theta_percent)
        : MotionCommand(MotionConstants::WALK),
          x_percent(x_percent),y_percent(y_percent),theta_percent(theta_percent)
        { setChainList(); }

    virtual ~WalkCommand() {}
public:
//    WalkParameters params;
    const float x_percent;    //0 - 1 magnitude
    const float y_percent;    //0 - 1 magnitude
    const float theta_percent; //0 - 1 magnitude
protected:
	virtual void setChainList() {
        chainList.assign(MotionConstants::WALK_CHAINS,
                         MotionConstants::WALK_CHAINS +
                         MotionConstants::WALK_NUM_CHAINS);
    }

public:
    friend std::ostream& operator<< (std::ostream &o, const WalkCommand &w)
        {
            return o << "WalkCommand("
                     << w.x_percent << "," << w.y_percent << "," << w.theta_percent
                     << ") ";
        }

};
}
}
