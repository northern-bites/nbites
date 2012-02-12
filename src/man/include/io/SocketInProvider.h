/**
 * @class SocketInProvider
 *
 * Opens a client-type socket that is made to connect to the
 * specified address and port
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <cerrno>
#include <string>
#include <aio.h>
#include <cstring>
#include <pthread.h>

#include "IOExceptions.h"
#include "InProvider.h"

namespace common {
namespace io {

class SocketInProvider: public InProvider {

public:
    static const int INVALID_FD = -1;


public:
    SocketInProvider(long address, short port) :
            port(port), address(address), is_open(false),
            file_descriptor(INVALID_FD) {
        //zeroes the aio control_block
        memset(&control_block, 0, sizeof(control_block));
    }

    virtual ~SocketInProvider() {
        is_open = false;
        close(file_descriptor);
    }

    virtual std::string debugInfo() const {
        return std::string(inet_ntoa(server_address.sin_addr));
    }

    virtual bool isOfTypeStreaming() const {
        return true;
    }

    virtual bool reachedEnd() const {
        return false;
    }

    void createSocket() throw(socket_exception) {
        file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

        if (file_descriptor < 0) {
            throw socket_exception(socket_exception::CREATE_ERR, errno);
            file_descriptor = INVALID_FD;
        }
    }

    //non-blocking! times out in 15 s
    //http://developerweb.net/viewtopic.php?id=3196)
    void connectSocket() throw(socket_exception) {
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(port);
        server_address.sin_addr.s_addr = htonl(address);

        // Set non-blocking
        long arg = fcntl(file_descriptor, F_GETFL, NULL);
        arg |= O_NONBLOCK;
        if (fcntl(file_descriptor, F_SETFL, arg) < 0) {
            throw socket_exception(socket_exception::FCNTL_ERR, errno);
        }

        int result = connect(file_descriptor,
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
                FD_SET(file_descriptor, &myset);
                //select waits for the file_descriptor to be ready
                if (select(file_descriptor + 1, NULL, &myset, NULL, &tv) > 0) {
                    //get the return code
                    int return_code;
                    socklen_t lon = sizeof(int);
                    getsockopt(file_descriptor, SOL_SOCKET, SO_ERROR,
                            (void*) (&return_code), &lon);
                    if (return_code) {
                        throw socket_exception(socket_exception::CREATE_ERR, return_code);
                    } else {
                        is_open = true;
                    }
                } else {
                    throw socket_exception(socket_exception::TIMED_OUT);
                }
            } else {
                throw socket_exception(socket_exception::CREATE_ERR, errno);
            }
        }
        // Set to blocking mode again...
        arg = fcntl(file_descriptor, F_GETFL, NULL);
        arg &= (~O_NONBLOCK);
        if (fcntl(file_descriptor, F_SETFL, arg) < 0) {
            throw socket_exception(socket_exception::FCNTL_ERR, errno);
        }
    }

    virtual void enqueBuffer(char* buffer, uint32_t size) const
            throw (aio_read_exception) {
        control_block.aio_fildes = file_descriptor;
        control_block.aio_buf = buffer;
        control_block.aio_nbytes = size;

        int result = aio_read(&control_block);

        if (result == -1) {
            throw aio_read_exception(aio_read_exception::ENQUE, errno);
        }
    }

    //warning - does a pthread_yield until finished!
    virtual void waitForReadToFinish() const throw (aio_read_exception) {

        //TODO: yielding is less than ideal, maybe using a callback with the aio
        //stuff be worthwhile - Octavian
        while (readInProgress() && is_open) {
            pthread_yield();
        }
        if (aio_error(&control_block) != 0) {
            throw aio_read_exception(aio_read_exception::READ, errno);
        }
    }

    virtual uint32_t aioReadCharBuffer(char* buffer, uint32_t size) const throw(aio_read_exception) {

        if (!opened()) {
            throw aio_read_exception(aio_read_exception::NOT_OPEN);
        }

        enqueBuffer(buffer, size);
        waitForReadToFinish();

        //aio_return returns the number of bytes actually read
        //after the read is done
        uint32_t read_bytes = aio_return(&control_block);
        //read the rest of the buffer if we haven't read enough
        return read_bytes;
    }

    virtual uint32_t readCharBuffer(char* buffer, uint32_t size) const throw(aio_read_exception) {
        return aioReadCharBuffer(buffer, size);
    }

    virtual bool readInProgress() const {
        return aio_error(&control_block) == EINPROGRESS;
    }

    void openCommunicationChannel() throw(socket_exception) {
        if (is_open) {
            return;
        }
        createSocket();
        connectSocket();
    }

    bool rewind(long int) const {
        return false;
    }

    //blocking!
    virtual void peekAt(char* buffer, uint32_t size) const throw(aio_read_exception) {
        if (!opened()) {
            throw aio_read_exception(aio_read_exception::NOT_OPEN);
        }

        if (recv(file_descriptor, buffer, size, MSG_PEEK) < 0) {
            throw aio_read_exception(aio_read_exception::READ, errno);
        }
    }

    virtual bool opened() const {
        return is_open;
    }

private:
    short port;
    long address;
    bool is_open;
    sockaddr_in server_address;
    mutable aiocb control_block;
    int file_descriptor;
};

}
}
