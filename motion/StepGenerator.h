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

#include <boost/shared_ptr.hpp>

#include "WalkingConstants.h"

/**
 * Simple container to hold information about future steps.
 */
struct Step {
    float x;
    float y;

    Step(const float _x, const float _y) : x(_x), y(_y) { }
};

class StepGenerator {
public:
    StepGenerator(const WalkingParameters *params);
    //const list<float> tick (float x, float y, float h);
    const list<float>* tick();

    void setWalkVector(const float _x, const float _y, const float _theta);

private: // Helper methods
    void generateStep(const float x, const float y,
                      const float theta);
    static const int getNumStepFrames(const float x, const float y,
                                      const float theta) {
        return static_cast<int>(sqrt(x*x + y*y));
    }

private:
    // Walk vector:
    //  * x - forward
    //  * y - lateral (left is positive)
    //  * theta - angular (counter-clockwise is positive)
    // NOTE/TODO: the units of these are not well-defined yet
    float x;
    float y;
    float theta;

    // need two zmp_ref in the future for the x and y directions
    list<float> zmp_ref;
    list<boost::shared_ptr<Step> > futureSteps;
    bool nextStepIsLeft;

    const WalkingParameters *walkParameters;
};

#endif
