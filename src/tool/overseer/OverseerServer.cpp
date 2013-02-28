
#include "OverseerServer.h"

#include <iostream>

#include "GroundTruth.h"
#include "io/Socket.h"
#include "io/SocketOutProvider.h"

namespace nbites {
namespace overseer {

using namespace qtool::overseer;
using namespace common::io;
using namespace std;
using namespace man::memory::log;

OverseerServer::OverseerServer(point<float>* ballPosition,
        vector<point<float> >* robotPositions) :
        Thread("OverseerServer"),
        groundTruthMessage(new GroundTruth(ballPosition, robotPositions)) {

}

OverseerServer::~OverseerServer() {
}

void OverseerServer::run() {
    int listening_socket = -1;

    while(running) {
        try {
            if (listening_socket == -1) {
                listening_socket = tcp::createSocket();
                tcp::bindSocket(listening_socket, OVERSEER_PORT);
                tcp::listenOnSocket(listening_socket);
            }
            sockaddr client_addr;
            socklen_t client_addr_len = 0;
            // we need to pass a 0ed client_addr to acceptConnections
            memset(&client_addr, 0, sizeof(client_addr));
            int client_socket = tcp::acceptConnections(listening_socket,
                    client_addr, client_addr_len);
            // create the logger that outputs to the client
            OutProvider::ptr socketOut(new SocketOutProvider(client_socket, client_addr));
            MessageLogger::ptr logger((new MessageLogger(socketOut, groundTruthMessage)));
            loggers.push_back(logger);
            logger->start();

        } catch (socket_exception& e) {
            cout << "Socket Error! " << e.what() << " on Overseer Server!" << endl;
            return ;
        }
    }
}

//TODO: we never remove loggers, we just keep on adding them and updating them
// we need a smart way to figure out when they fail
void OverseerServer::postData() {
    groundTruthMessage->update();
    for (Loggers::iterator i = loggers.begin(); i != loggers.end(); i++) {
        (*i)->signalToLog();
    }
}

}
}
