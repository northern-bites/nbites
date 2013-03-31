#pragma once

#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

#include "GameState.pb.h"
#include "BallModel.pb.h"
#include "LedCommand.pb.h"
#include "WorldModel.pb.h"

namespace man {
namespace behaviors {


class PyInterface
{
public:
    messages::GameState* gameState_ptr;
    messages::FilteredBall* filteredBall_ptr;
    messages::LedCommand* ledCommand_ptr;
    messages::WorldModel* worldModel_ptr[4]; //TODO: use a constant

    void setGameState_ptr(const messages::GameState* msg)
    {
        gameState_ptr = const_cast<messages::GameState*> (msg);
    }
    void setFilteredBall_ptr(const messages::FilteredBall* msg)
    {
        filteredBall_ptr = const_cast<messages::FilteredBall*> (msg);
    }
    void setLedCommand_ptr(const messages::LedCommand* msg)
    {
        ledCommand_ptr = const_cast<messages::LedCommand*> (msg);
    }
    void setWorldModel_ptr(const messages::WorldModel* msg,int i)
    {
        worldModel_ptr[i] = const_cast<messages::WorldModel*> (msg);
    }
    boost::python::list getWorldModelList(PyInterface interface)
    {
        boost::python::list list;
        for (int i=0; i<4; i++) {
            list.append(interface.worldModel_ptr[i]);
        }
        return list;
    }

};

void set_interface_ptr(boost::shared_ptr<PyInterface> ptr);
}
}
