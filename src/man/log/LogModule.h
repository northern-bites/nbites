#pragma once

#include "RoboGrams.h"
#include "IOExceptions.h"
#include <aio.h>
#include <errno.h>
#include <stdint.h>
#include <iostream>
#include <list>

namespace man {
namespace log {

static const std::string PATH = "/home/nao/nbites/frames/";
static const std::string HEADER = "THIS IS A DIFFERENT HEADER YEAH";
static const int NEW_FLAG = O_WRONLY | O_CREAT | O_TRUNC | O_APPEND;
static const int ALL_PERMISSIONS = S_IRWXU | S_IRWXG | S_IRWXO;

struct Write
{
    aiocb control;
    std::string buffer;
};

class LogBase : public portals::Module
{
public:
    LogBase(std::string name);

protected:
    virtual void run_() = 0;

    void openFile() throw (file_exception);
    void closeFile();
    void writeCharBuffer(const char* buffer, uint32_t size);
    void checkWrites();

    template <class T>
    void writeValue(const T &value) {
        writeCharBuffer(reinterpret_cast<const char *>(&value), sizeof(value));
    }

    std::list<Write> ongoing;
    bool fileOpen;
    int fileDescriptor;
    std::string fileName;
};

template<class T>
class LogModule : public LogBase {
public:
    LogModule(portals::OutPortal<T>* out, std::string name) : LogBase(name)
    {
        input.wireTo(out);
    }

    void writeMessage(T msg)
    {
        ongoing.push_back(Write());
        Write* current = &ongoing.back();
        msg.SerializeToString(&(current->buffer));
        writeValue<uint32_t>(msg.ByteSize());

        memset(&current->control, 0, sizeof(current->control));
        current->control.aio_fildes = fileDescriptor;
        current->control.aio_buf = const_cast<char *>(current->buffer.data());
        current->control.aio_nbytes = current->buffer.length();
        current->control.aio_sigevent.sigev_notify = SIGEV_NONE;

        int result = aio_write(&current->control);

        if (result == -1) {
            std::cout<< "AIO write enqueue failed with error " << strerror(errno)
                     << std::endl;
        }

    }

    void writeHeader()
    {
        std::cout << "Writing header to " << fileName << std::endl;
        writeCharBuffer(HEADER.data(), HEADER.length());
    }

protected:
    virtual void run_()
    {
        input.latch();

        if (!fileOpen) {
            try {
                openFile();
            } catch (io_exception& io_exception) {
                std::cout << io_exception.what() << std::endl;
                return;
            }
            this->writeHeader();
        }

        checkWrites();

        writeMessage(input.message());
    }

    portals::InPortal<T> input;
};

}
}
