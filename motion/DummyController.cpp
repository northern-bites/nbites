#include "DummyController.h"
#include "Kinematics.h"

DummyController::DummyController()
    : WalkController(), position(0.0f) {

}


const float DummyController::tick(const float reference) {
    static int frameNumber = 100;
    frameNumber++;

    if (frameNumber > 0)
        position = 50.0f*sin(frameNumber*180.0f/M_PI/1000); //std::min(20.0f, position + 1.0f);
    return position;
}
