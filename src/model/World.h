/*
 * Keeps track of the things that move in the world.
 */

#pragma once

#include <Eigen/Dense>
#include "RobotPosition.h"

typedef Eigen::Vector3f PhysicalBall;

static const float BALL_DIAMETER = 6.5f;
static const float BALL_RADIUS   = 0.5f * BALL_DIAMETER;

class World{
public:
    World() : ball(0, 0, BALL_RADIUS), robot() {};
    ~World() {};

    float ballX() { return ball[0]; };
    float ballY() { return ball[1]; };
    float ballZ() { return ball[2]; };

    float robotX() { return robot.getX(); }
    float robotY() { return robot.getY(); }
    float robotH() { return robot.getH(); }

    float headYaw() { return robot.getYaw(); }
    float headPitch() { return robot.getPitch(); }

private:
    PhysicalBall ball;
    RobotPosition robot;
};
