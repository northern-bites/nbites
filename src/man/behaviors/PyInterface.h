#pragma once

#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

#include "Common.h"

#include "GameState.pb.h"
#include "RobotLocation.pb.h"
#include "BallModel.pb.h"
#include "LedCommand.pb.h"
#include "WorldModel.pb.h"
#include "PMotion.pb.h"
#include "MotionStatus.pb.h"
#include "SonarState.pb.h"
#include "VisionField.pb.h"
#include "VisionRobot.pb.h"
#include "FootBumperState.pb.h"

namespace man {
namespace behaviors {


class PyInterface
{
public:
    messages::GameState       const * gameState_ptr;
    messages::VisionRobot     const * visionRobot_ptr;
    messages::VisionField     const * visionField_ptr;
    messages::VisionObstacle  const * visionObstacle_ptr;
    messages::RobotLocation   const * robotLocation_ptr;
    messages::FilteredBall    const * filteredBall_ptr;
    messages::LedCommand      const * ledCommand_ptr;
    messages::WorldModel      const * worldModel_ptr[NUM_PLAYERS_PER_TEAM];
    messages::MotionCommand   const * motionCommand_ptr;
    messages::MotionStatus    const * motionStatus_ptr;
    messages::SonarState      const * sonarState_ptr;
    messages::FootBumperState const * footBumperState_ptr;

    void setGameState_ptr(const messages::GameState* msg)
    {
        gameState_ptr = msg;
    }
    void setVisionRobot_ptr(const messages::VisionRobot* msg)
    {
        visionRobot_ptr = msg;
    }
    void setVisionObstacle_ptr(const messages::VisionObstacle* msg)
    {
        visionObstacle_ptr = msg;
    }
    void setVisionField_ptr(const messages::VisionField* msg)
    {
        visionField_ptr = msg;
    }
    void setRobotLocation_ptr(const messages::RobotLocation* msg)
    {
        robotLocation_ptr = msg;
    }
    void setFilteredBall_ptr(const messages::FilteredBall* msg)
    {
        filteredBall_ptr = msg;
    }
    void setLedCommand_ptr(const messages::LedCommand* msg)
    {
        ledCommand_ptr = msg;
    }
    void setWorldModel_ptr(const messages::WorldModel* msg,int i)
    {
        worldModel_ptr[i] = msg;
    }
    boost::python::list getWorldModelList()//PyInterface interface)
    {
        boost::python::list list;
        for (int i=0; i<NUM_PLAYERS_PER_TEAM; i++) {
            list.append(worldModel_ptr[i]);
        }
        return list;
    }
    void setMotionCommand_ptr(const messages::MotionCommand* msg)
    {
        motionCommand_ptr =  msg;
    }
    void setMotionStatus_ptr(const messages::MotionStatus* msg)
    {
        motionStatus_ptr = msg;
    }
    void setSonarState_ptr(const messages::SonarState* msg)
    {
        sonarState_ptr = msg;
    }
    void setFootBumperState_ptr(const messages::FootBumperState* msg)
    {
        footBumperState_ptr = msg;
    }

};

void set_interface_ptr(boost::shared_ptr<PyInterface> ptr);
}
}
