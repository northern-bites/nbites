/**
 * BallFilter.h - Abstract Ball Filter Class defines an interface for the module
 *
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date March 2013
 */

#pragma once

namespace man
{
namespace balltrack
{
class BallFilter
{
    BallFilter() {};
    virtual ~BallFilter(){};

    virutal void update(message::VisionBall visionBall,
                        messages::Motion odometry);
};
} // namespace balltrack
} // namespace man
