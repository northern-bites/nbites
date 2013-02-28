/**
 * Top-Level class for the Northern-bites Comm Module.
 * @author Wils Dawson and Josh Zalinger 5/14/12
 */

#include "CommModule.h"

#include <iostream>
#include <time.h>
#include <sys/time.h>

#include "Profiler.h"

namespace man {

namespace comm {

CommModule::CommModule(int team, int player) :
    portals::Module(),
    _gameStateOutput(base()),
    _myPlayerNumber(player)
{
    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; ++i)
    {
        _worldModels[i] = new portals::OutPortal<messages::WorldModel>(base());
    }

    timer = new CommTimer(&monotonic_micro_time);
    monitor = new NetworkMonitor(timer->timestamp());

    teamConnect = new TeamConnect(timer, monitor);
    gameConnect = new GameConnect(timer, monitor, team, player);

    portals::Message<messages::WorldModel> model(0);
    *model.get() = messages::WorldModel();
    _worldModelInput.setMessage(model);

    portals::Message<messages::GCResponse> response(0);
    *response.get() = messages::GCResponse();
    _gcResponseInput.setMessage(response);
}

CommModule::~CommModule()
{
    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; ++i)
    {
        delete _worldModels[i];
    }

    delete monitor;
    delete timer;
    delete teamConnect;
    delete gameConnect;
    std::cout << "Comm Destructed" << std::endl;
}

void CommModule::run_()
{
    PROF_ENTER(P_COMM);

    receive();

    teamConnect->checkDeadTeammates(_worldModels,
                                    timer->timestamp(),
                                    myPlayerNumber());

    burstRate = monitor->performHealthCheck(timer->timestamp());

    monitor->logOutput(timer->timestamp());

    if (timer->timeToSend() && myPlayerNumber() > 0)
    {
        send();
    }

    PROF_EXIT(P_COMM);
}

void CommModule::send()
{
    _worldModelInput.latch();
    messages::WorldModel message;

    teamConnect->send(_worldModelInput.message(), myPlayerNumber(),
                      gameConnect->myTeamNumber(), burstRate);

    timer->teamPacketSent();
}

void CommModule::receive()
{
    teamConnect->receive(_worldModels, 0, gameConnect->myTeamNumber());

    gameConnect->handle(_gameStateOutput, _gcResponseInput, myPlayerNumber());
}

int CommModule::checkPlayerNumber(int p)
{
    int player = p ? p : myPlayerNumber();
    if (player <= 0)
    {
        std::cerr << "Something is horribly wrong in Comm..." << std::endl;
    }
    return player;
}

void CommModule::setTeamNumber(int tn)
{
    gameConnect->setMyTeamNumber(tn, myPlayerNumber());
}

int CommModule::teamNumber()
{
    return gameConnect->myTeamNumber();
}

}
}
