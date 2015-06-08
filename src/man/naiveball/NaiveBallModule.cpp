#include "NaiveBallModule.h"

namespace man {
namespace naive {

using messages::NaiveBall;
// using messages::BallModel;


struct NaiveBallModule::BallState
{
        BallState(){}
        BallState(float x, float y) : rel_x(x), rel_y(y), distance(0.f), bearing(0.f) {}
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
    velocityEst = 0.f;
    xVelocityEst = 0.f;
    yVelocityEst = 0.f;
    frameOffCount = 0;
    currentIndex = -1;

    direction = 0;
    position_buffer = new BallState[NUM_FRAMES];
    vel_x_buffer = new float[NUM_FRAMES];
    vel_y_buffer = new float[NUM_FRAMES];
    dest_buffer = new BallState[10];
    bufferFull = false;
    velBufferFull = false;
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
        updateBuffers(); }


    if (frameOffCount > MAX_FRAMES_OFF) {
        clearBuffers();
    } else if (bufferFull) {
        naiveCheck();
    }

    portals::Message<messages::NaiveBall> naiveBallMessage(0);

    naiveBallMessage.get()->clear_position();
    naiveBallMessage.get()->clear_dest_buffer();

    if (bufferFull) {
        int p = currentIndex + 1;
        for (int i = 0; i < NUM_FRAMES; i++) {
            p = p % NUM_FRAMES;
            NaiveBall::Position* temp = naiveBallMessage.get()->add_position();
            temp->set_x(position_buffer[p].rel_x);
            temp->set_y(position_buffer[p].rel_y);
            p++;
        }
    } else {
        for (int i = 0; i++; i < currentIndex) {
            NaiveBall::Position* temp = naiveBallMessage.get()->add_position();
            temp->set_x(position_buffer[i].rel_x);
            temp->set_y(position_buffer[i].rel_y);
        }
    }

    if (velBufferFull) {
        calcPath();
        for (int i = 0; i < 10; i++) {
            NaiveBall::Position* temp = naiveBallMessage.get()->add_dest_buffer();
            temp->set_x(dest_buffer[i].rel_x);
            temp->set_y(dest_buffer[i].rel_y);
        }
    }

    naiveBallMessage.get()->set_velocity(velocityEst);
    naiveBallMessage.get()->set_stationary(checkIfStationary());
    naiveBallMessage.get()->set_yintercept(yIntercept);
    naiveBallOutput.setMessage(naiveBallMessage);

}

// Add new ball observation to the buffer
void NaiveBallModule::updateBuffers()
{
    currentIndex = (currentIndex + 1) % NUM_FRAMES;
    position_buffer[currentIndex] = BallState(myBall.rel_x(), myBall.rel_y(), myBall.distance(), myBall.bearing());

    if (bufferFull) {
        vel_x_buffer[currentIndex] = calculateVelocity(true);
        vel_y_buffer[currentIndex] = calculateVelocity(false);
        if (currentIndex == (NUM_FRAMES - 1)) velBufferFull = true;
    }

    if (currentIndex == NUM_FRAMES - 1) {
        bufferFull = true;
        // bufferOfNSeconds[currentSecIndex] = avgFrames(currentIndex - AVGING_FRAMES);
        // currentSecIndex = (currentSecIndex + 1) % 10;
        // if (currentSecIndex == 9) { secBufferFull = true; }
    }
}

void NaiveBallModule::clearBuffers()
{
    currentIndex = -1;
    velocityEst = 0.f;
    xVelocityEst = 0.f;
    yVelocityEst = 0.f;

    bufferFull = false;
    velBufferFull = false;
    frameOffCount = 0;
}

float NaiveBallModule::calculateVelocity(bool x)
{
    int startIndex = currentIndex + 1;
    int endIndex = currentIndex - AVGING_FRAMES -1;
    if (endIndex < 0) endIndex = endIndex + NUM_FRAMES;
    BallState start_avgs = avgFrames(startIndex);
    BallState end_avgs = avgFrames(endIndex);
    xVelocityEst = end_avgs.rel_x - start_avgs.rel_x;
    yVelocityEst = end_avgs.rel_y - start_avgs.rel_y;

    float dist = calcSumSquaresSQRT((xVelocityEst), (yVelocityEst));
    float bearChange = end_avgs.bearing - start_avgs.bearing;
    if (bearChange < 0.0) { direction = -1.f; }
    else if (bearChange > 0.0) {direction = 1.f; }
    velocityEst = (direction * dist / 1.f) * ALPHA + velocityEst * (1-ALPHA);

    return (x ? xVelocityEst : yVelocityEst);
}

void NaiveBallModule::calcPath()
{
    float accx, accy;
    // If buffer full, calculate acceleration
    accx = vel_x_buffer[currentIndex] - vel_x_buffer[(currentIndex + 1) % NUM_FRAMES];
    accy = vel_y_buffer[currentIndex] - vel_y_buffer[(currentIndex + 1) % NUM_FRAMES];

    float t = .5;
    for (int i = 0; i < 10; i ++) {
        float x = (.5)*accx*t*t + vel_x_buffer[currentIndex]*t + position_buffer[currentIndex].rel_x;
        float y = (.5)*accy*t*t + vel_y_buffer[currentIndex]*t + position_buffer[currentIndex].rel_y;
        dest_buffer[i] = BallState(x, y);
        t += .5;
    }

    if (vel_x_buffer[currentIndex] < 0) {
        t = -position_buffer[currentIndex].rel_x / vel_x_buffer[currentIndex];
        yIntercept = vel_y_buffer[currentIndex]*t + position_buffer[currentIndex].rel_y;
    } else { yIntercept = 0.0; }

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
        x_sum += position_buffer[startingIndex].rel_x;
        y_sum += position_buffer[startingIndex].rel_y;
        dist_sum += position_buffer[startingIndex].distance;
        bearing_sum += position_buffer[startingIndex].bearing;

        startingIndex += 1;

    }

    return BallState(x_sum / AVGING_FRAMES, y_sum / AVGING_FRAMES, dist_sum / AVGING_FRAMES, bearing_sum / AVGING_FRAMES);
}





// ------------- Print functions for debugging ---------------

void NaiveBallModule::print()
{
    BallState x = position_buffer[currentIndex];
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
        printBallState(position_buffer[i]);
    }
}

} // namespace man
} // namespace context
