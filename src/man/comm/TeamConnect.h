/**
 * Class to handle team communications.
 * @author Wils Dawson and Josh Zalinger 4/30/12
 */

#pragma once

#include "CommTimer.h"
#include "NetworkMonitor.h"
#include "TeamMember.h"
#include "UDPSocket.h"

#include "Common.h"

namespace man {

namespace comm {

class TeamConnect
{
public:
    /**
     * Constructor.
     */
    TeamConnect(CommTimer* t, NetworkMonitor* m);

    /**
     * Destructor.
     */
    ~TeamConnect();

    /**
     * Signal to send our current information to the team mates.
     * @param player: The player number whose information we should send.
     * @param team:   The team number we want to send as.
     * @param burst:  The number of copies of this packet we should send.
     *                If burst is less than 1, we won't send anything.
     */
    void send(int player, int team, int burst);

    /**
     * Signal to receive any information from the socket.
     * @param player: The player number whose information we want.
     *                If 0, recieve any player number.
     * @param team:   The team Number we want to revceive.
     */
    void receive(int player, int team);

    /**
     * Gets the TeamMember with the given number.
     * @param player: The player number of the TeamMember.
     * @return:       Pointer to the TeamMember.
     */
    TeamMember* getTeamMate(int player){return teamMates[player-1];}

    /**
     * Checks to see if there are any inactive teamMembers.
     * Inactive defined as haven't gotten a packet from them in a while.
     * Behaviors might also set inactive if robot is penalized.
     * @param time:   Timestamp of current time.
     * @param player: My player number (ignore me)
     */
    void checkDeadTeammates(llong time, int player);

private:
    /**
     * Sets up the socket to be used.
     */
    void setUpSocket();

    /**
     * Builds the packet header
     * @param packet: Pointer to the beginning of the packet.
     * @param robot:  Pointer to the TeamMember object.
     * @param tn:     Team Number we want to send.
     * @effect:       Header portion of the packet will be
     *                built. 'packet' will be preserved.
     * @effect:       Updates sequence number for TeamMember.
     * @return:       Float pointer to first byte after header.
     */
    float* buildHeader(char* packet, TeamMember* robot, int tn);

    /**
     * Verifies the packet we received from the socket.
     * @param packet: Pointer to the packet.
     * @param player: The player we want packets from.
     *                0 if we don't care.
     * @param team  : The team we want packets from.
     * @return:       Player number we received from.
     *                0 for failure.
     */
    int verify(char* packet, int player, int team);

    /**
     * Verifies the packet header information.
     * @param header: Pointer to the packet header struct.
     * @param team:   The team we want.
     * @return:       true for success, false for error.
     */
    bool verifyHeader(char* header, int team);

    CommTimer*      timer;
    NetworkMonitor* monitor;
    TeamMember*     teamMates[NUM_PLAYERS_PER_TEAM];
    UDPSocket*      socket;
};

}

}
