/**
 * @class SocketFDProvider
 *
 * Opens a file descriptor on a specified TCP port.
 *
 */


#pragma once

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <aio.h>
#include <errno.h>
#include <cstring>
#include <pthread.h>
#include <exception>
#include "InProvider.h"

namespace common {
namespace io {

class aio_read_exception : public std::exception {

public:
    enum code {
        ENQUE = 4,
        READ
    };

    aio_read_exception(code errcode) : errcode(errcode) {}

    virtual const char* what() const throw() {
        switch (errcode) {
        case (ENQUE) :
        return (std::string("enque error ") + strerror(errno)).c_str();
        break;
        case (READ) :
        return (std::string("read error ") + strerror(errno)).c_str();
        break;
        default:
        return (std::string("unknown error ") + strerror(errno)).c_str();
        break;
        }
        return (std::string("unknown error ") + strerror(errno)).c_str();
    }

private:
    code errcode;
};

class SocketInProvider : public InProvider {

public:
    SocketInProvider(long address, short port) :
        port(port), address(address), is_open(false) {
        //zeroes the aio control_block
        memset(&control_block, 0, sizeof(control_block));
    }

    virtual ~SocketInProvider() {
        close(file_descriptor);
    }

    virtual std::string debugInfo() const {
        return std::string(inet_ntoa(server_address.sin_addr));
    }

    virtual bool isOfTypeStreaming() const { return true; }

    void createSocket() {
        file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

        if (file_descriptor < 0) {
            std::cout << "Could not create socket! " << debugInfo() << std::endl;
            file_descriptor = -1;
        }
    }

    //blocking!
    void connectSocket() {
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(port);
        server_address.sin_addr.s_addr = htonl(address);

        if (connect(file_descriptor,
                reinterpret_cast<const sockaddr*>(&server_address),
                sizeof(server_address)) < 0) {
            std::cout << "Could not connect ! " << debugInfo() <<
                    ":" << port << std::endl;
        }
    }

    //busy blocks before the other write is done
    virtual bool regularReadCharBuffer(char* buffer, uint32_t size) const {
        if (!opened()) {
            std::cout<<"Cannot read to a not yet open channel "
                    <<debugInfo()<<std::endl;
            return false;
        }

        uint32_t read_bytes = 0;

        while ( read_bytes < size && read_bytes >= 0) {
            read_bytes +=
                 read(file_descriptor, buffer + read_bytes, size - read_bytes);
        }

        if (read_bytes == -1) {
            std::cout<<"Could not read in " << size <<" bytes from "
                    << debugInfo() << std::endl
                    << " with error " << strerror(errno) << std::endl;
            return false;
        }
        return true;
    }

    virtual void enqueBuffer(char* buffer, uint32_t size) const
                                    throw(aio_read_exception) {
        control_block.aio_fildes = file_descriptor;
        control_block.aio_buf = buffer;
        control_block.aio_nbytes = size;

        int result = aio_read(&control_block);

        if (result == -1 ) {
            throw aio_read_exception(aio_read_exception::ENQUE);
        }
    }

    //warning - does a pthread_yield until finished!
    virtual void waitForReadToFinish() const throw(aio_read_exception) {

        //TODO: yielding is less than ideal, maybe using a callback with the aio
        //stuff be worthwhile - Octavian
        while (readInProgress()) {
            pthread_yield();
        }
        if (aio_error(&control_block) != 0) {
            throw aio_read_exception(aio_read_exception::READ);
        }
    }

    virtual bool aioReadCharBuffer(char* buffer, uint32_t size) const {

        if (!opened()) {
            std::cout<<"Cannot read to a not yet open channel "
                    <<debugInfo()<<std::endl;
            return false;
        }

        try {
            enqueBuffer(buffer, size);
        } catch(aio_read_exception& err) {
            std::cerr << "Error with " << debugInfo() << " : " << std::endl
                    << err.what() << std::endl;
            return false;
        }

        try {
            waitForReadToFinish();
        } catch(aio_read_exception& err) {
            std::cerr << "Error with " << debugInfo() << " : " << std::endl
                    << err.what() << std::endl;
            return false;
        }

        //aio_return returns the number of bytes actually read
        //after the read is done
        uint32_t read_bytes = aio_return(&control_block);
        //read the rest of the buffer if we haven't read enough
        if (read_bytes < size) {
            return aioReadCharBuffer(buffer + read_bytes, size - read_bytes);
        }

        return true;
    }

    virtual bool readCharBuffer(char* buffer, uint32_t size) const {
        return aioReadCharBuffer(buffer, size);
    }

    virtual bool readInProgress() const {
        return aio_error(&control_block) == EINPROGRESS;
    }

    void openCommunicationChannel() {
        createSocket();
        connectSocket();
        is_open = true;
    }

    bool rewind(uint64_t offset) const {return false;}

    //blocking!
    virtual void peekAt(char* buffer, uint32_t size) const {
        if (!opened()) {
            std::cout<<"Cannot peek to a not yet open channel "
                    <<debugInfo()<<std::endl;
            return;
        }

        recv(file_descriptor, buffer, size, MSG_PEEK);
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
};

}
}
