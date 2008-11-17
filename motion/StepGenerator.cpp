#include "StepGenerator.h"

StepGenerator::StepGenerator(const WalkingParameters *params)
    : x(0.0f), y(0.0f), theta(0.0f),
      zmp_ref(list<float>()), futureSteps(),
      walkParameters(params), nextStepIsLeft(true) {
    setWalkVector(0,50,0); // for testing purposes
}

/*
const list<float>* StepGenerator::tick() {
    while (zmp_ref.size() < 41) {
        if (futureSteps.size() < 1)
            generateStep(x, y, theta); // with the current walk vector
        else {
            boost::shared_ptr<Step> nextStep = futureSteps.front();
            futureSteps.pop_front();

            // For now I'll define the ZMP reference curve as a sine curve
            for (int i = 0; i < getNumStepFrames(x, y, theta); i++) {
                // chop up the step into zmp_ref values ?!
            }
        }
    }

    zmp_ref.pop_front();
    return &zmp_ref;
}
*/

/**
 * Dummy code, puts ref zmp at 0 for 100 frames, and then 
 * executes a slow, sinusoidal pattern.
 */

const list<float>* StepGenerator::tick() {
    static int counter = 0;
    static bool firstFrame = false;
    if (firstFrame) {
        for (int i=0; i < 100; i++) {
            zmp_ref.push_back(0.0f);
        }
        firstFrame = false;
    }

    while(zmp_ref.size() < 41) {
        //zmp_ref.push_back(50.0f*sin(counter*180.0f/M_PI/1500));
        // Do a step function for kicks
        zmp_ref.push_back(50.0f*
                          (sin(counter*180.0f/M_PI/800) > 0 ? 1 : -1));
        counter++;
    }

    zmp_ref.pop_front(); // we want to keep moving in time
    return &zmp_ref;
}


void StepGenerator::setWalkVector(const float _x, const float _y,
                                  const float _theta)  {
    // WARNING: This method still assumes that we start with (0,0,0) as the
    // walk vector.
    x = _x;
    y = _y;
    theta = _theta;

    // We have to reevalaute future steps, so we forget about any future plans
    futureSteps.clear();

    // and we create new plans. We would like to always have enough steps
    // pre-planned, so that we always have enough previewable zmp_ref values.
    // For now we'll create 4 future steps.

    // Part of starting a walk is to create a zmp_ref that will end in the
    // polygon of the supporting leg. Once it is there, we will consider
    // taking steps.

    // This says that the next 30 zmp values are over the left leg
    for (int i = 0; i < 30; i++)
        zmp_ref.push_back(50.0f);
}

void StepGenerator::generateStep(const float x,
                                 const float y,
                                 const float theta) {
    boost::shared_ptr<Step> step(new Step(0,(nextStepIsLeft ?
                                             HIP_OFFSET_Y : -HIP_OFFSET_Y)));
    futureSteps.push_back(step);
}
