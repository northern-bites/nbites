/**
 * Vision Ball class
 */

#include "VisualBall.h"
#include "math.h"

namespace man {
namespace vision {

VisualBall::VisualBall()
{
    init();
    framesOn = 0;
    framesOff = 0;
}

/* Initializes Variables every frame. Make sure before using any of these
 * variables, to check to see if the ball is in frame by checking distance
 * first (if it's > 0)
 */
void VisualBall::init() {
    // Main Variables
    width = 0;
    height = 0;
    radius = 0;
    centerX = 0;
    centerY = 0;
    angleX = 0;
    angleY = 0;
    distance = 0;
    bearing = 0;
    elevation = 0;
    heat = 0.0f;
    on = false;
}

void VisualBall::setDistanceEst(estimate ball_est)
{
    setBearingWithSD(ball_est.bearing);
    setElevation(ball_est.elevation);
    setDistanceWithSD(ball_est.dist);
}

void VisualBall::setDistanceWithSD(float _dist)
{
    distance = _dist;
    setDistanceSD(ballDistanceToSD(distance));
}


void VisualBall::setBearingWithSD(float _bearing)
{
    bearing = _bearing;
    setBearingSD(ballBearingToSD(_bearing));
}

}
}
