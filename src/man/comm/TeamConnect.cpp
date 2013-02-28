/**
 * Class to handle team communications.
 * @author Josh Zalinger and Wils Dawson 4/30/12
 */

#include "TeamConnect.h"

#include <unistd.h>
#include <iostream>
#include <string>

#include "CommDef.h"

#include "DebugConfig.h"

namespace man {

namespace comm {

TeamConnect::TeamConnect(CommTimer* t, NetworkMonitor* m)
    : timer(t), monitor(m)
{
    socket = new UDPSocket();
    setUpSocket();
}

TeamConnect::~TeamConnect()
{
    delete socket;
}

void TeamConnect::setUpSocket()
{
    std::string ipTarget = "255.255.255.255";
    static char buf[100] = {0};

    bool BROADCAST = true;

    socket->setBlocking(false);

    if (BROADCAST)
    {
#ifdef DEBUG_COMM
        std::cout << "Comm set to Broadcast" << std::endl;
#endif
        socket->setBroadcast(true);
        goto end;
    }

    socket->setBroadcast(false);
    socket->setMulticastLoopback(false);
    socket->setMulticastInterface();
    socket->setMulticastTTL((char)1); // should be 1 to keep on subnet

    if (!buf[0])
    {
        std::string name;

        if (gethostname(buf, sizeof(buf)) < 0)
        {
            std::cerr << "\nError getting hostname in "
                      << "TeamConnect::setUpSocket()"
                      << std::endl;
            goto end;
        }
        else
            name = buf;

        for (int i = 0; i < NUM_ROBOTS; ++i)
        {
            if (robotIPs[i].name.compare(name) == 0)
            {
                ipTarget = robotIPs[i].ip;
                socket->setBroadcast(false);
                break;
            }
            if (i == NUM_ROBOTS-1)
            {
                std::cerr << "\nError finding IP for hostname in "
                          << "TeamConnect::setUpSocket()" << std::endl;
                goto end;
            }
        }
        ipTarget = "239" + ipTarget.substr(ipTarget.find('.'));
    }

end:

#ifdef DEBUG_COMM
    std::cout << "Target ip is set to: " << ipTarget.c_str() << std::endl;
#endif

    socket->setTarget(ipTarget.c_str(), TEAM_PORT);
    socket->bind("", TEAM_PORT); // listen for anything on our port.

    //join team's multicast...
    for (int i = 0; i < NUM_ROBOTS; ++i)
    {
        ipTarget = robotIPs[i].ip;
        ipTarget = "239" + ipTarget.substr(ipTarget.find('.'));
        socket->joinMulticast(ipTarget.c_str());
    }
}

void TeamConnect::send(const messages::WorldModel& model,
                       int player, int team, int burst = 1)
{
    if (!model.IsInitialized())
    {
#ifdef DEBUG_COMM
        std::cerr << "Comm does not have a valid input to send." << std::endl;
#endif
        return;
    }
    TeamMemberInfo robot = teamMates[player-1];

    portals::Message<messages::TeamPacket> teamMessage(0);
    *teamMessage.get() = messages::TeamPacket();

    messages::TeamPacket* packet = teamMessage.get();

    packet->mutable_payload()->CopyFrom(model);
    packet->set_sequence_number(robot.seqNum + 1);
    packet->set_player_number(player);
    packet->set_team_number(team);
    packet->set_header(UNIQUE_ID);
    packet->set_timestamp(timer->timestamp());

    char datagram[packet->ByteSize()];
    packet->SerializeToArray(&datagram[0], packet->GetCachedSize());

    for (int i = 0; i < burst; ++i)
    {
        socket->sendToTarget(&datagram[0], sizeof(packet));
    }
}

void TeamConnect::receive(portals::OutPortal<messages::WorldModel>* modelOuts [NUM_PLAYERS_PER_TEAM],
                          int player, int team)
{
    char packet[250];
    int result;
    int playerNum;

    do
    {
        //initial setup
        portals::Message<messages::TeamPacket> teamMessage(0);
        *teamMessage.get() = messages::TeamPacket();
        memset(&packet[0], 0, sizeof(packet));

        //actually check socket
        result = socket->receive(&packet[0], sizeof(packet));

        // Save the current time for later.
        llong currtime = timer->timestamp();

        if (result <= 0)
            break; //leave on error or nothing to receive.

        if (teamMessage.get()->ParseFromArray(&packet[0], result))
        {
            std::cerr << "Failed to parse GPB from socket in TeamConnect"
                      << std::endl;
        }

        if (!verify(teamMessage.get(), currtime, player, team))
            continue;  // Bad Packet.

#ifdef DEBUG_COMM
        std::cout << "Recieved a packet:\n\n"
                  << teamMessage.get()->DebugString()
                  << std::endl;
#endif

        playerNum = teamMessage.get()->player_number();
        portals::Message<messages::WorldModel> model(&teamMessage.get()->payload());
        modelOuts[playerNum-1]->setMessage(model);
    } while (result > 0);
}

bool TeamConnect::verify(messages::TeamPacket* packet, llong currtime,
                        int player, int team)
{
    if (memcmp(packet->header().c_str(), UNIQUE_ID, sizeof(UNIQUE_ID)))
    {
#ifdef DEBUG_COMM
        std::cout << "Received packet with bad ID"
                  << " in TeamConnect::verifyHeader()" << std::endl;
#endif
        return false;
    }

    if (packet->team_number() != team)
    {
#ifdef DEBUG_COMM
        std::cout << "Received packet with bad teamNumber"
                  << " in TeamConnect::verifyHeader()" << std::endl;
#endif
        return false;
    }

    int playerNum = packet->player_number();

    if (playerNum < 0 || playerNum > NUM_PLAYERS_PER_TEAM)
    {
#ifdef DEBUG_COMM
        std::cout << "Received packet with bad playerNumber"
                  << " in TeamConnect::verify()" << std::endl;
#endif
        return false;
    }

    // if we care about who we recieve from:
    if (player != 0 && player != playerNum)
    {
#ifdef DEBUG_COMM
        std::cout << "Received packet with unwanted playerNumber"
                  << " in TeamConnect::verify()" << std::endl;
#endif
        return false;
    }

    int seqNumber = packet->sequence_number();

    TeamMemberInfo robot = teamMates[playerNum-1];
    if (seqNumber <= robot.seqNum)
    {
#ifdef DEBUG_COMM
        std::cout << "Received packet with old sequenceNumber"
                  << " in TeamConnect::verify()" << std::endl;
#endif
        return false;
    }

    // Success! Update seqNum and timeStamp and parse!
    int lastSeqNum = robot.seqNum;
    int delayed = seqNumber - lastSeqNum - 1;
    robot.seqNum = seqNumber;

    llong ts = packet->timestamp();

    // Now attempt to syncronize the clocks of this robot and
    // the robot from which we just received. Eventually the
    // two clocks will reach an equilibrium point (within a
    // reasonable margin of error) without the use of internet
    // based clock syncronizing (don't need outside world).
    llong newOffset = 0;

    if (ts + MIN_PACKET_DELAY > currtime)
    {
        newOffset = ts + MIN_PACKET_DELAY - currtime;
        timer->addToOffset(newOffset);
    }
    robot.timestamp = ts;

    // Update the monitor
    monitor->packetsDropped(delayed);
    monitor->packetReceived(ts, currtime + newOffset);

    return true;
}

void TeamConnect::checkDeadTeammates(portals::OutPortal<messages::WorldModel>* modelOuts [NUM_PLAYERS_PER_TEAM],
                                     llong time, int player)
{
    TeamMemberInfo robot;
    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; ++i)
    {
        robot = teamMates[i];
        if (i+1 == player)
            continue;
        else if (time - robot.timestamp > TEAMMATE_DEAD_THRESHOLD)
        {
            portals::Message<messages::WorldModel> msg(0);
            *msg.get() = messages::WorldModel();
            msg.get()->set_active(false);
            modelOuts[i]->setMessage(msg);
        }
    }
}

}
}
