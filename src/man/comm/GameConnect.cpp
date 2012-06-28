/**
 * This class provides functionality for communicating with the
 * GameController. Uses UDP for transmission and reception and
 * keeps track of the data with the GameData class.
 * @author Wils Dawson 5/29/2012
 */

#include <string>

#include "RoboCupGameControlData.h"

//TODO: actually include Common.h
//#include "Common.h"
static const int NUM_PLAYERS_PER_TEAM = 4;
#define DEBUG_COMM

GameConnect::GameConnect(CommTimer* t, NetworkMonitor* m) :
    _timer(t), _monitor(m)
{
    _data = new GameData()
}

GameConnect::~GameConnect()
{
    delete _data;
}

GameConnect::setUpSocket()
{
    socket->setBlocking(false);
    socket->setBroadcast(true);

    socket->bind("", GAMECONTROLLER_PORT); // Listen on the GC port.

    socket->setTarget(ipTarget.c_str(), GAMECONTROLLER_PORT);
}

GameConnect::handle(int player = 0)
{
    //TODO: find out if this is the correct size.
    char packet[sizeof(struct RoboCupGameControlData)];
    int result;
    struct sockaddr from;
    do
    {
        memset(&packet[0], 0, sizeof(struct RoboCupGameControlData));

        result = socket->receiveFrom(&packet[0], sizeof(packet),
                                     &from, sizeof(from));

        if (result <= 0)
            break;

        if (!verify(&packet[0]))
            continue;  // Bad Packet.

        //TODO: make sure casting like this works.
        getGameData->setControl((struct RoboCupGameControlData)packet);

        //std::cout << "Received a packet!" << std::endl;

        //TODO: check this...
        socket->setTarget(from);
        socket->setBroadcast(false);

        if (player)
            respond(player);
        else
            for (int i = 1; i <= NUM_PLAYERS_PER_TEAM; ++i)
                respond(i);

    } while (result > 0);
}

GameConnect::verify(char* packet)
{
    struct RoboCupGameControlData data = (struct RoboCupGameControlData)packet;

    if (memcmp(data.header, GAMECONTROLLER_STRUCT_HEADER,
               sizeof(data.header)))
    {
#ifdef DEBUG_COMM
        std::cout << "GameConnect::verify() found a bad header" << std::endl;
#endif
        return false;
    }
    if (memcmp(data.version, GAMECONTROLLER_STRUCT_VERSION,
               sizeof(data.version)))
    {
#ifdef DEBUG_COMM
        std::cout << "GameConnect::verify() found a bad version" << std::endl;
#endif
        return false;
    }
    return true;
}

GameConnect::respond(int player, unsigned int msg = GAMECONTROLLER_RETURN_MSG_ALIVE)
{
    struct RoboCupGameControlReturnData response;
    response.header = GAMECONTROLLER_RETURN_STRUCT_HEADER;
    response.version = GAMECONTROLLER_RETURN_STRUCT_VERSION;
    response.team = (uint16)_myTeamNumber;
    response.player = (uint16)player;
    response.message = msg;

    //TODO: see if this cast works.
    socket->sendToTarget((char*)&response, sizeof(response));
}
