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
    stationaryOffFrameCount = 0;

    position_buffer = new BallState[NUM_FRAMES];
    vel_x_buffer = new float[NUM_FRAMES];
    vel_y_buffer = new float[NUM_FRAMES];
    dest_buffer = new BallState[NUM_DEST];
    bufferFull = false;
    velBufferFull = false;
    yIntercept = 0.f;
}

NaiveBallModule::~NaiveBallModule()
{
}

void NaiveBallModule::run_()
{
    ballIn.latch();
    myBall = ballIn.message();

    // If can see the ball, update the buffer
    // Make sure no weird ball estimates
    if (myBall.vis().on() && myBall.vis().distance() > 5 && myBall.vis().distance() < 800) {
        updateBuffers(); }
    else frameOffCount++;

    if (frameOffCount > MAX_FRAMES_OFF) clearBuffers();

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
        for (int i = 0; i < NUM_DEST; i++) {
            NaiveBall::Position* temp = naiveBallMessage.get()->add_dest_buffer();
            temp->set_x(dest_buffer[i].rel_x);
            temp->set_y(dest_buffer[i].rel_y);
        }
    }

    if (checkIfStationary() == true) {
        naiveBallMessage.get()->set_stationary(true);
        yIntercept = 0.f;
    }
    else {
        naiveBallMessage.get()->set_stationary(false);
    }

    naiveBallMessage.get()->set_velocity(velocityEst);
    naiveBallMessage.get()->set_yintercept(yIntercept);
    naiveBallMessage.get()->set_x_vel(vel_x_buffer[currentIndex]);
    naiveBallMessage.get()->set_y_vel(vel_y_buffer[currentIndex]);
    naiveBallOutput.setMessage(naiveBallMessage);

}

// Add new ball observation to the buffer
void NaiveBallModule::updateBuffers()
{
    currentIndex = (currentIndex + 1) % NUM_FRAMES;
    position_buffer[currentIndex] = BallState(myBall.rel_x(), myBall.rel_y(), myBall.distance(), myBall.bearing());

    if (bufferFull) {
        calculateVelocity();
        vel_x_buffer[currentIndex] = xVelocityEst;
        vel_y_buffer[currentIndex] = yVelocityEst;
        if (currentIndex == (NUM_FRAMES - 1)) velBufferFull = true;
    }

    if (currentIndex == NUM_FRAMES - 1) bufferFull = true;
}

void NaiveBallModule::clearBuffers()
{
    currentIndex = -1;
    velocityEst = 0.f;
    xVelocityEst = 0.f;
    yVelocityEst = 0.f;
    yIntercept = 0.f;
    bufferFull = false;
    velBufferFull = false;
    frameOffCount = 0;
    stationaryOffFrameCount = 0;
}

void NaiveBallModule::calculateVelocity()
{
    int startIndex = currentIndex + 1;
    int endIndex = currentIndex - AVGING_FRAMES -1;
    if (endIndex < 0) endIndex = endIndex + NUM_FRAMES;
    BallState start_avgs = avgFrames(startIndex);
    BallState end_avgs = avgFrames(endIndex);
    float denominator = (float)NUM_FRAMES / 30.f;
    xVelocityEst = (end_avgs.rel_x - start_avgs.rel_x) / denominator * ALPHA + xVelocityEst * (1 - ALPHA);
    yVelocityEst = (end_avgs.rel_y - start_avgs.rel_y) / denominator * ALPHA + yVelocityEst * (1 - ALPHA);

    float dist = calcSumSquaresSQRT((xVelocityEst), (yVelocityEst));
    velocityEst = (dist / denominator) * ALPHA + velocityEst * (1-ALPHA);
}

void NaiveBallModule::calcPath()
{
    // float accx, accy;
    // accx = vel_x_buffer[currentIndex] - vel_x_buffer[(currentIndex + 1) % NUM_FRAMES];
    // accy = vel_y_buffer[currentIndex] - vel_y_buffer[(currentIndex + 1) % NUM_FRAMES];

    // accx = (accx < 0.f ? accx * -FRICTION : accx * FRICTION);
    // accy = (accy < 0.f ? accy * -FRICTION : accy * FRICTION);

    float t = .5;
    for (int i = 0; i < NUM_DEST; i ++) {
        // float x = (.5)*accx*t*t + vel_x_buffer[currentIndex]*t + position_buffer[currentIndex].rel_x;
        // float y = (.5)*accy*t*t + vel_y_buffer[currentIndex]*t + position_buffer[currentIndex].rel_y;
        float x = pow(.9, i) * vel_x_buffer[currentIndex]*t + position_buffer[currentIndex].rel_x;
        float y = pow(.9, i) * vel_y_buffer[currentIndex]*t + position_buffer[currentIndex].rel_y;
        // float yvel = (vel_y_buffer[currentIndex] < 0.f ? vel_y_buffer[currentIndex] * -FRICTION : vel_y_buffer[currentIndex] * FRICTION);
        // float xvel = (vel_x_buffer[currentIndex] < 0.f ? vel_x_buffer[currentIndex] * -FRICTION : vel_x_buffer[currentIndex] * FRICTION);
        // float x = yvel*t + position_buffer[currentIndex].rel_x;
        // float y = xvel*t + position_buffer[currentIndex].rel_y;
        dest_buffer[i] = BallState(x, y);
        t += .5;
    }

    // Check if ball is coming towards robot, then calculate where it will intercept robot's ya-xis
    if (velBufferFull && vel_x_buffer[currentIndex] < 0) {
        // int p = (currentIndex == 0) ? NUM_FRAMES : currentIndex-1;
        // float m = (position_buffer[currentIndex].rel_x - position_buffer[p].rel_x) / (position_buffer[currentIndex].rel_y - position_buffer[p].rel_y);
        // float b = position_buffer[p].rel_x - m*position_buffer[p].rel_y;
        // yIntercept = -b / m;

        t = -position_buffer[currentIndex].rel_x / vel_x_buffer[currentIndex];
        if (yIntercept == 0.f) yIntercept = vel_y_buffer[currentIndex]*t + position_buffer[currentIndex].rel_y;
        else yIntercept = (vel_y_buffer[currentIndex]*t + position_buffer[currentIndex].rel_y)*ALPHA + (1-ALPHA)*(yIntercept);

    } else { yIntercept = 0.f; }

}


// Is in motion if checkIfStationary returns false x times in a row
bool NaiveBallModule::checkIfStationary()
{
    if (fabs(xVelocityEst) < STATIONARY_THRESHOLD && fabs(yVelocityEst) < STATIONARY_THRESHOLD) {
        stationaryOffFrameCount = 0;    // Reset to 0
        return true; }
    stationaryOffFrameCount++;
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
