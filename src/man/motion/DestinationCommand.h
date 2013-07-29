#pragma once

/**
 * Destination commands take parameters in the following units:
 * X/Y mm relative to our current position to walk to, rads to turn to
 */
#include "MotionConstants.h"
#include "MotionCommand.h"

namespace man
{
namespace motion
{
class DestinationCommand : public MotionCommand
{
public:
    typedef boost::shared_ptr<DestinationCommand> ptr;

    DestinationCommand(float _x_mm, float _y_mm, float _theta_rads, 
                       float _gain, bool ped, bool kick,
                       float ball_x, float ball_y, int kick_type)
        : MotionCommand (MotionConstants::DESTINATION),
          x_mm(_x_mm),y_mm(_y_mm),theta_rads(_theta_rads),
          gain(_gain),pedantic(ped),motionKick(kick),kickBallRelX(ball_x),
          kickBallRelY(ball_y),kickType(kick_type)
    {
        setChainList();
    }

    virtual ~DestinationCommand() {}

public:
    // DestinationCommand parameters
    const float x_mm;
    const float y_mm;
    const float theta_rads;
    const float gain;
    const bool pedantic;

    // Params for motion kicking
    bool motionKick;
    float kickBallRelX;
    float kickBallRelY;
    int kickType;

protected:
    virtual void setChainList() {
        chainList.assign(MotionConstants::WALK_CHAINS,
                         MotionConstants::WALK_CHAINS +
                         MotionConstants::WALK_NUM_CHAINS);
    }

public:
    friend std::ostream& operator<< (std::ostream &o, const DestinationCommand &w)
        {
            return o << "DestinationCommand("
                     << w.x_mm << "," << w.y_mm << "," << w.theta_rads
                     << ") at: " << w.gain*100 << "% with pedantic set to: " 
                     << w.pedantic;
        }

};
} // namespace motion
} // namespace man
