#ifndef Step_h_DEFINED
#define Step_h_DEFINED

#include <boost/shared_ptr.hpp>
#include <iostream>
#include "Gait.h"

enum StepType {
    REGULAR_STEP=0,
    //START_STEP,
    END_STEP,
    //NULL_STEP
};

enum Foot {
    LEFT_FOOT = 0,
    RIGHT_FOOT
};

struct WalkVector {
  float x;
  float y;
  float theta;
};

static const WalkVector ZERO_WALKVECTOR = {0.0f,0.0f,0.0f};

/**
 * Simple container to hold information about future steps.
 */
class Step{
public:
    Step(const Step & other);
    Step(const WalkVector &target,
         const AbstractGait & gait,	 
	 const Foot _foot,
	 const WalkVector &last = ZERO_WALKVECTOR,
         const StepType _type = REGULAR_STEP);
    // Copy constructor to allow changing reference frames:
    Step(const float new_x, const float new_y, const float new_theta,
         const Step& other);

    void updateFrameLengths(const float duration,
                            const float dblSuppF);

    void setStepSize(const WalkVector &target,
		     const WalkVector &last);
    
    friend std::ostream& operator<< (std::ostream &o, const Step &s)
        {
            return o << "Step(" << s.x << "," << s.y << "," << s.theta
                     << ") in " << s.stepConfig[WP::DURATION]
                     <<" secs with foot "
                     << s.foot << " and type " << s.type;
        }

public:
    float x;
    float y;
    float theta;
    WalkVector walkVector;
    unsigned int stepDurationFrames;
    unsigned int doubleSupportFrames;
    unsigned int singleSupportFrames;
    float sOffsetY;
    Foot foot;
    StepType type;
    bool zmpd;

    float stepConfig[WP::LEN_STEP_CONFIG];
    float zmpConfig[WP::LEN_ZMP_CONFIG];
    float stanceConfig[WP::LEN_STANCE_CONFIG];
private:
    void copyGaitAttributes(const float _step_config[],
                            const float _zmp_config[],
			    const float _stance_config[]);
    void copyAttributesFromOther(const Step &other);

};

static const boost::shared_ptr<Step> EMPTY_STEP =
  boost::shared_ptr<Step>(new Step(ZERO_WALKVECTOR,
                                     DEFAULT_GAIT,
                                     LEFT_FOOT));
#endif
