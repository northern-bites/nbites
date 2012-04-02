/**
 * @class SocketFDProvider
 *
 * Opens a socket on a specified TCP port.
 * When openCommunicationChannel is called, the provider will block
 * until a client connects. Only one client can be connected at a time!
 *
 */


#pragma once

#include <iostream>
#include <string>
#include "OutProvider.h"

#include "Socket.h"

namespace common {
namespace io {

class SocketOutProvider : public OutProvider {

public:
    /*
     * creates a listening socket on port
     * openCommuncationChannel() will then block to accept a connection
     * on that socket
     */
    SocketOutProvider(unsigned short port) :
        port(port), is_open(false), listening_socket(-1) {

        memset(&client_address, 0, sizeof(client_address));
    }

    /*
     * takes the client_descriptor as the socket fd of the client
     * openCommunicationChannel() will not do anything (since we already have
     * a connection); it still needs to be called in order to use
     * the SocketOut as a formality to flip is_open to true
     *
     * the client file descriptor is now owned by SocketOutProvider and
     * might get closed at some point
     */
    SocketOutProvider(int client_descriptor, sockaddr client_address) :
        client_address(client_address), is_open(false) {
        file_descriptor = client_descriptor;
    }

    virtual ~SocketOutProvider() {
        this->closeCommunicationChannel();
        if (listening_socket != -1) {
            close(listening_socket);
        }
    }

    virtual std::string debugInfo() const {
        return std::string("SocketOut server with client " +
                           std::string(inet_ntoa(((sockaddr_in*)&client_address)->sin_addr)));
    }

    /**
     * if no client socket fd is present, it will create a listening socket and
     * listen on it (acceptConnections is a blocking call)
     */
    void openCommunicationChannel() throw (socket_exception) {

        if (listening_socket == -1) {
            listening_socket = tcp::createSocket();
            tcp::bindSocket(listening_socket, port);
            tcp::listenOnSocket(listening_socket);
        }

        if (file_descriptor == -1) {
            file_descriptor = tcp::acceptConnections(listening_socket,
                    client_address, client_address_len);
        }
        is_open = true;
    }

    /**
     * closeCommunicationChannel() will close the CLIENT socket that is currently open;
     * not the listening socket
     */
    virtual void closeCommunicationChannel() {
        if (is_open) {
            close(file_descriptor);
            file_descriptor = -1;
            is_open = false;
        }
    }

    bool opened() const {
        return is_open;
    }

private:
    unsigned short port;
    sockaddr client_address;
    socklen_t client_address_len;
    mutable bool is_open;
    int listening_socket;
};

}
}
