/**
 * Vision Ball class
 */

#include "Ball.h"
#include "math.h"

Ball::Ball(Vision *vis)
{
    vision = vis; // stores vision class pointer
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



void Ball::setDistance()
{
    // translate distance estimates to body center
    estimate ball_est;
    ball_est = vision->pose->bodyEstimate(getCenterX(),getCenterY(),
                                          static_cast<float>(focDist));
    setBearing(ball_est.bearing);
    setElevation(ball_est.elevation);
    dist = ball_est.dist;

    // Calcualate and set standard deviations
    setDistanceSD(ballDistanceToSD(dist));
    setBearingSD(ballBearingToSD(ball_est.bearing));
}



