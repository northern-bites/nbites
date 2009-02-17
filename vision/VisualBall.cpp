/**
 * Vision Ball class
 */

#include "VisualBall.h"
#include "math.h"

Ball::Ball()
{
    init();
}

/* Initializes Variables every frame. Make sure before using any of these
 * variables, to check to see if the ball is in frame by checking distance
 * first (if it's > 0)
 */
void Ball::init() {
    // Main Variables
    width = 0;
    height = 0;
    radius = 0;
    centerX = 0;
    centerY = 0;
    angleX = 0;
    angleY = 0;
    focDist = 0;
    dist = 0;
    bearing = 0;
    elevation = 0;
}

void Ball::setDistanceEst(estimate ball_est)
{
    setBearingWithSD(ball_est.bearing);
    setElevation(ball_est.elevation);
    setDistanceWithSD(ball_est.dist);
}

void Ball::setDistanceWithSD(float _dist)
{
    dist = _dist;
    setDistanceSD(ballDistanceToSD(dist));
}


void Ball::setBearingWithSD(float _bearing)
{
    bearing = _bearing;
    setBearingSD(ballBearingToSD(_bearing));
}
