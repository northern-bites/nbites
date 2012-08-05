/**
 * This class provides functionality for communicating with the
 * GameController. Uses UDP for transmission and reception and
 * keeps track of the data with the GameData class.
 * @author Wils Dawson 5/29/2012
 */

#ifndef GameConnect_H
#define GameConnect_H

#include "CommTimer.h"
#include "NetworkMonitor.h"
#include "UDPSocket.h"
#include "GameData.h"
#include "RoboCupGameControlData.h"

class GameConnect
{
public:
    /**
     * Constructor
     */
    GameConnect(CommTimer* t, NetworkMonitor* m);

    /**
     * Destructor
     */
    ~GameConnect();

    /**
     * Signal to receive any information from the socket
     * and send any appropriate response back to the GC.
     * @param player: The player number to respond as.
     *                0 to respond as all.
     */
    void handle(int player);

    /**
     * @return: The pointer to the GameData.
     */
    GameData* getGameData() {return _data;}

    /**
     * Checks if the specified player is penalized.
     * @param player: The player number to check.
     * @return:       Number of seconds left in penalty.
     *                -1 if not penalized.
     */
    int checkPenalty(int player);

    /*************************************************
     *               Button Interaction              *
     *************************************************/

    /**
     * Signal for manually advancing the game state.
     */
    void advanceButtonClickState();

    /**
     * Signal for manually changing the team color.
     */
    void toggleTeamColor();

    /**
     * Signal for manually toggling our kickoff.
     */
    void toggleKickoff();

    /**
     * Called when we have a remote GC and we advanced
     * the game state to penalized.
     */
    void manualPenalize();

    void setMyTeamNumber(int tn) {_myTeamNumber = tn;}
    int  myTeamNumber() {return _myTeamNumber;}

private:
    /**
     * Sets up the socket to be used.
     */
    void setUpSocket();

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
    GameData*       _data;
    UDPSocket*      _socket;
    int             _myTeamNumber;

    bool            _haveRemoteGC;
};
#endif
