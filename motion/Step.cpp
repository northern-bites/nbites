#include <string>
#include "Step.h"
#include "MotionConstants.h"
#include "Observer.h"

Step::Step(const Step& other)
{
    copyAttributesFromOther(other);
}

Step::Step(const float _x, const float _y, const float _theta,
           const AbstractGait & gait,
           const Foot _foot, const StepType _type)
    : x(_x),y(_y), theta(_theta),
      foot(_foot),type(_type),zmpd(false)
{
    copyGaitAttributes(gait.step,gait.zmp);

    switch(_type){
    case REGULAR_STEP:
        updateFrameLengths(stepConfig[WP::DURATION],
                           stepConfig[WP::DBL_SUPP_P]);
        break;
    case END_STEP:
        //For end steps, we are always double support, and
        //we make the length the preview period
        updateFrameLengths(static_cast<float>(Observer::NUM_PREVIEW_FRAMES) *
                           MotionConstants::MOTION_FRAME_LENGTH_S,
                           1.0f);
        break;
    }
}


// Copy constructor to allow changing reference frames:
Step::Step(const float new_x, const float new_y, const float new_theta,
           const Step & other)
{
    copyAttributesFromOther(other);
    x = new_x;
    y = new_y;
    theta = new_theta;
}

void Step::updateFrameLengths(const float duration,
                              const float dblSuppF){
    //need to calculate how many frames to spend in double, single
    stepDurationFrames =
        static_cast<unsigned int>( duration/
                                  MotionConstants::MOTION_FRAME_LENGTH_S);

    doubleSupportFrames =
        static_cast<unsigned int>(duration *dblSuppF/
                                  MotionConstants::MOTION_FRAME_LENGTH_S);
    singleSupportFrames = stepDurationFrames - doubleSupportFrames;
}


void Step::copyAttributesFromOther(const Step &other){
    x = other.x;
    y = other.y;
    theta = other.theta;
    stepDurationFrames = other.stepDurationFrames;
    doubleSupportFrames = other.doubleSupportFrames;
    singleSupportFrames = other.singleSupportFrames;
    foot = other.foot;
    type = other.type;
    zmpd = other.zmpd;
    copyGaitAttributes(other.stepConfig,other.zmpConfig);
}

void Step::copyGaitAttributes(const float _step_config[WP::LEN_STEP_CONFIG],
                              const float _zmp_config[WP::LEN_ZMP_CONFIG]){
    memcpy(stepConfig,_step_config,sizeof(float)*WP::LEN_STEP_CONFIG);
    memcpy(zmpConfig,_zmp_config,sizeof(float)*WP::LEN_ZMP_CONFIG);
}
