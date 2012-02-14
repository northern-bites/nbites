/**
 * @class Socket
 *
 * Common socket wrappings
 *
 * @author Octavian Neamtu
 *
 */

#pragma once

#include "IOExceptions.h"

#include <cerrno>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <iostream>
using namespace std;

namespace common {
namespace io {
namespace tcp {

static int createSocket() throw (socket_exception) {

    int file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (file_descriptor < 0) {
        throw socket_exception(socket_exception::CREATE_ERR, errno);
    }
    return file_descriptor;
}

static void bindSocket(int socket_fd, unsigned short port) throw (socket_exception) {
    sockaddr_in my_address;
    memset(&my_address, 0, sizeof(my_address));
    my_address.sin_family = AF_INET;
    my_address.sin_port = htons(port);
    my_address.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(socket_fd, reinterpret_cast<sockaddr *>(&my_address),
            sizeof(my_address)) < 0) {
        throw socket_exception(socket_exception::BIND_ERR, errno);
    }
}

static void listenOnSocket(int socket_fd) throw (socket_exception) {
    if (listen(socket_fd, 2) < 0) {
        throw socket_exception(socket_exception::LISTEN_ERR, errno);
    }
}

//blocking!
static int acceptConnections(int socket_fd, sockaddr& client_address,
        socklen_t& client_address_len) throw (socket_exception) {
    int file_descriptor = accept(socket_fd, &client_address,
            &client_address_len);
    if (file_descriptor < 0) {
        throw socket_exception(socket_exception::ACCEPT_ERR, errno);
    }
    return file_descriptor;
}

//non-blocking! times out in 15 s
//http://developerweb.net/viewtopic.php?id=3196)
static void connectSocket(int socket_fd, long address,
        unsigned short port, int timeout = 15) throw (socket_exception) {
    sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = htonl(address);

    // Set non-blocking
    long arg = fcntl(socket_fd, F_GETFL, NULL);
    arg |= O_NONBLOCK;
    if (fcntl(socket_fd, F_SETFL, arg) < 0) {
        throw socket_exception(socket_exception::FCNTL_ERR, errno);
    }

    int result = connect(socket_fd,
            reinterpret_cast<const sockaddr*>(&server_address),
            sizeof(server_address));

    if (result < 0) {
        if (errno == EINPROGRESS) {
            //timeout
            struct timeval tv;
            tv.tv_sec = 15;
            tv.tv_usec = 0;
            //required for the select
            fd_set myset;
            FD_ZERO(&myset);
            FD_SET(socket_fd, &myset);
            //select waits for the socket_fd to be ready
            if (select(socket_fd + 1, NULL, &myset, NULL, &tv) > 0) {
                //get the return code
                int return_code;
                socklen_t lon = sizeof(int);
                getsockopt(socket_fd, SOL_SOCKET, SO_ERROR,
                        (void*) (&return_code), &lon);
                if (return_code) {
                    throw socket_exception(socket_exception::CREATE_ERR,
                            return_code);
                }
            } else {
                throw socket_exception(socket_exception::TIMED_OUT);
            }
        } else {
            throw socket_exception(socket_exception::CREATE_ERR, errno);
        }
    }
    // Set to blocking mode again...
    arg = fcntl(socket_fd, F_GETFL, NULL);
    arg &= (~O_NONBLOCK);
    if (fcntl(socket_fd, F_SETFL, arg) < 0) {
        throw socket_exception(socket_exception::FCNTL_ERR, errno);
    }
}

}
}
}
