#include "NaiveBallModule.h"

namespace man {
namespace naive {

using messages::NaiveBall;
// using messages::BallModel;


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
    xVelocityEst = 0.f;
    yVelocityEst = 0.f;
    ballDestX = 0.f;
    ballDestY = 0.f;
    frameOffCount = 0;
    currentIndex = 0;
    currentSecIndex = 0;
    secCounter = 0;
    direction = 0;
    ballStateBuffer = new BallState[NUM_FRAMES];
    bufferOfNSeconds = new BallState[10];
    bufferFull = false;
    secBufferFull = false;
    yIntercept = 0.f;
}

NaiveBallModule::~NaiveBallModule()
{
}

void NaiveBallModule::run_()
{
    // Add observation (ball distance/position, ball bearing) to buffer

    // get myInfo:
    ballIn.latch();
    myBall = ballIn.message();

    // If can see the ball, update the buffer
    // Make sure no weird ball estimates
    if (myBall.vis().on() && myBall.vis().distance() > 5 && myBall.vis().distance() < 800) {
        updateBuffer();
    } else { frameOffCount++; }

    if (frameOffCount > MAX_FRAMES_OFF) {
        clearBuffer();
    } else if (bufferFull) {
        naiveCheck();
    }

    //printf("Velocity: %f\n", velocityEst);
    //printf("yIntercept: %f\n", yIntercept);
    // checkIfStationary() ? printf("Stationary\n") : printf("moving\n");

    portals::Message<messages::NaiveBall> naiveBallMessage(0);

    naiveBallMessage.get()->clear_position();
    naiveBallMessage.get()->clear_sec_buffer();
    if (bufferFull) {
        printf("Buffer is full; setting the position");
        int p = currentIndex;
        for (int i = 0; i < NUM_FRAMES; i++) {
            p = p % NUM_FRAMES;
            NaiveBall::Position* temp = naiveBallMessage.get()->add_position();
            temp->set_x(ballStateBuffer[p].rel_x);
            temp->set_y(ballStateBuffer[p].rel_y);
            p++;
        }
    }
    if (secBufferFull) {
        printf("Sec buffer full, writing\n");
        int p = currentSecIndex;
        for (int i = 0; i < 10; i++) {
            p = p % 10;
            NaiveBall::Position* temp = naiveBallMessage.get()->add_sec_buffer();
            temp->set_x(bufferOfNSeconds[p].rel_x);
            temp->set_y(bufferOfNSeconds[p].rel_y);
            p++;
        }
    }

    naiveBallMessage.get()->set_velocity(velocityEst);
    naiveBallMessage.get()->set_stationary(checkIfStationary());
    naiveBallMessage.get()->set_yintercept(yIntercept);
    naiveBallOutput.setMessage(naiveBallMessage);

}

// Add new ball observation to the buffer
void NaiveBallModule::updateBuffer()
{
    currentIndex = (currentIndex + 1) % NUM_FRAMES;
    ballStateBuffer[currentIndex] = BallState(myBall.rel_x(), myBall.rel_y(), myBall.distance(), myBall.bearing());
    if (currentIndex == NUM_FRAMES - 1) {
        bufferFull = true;
        bufferOfNSeconds[currentSecIndex] = avgFrames(currentIndex - AVGING_FRAMES);
        currentSecIndex = (currentSecIndex + 1) % 10;
        if (currentSecIndex == 9) { secBufferFull = true; }
    }
}

void NaiveBallModule::clearBuffer()
{
    currentIndex = 0;
    currentSecIndex = 0;
    velocityEst = 0.f;
    bufferFull = false;
    secBufferFull = false;
    frameOffCount = 0;
}

// Check if ball is moving, and if so, in what direction, how fast, etc.
void NaiveBallModule::naiveCheck()
{
    // Get average of first and last x number of frames to get a position estimate
    // at beginning and end
    int startIndex = currentIndex+1;
    int endIndex = currentIndex - AVGING_FRAMES - 1;
    if (endIndex < 0) endIndex = endIndex + NUM_FRAMES;

    BallState start_avgs = avgFrames(startIndex);
    BallState end_avgs = avgFrames(endIndex);
    xVelocityEst = end_avgs.rel_x - start_avgs.rel_x;
    yVelocityEst = end_avgs.rel_y - start_avgs.rel_y;

    float dist = calcSumSquaresSQRT((xVelocityEst), (yVelocityEst));
    float bearChange = end_avgs.bearing - start_avgs.bearing;

    if (bearChange < 0.0) { direction = -1; }
    else if (bearChange > 0.0) {direction = 1; }
    velocityEst = (direction * dist / 1.f) * ALPHA + velocityEst * (1-ALPHA);

    if (xVelocityEst < 0.f && !checkIfStationary()) { naivePredict(end_avgs); }

    // velocityEst = (dist / 1.f);
    // TODO possibly take into account (possibly -as in probably) previous estimates

}

void NaiveBallModule::naivePredict(NaiveBallModule::BallState b)
{
    // predict at what y position the ball will hit the robot's x==0
    float x = b.rel_x;
    float y = b.rel_y;
    float x_dest = 5.f; // want to see when ball is at 0 TODO probably do something higher

    float t = (x_dest - x) / xVelocityEst; // calculate time until ball is at dest

    yIntercept = y + t * yVelocityEst;

// --------------------


// ----------------



    //printf("I think my yIntercept is: %f\n", yIntercept);

    // if(stationary)
    // {
    //     relXDest = x(0);
    //     relyIntercept = x(1);
    // }
    // else // moving
    // {
    //     float speed = getSpeed();

    //     //Calculate time until stop
    //     float timeToStop = std::abs(speed / params.ballFriction);

    //     //Calculate deceleration in each direction
    //     float decelX = (x(2)/speed) * params.ballFriction;
    //     float decelY = (x(3)/speed) * params.ballFriction;

    //     // Calculate end position
    //     relXDest = x(0) + x(2)*timeToStop + .5f*decelX*timeToStop*timeToStop;
    //     relyIntercept = x(1) + x(3)*timeToStop + .5f*decelY*timeToStop*timeToStop;

    //     //Calculate the time until intersects with robots y axis
    //     float timeToIntersect = NBMath::getLargestMagRoot(x(0),x(2),.5f*decelX);
    //     // Use quadratic :(
    //     relYIntersectDest = x(1) + x(3)*timeToStop + .5f*decelY*timeToStop*timeToStop;
    // }

}

bool NaiveBallModule::checkIfStationary()
{
    if (xVelocityEst < STATIONARY_THRESHOLD && yVelocityEst < STATIONARY_THRESHOLD) { return true; }
    return false;
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
        startingIndex = startingIndex % NUM_FRAMES;
        x_sum += ballStateBuffer[startingIndex].rel_x;
        y_sum += ballStateBuffer[startingIndex].rel_y;
        dist_sum += ballStateBuffer[startingIndex].distance;
        bearing_sum += ballStateBuffer[startingIndex].bearing;

        startingIndex += 1;

    }

