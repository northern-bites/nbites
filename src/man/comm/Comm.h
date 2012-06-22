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

#ifndef Comm_H
#define Comm_H

#include <pthread.h>
//TODO: Actually use this...
//#include <boost/shared_ptr.hpp>

#include "TeamConnect.h"
#include "CommTimer.h"
#include "NetworkMonitor.h"

//TODO: extend Thread
class Comm// : public Thread
{
public:
    //TODO: This is in the Thread class
    bool running;

    /**
     * Constructor.
     */
    Comm();

    /**
     * Destructor.
     */
    virtual ~Comm();

    /**
     * Starts the thread.
     */
    int start();

    /**
     * Stops the thread.
     */
    void stop();

    /**
     * Runs the thread. Main execution in here.
     */
    void run();

    /**
     * Sends any data necessary.
     */
    void send();

    /**
     * Receives any data that is waiting.
     */
    void receive();

/*****************************************************
 * When updating the following functions, be sure to *
 * update them in TeamConnect as well!               *
 *****************************************************/

    /**
     * Sets all data from loc that we want to communicate.
     * @param p:  The player number we want to update.
     *            If 0, uses default provided by noggin.
     * @param x:  My x location on the field.
     * @param y:  My y location on the field.
     * @param h:  My heading on the field.
     * @param xu: My uncertainty in my x location.
     * @param yu: My uncertainty in my y location.
     * @param hu: My uncertainty in my heading.
     */
    void setLocData(int player,
                    float x , float y , float h ,
                    float xu, float yu, float hu);

    /**
     * Sets all data about the ball that we want to communicate.
     * @param p:  The player number we want to update.
     *            If 0, uses default provided by noggin.
     * @param d:  The distance from me to the ball.
     * @param b:  The bearing from me to the ball.
     * @param du: The uncertainty in the ball distance.
     * @param bu: The uncertainty in the ball bearing.
     */
    void setBallData(int p,
                     float d , float b ,
                     float du, float bu);

    /**
     * Sets all behavioral data that we want to communicate.
     * @param p:  The player number we want to update.
     *            If 0, uses default provided by noggin.
     * @param r:  My playbook role.
     * @param sr: My playbook subrole.
     * @param ct: My chase time.
     */
    void setBehaviorData(int p,
                         float r, float sr, float ct);

    void setMyPlayerNumber(int p) {_myPlayerNumber = p;}
    int  myPlayerNumber() {return _myPlayerNumber;}

    void setTeamNumber(int tn);
    int  teamNumber();

private:
    /**
     * @param p: Returns if non-zero, otherwise returns 'myPlayerNumber'
     *           If 'myPlayerNumber' is 0, print a message. Prepare for error.
     */
    int checkPlayerNumber(int p);

    pthread_mutex_t  comm_mutex;  // Mutex lock for threaded data access.
    NetworkMonitor*  monitor;
    CommTimer*       timer;
    TeamConnect*     teamConnect; // For communicating with TeamMates.

    int burstRate;

    int _myPlayerNumber;
};

#endif // Comm_H
