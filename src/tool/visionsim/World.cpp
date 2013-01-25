#include "World.h"

namespace tool{
namespace visionsim{

// Moves the robot by the desired displacement
void World::moveRobot(float dx, float dy, float dh)
{
    robot.setX(robot.getX()+dx);
    robot.setY(robot.getX()+dy);
    robot.setH(robot.getX()+dh);
}

// Moves the robot to the desired location
void World::moveRobotTo(float xn, float yn, float hn)
{
    robot.setX(xn);
    robot.setY(yn);
    robot.setH(hn);
}

// Moves the robot's head position by the desired displacement
void World::moveHead(float dy, float dp)
{
    robot.setYaw(robot.getYaw()+dy);
    robot.setPitch(robot.getPitch()+dp);
}

// Moves the robot's head position to the desired position
void World::moveHeadTo(float y, float p)
{
    robot.setYaw(y);
    robot.setPitch(p);
}

// Moves the ball by the desired displacement
void World::moveBall(float dx, float dy)
{
    ball[X_VALUE] = ball[X_VALUE] + dx;
    ball[Y_VALUE] = ball[Y_VALUE] + dy;
}

// Moves the ball by to the desired location
void World::moveBallTo(float x, float y)
{
    ball[X_VALUE] = x;
    ball[Y_VALUE] = y;
}

}
}
