#pragma once

#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

#include "Common.h"

#include "GameState.pb.h"
#include "BallModel.pb.h"
#include "LedCommand.pb.h"
#include "WorldModel.pb.h"
#include "PMotion.pb.h"
#include "VisionField.pb.h"
#include "VisionRobot.pb.h"

namespace man {
namespace behaviors {


class PyInterface
{
public:
    messages::GameState     const * gameState_ptr;
    messages::VisionBall    const * visionBall_ptr;
    messages::VisionRobot   const * visionRobot_ptr;
    messages::VisionField   const * visionField_ptr;
    messages::FilteredBall  const * filteredBall_ptr;
    messages::LedCommand    const * ledCommand_ptr;
    messages::WorldModel    const * worldModel_ptr[NUM_PLAYERS_PER_TEAM];
    messages::MotionCommand const * motionCommand_ptr;

    void setGameState_ptr(const messages::GameState* msg)
    {
        gameState_ptr = msg;
    }
    void setVisionBall_ptr(const messages::VisionBall* msg)
    {
        visionBall_ptr = msg;
    }
    void setVisionRobot_ptr(const messages::VisionRobot* msg)
    {
        visionRobot_ptr = msg;
    }
    void setVisionField_ptr(const messages::VisionField* msg)
    {
        visionField_ptr = msg;
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
    boost::python::list getWorldModelList(PyInterface interface)
    {
        boost::python::list list;
        for (int i=0; i<4; i++) {
            list.append(interface.worldModel_ptr[i]);
        }
        return list;
    }
    void setMotionCommand_ptr(const messages::MotionCommand* msg)
    {
        motionCommand_ptr =  msg;
    }

};

void set_interface_ptr(boost::shared_ptr<PyInterface> ptr);
}
}
