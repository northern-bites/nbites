/**
 * The StepGenerator (will) house a ton of logic relating to how the steps are
 * configured. It must handle the following tasks:
 *  - Provide future ZMP_REF values for both x and y dimensions
 *  - Decide where to place footsteps depending on input x,y,h
 *  - Handle stopping, both as a motion vector, and when requested
 *    to stop by the underlying switchboard/provider.
 *
 * Subtasks:
 *  - Manage the moving coordinate frame while the robot is walking
 *  - Decide when to throw away dated footsteps
 *  - Decide how to add new footsteps (when, etc)
 *
 *  TODO:
 *  - Figure out what the units are for the walk vector.
 *  - Figure out how to calculate number of required pre-planned steps,
 *    so that we always have enough previewable zmp_ref values.
 */

#ifndef _StepGenerator_h_DEFINED
#define _StepGenerator_h_DEFINED

#include <cmath>
#include <list>
using std::list;

#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>

#include "Structs.h"
#include "PreviewController.h"
#include "WalkingConstants.h"
#include "WalkingLeg.h"

typedef boost::tuple<const list<float>*, const list<float>*> zmp_xy;

/**
 * Simple container to hold information about future steps.
 */
struct Step:point<float> {
    float theta;
    float time;
    Foot foot;
    Step(const float _x, const float _y, const float _theta,
         const float _time, const Foot _foot)
        : point<float>(_x,_y), theta(_theta), time(_time), foot(_foot) {}
};

class StepGenerator {
public:
    StepGenerator(const WalkingParameters *params,
                  WalkingLeg *_left, WalkingLeg *_right);
    boost::tuple<const list<float>*,const list<float>*> tick();

    void setWalkVector(const float _x, const float _y, const float _theta);

private: // Helper methods
    void generateStep(const float _x, const float _y,
                      const float _theta);
    void fillZMP(const boost::shared_ptr<Step> newStep );

private:
    // Walk vector:
    //  * x - forward
    //  * y - lateral (left is positive)
    //  * theta - angular (counter-clockwise is positive)
    // NOTE/TODO: the units of these are not well-defined yet
    float x;
    float y;
    float theta;


    // need to store future zmp_ref values (points in xy)
    list<float> zmp_ref_x, zmp_ref_y;
    list<boost::shared_ptr<Step> > futureSteps;
    boost::shared_ptr<Step> lastZMPDStep; //Last step turned into ZMP values
    point<float> coordOffsetLastZMPDStep;

    const WalkingParameters *walkParameters;
    bool nextStepIsLeft;

    WalkingLeg *leftLeg, *rightLeg;
};

#endif
