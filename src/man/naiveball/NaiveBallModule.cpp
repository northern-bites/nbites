#include "NaiveBallModule.h"

namespace man {
namespace context {

NaiveBallModule::NaiveBallModule() :
    portals::Module(),
    naiveBallOutput(base()),
    naiveBallBase(base())
{
    velocityEst = 0.f;
    frameOffCount = 0;
    currentIndex = 0;
    buffSize = NUM_FRAMES;
    ballStateBuffer = new BallState[buffSize];
}

NaiveBallModule::~NaiveBallModule()
{
}

void NaiveBallModule::run_()
{
    // Add observation (ball distance/position, ball bearing) to buffer
    // Cycles through buffer and moves up

    // get myInfo:
    locIn.latch();
    myX = locIn.message().x();
    myY = locIn.message().y();
    myH = locIn.message().h();
    ballIn.latch();
    myBall = ballIn.message();


    // If can see the ball, update the buffer
    if (myBall.on()) {
        updateBuffer();
    } else { frameOffCount++; }

    if (frameOffCount > MAX_FRAMES_OFF) {
        clearBuffer();
    } else if (bufferFull) {
        naiveCheck();
    }

    portals::Message<messages::NaiveBall> naiveBallMessage(0);

    naiveBallMessage.get()->set_velocity(velocityEst);
    naiveBallOutput.setMessage(naiveBallMessage);

}

// Add new ball observation to the buffer
void NaiveBallModule::updateBuffer()
{
    currentIndex = (currentIndex + 1) % buffSize;
    ballStateBuffer[currentIndex] = BallState(myBall.rel_x(), myBall.rel_y(), myBall.distance(), myBall.bearing());
    if (currentIndex == buffSize - 1) {
        bufferFull = true;
    }
}

void NaiveBallModule::clearBuffer() {
    currentIndex = 0;
    bufferFull = false;
}

// Check if ball is moving, and if so, in what direction, how fast, etc.
void NaiveBallModule::naiveCheck() {



}


} // namespace man
} // namespace context
