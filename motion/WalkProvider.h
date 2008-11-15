
#ifndef _WalkProvider_h_DEFINED
#define _WalkProvider_h_DEFINED

/**
 * The walk provider is the central connection point between all the parts
 * of the walking system. It ties together the controller, and inverse
 * kinematics.
 *
 * The walk provider should be asked to calculateNextJoints each timestep
 * It in turn asks for a new CoM target from the controller, and supervises
 * the left and right `WalkingLegs` in reaching that target.
 *
 * At the end, it updates the super class' joints for each leg chain
 * which are read by the switchboard.
 */

#include <vector>
using namespace std;

#include "MotionProvider.h"
#include "WalkingConstants.h"
#include "WalkController.h"
#include "PreviewController.h"
#include "WalkingLeg.h"
#include "StepGenerator.h"

class WalkProvider : public MotionProvider {
public:
    WalkProvider();
    virtual ~WalkProvider();

    void requestStop();
    void calculateNextJoints();
    void setMotion(float x, float y, float theta);

private:
    WalkingParameters walkParameters;
    WalkController *controller;
    WalkingLeg left, right;
    StepGenerator stepGenerator;
};

#endif
