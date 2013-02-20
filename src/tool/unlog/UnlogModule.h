#pragma once

#include "RoboGrams.h"
#include "/home/ecat/nbites/src/man/log/IOExceptions.h"
#include <stdint.h>
#include <iostream>
#include <stdio.h>

namespace tool {
namespace unlog {

// Log version--in case we need to upgrade and stay backwards compatible
static const std::string VERSION = "2.0";
// Header. This could be updated with more useful information.
static const std::string HEADER = "NORTHERN BITES LOG FILE VERSION " + VERSION;
class UnlogBase : public portals::Module
{
public:
    UnlogBase(std::string path);
    virtual ~UnlogBase();

protected:
    virtual void run_() = 0;

    void openFile() throw (man::log::file_exception);
    void closeFile();
    uint32_t readCharBuffer(char* buffer, uint32_t size)
        const throw (man::log::file_read_exception);

    template <typename T>
    T readValue() {
        T value;
        readCharBuffer((char *)(&value), sizeof(value));
        return value;
    }

    bool fileOpen;
    FILE* file;
    std::string fileName;
};

template<class T>
class UnlogModule : public UnlogBase
{
public:
    UnlogModule(std::string path) : UnlogBase(path),
                                    output(base()) {}

    portals::OutPortal<T> output;

protected:
    void run_()
    {
        readNextMessage();
    }

    void readHeader()
    {
        char head[35];
        uint32_t br = readCharBuffer(head, 35);
        std::cout << head << std::endl;
        std::cout << br << std::endl;
    }

    bool readNextMessage() {
        if (!fileOpen)
        {
            openFile();
            readHeader();
        }

        // End of file
        if (feof(file)) {
            return false;
        }

        currentMessageSize = readValue<uint32_t>();
        std::cout << "got size " << currentMessageSize << std::endl;

        messageSizes.push_back(currentMessageSize);

        uint32_t bytes;
        char buffer[35];

        try {
            bytes = readCharBuffer(buffer, 35);
        } catch (std::exception& read_exception) {
            std::cout << read_exception.what() << std::endl;
            return false;
        }

        std::cout << "read " << bytes << " bytes." << std::endl;
        std::cout << "buffer is " << buffer << std::endl;

        if (bytes) {
            T msg;
            std::cout << "Bytesize " << msg.ByteSize() << std::endl;
            msg.ParseFromString(buffer);
            std::cout << msg.DebugString() << std::endl;

            return true;
        }

        return false;
    }

    std::vector<uint32_t> messageSizes;
    uint32_t currentMessageSize;
};

}
}


