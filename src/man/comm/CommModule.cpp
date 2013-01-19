// This file is part of Man, a robotic perception, locomotion, and
// team strategy application created by the Northern Bites RoboCup
// team of Bowdoin College in Brunswick, Maine, for the Aldebaran
// Nao robot.
//
// Man is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Man is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.

/**
 * Top-Level class for the Northern-bites Comm Module.
 * @author Wils Dawson and Josh Zalinger 5/14/12
 */

#include "CommModule.h"

#include <iostream>
#include <time.h>
#include <sys/time.h>  //TODO: Do we need for Robots?

#include "Profiler.h"
#include "Common.h"

namespace man {

namespace comm {

CommModule::CommModule()
{
    timer = new CommTimer(&monotonic_micro_time);
    monitor = new NetworkMonitor(timer->timestamp());

    teamConnect = new TeamConnect(timer, monitor);
    gameConnect = new GameConnect(timer, monitor);

    std::cout << "Comm Constructed" << std::endl;
}

CommModule::~CommModule()
{
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

    // Update teammates.
    teamConnect->checkDeadTeammates(timer->timestamp(), myPlayerNumber());

    // Update the monitor.
    burstRate = monitor->performHealthCheck(timer->timestamp());

    monitor->logOutput(timer->timestamp());

    if (timer->timeToSend() && myPlayerNumber() > 0)
        send();

    PROF_EXIT(P_COMM);
}

void CommModule::send()
{
    teamConnect->send(myPlayerNumber(), gameConnect->myTeamNumber(), burstRate);
    timer->teamPacketSent();
}

void CommModule::receive()
{
    teamConnect->receive(0, gameConnect->myTeamNumber());

    gameConnect->handle(myPlayerNumber());
}

GameData CommModule::getGameData()
{
    return *(gameConnect->getGameData());
}

TeamMember CommModule::getTeammate(int player)
{
    return *(teamConnect->getTeamMate(player));
}

int CommModule::checkPlayerNumber(int p)
{
    int player = p ? p : myPlayerNumber();
    if (player <= 0)
    {
        std::cout << "Trying to set Comm data with bad player number!\n"
                  << "Set Comm's player number through brain first!" << std::endl;
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
