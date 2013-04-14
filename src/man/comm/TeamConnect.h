/**
 * Class to handle team communications.
 * @author Wils Dawson and Josh Zalinger 4/30/12
 */

#pragma once

#include "CommTimer.h"
#include "NetworkMonitor.h"
#include "UDPSocket.h"

#include "RoboGrams.h"
#include "WorldModel.pb.h"
#include "TeamPacket.pb.h"

#include "Common.h"

namespace man {

namespace comm {

struct TeamMemberInfo {
    int   seqNum;
    llong timestamp;
};

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
     * @param model:  The model to send.
     * @param player: The player number whose information we should send.
     * @param team:   The team number we want to send as.
     * @param burst:  The number of copies of this packet we should send.
     *                If burst is less than 1, we won't send anything.
     */
    void send(const messages::WorldModel& model, int player, int team, int burst);

    /**
     * Signal to receive any information from the socket.
     * @param modelOuts: The OutPortals to give the models to.
     * @param player   : The player number whose information we want.
     *                   If 0, recieve any player number.
     * @param team     : The team Number we want to revceive.
     */
    void receive(portals::OutPortal<messages::WorldModel>* modelOuts [NUM_PLAYERS_PER_TEAM],
                 int player, int team);

    /**
     * Checks to see if there are any inactive teamMembers.
     * Inactive defined as haven't gotten a packet from them in a while.
     * Behaviors might also set inactive if robot is penalized.
     * @param modelOuts: The OutPortals to give models to.
     * @param time:      Timestamp of current time.
     * @param player:    My player number (ignore me)
     */
    void checkDeadTeammates(portals::OutPortal<messages::WorldModel>* modelOuts [NUM_PLAYERS_PER_TEAM],
                            llong time, int player);

private:
    /**
     * Sets up the socket to be used.
     */
    void setUpSocket();

    /**
     * Verifies the packet we received from the socket.
     * @param packet  : Pointer to the packet.
     * @param currtime: Current time at reception
     * @param player  : The player we want packets from.
     *                  0 if we don't care.
     * @param team    : The team we want packets from.
     * @return        : True on success, false on fail.
     */
    bool verify(messages::TeamPacket* packet, llong currtime, int player, int team);

    CommTimer*      timer;
    NetworkMonitor* monitor;
    UDPSocket*      socket;

    struct TeamMemberInfo teamMates[NUM_PLAYERS_PER_TEAM];
};

}
}
