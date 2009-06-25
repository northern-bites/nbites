#include "Step.h"
#include "MotionConstants.h"

Step::Step(const float _x, const float _y, const float _theta,
           const float _duration, const float doubleSupportFraction,
           const Foot _foot, const StepType _type)
    : x(_x),y(_y), theta(_theta), stepDuration(_duration),
      foot(_foot),type(_type),zmpd(false)
{
    updateFrameLengths(doubleSupportFraction);
}
// Copy constructor to allow changing reference frames:
Step::Step(const float new_x, const float new_y, const float new_theta,
           const boost::shared_ptr<Step> other)
    : x(new_x),y(new_y), theta(new_theta),
      stepDuration(other->stepDuration),
      stepDurationFrames(other->stepDurationFrames),
      doubleSupportFrames(other->doubleSupportFrames),
      singleSupportFrames(other->singleSupportFrames),
      foot(other->foot),type(other->type),zmpd(other->zmpd){}

void Step::updateFrameLengths(float doubleSupportFraction){
    //need to calculate how many frames to spend in double, single
    stepDurationFrames =
        static_cast<unsigned int>(stepDuration /
                                  MotionConstants::MOTION_FRAME_LENGTH_S);

    doubleSupportFrames =
        static_cast<unsigned int>(stepDuration *
                                  doubleSupportFraction/
                                  MotionConstants::MOTION_FRAME_LENGTH_S);
    singleSupportFrames = stepDurationFrames - doubleSupportFrames;
}
