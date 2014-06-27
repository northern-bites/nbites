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
#include "FieldConstants.h"

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
    socket->setBlocking(false);
    socket->setBroadcast(true);

    socket->setTarget(IP_TARGET, TEAM_PORT);
    socket->bind("", TEAM_PORT); // listen for anything on our port
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
    
    splMessage.pose[0] = (model.my_x()-CENTER_FIELD_X)*CM_TO_MM;
    splMessage.pose[1] = (model.my_y()-CENTER_FIELD_Y)*CM_TO_MM;
    splMessage.pose[2] = model.my_h();
   
    splMessage.walkingTo[0] = (model.walking_to_x()-CENTER_FIELD_X)*CM_TO_MM;
    splMessage.walkingTo[1] = (model.walking_to_y()-CENTER_FIELD_Y)*CM_TO_MM;
    
    if (model.in_kicking_state()) {
        splMessage.shootingTo[0] = (model.kicking_to_x()-CENTER_FIELD_X)*CM_TO_MM;
        splMessage.shootingTo[1] = (model.kicking_to_y()-CENTER_FIELD_Y)*CM_TO_MM;
    }
    else {
        splMessage.shootingTo[0] = splMessage.pose[0];
        splMessage.shootingTo[1] = splMessage.pose[1];
    }
    
    splMessage.ballAge = model.ball_age()*1000; // seconds to milliseconds

    // @TODO: the logic for this conversion is somewhere else in the code, use it
    splMessage.ball[0] = model.ball_dist()*CM_TO_MM * (float)cos(model.ball_bearing()*TO_RAD);
    splMessage.ball[1] = model.ball_dist()*CM_TO_MM * (float)sin(model.ball_bearing()*TO_RAD);

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
        portals::Message<messages::WorldModel> model(0);
        messages::WorldModel *message = model.get();

        message->set_timestamp(teamMessage.get()->payload().timestamp());

#ifdef DEBUG_COMM
        std::cout << "Debugging SPL comm" << std::endl;
        std::cout << "Fallen: " << splMessage.fallen << std::endl;
        std::cout << "Pose: " << splMessage.pose[0] << std::endl;
        std::cout << "Pose: " << splMessage.pose[1] << std::endl;
        std::cout << "Pose: " << splMessage.pose[2] << std::endl;
        std::cout << "Walking to: " << splMessage.walkingTo[0] << std::endl;
        std::cout << "Walking to: " << splMessage.walkingTo[1] << std::endl;
        std::cout << "Shooting to: " << splMessage.shootingTo[0] << std::endl;
        std::cout << "Shooting to: " << splMessage.shootingTo[1] << std::endl;
        std::cout << "Ball age: " << splMessage.ballAge << std::endl;
        std::cout << "Ball: " << splMessage.ball[0] << std::endl;
        std::cout << "Ball: " << splMessage.ball[1] << std::endl;
        std::cout << "Ball vel: " << splMessage.ballVel[0] << std::endl;
        std::cout << "Ball vel: " << splMessage.ballVel[1] << std::endl;
#endif

        playerNum = splMessage.playerNum;
        message->set_fallen(splMessage.fallen);

        message->set_my_x(splMessage.pose[0]*MM_TO_CM + CENTER_FIELD_X);
        message->set_my_y(splMessage.pose[1]*MM_TO_CM + CENTER_FIELD_Y);
        message->set_my_h(splMessage.pose[2]);

        message->set_walking_to_x(splMessage.walkingTo[0]*MM_TO_CM + CENTER_FIELD_X);
        message->set_walking_to_y(splMessage.walkingTo[1]*MM_TO_CM + CENTER_FIELD_Y);

        message->set_kicking_to_x(splMessage.shootingTo[0]*MM_TO_CM + CENTER_FIELD_X);
        message->set_kicking_to_y(splMessage.shootingTo[1]*MM_TO_CM + CENTER_FIELD_Y);

        message->set_ball_age(splMessage.ballAge/1000); // milliseconds to seconds

        // @TODO: the logic for this conversion is somewhere else in the code, use it
        message->set_ball_dist((float)sqrt((float)pow(splMessage.ball[0], 2) + 
                                           (float)pow(splMessage.ball[1], 2))*MM_TO_CM);
        message->set_ball_bearing((float)atan(splMessage.ball[1]/splMessage.ball[0])*TO_DEG);

        message->set_ball_vel_x(splMessage.ballVel[0]*MM_TO_CM);
        message->set_ball_vel_y(splMessage.ballVel[1]*MM_TO_CM);

        // so that world view displays drop-in players correctly
        message->set_ball_on(!splMessage.ballAge);
        message->set_in_kicking_state(splMessage.pose[0] != splMessage.shootingTo[0] ||
                                      splMessage.pose[1] != splMessage.shootingTo[1]);
        message->set_role(6);

        // so that behaviors actually processes world model
        message->set_active(true);
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
