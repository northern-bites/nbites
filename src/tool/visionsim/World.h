/*
 * Keeps track of the things that move in the world.
 */

#pragma once

#include <Eigen/Dense>
#include "RobotPosition.h"

namespace tool{
namespace visionsim{

typedef Eigen::Vector3f PhysicalBall;

class World{
public:
    World() : ball(0, 0, BALL_RADIUS), robot() {};
    ~World() {};

    float ballX() { return ball[X_VALUE]; };
    float ballY() { return ball[Y_VALUE]; };
    float ballZ() { return ball[Z_VALUE]; };

    float robotX() { return robot.getX(); }
    float robotY() { return robot.getY(); }
    float robotH() { return robot.getH(); }

    float headYaw() { return robot.getYaw(); }
    float headPitch() { return robot.getPitch(); }

    void moveRobot(float dx, float dy, float dh);
    void moveRobotTo(float xn, float yn, float hn);

    void moveHead(float dy, float dp);
    void moveHeadTo(float y, float p);

    void moveBall(float dx, float dy);
    void moveBallTo(float x, float y);

private:
    PhysicalBall ball;
    RobotPosition robot;
};

}
}
