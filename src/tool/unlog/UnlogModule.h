#pragma once

#include "RoboGrams.h"
#include "LogDefinitions.h"
#include <stdint.h>
#include <iostream>
#include <stdio.h>

namespace tool {
namespace unlog {

class UnlogBase : public portals::Module
{
public:
    UnlogBase(std::string path);
    virtual ~UnlogBase();

protected:
    virtual void run_() = 0;

    void openFile() throw (file_exception);
    void closeFile();
    uint32_t readCharBuffer(char* buffer, uint32_t size)
        const throw (file_read_exception);

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
        int len = HEADER.length();

        char head[len];
        uint32_t br = readCharBuffer(head, len);
        std::cout << "Read header from " << fileName << std::endl;
    }

    bool readNextMessage() {
        if (!fileOpen)
        {
            openFile();
            readHeader();
        }

        // End of file
        if (feof(file)) {
            std::cout << "End of log file " << fileName << std::endl;
            return false;
        }

        currentMessageSize = readValue<uint32_t>();

        messageSizes.push_back(currentMessageSize);

        uint32_t bytes;
        char buffer[currentMessageSize];

        try {
            bytes = readCharBuffer(buffer, currentMessageSize);
        } catch (std::exception& read_exception) {
            std::cout << read_exception.what() << std::endl;
            return false;
        }

        if (bytes) {
            T msg;
            msg.ParseFromString(std::string(buffer, bytes));
            // This is for debugging until the tool is back again
            std::cout << "\nCurrent Message:\n" << msg.DebugString();
            return true;
        }

        // We read zero bytes at the end of a file w/o hitting feof
        std::cout << "End of log file " << fileName << std::endl;
        return false;
    }

    std::vector<uint32_t> messageSizes;
    uint32_t currentMessageSize;
};

}
}
