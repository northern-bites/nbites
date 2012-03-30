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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include "OutProvider.h"

namespace common {
namespace io {

class SocketOutProvider : public OutProvider {

public:
    SocketOutProvider(short port) : port(port), is_open(false) {
        createSocket();
        bindSocket();
        listenOnSocket();
    }

    virtual ~SocketOutProvider() {
        if (is_open) {
            close(file_descriptor);
        }
        close(listening_socket);
    }

    virtual std::string debugInfo() const {
        return std::string(inet_ntoa(my_address.sin_addr));
    }

    void createSocket() {
        listening_socket = socket(AF_INET, SOCK_STREAM, 0);

        if (file_descriptor < 0) {
            std::cout << "Could not create socket! " << debugInfo() << std::endl;
            file_descriptor = -1;
        }
    }

    void bindSocket() {
        my_address.sin_family = AF_INET;
        my_address.sin_port = htons(port);
        my_address.sin_addr.s_addr = htonl(INADDR_ANY);
        if (bind(listening_socket, reinterpret_cast<sockaddr *>(&my_address),
                sizeof(my_address)) < 0) {
            std::cout << "Could not bind socket! " << debugInfo() << std::endl;
        }
    }

    void listenOnSocket() {
        if (listen(listening_socket, 2) < 0) {
            std::cout << "Could not listen on socket! " << debugInfo()
                                                        << std::endl;
        }
    }

    //blocking!
    void acceptConnections() {
        file_descriptor = accept(listening_socket,
                                 &client_address, &client_address_len);
        if (file_descriptor < 0) {
            std::cout << "Could not accept connections ! " << debugInfo()
                                                           << std::endl;
        }
    }

    void openCommunicationChannel() {
        acceptConnections();
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
    short port;
    sockaddr_in my_address;
    sockaddr client_address;
    socklen_t client_address_len;
    mutable bool is_open;
    int listening_socket;
};

}
}
