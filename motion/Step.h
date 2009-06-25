#ifndef Step_h_DEFINED
#define Step_h_DEFINED

#include <boost/shared_ptr.hpp>
#include <iostream>

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

/**
 * Simple container to hold information about future steps.
 */
class Step{
public:
    Step(const float _x, const float _y, const float _theta,
         const float _duration,
         const float doubleSupportFraction,const Foot _foot,
         const StepType _type = REGULAR_STEP);
    // Copy constructor to allow changing reference frames:
    Step(const float new_x, const float new_y, const float new_theta,
         const boost::shared_ptr<Step> other);

    void updateFrameLengths(float doubleSupportFraction);

    friend std::ostream& operator<< (std::ostream &o, const Step &s)
        {
            return o << "Step(" << s.x << "," << s.y << "," << s.theta
                     << ") in " << s.stepDuration <<" secs with foot "
                     << s.foot << " and type " << s.type;
        }

public:
    float x;
    float y;
    float theta;
    float stepDuration;
    unsigned int stepDurationFrames;
    unsigned int doubleSupportFrames;
    unsigned int singleSupportFrames;
    Foot foot;
    StepType type;
    bool zmpd;
};

static const boost::shared_ptr<Step> EMPTY_STEP =
    boost::shared_ptr<Step>(new Step(0.0f,0.0f,0.0f,
                                     0.0f,0.0f,
                                     LEFT_FOOT));
#endif
