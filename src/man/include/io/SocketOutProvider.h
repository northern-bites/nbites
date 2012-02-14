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
    SocketOutProvider(unsigned short port) : port(port), is_open(false) {
        memset(&client_address, 0, sizeof(client_address));
    }

    virtual ~SocketOutProvider() {
        if (is_open) {
            close(file_descriptor);
        }
        close(listening_socket);
    }

    virtual std::string debugInfo() const {
        return std::string("SocketOut server with client " +
                           std::string(inet_ntoa(((sockaddr_in*)&client_address)->sin_addr)));
    }

    void openCommunicationChannel() throw (socket_exception) {
        if (file_descriptor == -1) {
            listening_socket = tcp::createSocket();
            tcp::bindSocket(listening_socket, port);
            tcp::listenOnSocket(listening_socket);
        }
        file_descriptor = tcp::acceptConnections(listening_socket,
                client_address, client_address_len);
        is_open = true;
    }

    virtual void closeCommunicationChannel() const {
        close(file_descriptor);
        is_open = false;
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
