/**
 * The StepGenerator (will) house a ton of logic relating to how the steps are
 * configured. It must handle the following tasks:
 *  - Provide future ZMP_REF values for both x and y dimensions
 *  - Decide where to place footsteps depending on input x,y,h
 *  - Handle stopping, both as a motion vector, and when requested
 *    to stop by the underlying switchboard/provider.
 *
 *  Subtasks:
 *    Manage the moving coordinate frame while the robot is walking
 *    Decide when to throw away dated footsteps
 *    Decide how to add new footsteps (when, etc)
 *
 */

#ifndef _StepGenerator_h_DEFINED
#define _StepGenerator_h_DEFINED

#include <cmath>
#include <list>
using std::list;

class StepGenerator {
public:
    StepGenerator();
    //const list<float> tick (float x, float y, float h);
    const list<float>* tick();

private:
    //need two of these
    list<float> zmp_ref;
};

#endif
