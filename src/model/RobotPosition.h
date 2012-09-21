/*
 * Describes the robot's location and head position.
 */

#pragma once

// From NAO documentation
static const float MAX_HEAD_PITCH =   29.5f;
static const float MIN_HEAD_PITCH =  -38.5f;
static const float MAX_HEAD_YAW   =  119.5f;
static const float MIN_HEAD_YAW   = -119.5f;

class RobotPosition {
public:
    RobotPosition();
    ~RobotPosition() {};

    // Location control
    void setX(float xn) { x = xn; }
    void setY(float yn) { y = yn; }
    void setH(float hn) { h = hn; }

    float getX() { return x; }
    float getY() { return y; }
    float getH() { return h; }

    void moveRobot(float dx, float dy, float dh);
    void moveRobotTo(float xn, float yn, float hn);

    // Head control
    void setYaw(float y) { headYaw  = y; }
    void setPitch(float p) { headPitch = p; }
    float getYaw() { return headYaw; }
    float getPitch() { return headPitch; }

    void moveHead(float dy, float dp);
    void moveHeadTo(float y, float p);

private:
    float x;
    float y;
    float h;
    float headYaw;
    float headPitch;
};
