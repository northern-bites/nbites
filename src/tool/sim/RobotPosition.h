/*
 * @class RobotPosition
 *
 * Describes the robot's location and head position; very simple storage
 * class.
 *
 * @author Lizzie Mamantov
 */

#pragma once

#include "WorldConstants.h"

namespace tool{
namespace visionsim{

class RobotPosition {
public:
    RobotPosition() : x(0), y(0), h(0), headYaw(0), headPitch(0) {};
    ~RobotPosition() {};

    // Location control
    void setX(float xn) { x = xn; }
    void setY(float yn) { y = yn; }
    void setH(float hn) { h = hn; }
    float getX() { return x; }
    float getY() { return y; }
    float getH() { return h; }

    // Head control
    void setYaw(float y) { headYaw  = y; }
    void setPitch(float p) { headPitch = p; }
    float getYaw() { return headYaw; }
    float getPitch() { return headPitch; }

private:
    float x;
    float y;
    float h;
    float headYaw;
    float headPitch;
};

}
}
