/**
 * Class to handle team communications.
 * @author Josh Zalinger and Wils Dawson 4/30/12
 */

#include "TeamConnect.h"

#include <unistd.h>
#include <iostream>
#include <string>
#include <math.h>

#include "CommDef.h"
#include "DebugConfig.h"
#include "Profiler.h"
#include "SPLStandardMessage.h"
#include "NBMath.h"

namespace man {

namespace comm {

TeamConnect::TeamConnect(CommTimer* t, NetworkMonitor* m)
    : timer(t), monitor(m), myLastSeqNum(0)
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

    // create instance SPLStandardMessage
    struct SPLStandardMessage splMessage;

PROF_ENTER(P_COMM_BUILD_PACKET);

    portals::Message<messages::TeamPacket> teamMessage(0);

    messages::TeamPacket* arbData = teamMessage.get();

    // create packet from message fields
    arbData->mutable_payload()->CopyFrom(model);
    arbData->set_sequence_number(myLastSeqNum++);
    arbData->set_player_number(player);
    arbData->set_team_number(team);
    arbData->set_header(UNIQUE_ID);
    arbData->set_timestamp(timer->timestamp());

    // build packet the regular way, using arbData
    strncpy(splMessage.header, SPL_STANDARD_MESSAGE_STRUCT_HEADER, sizeof(splMessage.header));
    splMessage.version = SPL_STANDARD_MESSAGE_STRUCT_VERSION;
    splMessage.playerNum = (uint8_t)arbData->player_number();
    splMessage.team = (uint8_t)arbData->team_number();
    splMessage.fallen = (uint8_t)model.fallen();
    
    splMessage.pose[0] = model.my_x()*CM_TO_MM;
    splMessage.pose[1] = model.my_y()*CM_TO_MM;
    splMessage.pose[2] = model.my_h()*TO_DEG;
    
    splMessage.walkingTo[0] = model.walking_to_x()*CM_TO_MM;
    splMessage.walkingTo[0] = model.walking_to_y()*CM_TO_MM;
    
    splMessage.shootingTo[0] = model.kicking_to_x()*CM_TO_MM;
    splMessage.shootingTo[0] = model.kicking_to_y()*CM_TO_MM;
    
    splMessage.ballAge = model.ball_age();

    // @TODO: these seem to be nan at this point; also this logic is somewhere else in the code (maybe the math module?), use that
    // also using bad is bad (mostly for performance reasons) mmmkay
    // splMessage.ball[0] = model.my_x()*CM_TO_MM + model.ball_dist()*CM_TO_MM * (float)asin(model.ball_bearing());
    // splMessage.ball[1] = model.my_x()*CM_TO_MM + model.ball_dist()*CM_TO_MM * (float)acos(model.ball_bearing());
    
    splMessage.ballVel[0] = model.ball_vel_x()*CM_TO_MM;
    splMessage.ballVel[1] = model.ball_vel_y()*CM_TO_MM;

PROF_EXIT(P_COMM_BUILD_PACKET);

PROF_ENTER(P_COMM_SERIALIZE_PACKET);

    // serialize the teamMessage for putting into the final field of the packet
    int dataByteSize = arbData->ByteSize();
    char datagram_arbdata[dataByteSize];
    arbData->SerializeToArray(datagram_arbdata, dataByteSize);

