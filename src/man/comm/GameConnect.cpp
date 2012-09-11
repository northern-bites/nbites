/**
 * This class provides functionality for communicating with the
 * GameController. Uses UDP for transmission and reception and
 * keeps track of the data with the GameData class.
 * @author Wils Dawson 5/29/2012
 */

#include "GameConnect.h"

#include <string>
#include <iostream>
#include <sys/socket.h>

#include "RoboCupGameControlData.h"

//TODO: actually include Common.h
//#include "Common.h"
static const int NUM_PLAYERS_PER_TEAM = 4;
#define DEBUG_COMM

GameConnect::GameConnect(CommTimer* t, NetworkMonitor* m)
    : _timer(t), _monitor(m)
{
    _socket = new UDPSocket();
    setUpSocket();

    _data   = new GameData(101);
    std::cout << "GameConnect Contstructed" << std::endl;
}

GameConnect::~GameConnect()
{
    delete _data;
    std::cout << "GameConnect destructor" << std::endl;
}

void GameConnect::setUpSocket()
{
    _socket->setBlocking(false);
    _socket->setBroadcast(true);

    _socket->bind("", GAMECONTROLLER_PORT); // Listen on the GC port.

    _socket->setTarget("255.255.255.255", GAMECONTROLLER_PORT);
}

void GameConnect::handle(int player = 0)
{
    char packet[sizeof(struct RoboCupGameControlData)];
    int result;
    struct sockaddr from;
    int addrlen = sizeof(from);
    do
    {
        memset(&packet[0], 0, sizeof(struct RoboCupGameControlData));

        result = _socket->receiveFrom(&packet[0], sizeof(packet),
                                      &from, &addrlen);

        if (result <= 0)
            break;

        if (!verify(&packet[0]))
            continue;  // Bad Packet.

        //TODO: make sure casting like this works.
        getGameData()->setControl(*(struct RoboCupGameControlData*)packet);

        //std::cout << "Received a packet!" << std::endl;

        //TODO: check this...
        _socket->setTarget(from);
        _socket->setBroadcast(false);

        if (player)
            respond(player);
        else
            for (int i = 1; i <= NUM_PLAYERS_PER_TEAM; ++i)
                respond(i);

    } while (result > 0);
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
    return true;
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

    //TODO: see if this cast works.
    int result = _socket->sendToTarget((char*)&response, sizeof(response));

#ifdef DEBUG_COMM
    if (result <= 0)
    {
        std::cout << "GameConnect::respond() Unable to send" << std::endl;
    }
#endif
}

void GameConnect::setMyTeamNumber(int tn)
{
    _myTeamNumber = tn;
    delete _data;
    _data = new GameData(_myTeamNumber);
}
