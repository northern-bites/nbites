#pragma once

#include "RoboGrams.h"
#include "IOExceptions.h"
#include "Common.h"
#include "synchro.h"
#include <string>
#include <fcntl.h>
#include <aio.h>
#include <stdint.h>
#include <stdexcept>
#include <iostream>

namespace man {
namespace log {

static const std::string PATH = "/home/nao/nbites/frames";

class LoggerBase : public synchro::Thread {

    enum OpenFlags {
        //O_APPEND is crucial if you want to use aio_write on the file
        //descriptor you open
        NEW = O_WRONLY | O_CREAT | O_TRUNC | O_APPEND,
        EXISTING = O_RDONLY
    };

    enum OpenPermissions {
        PERMISSIONS_ALL = S_IRWXU | S_IRWXG | S_IRWXO
    };

public:
    LoggerBase(std::string name);
    virtual ~LoggerBase();

    virtual void writeToLog() = 0;
    virtual void writeHeader() = 0;
    virtual void run() = 0;
    void openCommunicationChannel() throw (file_exception);
    bool fileOpened() const;
    void closeCommunicationChannel();
    bool writingInProgress() const;
    void waitForWriteToFinish() throw (std::runtime_error);
    void writeCharBuffer(const char* buffer, uint32_t size);

    template <class T>
    void writeValue(const T &value) {
        writeCharBuffer(reinterpret_cast<const char *>(&value), sizeof(value));
    }

protected:
    std::string file_name;
    struct aiocb control_block;
    bool is_open;
    int file_descriptor;
};

template<class T>
class Logger : public LoggerBase {
public:
    Logger(portals::OutPortal<T>* out, std::string name) : LoggerBase(name)
    {
        input.wireTo(out, true);
    }

    void writeToLog()
    {
        input.latch();
        input.message().SerializeToString(&write_buffer);
        writeValue<uint32_t>(sizeof(write_buffer));
        writeCharBuffer(write_buffer.data(), write_buffer.size());
    }

    void writeHeader()
    {
        write_buffer = "NBITES " + name + " LOG VERSION 1.0 ";
        writeCharBuffer(write_buffer.data(), write_buffer.size());
    }


    void run()
    {
        while (running) {
            if (!fileOpened()) {
                try {
                    openCommunicationChannel();
                } catch (io_exception& io_exception) {
                    std::cout << io_exception.what() << std::endl;
                    return;
                }
                std::cout << "Writing header to " << file_name << std::endl;
                this->writeHeader();
            }
            this->waitForSignal();
            this->writeToLog();
            this->waitForWriteToFinish();
        }
    }

protected:
    portals::InPortal<T> input;
    std::string write_buffer;
};

}
}
