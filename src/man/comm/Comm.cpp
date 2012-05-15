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

//#include "Profiler.h"  
//#include "Common.h"
static long long monotonic_micro_time(void)
{
    // Needed for microseconds which we convert to milliseconds
    struct timespec tv;
    clock_gettime(CLOCK_MONOTONIC, &tv);

    return tv.tv_sec * 1000000 + tv.tv_nsec / 1000;
}

/** To lock the mutex:
	pthread_mutex_lock (&comm_mutex);

	To unlock the mutex:
	pthread_mutex_unlock (&comm_mutex);
*/

Comm::Comm()
	: Thread("Comm")
{
	_myPlayerNumber = 2;  

	timer = new CommTimer(&monotonic_micro_time);
	teamConnect = new TeamConnect(timer);
	monitor = new NetworkMonitor(timer->timestamp());
	pthread_mutex_init(&comm_mutex, NULL);
}

Comm::~Comm()
{
	std::cout << "Comm destructor" << std::endl;
	delete monitor;
	delete teamConnect;
	delete timer;
	pthread_mutex_destroy(&comm_mutex);
}

int Comm::start()
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
		// Start the profiler.
		//PROF_ENTER(P_COMM);  

		receive();

		// Update teammates.
		teamConnect->checkDeadTeammates(timer->timestamp(), myPlayerNumber());

		// Update the monitor.
		monitor->performHealthCheck();
		monitor->logOutput(timer->timestamp());

		if (timer->timeToSend())
			send();

		// Stop the profiler.
		//PROF_EXIT(P_COMM);   
	}
}

void Comm::send()
{
	pthread_mutex_lock (&comm_mutex);

    

	pthread_mutex_unlock (&comm_mutex);
}

void Comm::receive()
{
	pthread_mutex_lock (&comm_mutex);

    

	pthread_mutex_unlock (&comm_mutex);
}

void Comm::setLocData(int p,
				float x , float y , float h ,
				float xu, float yu, float hu)
{
	int player = checkPlayerNumber(p);
	pthread_mutex_lock (&comm_mutex);

    

	pthread_mutex_unlock (&comm_mutex);
}

void Comm::setBallData(int p,
				 float d , float b ,
				 float du, float bu)
{
	int player = checkPlayerNumber(p);
	pthread_mutex_lock (&comm_mutex);

    

	pthread_mutex_unlock (&comm_mutex);
}

void Comm::setBehaviorData(int p,
					 float r, float sr, float ct)
{
	int player = checkPlayerNumber(p);
	pthread_mutex_lock (&comm_mutex);

    

	pthread_mutex_unlock (&comm_mutex);
}

int Comm::checkPlayerNumber(int p)
{
	int player = p ? p : myPlayerNumber();
	if (player == 0)
	{
		std::cout << "Trying to set Comm data with bad player number!\n"
				  << "Set Comm's player number through brain first!" << std::endl;
	}
	return player;
}
