/**
 * Top-Level class for the Northern-bites Comm Module.
 * @author Wils Dawson and Josh Zalinger 5/14/12
 */

#include "CommModule.h"

#include <iostream>
#include <time.h>
#include <sys/time.h>

#include "Profiler.h"
#include "DebugConfig.h"

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
    _worldModelInput.setMessage(model);

    portals::Message<messages::GCResponse> response(0);
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
}

void CommModule::run_()
{
    PROF_ENTER(P_COMM_RECEIVE);
    receive();
    PROF_EXIT(P_COMM_RECEIVE);

    teamConnect->checkDeadTeammates(_worldModels,
                                    timer->timestamp(),
                                    myPlayerNumber());

    burstRate = monitor->performHealthCheck(timer->timestamp());

#ifdef LOG_COMM
    monitor->logOutput(timer->timestamp());
#endif

    PROF_ENTER(P_COMM_SEND);
    if (timer->timeToSend() && myPlayerNumber() > 0)
    {
        send();
    }
    else
    {
// Make profiler happy. SO UGLY!!!
PROF_ENTER(P_COMM_BUILD_PACKET);
PROF_EXIT(P_COMM_BUILD_PACKET);
PROF_ENTER(P_COMM_SERIALIZE_PACKET);
PROF_EXIT(P_COMM_SERIALIZE_PACKET);
PROF_ENTER(P_COMM_TO_SOCKET);
PROF_EXIT(P_COMM_TO_SOCKET);
PROF_ENTER(P_COMM_TIMER);
PROF_EXIT(P_COMM_TIMER);
    }
    PROF_EXIT(P_COMM_SEND);
}

void CommModule::send()
{
    _worldModelInput.latch();

    teamConnect->send(_worldModelInput.message(), myPlayerNumber(),
                      gameConnect->myTeamNumber(), burstRate);
    PROF_ENTER(P_COMM_TIMER);
    timer->teamPacketSent();
    PROF_EXIT(P_COMM_TIMER);
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
