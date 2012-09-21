#include "RobotPosition.h"

RobotPosition::RobotPosition() : x(0),
                                 y(0),
                                 h(0),
                                 headYaw(0),
                                 headPitch(0)
{
}

void RobotPosition::moveRobot(float dx, float dy, float dh)
{
    x+=dx;
    y+=dy;
    h+=dy;
}

void RobotPosition::moveRobotTo(float xn, float yn, float hn)
{
    x=xn;
    y=yn;
    h=hn;
}

void RobotPosition::moveHead(float dy, float dp)
{
    headYaw+=dy;
    headPitch+=dp;
}

void RobotPosition::moveHeadTo(float y, float p)
{
    headYaw=y;
    headPitch=p;
}

