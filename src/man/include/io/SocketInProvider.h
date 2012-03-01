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
#include <fcntl.h>
#include <string>
#include <aio.h>
#include <cstring>
#include <pthread.h>

#include "Socket.h"

#include "IOExceptions.h"
#include "InProvider.h"

namespace common {
namespace io {

class SocketInProvider: public InProvider {

public:
    SocketInProvider(long address, unsigned short port) :
                    port(port),
                    address(address),
                    is_open(false),
                    file_descriptor(-1) {
        //zeroes the aio control_block
        memset(&control_block, 0, sizeof(control_block));
    }

    virtual ~SocketInProvider() {
        closeChannel();
    }

    virtual void closeChannel() {
        is_open = false;
        close(file_descriptor);
    }

    virtual std::string debugInfo() const {
        struct in_addr addr;
        addr.s_addr = address;
        return "SocketIn client connecting to " + std::string(inet_ntoa(addr));
    }

    virtual bool isOfTypeStreaming() const { return true; }
    virtual bool reachedEnd() const { return false; }
    bool rewind(long int) const { return false; }

    virtual void readCharBuffer(char* buffer, uint32_t size) const
                throw (aio_read_exception) {
            aioReadCharBuffer(buffer, size);
    }

    virtual void aioReadCharBuffer(char* buffer, uint32_t size) const
            throw (aio_read_exception) {

        if (!opened()) {
            throw aio_read_exception(aio_read_exception::NOT_OPEN);
        }

        if (readInProgress()) {
            throw aio_read_exception(aio_read_exception::IN_PROGRESS);
        }

        enqueBuffer(buffer, size);
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

    virtual uint32_t bytesRead() const throw (aio_read_exception) {

        if (readInProgress()) {
            throw aio_read_exception(aio_read_exception::IN_PROGRESS);
        }

        if (aio_error(&control_block) != 0) {
            throw aio_read_exception(aio_read_exception::READ, errno);
        }

        return aio_return(&control_block);
    }

    virtual bool readInProgress() const {
        return aio_error(&control_block) == EINPROGRESS;
    }

    void openCommunicationChannel() throw (socket_exception) {
        if (is_open) {
            return;
        }
        file_descriptor = tcp::createSocket();
        tcp::connectSocket(file_descriptor, address, port);
        is_open = true;
    }

    //blocking!
    virtual void peekAt(char* buffer, uint32_t size) const
            throw (aio_read_exception) {
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
    unsigned short port;
    long address;
    bool is_open;
    mutable aiocb control_block;
    int file_descriptor;
};

}
}