    return BallState(x_sum / AVGING_FRAMES, y_sum / AVGING_FRAMES, dist_sum / AVGING_FRAMES, bearing_sum / AVGING_FRAMES);
}

// ------------- Print functions for debugging ---------------

void NaiveBallModule::print()
{
    BallState x = ballStateBuffer[currentIndex];
    printf("%s\n", bufferFull ? "BufferFull" : "Buffer not full");
    printf("Current ball state:\nrel_xy: (%f, %f)\ndistance:%f\nbearing:%f\n", x.rel_x, x.rel_y, x.distance, x.bearing);
    printf("VelocityEst = %f cm/sec\n", velocityEst);
    if (velocityEst > 0.f) {
        printf("BALL IS MOVING\n");
    } else {
        printf("NOT moving\n");
    }
}

void NaiveBallModule::printBallState(NaiveBallModule::BallState x) {
    printf("::BALL STATE::\nrel_xy: (%f, %f)\ndistance:%f\nbearing:%f\n", x.rel_x, x.rel_y, x.distance, x.bearing);
}

void NaiveBallModule::printBuffer() {
    printf("WHOLE BUFFER: \n");
    for (int i = 0; i < NUM_FRAMES; i++) {
        printf("i = %i, value = ", i);
        printBallState(ballStateBuffer[i]);
    }
}

} // namespace man
} // namespace context
