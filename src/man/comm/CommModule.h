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
#pragma once

#include "RoboGrams.h"

#include "TeamConnect.h"
#include "GameConnect.h"
#include "CommTimer.h"
#include "NetworkMonitor.h"
#include "Common.h"

#include "WorldModel.pb.h"
#include "TeamPacket.pb.h"
#include "GameState.pb.h"
#include "GCResponse.pb.h"

namespace man{

namespace comm{

class CommModule : public portals::Module
{
public:
    /**
     * Constructor.
     * @param team:   The team number for the robot.
     * @param player: The player number for the robot.
     */
    CommModule(int team, int player);

    /**
     * Destructor.
     */
    virtual ~CommModule();

    /**
     * Runs the module. Main execution in here.
     */
    virtual void run_();

    /**
     * Sends any data necessary.
     */
    void send();

    /**
     * Receives any data that is waiting.
     */
    void receive();

    void setMyPlayerNumber(int p) {_myPlayerNumber = p;}
    int  myPlayerNumber() {return _myPlayerNumber;}

    void setTeamNumber(int tn);
    int  teamNumber();

    /***** OUT PORTALS *****/
    portals::OutPortal<messages::WorldModel>* _worldModels[NUM_PLAYERS_PER_TEAM];
    portals::OutPortal<messages::GameState> _gameStateOutput;

    /***** IN  PORTALS *****/
    portals::InPortal<messages::WorldModel> _worldModelInput;
    portals::InPortal<messages::GCResponse> _gcResponseInput;

private:
    /**
     * @param p: Returns if non-zero, otherwise returns 'myPlayerNumber'
     *           If 'myPlayerNumber' is 0, print a message. Prepare for error.
     */
    int checkPlayerNumber(int p);

    NetworkMonitor*  monitor;
    CommTimer*       timer;
    TeamConnect*     teamConnect; // For communicating with TeamMates.
    GameConnect*     gameConnect; // For communicating with GameController.

    int burstRate;

    int _myPlayerNumber;
};

}

}
