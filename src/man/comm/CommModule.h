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
