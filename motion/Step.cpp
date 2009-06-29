#include <string>
#include "Step.h"
#include "MotionConstants.h"
#include "Observer.h"

#define DEBUG_STEP

Step::Step(const Step& other)
{
    copyAttributesFromOther(other);
}

Step::Step(const float _x, const float _y, const float _theta,
           const AbstractGait & gait,
           const Foot _foot, const StepType _type)
  : foot(_foot),type(_type),zmpd(false)
{
  copyGaitAttributes(gait.step,gait.zmp,gait.stance);

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

    //After we assign elements of the gait to this step, lets clip 
    setStepSize(_x,_y,_theta);
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
    copyGaitAttributes(other.stepConfig,other.zmpConfig,other.stanceConfig);
}

void Step::copyGaitAttributes(const float _step_config[WP::LEN_STEP_CONFIG],
                              const float _zmp_config[WP::LEN_ZMP_CONFIG],
			      const float _stance_config[WP::LEN_STANCE_CONFIG]){
    memcpy(stepConfig,_step_config,sizeof(float)*WP::LEN_STEP_CONFIG);
    memcpy(zmpConfig,_zmp_config,sizeof(float)*WP::LEN_ZMP_CONFIG);
    memcpy(stanceConfig,_stance_config,sizeof(float)*WP::LEN_STANCE_CONFIG);
}


void Step::setStepSize(const float new_x,
			const float new_y,
			const float new_theta){
      //convert speeds in cm/s and rad/s into steps and clip according to the gait
  const float clipped_x =  NBMath::clip(new_x,stepConfig[WP::MAX_VEL_X]
					*stepConfig[WP::DURATION]);

  //For y, need to account for leg separation as well
  const float clipped_y = NBMath::clip(new_y,stepConfig[WP::MAX_VEL_Y]
				       *stepConfig[WP::DURATION] 
				       +stanceConfig[WP::LEG_SEPARATION_Y]*0.5f);
    //we only turn every other step, so double the turning!
  const float clipped_theta = NBMath::clip(new_theta,stepConfig[WP::MAX_VEL_THETA]
					   *stepConfig[WP::DURATION]);

  x = clipped_x;
  y = clipped_y;
  theta = clipped_theta;

#ifdef DEBUG_STEP
  std::cout << "Clipped new step to ("<<x<<","<<y<<","<<theta<<")"<<std::endl;
#endif

}
