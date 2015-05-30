#include "NaiveBallModule.h"

namespace man {
namespace naive {

struct NaiveBallModule::BallState
{
        BallState(){}
        BallState(float rel_x_, float rel_y_, float distance_, float bearing_) : rel_x(rel_x_), rel_y(rel_y_), distance(distance_), bearing(bearing_) {}
        float rel_x;
        float rel_y;
        float distance;
        float bearing;
};

NaiveBallModule::NaiveBallModule() :
    portals::Module(),
    naiveBallOutput(base())
{
    count = 0;
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
    // locIn.latch();
    // myX = locIn.message().x();
    // myY = locIn.message().y();
    // myH = locIn.message().h();
    ballIn.latch();
    myBall = ballIn.message();


    // If can see the ball, update the buffer
    if (myBall.vis().on()) {
        updateBuffer();
    } else { frameOffCount++; }

    if (frameOffCount > MAX_FRAMES_OFF) {
        clearBuffer();
    } else if (bufferFull) {
        naiveCheck();
    }
    // if (currentIndex > 19) {
    //     print();
    //     // count = 0;
    // }
    // count++;

    printf("velocity: %f\n", velocityEst);

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

void NaiveBallModule::clearBuffer()
{
    currentIndex = 0;
    velocityEst = 0.f;
    bufferFull = false;
    frameOffCount = 0;
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
    float dist = calcSumSquaresSQRT((end_avgs.rel_x - start_avgs.rel_x), (end_avgs.rel_y - start_avgs.rel_y));
    // velocityEst = (dist / NUM_FRAMES) * ALPHA + velocityEst * (1-ALPHA);
    velocityEst = (dist / NUM_FRAMES);
    if (velocityEst > 500) {
        printf("\n\n WEIRD: avgs: \n");
        printBallState(start_avgs);
        printBallState(end_avgs);
        printf("\n");

    }
    // TODO possibly take into account (possibly as in probably) previous estimates

}

float NaiveBallModule::calcSumSquaresSQRT(float a, float b)
{
    return sqrt(a*a + b*b);
}

NaiveBallModule::BallState NaiveBallModule::avgFrames(int startingIndex)
{
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

void NaiveBallModule::print() {
    BallState x = ballStateBuffer[currentIndex];
    printf("%s\n", bufferFull ? "BufferFull" : "Buffer not full");
    printf("Current ball state:\nrel_xy: (%f, %f)\ndistance:%f\nbearing:%f\n", x.rel_x, x.rel_y, x.distance, x.bearing);
    printf("VelocityEst = %f\n", velocityEst);
    if (velocityEst > 0.f) {
        printf("BALL IS MOVING\n");
    } else {
        printf("NOT moving\n");
    }

}

void NaiveBallModule::printBallState(NaiveBallModule::BallState x) {
    printf("::BALL STATE::\nrel_xy: (%f, %f)\ndistance:%f\nbearing:%f\n", x.rel_x, x.rel_y, x.distance, x.bearing);
}

} // namespace man
} // namespace context
