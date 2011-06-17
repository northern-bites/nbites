#include <string>
#include "Step.h"
#include "MotionConstants.h"
#include "Observer.h"

using namespace std;

//#define DEBUG_STEP

Step::Step(const Step& other)
{
    copyAttributesFromOther(other);
}

Step::Step(const WalkVector &target,
           const AbstractGait & gait, const Foot _foot,
	   const WalkVector &last,
	   const StepType _type)
  : walkVector(target), sOffsetY(gait.stance[WP::LEG_SEPARATION_Y]*0.5f),
    foot(_foot),type(_type),zmpd(false)
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
                           MOTION_FRAME_LENGTH_S,
                           1.0f);
        break;
    }

    //After we assign elements of the gait to this step, lets clip
    setStepSize(target,last);
    setStepLiftMagnitude();
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
        static_cast<unsigned int>( duration / MOTION_FRAME_LENGTH_S);

    doubleSupportFrames =
        static_cast<unsigned int>(duration *dblSuppF / MOTION_FRAME_LENGTH_S);
    singleSupportFrames = stepDurationFrames - doubleSupportFrames;
}


void Step::copyAttributesFromOther(const Step &other){
    x = other.x;
    y = other.y;
    theta = other.theta;
    walkVector = other.walkVector;
    stepDurationFrames = other.stepDurationFrames;
    doubleSupportFrames = other.doubleSupportFrames;
    singleSupportFrames = other.singleSupportFrames;
    sOffsetY= other.sOffsetY;
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


void Step::setStepSize(const WalkVector &target,
		       const WalkVector &last){

  WalkVector new_walk =ZERO_WALKVECTOR;

#ifdef DEBUG_STEP
  printf("Input to setStepSpeed is (%g,%g,%g), (%g,%g,%g), \n",target.x,target.y,target.theta,
	 last.x,last.y,last.theta);
#endif
  new_walk = elipseClipVelocities(target);
#ifdef DEBUG_STEP
  printf("After vel clipping (%g,%g,%g)\n",new_walk.x,new_walk.y,new_walk.theta);
#endif

  new_walk = accelClipVelocities(new_walk,last);
  //new_walk = target;
#ifdef DEBUG_STEP
  printf("After accel clipping (%g,%g,%g)\n",new_walk.x,new_walk.y,new_walk.theta);
#endif

  walkVector = new_walk; //save the walk vector for next time

  //check  if we need to clip lateral movement of this leg
  new_walk = lateralClipVelocities(new_walk);

#ifdef DEBUG_STEP
  printf("After leg clipping (%g,%g,%g)\n",new_walk.x,new_walk.y,new_walk.theta);
#endif

  //Now that we have clipped the velocities, we need to convert them to distance
  //for use with this step. Note that for y and theta, we need a factor of
  //two, since you can only stafe on every other step.

  //HACK! for bacwards compatibility, the step_y is not adjusted correctly
  const float step_x = new_walk.x*stepConfig[WP::DURATION];
  const float step_y = new_walk.y*stepConfig[WP::DURATION];//*2.0f;
  const float step_theta = new_walk.theta*stepConfig[WP::DURATION]*2.0f;

  //Huge architectural HACK!!!  We need to fix our transforms so we don't need to do this
  //anymore
  //This hack pmakes it possible to turn about the center of the robot, rather than the
  //center of the foot
  const float leg_sign = (foot==LEFT_FOOT ?
			  1.0f : -1.0f);
  const float computed_x = step_x - sin(std::abs(step_theta)) *sOffsetY;
  const float computed_y = step_y +
    leg_sign*sOffsetY*cos(step_theta);
  const float computed_theta = step_theta;



  x = computed_x;
  y = computed_y;
  theta = computed_theta;



#ifdef DEBUG_STEP
  std::cout << "Clipped new step to ("<<x<<","<<y<<","<<theta<<")"<<std::endl;
#endif

}


const WalkVector Step::elipseClipVelocities(const WalkVector & source){
  // std::cout << "Ellipsoid clip input ("<<source.x<<","<<source.y
  // 			<<","<<source.theta<<")"<<std::endl;

  //Convert velocities to distances, clip them with an ellipse,
  //then convert back to velocities

  const float theta = NBMath::safe_atan2(source.y,source.x);
  const float xy_mag = std::sqrt(std::pow(source.y,2) + std::pow(source.x,2));

  //To find phi, the elevation out of the xy plane, we need to find a way to compare
  //radians to millimeters. The current way is to consider 'equally-weighted' 
  //turning and lateral/forward motion by 'converting' the radians to something 
  //more equally weighted with mm's
  const float max_xy_mag = std::sqrt(std::pow(
										 stepConfig[WP::MAX_VEL_Y]
										 *std::sin(theta),2)
									 + std::pow(
										 stepConfig[WP::MAX_VEL_X]
										 *std::cos(theta),2));
  const float rad_to_mm = (max_xy_mag / stepConfig[WP::MAX_VEL_THETA]);
  // cout << "xy_mag = " << xy_mag << " converted theta = " << source.theta*rad_to_mm<<endl;
  const float phi = NBMath::safe_atan2(xy_mag,source.theta*rad_to_mm);

  // cout << "Ellipsoid vel. clipping: theta = "<<theta<<", phi="<<phi<<endl;

  float forward_max =0.0f;
  if(source.x > 0)
    forward_max = std::abs(stepConfig[WP::MAX_VEL_X]
						   *std::cos(theta)
						   *std::sin(phi));
  else
    forward_max = std::abs(stepConfig[WP::MIN_VEL_X]
						   *std::cos(theta)
						   *std::sin(phi));

  const float horizontal_max =
    std::abs(stepConfig[WP::MAX_VEL_Y]
			 *std::sin(theta)
			 *std::sin(phi));

  const float turning_max =
    std::abs(stepConfig[WP::MAX_VEL_THETA]
			 *std::cos(phi));
  // cout << "Clipping y="<<source.y<<" according to"<<horizontal_max<<endl;
  const float new_y_vel = NBMath::clip(source.y,horizontal_max);
  // cout << "Clipping x="<<source.x<<" according to"<<forward_max<<endl;
  const float new_x_vel = NBMath::clip(source.x,forward_max);
  // cout << "Clipping theta="<<source.theta<<" according to"<<turning_max<<endl;
  const float new_theta_vel = NBMath::clip(source.theta,turning_max);


  const WalkVector clippedVelocity = {new_x_vel,new_y_vel,new_theta_vel};
  // std::cout << "Ellipsoid clip output ("<<clippedVelocity.x<<","<<clippedVelocity.y
  // 			<<","<<clippedVelocity.theta<<")"<<std::endl;
  return clippedVelocity;
}

const WalkVector Step::accelClipVelocities(const WalkVector & source,
					   const WalkVector & last){
  WalkVector result = source;
  //clip velocities according to the last step, only if we aren't stopping
  if(source.x != 0.0f ||  source.y != 0.0f || source.theta!= 0.0f){
    result.x = NBMath::clip(source.x,
			    last.x - stepConfig[WP::MAX_ACC_X],
			    last.x + stepConfig[WP::MAX_ACC_X]);
    result.y = NBMath::clip(source.y,
			    last.y - stepConfig[WP::MAX_ACC_Y]*0.5f,
			    last.y + stepConfig[WP::MAX_ACC_Y]*0.5f);
    result.theta = NBMath::clip(source.theta,
				last.theta - stepConfig[WP::MAX_ACC_THETA]*0.5f,
				last.theta + stepConfig[WP::MAX_ACC_THETA]*0.5f);
  }
  return result;
}



const WalkVector Step::lateralClipVelocities(const WalkVector & source){
  WalkVector result = source;

  //check  if we need to clip lateral movement of this leg
  if(result.y > 0){
    if(!foot == LEFT_FOOT){
      result.y = 0.0f;
    }
  }else if(result.y < 0){
    if(foot == LEFT_FOOT){
      result.y = 0.0f;
    }
  }

  if(result.theta > 0){
    if(!foot == LEFT_FOOT){
      result.theta = 0.0f;
    }
  }else if (result.theta < 0){
    if(foot == LEFT_FOOT){
      result.theta = 0.0f;
    }
  }

  return result;
}



void Step::setStepLiftMagnitude(){

  const float percent_of_forward_max  = NBMath::clip(x,0,stepConfig[WP::MAX_VEL_X])
    / stepConfig[WP::MAX_VEL_X];

  stepConfig[WP::FOOT_LIFT_ANGLE] *= percent_of_forward_max;
}
