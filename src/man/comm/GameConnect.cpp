/**
 * This class provides functionality for communicating with the
 * GameController. Uses UDP for transmission and reception.
 * @author Wils Dawson 5/29/2012
 */

#include "GameConnect.h"

#include <string>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "RoboCupGameControlData.h"

#include "Common.h"

#include "DebugConfig.h"

namespace man {

namespace comm {

GameConnect::GameConnect(CommTimer* t, NetworkMonitor* m, int team, int player)
    : _timer(t), _monitor(m), _myTeamNumber(team)
{
    _socket = new UDPSocket();
    setUpSocket();
}

GameConnect::~GameConnect()
{
    delete _socket;
    std::cout << "GameConnect destructor" << std::endl;
}

void GameConnect::setUpSocket()
{
    _socket->setBlocking(false);
    _socket->setBroadcast(false);

    _socket->bind("", GAMECONTROLLER_PORT); // Listen on the GC port.

    _socket->setTarget("255.255.255.255", GAMECONTROLLER_PORT);
}

void GameConnect::handle(portals::OutPortal<messages::GameState>& out,
                         portals::InPortal<messages::GCResponse>& in,
                         int player)
{
    char packet[sizeof(struct RoboCupGameControlData)];
    int result;
    struct sockaddr from;
    int addrlen = sizeof(from);
    struct RoboCupGameControlData control;
    do
    {
        portals::Message<messages::GameState> gameMessage(0);
        memset(&packet[0], 0, sizeof(struct RoboCupGameControlData));

        result = _socket->receiveFrom(&packet[0], sizeof(packet),
                                      &from, &addrlen);

        if (result <= 0)
            break;

        if (!verify(&packet[0]))
            continue;  // Bad Packet.
        _haveRemoteGC = true;

        memcpy(&control, &packet[0], sizeof(RoboCupGameControlData));
        fillMessage(gameMessage.get(), control);

#ifdef DEBUG_COMM
        std::cout << "Received Game Controller message:\n\n"
                  << gameMessage.get()->DebugString()
                  << std::endl;
#endif

        out.setMessage(gameMessage);

        // Reset the target to the GC that we are getting data from.
        char destination[INET_ADDRSTRLEN];

        inet_ntop(AF_INET, &((struct sockaddr_in*)&from)->sin_addr,
                  &destination[0], INET_ADDRSTRLEN);
        _socket->setTarget(&destination[0], GAMECONTROLLER_PORT);
        _socket->setBroadcast(false);

        in.latch();

        if (player)
            respond(player, in.message().status());
        else
        {
            //@TODO: Currently can't respond with correct status,
            //need InPortals for other players... Too much overhead?
            // for (int i = 1; i <= NUM_PLAYERS_PER_TEAM; ++i)
            //     respond(i);
        }
    } while (true);
}

bool GameConnect::verify(char* packet)
{
    struct RoboCupGameControlData* data = (struct RoboCupGameControlData*)packet;
    int gc_version = GAMECONTROLLER_STRUCT_VERSION;

    if (memcmp(data->header, GAMECONTROLLER_STRUCT_HEADER,
               sizeof(data->header)))
    {
#ifdef DEBUG_COMM
        std::cout << "GameConnect::verify() found a bad header" << std::endl;
#endif
        return false;
    }
    if (memcmp(&data->version, &gc_version,
               sizeof(data->version)))
    {
#ifdef DEBUG_COMM
        std::cout << "GameConnect::verify() found a bad version" << std::endl;
#endif
        return false;
    }

    int tn = myTeamNumber();
    if (memcmp(&(data->teams[0].teamNumber), &tn, sizeof(char)) &&
        memcmp(&(data->teams[1].teamNumber), &tn, sizeof(char)))
    {
#ifdef DEBUG_COMM
        std::cout << "GameConnect::verify() incorrect teamNumber" << std::endl;
#endif
        return false;
    }

    return true;
}

void GameConnect::fillMessage(messages::GameState* msg,
                              struct RoboCupGameControlData& control)
{
    msg->set_state(control.state);
    msg->set_first_half(control.firstHalf);
    msg->set_kick_off_team(control.kickOffTeam);
    msg->set_secondary_state(control.secondaryState);
    msg->set_drop_in_team(control.dropInTeam);
    msg->set_drop_in_time(control.dropInTime);
    msg->set_secs_remaining(control.secsRemaining);
    msg->set_have_remote_gc(_haveRemoteGC);

    messages::TeamInfo* blue = msg->add_team();
    messages::TeamInfo* red  = msg->add_team();
    fillTeam(blue, control.teams[TEAM_BLUE]);
    fillTeam(red , control.teams[TEAM_RED ]);

}


void GameConnect::fillTeam(messages::TeamInfo* msg,
                           struct TeamInfo& team)
{
    msg->set_team_number(team.teamNumber);
    msg->set_team_color(team.teamColour);
    msg->set_score(team.score);
    msg->set_goal_color(team.goalColour);

    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; ++i)
    {
        messages::RobotInfo* player = msg->add_player();
        fillPlayer(player, team.players[i]);
    }
}

void GameConnect::fillPlayer(messages::RobotInfo* msg,
                             struct RobotInfo& player)
{
    msg->set_penalty(player.penalty);
    msg->set_secs_left(player.secsTillUnpenalised);
}

void GameConnect::respond(int player, unsigned int msg)
{
    struct RoboCupGameControlReturnData response;
    memcpy(&response.header, GAMECONTROLLER_RETURN_STRUCT_HEADER,
           sizeof(response.header));
    response.version = GAMECONTROLLER_RETURN_STRUCT_VERSION;
    response.team = (uint16)_myTeamNumber;
    response.player = (uint16)player;
    response.message = msg;

    char packet[sizeof(RoboCupGameControlReturnData)];
    memcpy(&packet[0], &response, sizeof(RoboCupGameControlReturnData));

    int result = _socket->sendToTarget(&packet[0], sizeof(response));

#ifdef DEBUG_COMM
    if (result <= 0)
    {
        std::cout << "GameConnect::respond() Unable to send" << std::endl;
    }
#endif
}

void GameConnect::setMyTeamNumber(int tn, int pn)
{
    _myTeamNumber = tn;
}

}
}
