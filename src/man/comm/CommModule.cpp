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

#include "Comm.h"

#include <iostream>
#include <time.h>
#include <sys/time.h>  //TODO: Do we need for Robots?

#include "Profiler.h"
#include "Common.h"

Comm::Comm()
  : Thread("Comm")
{
    timer = new CommTimer(&monotonic_micro_time);
    monitor = new NetworkMonitor(timer->timestamp());

    teamConnect = new TeamConnect(timer, monitor);
    gameConnect = new GameConnect(timer, monitor);

    pthread_mutex_init(&comm_mutex, NULL);

    std::cout << "Comm Constructed" << std::endl;
}

Comm::~Comm()
{
    std::cout << "Comm destructor" << std::endl;
    delete monitor;
    delete timer;
    delete teamConnect;
    delete gameConnect;
    pthread_mutex_destroy(&comm_mutex);
}

int  Comm::start()
{
    return Thread::start();
}

void Comm::stop()
{
    Thread::stop();
}

void Comm::run()
{
    llong lastMonitorOutput = timer->timestamp();
    // end ??? section

    while(running)
    {
        PROF_ENTER(P_COMM);

        receive();

        // Update teammates.
        teamConnect->checkDeadTeammates(timer->timestamp(), myPlayerNumber());

        // Update the monitor.
        int health = monitor->performHealthCheck(timer->timestamp());

        burstRate = health;

        monitor->logOutput(timer->timestamp());

        if (timer->timeToSend() && myPlayerNumber() > 0)
            send();

        PROF_EXIT(P_COMM);
    }
}

void Comm::send()
{
    pthread_mutex_lock(&comm_mutex);

    teamConnect->send(myPlayerNumber(), gameConnect->myTeamNumber(), burstRate);
    timer->teamPacketSent();

    pthread_mutex_unlock(&comm_mutex);
}

void Comm::receive()
{
    pthread_mutex_lock(&comm_mutex);

    teamConnect->receive(0, gameConnect->myTeamNumber());

    gameConnect->handle(myPlayerNumber());

    pthread_mutex_unlock(&comm_mutex);
}

GameData Comm::getGameData()
{
    pthread_mutex_lock(&comm_mutex);

    GameData d = *(gameConnect->getGameData());

    pthread_mutex_unlock(&comm_mutex);

    return d;
}

TeamMember Comm::getTeammate(int player)
{
    pthread_mutex_lock(&comm_mutex);

    TeamMember m = *(teamConnect->getTeamMate(player));

    pthread_mutex_unlock(&comm_mutex);

    return m;
}

void Comm::setLocData(int p,
                      float x , float y , float h ,
                      float xu, float yu, float hu)
{
    int player = checkPlayerNumber(p);
    pthread_mutex_lock(&comm_mutex);

    teamConnect->setLocData(p, x, y, h, xu, yu, hu);

    pthread_mutex_unlock(&comm_mutex);
}

void Comm::setBallData(int p, float on,
                       float d , float b ,
                       float du, float bu)
{
    int player = checkPlayerNumber(p);
    pthread_mutex_lock(&comm_mutex);

    teamConnect->setBallData(p, on, d, b, du, bu);

    pthread_mutex_unlock(&comm_mutex);
}

void Comm::setBehaviorData(int p,
                           float r, float sr, float ct)
{
    int player = checkPlayerNumber(p);
    pthread_mutex_lock(&comm_mutex);

    teamConnect->setBehaviorData(p, r, sr, ct);

    pthread_mutex_unlock(&comm_mutex);
}

int Comm::checkPlayerNumber(int p)
{
    int player = p ? p : myPlayerNumber();
    if (player <= 0)
    {
        std::cout << "Trying to set Comm data with bad player number!\n"
                  << "Set Comm's player number through brain first!" << std::endl;
    }
    return player;
}

void Comm::setTeamNumber(int tn)
{
    gameConnect->setMyTeamNumber(tn, myPlayerNumber());
}

int Comm::teamNumber()
{
    return gameConnect->myTeamNumber();
}
