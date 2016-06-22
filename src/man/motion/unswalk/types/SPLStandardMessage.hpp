#ifndef SPLSTANDARDMESSAGE_H
#define SPLSTANDARDMESSAGE_H

#include <stdint.h>
#include "types/BroadcastData.hpp"

#define SPL_STANDARD_MESSAGE_STRUCT_HEADER  "SPL "
#define SPL_STANDARD_MESSAGE_STRUCT_VERSION 5
#define SPL_STANDARD_MESSAGE_DATA_SIZE      800

namespace SPLStandardIntention {
  enum Intention {
    NOTHING,
    KEEPER,
    DEFENDER,
    STRIKER,
    LOST
  };
}

struct SPLStandardMessage 
{
  char header[4];        // "SPL "
  uint8_t version;       // has to be set to SPL_STANDARD_MESSAGE_STRUCT_VERSION
  uint8_t playerNum;     // 1-5
  uint8_t teamColor;     // 0 is blue, 1 is red 
  uint8_t fallen;        // 1 means that the robot is fallen, 0 means that the robot can play

  // position and orientation of robot
  // coordinates in millimeters
  // 0,0 is in centre of field
  // +ve x-axis points towards the goal we are attempting to score on
  // +ve y-axis is 90 degrees counter clockwise from the +ve x-axis
  // angle in radians, 0 along the +x axis, increasing counter clockwise
  float pose[3];      // x,y,theta
  
  // the robot's target position on the field
  // the coordinate system is the same as for the pose
  // if the robot does not have any target, this attribute should be set to the robot's position
  float walkingTo[2]; 
  
  // the target position of the next shot (either pass or goal shot)
  // the coordinate system is the same as for the pose
  // if the robot does not intend to shoot, this attribute should be set to the robot's position
  float shootingTo[2]; 

  // Ball information
  int32_t ballAge;        // milliseconds since this robot last saw the ball. -1 if we haven't seen it

  // position of ball relative to the robot
  // coordinates in millimeters
  // 0,0 is in centre of the robot
  // +ve x-axis points forward from the robot
  // +ve y-axis is 90 degrees counter clockwise from the +ve x-axis
  float ball[2];

  // velocity of the ball (same coordinate system as above)
  float ballVel[2];

  // describes what the robot intends to do:
  // 0 - nothing particular (default)
  // 1 - wants to be keeper
  // 2 - wants to play defense
  // 3 - wants to play the ball
  // 4 - robot is lost
  // (the second byte is a padding byte)
  uint16_t intention;
  
  // number of bytes that is actually used by the data array
  uint16_t numOfDataBytes;

  // buffer for arbitrary data, teams do not need to send more than specified in numOfDataBytes
  uint8_t data[SPL_STANDARD_MESSAGE_DATA_SIZE];

#ifdef __cplusplus
  // constructor
  SPLStandardMessage()
  {
    char header_string[5] = SPL_STANDARD_MESSAGE_STRUCT_HEADER;
    header[0] = header_string[0];
    header[1] = header_string[1];
    header[2] = header_string[2];
    header[3] = header_string[3];
    version = SPL_STANDARD_MESSAGE_STRUCT_VERSION;
    playerNum = 0;
    teamColor = 0;
    fallen = 0;
    pose[0] = 0.f;
    pose[1] = 0.f;
    pose[2] = 0.f;
    walkingTo[0] = 0.f;
    walkingTo[1] = 0.f;
    shootingTo[0] = 0.f;
    shootingTo[1] = 0.f;
    ballAge = -1;
    ball[0] = 0.f;
    ball[1] = 0.f;
    ballVel[0] = 0.f;
    ballVel[1] = 0.f;
    intention = 0;
    numOfDataBytes = 0;
  }

  SPLStandardMessage(const int &playerNum,
                     const int &teamColour,
                     const int &fallen,
                     const AbsCoord &robotPos,
                     const AbsCoord &walking,
                     const AbsCoord &shooting,
                     const int &ballAge,
                     const AbsCoord &ballPosition,
                     const AbsCoord &ballVelocity,
                     const SPLStandardIntention::Intention &intention,
                     const BroadcastData &broadcast)
     : playerNum(playerNum),
       teamColor(teamColour),
       fallen(fallen),
       ballAge(ballAge),
       intention(intention) {

      char header_string[5] = SPL_STANDARD_MESSAGE_STRUCT_HEADER;
      header[0] = header_string[0];
      header[1] = header_string[1];
      header[2] = header_string[2];
      header[3] = header_string[3];
      version = SPL_STANDARD_MESSAGE_STRUCT_VERSION;
      
      pose[0] = robotPos.x();
      pose[1] = robotPos.y();
      pose[2] = robotPos.theta();

      walkingTo[0] = walking.x();
      walkingTo[1] = walking.y();

      shootingTo[0] = shooting.x();
      shootingTo[1] = shooting.y();

      AbsCoord ballPosRR = ballPosition.convertToRobotRelativeCartesian(robotPos);
      AbsCoord ballVelRR = ballVelocity.convertToRobotRelativeCartesian(robotPos);
      ball[0] = ballPosRR.x();
      ball[1] = ballPosRR.y();
      ballVel[0] = ballVelRR.x();
      ballVel[1] = ballVelRR.y();

      // Everything else we need goes into the "data" section
      numOfDataBytes = sizeof(broadcast);
      memcpy(data, &broadcast, numOfDataBytes);
  }


#endif
};

#endif // SPLSTANDARDMESSAGE_H
