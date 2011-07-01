#include "VisualRobot.h"
VisualRobot::VisualRobot() : VisualDetection()
{
    init();
    framesOn = 0;
    framesOff = 0;
}

VisualRobot::VisualRobot(const VisualRobot& o) : VisualDetection(o)
{}

// Initialization, happens every frame.
void VisualRobot::init()
{
    width = 0;
    height = 0;
    setX(0);
    setY(0);
    centerX = 0;
    centerY = 0;
    angleX = 0;
    angleY = 0;
    focDist = 0;
    setDistance(0);
    setBearing(0);
    elevation = 0;
    on = false;
}

/**
 * Update the robot values from the blob
 *
 * @param b The blob to update our object from.
 */
void VisualRobot::updateRobot(Blob b)
{
    setLeftTopX(b.getLeftTopX());
    setLeftTopY(b.getLeftTopY());
    setLeftBottomX(b.getLeftBottomX());
    setLeftBottomY(b.getLeftBottomY());
    setRightTopX(b.getRightTopX());
    setRightTopY(b.getRightTopY());
    setRightBottomX(b.getRightBottomX());
    setRightBottomY(b.getRightBottomY());
    setX(b.getLeftTopX());
    setY(b.getLeftTopY());
    setWidth(dist(b.getRightTopX(), b.getRightTopY(), b.getLeftTopX(),
                       b.getLeftTopY()));
    setHeight(dist(b.getLeftTopX(), b.getLeftTopY(), b.getLeftBottomX(),
                        b.getLeftBottomY()));
    setCenterX(getLeftTopX() + ROUND2(getWidth() / 2));
    setCenterY(getRightTopY() + ROUND2(getHeight() / 2));
    setDistance(1);
}

/**
 * Calculate and set the standard deviation for the distance measurement.
 * Set the distance measurement.
 *
 * @param _distance the distance estimate to be set
 */
void VisualRobot::setDistanceWithSD(float _distance)
{
    setDistance(_distance);
    setDistanceSD(robotDistanceToSD(_distance));
}

/**
 * Calculate and set the standard deviation for the bearing measurement.
 * Set the bearing measurement.
 *
 * @param _bearing the distance estimate to be set
 */
void VisualRobot::setBearingWithSD(float _bearing)
{
    setBearing(_bearing);
    setBearingSD(robotBearingToSD(_bearing));
}