    // put it into the packet, along with its size
    memcpy(splMessage.data, datagram_arbdata, dataByteSize);
    splMessage.numOfDataBytes = (uint16_t) dataByteSize;

PROF_EXIT(P_COMM_SERIALIZE_PACKET);

PROF_ENTER(P_COMM_TO_SOCKET);
    for (int i = 0; i < burst; ++i)
    {
        socket->sendToTarget((char*) &splMessage, sizeof(SPLStandardMessage));
    }
PROF_EXIT(P_COMM_TO_SOCKET);
}

void TeamConnect::receive(portals::OutPortal<messages::WorldModel>* modelOuts [NUM_PLAYERS_PER_TEAM],
                          int player, int team)
{
    char packet[sizeof(SPLStandardMessage)];
    int result;
    int playerNum;

    do
    {
        // initial setup
        struct SPLStandardMessage splMessage;
        memset(&splMessage, 0, sizeof(SPLStandardMessage)); // @TODO: neccessary?

        // actually check socket
        result = socket->receive((char*) &splMessage, sizeof(SPLStandardMessage));

        llong recvdtime = timer->timestamp();

        if (result <= 0 || result != sizeof(SPLStandardMessage)) {
            break; // leave on error or nothing to receive
        }

        // deserialize the SPLMessage's teamMessage.get() field into a TeamPacket
        portals::Message<messages::TeamPacket> teamMessage(0);
        if (!teamMessage.get()->ParseFromArray(splMessage.data, splMessage.numOfDataBytes))
        {
            std::cerr << "Failed to parse GPB from socket in TeamConnect. " << "numOfDataBytes" << splMessage.numOfDataBytes << std::endl;
        }

        if (!verify(&splMessage, teamMessage.get()->sequence_number(), teamMessage.get()->timestamp(), recvdtime, player, team))
        {
            continue;  // bad packet
        }

#ifdef DEBUG_COMM
        std::cout << "Recieved a packet\n"
                  << teamMessage.get()->DebugString()
                  << std::endl;
#endif

#ifdef USE_SPL_COMM
        // create a WorldModel with data from splMessage
        // @TODO: check that all the fields are correct
        portals::Message<messages::WorldModel> model(0);

        model.get()->set_timestamp(teamMessage.get()->payload().timestamp());

        playerNum = splMessage.playerNum;
        model.get()->set_fallen(splMessage.fallen);

        model.get()->set_my_x(splMessage.pose[0]*MM_TO_CM);
        model.get()->set_my_y(splMessage.pose[1]*MM_TO_CM);
        model.get()->set_my_h(splMessage.pose[2]*TO_RAD);

        model.get()->set_walking_to_x(splMessage.walkingTo[0]*MM_TO_CM);
        model.get()->set_walking_to_y(splMessage.walkingTo[1]*MM_TO_CM);

        model.get()->set_kicking_to_x(splMessage.shootingTo[0]*MM_TO_CM);
        model.get()->set_kicking_to_y(splMessage.shootingTo[1]*MM_TO_CM);

        model.get()->set_ball_age(splMessage.ballAge*1000); // seconds to milliseconds

        // @TODO: these seem to be nan at this point; also this logic is somewhere else in the code (maybe the math module?), use that
        // also using bad is bad (mostly for performance reasons) mmmkay
        // model.get()->set_ball_dist((float)sqrt((float)pow(splMessage.ball[0]/10, 2) + (float)pow(splMessage.ball[1]/10, 2)));
        // model.get()->set_ball_bearing((float)atan((splMessage.ball[1]/10)/(splMessage.ball[0]/10)));

        model.get()->set_ball_vel_x(splMessage.ballVel[0]*MM_TO_CM);
        model.get()->set_ball_vel_y(splMessage.ballVel[1]*MM_TO_CM);
#else
        playerNum = teamMessage.get()->player_number();
        portals::Message<messages::WorldModel> model(&teamMessage.get()->payload());
#endif

        modelOuts[playerNum-1]->setMessage(model);
    } while (result > 0);
}

bool TeamConnect::verify(SPLStandardMessage* splMessage, int seqNumber, int64_t timestamp, llong recvdtime,
                        int player, int team)
{
    if (!memcmp(splMessage->header, SPL_STANDARD_MESSAGE_STRUCT_HEADER, sizeof(SPL_STANDARD_MESSAGE_STRUCT_HEADER)))
    {
#ifdef DEBUG_COMM
        std::cout << "Received packet with bad header"
                  << " in TeamConnect::verifyHeader()" << std::endl;
#endif
        return false;
    }

    if (splMessage->team != team)
    {
#ifdef DEBUG_COMM
        std::cout << "Received packet with bad teamNumber"
                  << " in TeamConnect::verifyHeader()" << std::endl;
#endif
        return false;
    }

    int playerNum = splMessage->playerNum;

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

    if (seqNumber <= teamMates[playerNum-1].seqNum)
    {
        if (teamMates[playerNum-1].seqNum - seqNumber < RESET_SEQ_NUM_THRESHOLD)
        {
#ifdef DEBUG_COMM
            std::cout << "Received packet with old sequenceNumber"
                      << " in TeamConnect::verify()" << std::endl;
#endif
            return false;
        }
        // else we've restarted a robot, so consider it's packets new
    }

    // success, update seqNum and timeStamp and parse
    int lastSeqNum = teamMates[playerNum-1].seqNum;
    int delayed = seqNumber - lastSeqNum - 1;
    teamMates[playerNum-1].seqNum = seqNumber;
    
    // now attempt to syncronize the clocks of this robot and
    // the robot from which we just received, eventually the
    // two clocks will reach an equilibrium point (within a
    // reasonable margin of error) without the use of internet
    // based clock syncronizing (don't need outside world)
    llong newOffset = 0;

    if (timestamp + MIN_PACKET_DELAY > recvdtime)
    {
        newOffset = timestamp + MIN_PACKET_DELAY - recvdtime;
        timer->addToOffset(newOffset);
    }
    teamMates[playerNum-1].timestamp = timer->timestamp(); // @TODO: why is this not recvdtime (the time when the packet was recieved)?

    // update the monitor
    monitor->packetsDropped(delayed);
    monitor->packetReceived(timestamp, recvdtime + newOffset);

    return true;
}

void TeamConnect::checkDeadTeammates(portals::OutPortal<messages::WorldModel>* modelOuts [NUM_PLAYERS_PER_TEAM],
                                     llong time, int player)
{
    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; ++i)
    {
        if (i+1 == player)
        {
            continue;
        }
        else if (time - teamMates[i].timestamp > TEAMMATE_DEAD_THRESHOLD)
        {
            portals::Message<messages::WorldModel> msg(0);
            msg.get()->set_active(false);
            modelOuts[i]->setMessage(msg);
        }
    }
}

}
}
