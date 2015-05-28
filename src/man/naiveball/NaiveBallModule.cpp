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
    velocityEst = 0.f;
    bufferFull = false;
}

// Check if ball is moving, and if so, in what direction, how fast, etc.
void NaiveBallModule::naiveCheck() {
    // Get average of first and last x number of frames to get a position estimate
    // at beginning and end
    int startIndex = currentIndex+1;
    int endIndex = currentIndex - AVGING_FRAMES;
    if (endIndex < 0) {
        endIndex = endIndex + buffSize;
    }

    BallState start_avgs = avgFrames(startIndex);
    BallState end_avgs = avgFrames(endIndex);
    float dist = sqrt(pow((end_avgs.rel_x - start_avgs.rel_x), 2.f) + pow((end_avgs.rel_y - start_avgs.rel_y), 2.f));
    // velocityEst = (dist / NUM_FRAMES) * ALPHA + velocityEst * (1-ALPHA);
    velocityEst = (dist / NUM_FRAMES);
    // TODO possibly take into account (possibly as in probably) previous estimates

}

BallState avgFrames(int startingIndex) {
    float x_sum = 0.f;
    float y_sum = 0.f;
    float dist_sum = 0.f;
    float bearing_sum = 0.f;
    for (int i = 0; i < AVGING_FRAMES; i++) {
        x_sum += ballStateBuffer[startingIndex].rel_x;
        y_sum += ballStateBuffer[startingIndex].rel_y;
        dist_sum += ballStateBuffer[startingIndex].distance;
        bearing_sum += ballStateBuffer[startingIndex].bearing;
        if (startingIndex + 1 == buffSize) {
            startingIndex = 0;
        } else {
            startingIndex += 1;
        }
    }
    return BallState(x_sum / AVGING_FRAMES, y_sum / AVGING_FRAMES, dist_sum / AVGING_FRAMES, bearing_sum / AVGING_FRAMES);
}

} // namespace man
} // namespace context
