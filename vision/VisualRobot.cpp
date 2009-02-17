#include "VisualRobot.h"
VisualRobot::VisualRobot() : {
    init();
}

VisualRobot::VisualRobot(const VisualRobot&) {
}

// Initialization, happens every frame.
void VisualRobot::init(){
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
    idCertainty = NOT_SURE;
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
    setDistanceSD(backstopDistanceToSD(_distance));
    }
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
    setBearingSD(backstopBearingToSD(_bearing));
    }
}
