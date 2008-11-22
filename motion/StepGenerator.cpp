#include "StepGenerator.h"
#include <iostream>

StepGenerator::StepGenerator(const WalkingParameters *params,
                             WalkingLeg *_left, WalkingLeg *_right)
    : x(0.0f), y(0.0f), theta(0.0f),
      zmp_ref_x(list<float>()),zmp_ref_y(list<float>()), futureSteps(),
      lastZMPDStep(new Step(0,0,0,0,LEFT_FOOT)), coordOffsetLastZMPDStep(0,0),
      walkParameters(params), nextStepIsLeft(true),
      leftLeg(_left), rightLeg(_right) {

    setWalkVector(0,0,0); // for testing purposes. The function doesn't even
    // honor the parameters passed to it yet
}


zmp_xy StepGenerator::tick() {
    static float lastZMP_x = 0;
    static float lastZMP_y = 0;

    while (zmp_ref_y.size() <= PreviewController::NUM_PREVIEW_FRAMES) {
        if (futureSteps.size() < 1)
            generateStep(x, y, theta); // with the current walk vector
        else {
            boost::shared_ptr<Step> nextStep = futureSteps.front();
            futureSteps.pop_front();

            fillZMP(nextStep);
        }
    }

    float newZMP_x = zmp_ref_x.front();
    float newZMP_y = zmp_ref_y.front();
    // Hackish. Since ZMP values stay the same throughout a support mode, this
    // is an effective way of knowing when to switch support modes. If we
    // decided to go with a zmp curve instead of a step function, we would
    // be in trouble.
    if (newZMP_x != lastZMP_x || newZMP_y != lastZMP_y) {
        leftLeg->switchSupportMode();
        rightLeg->switchSupportMode();
    }

    lastZMP_x = newZMP_x;
    lastZMP_y = newZMP_y;
    zmp_ref_x.pop_front();
    zmp_ref_y.pop_front();
    return zmp_xy(&zmp_ref_x, &zmp_ref_y);
}


//Step length always must be a multiple of the motion frame length
void StepGenerator::fillZMP(const boost::shared_ptr<Step> newStep ){
    //look at the last ZMPD Step and the newStep, and make ZMP values
    float stepTime = newStep->time;
    int numChops = static_cast<int>(stepTime / walkParameters->motion_frame_length_s);
    float start_x = lastZMPDStep->x + coordOffsetLastZMPDStep.x;
    float start_y = lastZMPDStep->y + coordOffsetLastZMPDStep.y;
    float end_x = newStep->x + coordOffsetLastZMPDStep.x;
    float end_y = newStep->y + coordOffsetLastZMPDStep.y;

    std::cout << "start_x: " << start_x << "end_x: " << end_x << std::endl;

    for(int i = 0; i< numChops; i++){
        float new_x =
            end_x;
            //start_x + (static_cast<float>(i)/numChops)*(end_x-start_x);
        float new_y =
            end_y;
            //start_y + (static_cast<float>(i)/numChops)*(end_y-start_y);

        zmp_ref_x.push_back(new_x);
        zmp_ref_y.push_back(new_y);
    }
    //update the lastZMPD Step
    int sign = (newStep->foot == LEFT_FOOT ? 1 : -1);
    lastZMPDStep = newStep;
    coordOffsetLastZMPDStep.x += newStep->x;
    coordOffsetLastZMPDStep.y += newStep->y - sign*HIP_OFFSET_Y; //shift to 0
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

//     for(int i = 0; i < 4; i++){
//         generateStep(x,y,theta);
//     }

    // and we create new plans. We would like to always have enough steps
    // pre-planned, so that we always have enough previewable zmp_ref values.
    // For now we'll create 4 future steps.

    // Part of starting a walk is to create a zmp_ref that will end in the
    // polygon of the supporting leg. Once it is there, we will consider
    // taking steps.


    // This says that the next 30 zmp values are over the left leg
    for (int i = 0; i < 50; i++){
        zmp_ref_y.push_back(0.0f);
        zmp_ref_x.push_back(0.0f);
    }

    // This says that the next 30 zmp values are over the left leg
    for (int i = 0; i < 50; i++){
        zmp_ref_y.push_back(50.0f);
        zmp_ref_x.push_back(0.0f);
    }

    // we start by using our left foot as support and the right as "double
    // support". The right foot will switch into swinging
    leftLeg->switchSupportMode(PERSISTENT_DOUBLE_SUPPORT);
    rightLeg->switchSupportMode(DOUBLE_SUPPORT);

    //dummy
    lastZMPDStep = boost::shared_ptr<Step>(new Step(0,HIP_OFFSET_Y,0,
                                                    walkParameters->stepDuration, LEFT_FOOT));
    coordOffsetLastZMPDStep = point<float>(0,0);
}


//currently only does two sets of steps side by side
void StepGenerator::generateStep(const float _x,
                                 const float _y,
                                 const float _theta) {
    boost::shared_ptr<Step> step(new Step(_x,(nextStepIsLeft ?
                                             HIP_OFFSET_Y : -HIP_OFFSET_Y),
                                          0, walkParameters->stepDuration,
                                          (nextStepIsLeft ?
                                           LEFT_FOOT : RIGHT_FOOT)));


    futureSteps.push_back(step);
    //switch feet after each step is generated
    nextStepIsLeft = !nextStepIsLeft;
}
