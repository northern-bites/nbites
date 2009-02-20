#include "VisualBackstop.h"
VisualBackstop::VisualBackstop()
{
    init();
}

VisualBackstop::VisualBackstop(const VisualBackstop&) {}

// Initialization, happens every frame.
void VisualBackstop::init()
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
}

/**
 * Calculate and set the standard deviation for the distance measurement.
 * Set the distance measurement.
 *
 * @param _distance the distance estimate to be set
 */
void VisualBackstop::setDistanceWithSD(float _distance)
{
    setDistance(_distance);
    setDistanceSD(backstopDistanceToSD(_distance));
}

/**
 * Calculate and set the standard deviation for the bearing measurement.
 * Set the bearing measurement.
 *
 * @param _bearing the distance estimate to be set
 */
void VisualBackstop::setBearingWithSD(float _bearing)
{
    setBearing(_bearing);
    setBearingSD(backstopBearingToSD(_bearing));
}

/*
 * As we saw with beacons, we tend to work with blobs for convenience.  So at some point
 * we need to transfer their contents over to the field object that we have identified.
 * @param b    the blob that contains the information we need
 */
void VisualBackstop::updateBackstop(blob * b)
{
    setLeftTopX(b->leftTop.x);
    setLeftTopY(b->leftTop.y);
    setLeftBottomX(b->leftBottom.x);
    setLeftBottomY(b->leftBottom.y);
    setRightTopX(b->rightTop.x);
    setRightTopY(b->rightTop.y);
    setRightBottomX(b->rightBottom.x);
    setRightBottomY(b->rightBottom.y);
    setX(b->leftTop.x);
    setY(b->leftTop.y);
    setWidth(dist(b->rightTop.x, b->rightTop.y, b->leftTop.x, b->leftTop.y));
    setHeight(dist(b->leftTop.x, b->leftTop.y,
                   b->leftBottom.x, b->leftBottom.y));
    setCenterX(getLeftTopX() + ROUND2(getWidth() / 2));
    setCenterY(getRightTopY() + ROUND2(getHeight() / 2));
    setDistance(1);
}
