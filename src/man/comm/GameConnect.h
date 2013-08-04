/**
 * This class provides functionality for communicating with the
 * GameController. Uses UDP for transmission and reception.
 * @author Wils Dawson 5/29/2012
 */

#pragma once

#include "CommTimer.h"
#include "NetworkMonitor.h"
#include "UDPSocket.h"
#include "RoboCupGameControlData.h"

#include "RoboGrams.h"
#include "GameState.pb.h"
#include "GCResponse.pb.h"

namespace man {

namespace comm {

class GameConnect
{
public:
    /**
     * Constructor
     */
    GameConnect(CommTimer* t, NetworkMonitor* m, int team, int player);

    /**
     * Destructor
     */
    ~GameConnect();

    /**
     * Signal to receive any information from the socket
     * and send any appropriate response back to the GC.
     * @param out   : The OutPortal to send the state to.
     * @param in    : The InPortal containing our status.
     * @param player: The player number to respond as.
     *                0 to respond as all.
     */
    void handle(portals::OutPortal<messages::GameState>& out,
                portals::InPortal<messages::GCResponse>& in,
                int player);

    void setMyTeamNumber(int tn, int pn);
    int  myTeamNumber() {return _myTeamNumber;}

private:
    /**
     * Sets up the socket to be used.
     */
    void setUpSocket();

    /**
     * Builds a GameState message from RoboCupGameControlData
     * @param msg    : The GameState message to fill up.
     * @param control: The RoboCupGameControlData to use.
     */
    void fillMessage(messages::GameState* msg,
                     struct RoboCupGameControlData& control);

    /***** HELPERS *****/
    void fillTeam(  messages::TeamInfo* msg,
                    struct TeamInfo& team);
    void fillPlayer(messages::RobotInfo* msg,
                    struct RobotInfo& player);

    /**
     * Builds and sends a response to the Game Controller.
     * @param player: The player to respond as.
     * @param msg:    The message to respond with.
     *                Defined in RoboCupGameControlData.h
     */
    void respond(int player, unsigned int msg = GAMECONTROLLER_RETURN_MSG_ALIVE);

    /**
     * Verifies the packet we received from the socket.
     * @param packet: Pointer to the packet.
     */
    bool verify(char* packet);

    CommTimer*      _timer;
    NetworkMonitor* _monitor;
    UDPSocket*      _socket;
    int             _myTeamNumber;

    bool            _haveRemoteGC;
    long long       _gcTimestamp;
};

}
}
