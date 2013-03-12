/*
 * @class LogModule
 * @class LogBase
 *
 * The LogModule templated class is can take any type of message input
 * and will log it to a file. It inherits from LogBase to avoid template
 * bloat and so that we can keep lists etc of LogModules if necessary.
 * The base class provides basic IO operations and the templated class
 * specializes it for messages.
 *
 * We use AIO because it allows us to queue writes, then immediately return
 * rather than having the program block. See
 *      http://www.ibm.com/developerworks/linux/library/l-async/
 * for a good explanation of what this means. This is why you will see
 * all of the aio calls rather than simple writes.
 *
 * Note that log files are written HEADER | SIZE | MSG | SIZE | MSG ...
 * where each SIZE gives the number of bytes in the next MSG.
 *
 * @author Lizzie Mamantov
 * @date February 2013
 * @author Octavian Neamtu
 * @date 2012 -- Much of the file stuff here was stolen from the previous
 *               version of the NBites logging system.
 */

#pragma once

#include "RoboGrams.h"
#include "LogDefinitions.h"
#include "DebugConfig.h"
#include <aio.h>
#include <errno.h>
#include <stdint.h>
#include <iostream>
#include <list>

namespace man {
namespace log {

// The filepath on the robot where logs will end up
static const std::string PATH = "/home/nao/nbites/log/";

// Flags needed to open files appropriately
static const int NEW_FLAG = O_WRONLY | O_CREAT | O_TRUNC | O_APPEND;
static const int ALL_PERMISSIONS = S_IRWXU | S_IRWXG | S_IRWXO;

// Default value for write list size; can be reset--should be small
// for large messages that could take up a lot of memory!
static const int DEFAULT_MAX_WRITES = 10;

/*
 * This struct is used to hold together an aiocb (control block) and the
 * buffer it's writing from. These need to stay unchanged while the write
 * is occurring and the control block needs a pointer to its buffer, so
 * it makes things easier to keep them together in this struct.
 */
struct Write
{
    aiocb control;
    std::string buffer;
};

// Base Class
class LogBase : public portals::Module
{
public:
    // The name is used as the filename
    LogBase(std::string name);
    virtual ~LogBase();

    // For controlling the max size of write list
    int getMaxWrites() { return maxWrites; }
    void setMaxWrites(int max) { maxWrites = max; }

protected:
    // Note that inheriting classes still need to implement this!
    virtual void run_() = 0;

    // Basic file/writing operations
    void openFile() throw (file_exception);
    void closeFile();
    void writeCharBuffer(const char* buffer, uint32_t size);
    void checkWrites();

    // Writes any type that can be reinterpret_casted to a string
    template <class T>
    void writeValue(const T &value) {
        writeCharBuffer(reinterpret_cast<const char *>(&value), sizeof(value));
    }

    // Keeps track of all writes that haven't finished yet
    std::list<Write> ongoing;
    // Has the file been opened?
    bool fileOpen;
    // The file that we've opened/are writing to
    int fileDescriptor;
    // The full path of the file
    std::string fileName;
    // Stores the maximum number of writes that should happen concurrently
    unsigned int maxWrites;
};

// Template Class
template<class T>
class LogModule : public LogBase {
public:
    /*
     * @brief Takes an OutPortal and wires it to this new module so that
     *        we can log its output.
     */
    LogModule(portals::OutPortal<T>* out, std::string name) : LogBase(name)
    {
        input.wireTo(out);
    }

    /*
     * @brief Serializes a message and writes the message's size and
     *        serialization to the file. Note: relies on protobuf methods
     *        like SerializeToString. If you ever want this class to log
     *        something that is not a protobuf, make sure it has these
     *        methods and can pretend to be one, or specialize this template.
     */
    void writeMessage(T msg)
    {
        // Don't enqueue this write if we've hit the upper limit
        if (ongoing.size() == maxWrites)
        {
#ifdef DEBUG_LOGGING
        std::cout << "Dropped a message because there are already "
                  << maxWrites << " ongoing writes to " << fileName
                  << std::endl;
#endif
        return;
        }

        // Add a new write to the list of current writes
        ongoing.push_back(Write());
        Write* current = &ongoing.back();

        // Serialize directly into the Write's buffer to avoid a copy
        msg.SerializeToString(&(current->buffer));
        // Write ths size of the message that will be written
        writeValue<uint32_t>(current->buffer.length());

        // Recommended by aio--zeroes the control block
        memset(&current->control, 0, sizeof(current->control));

        // Configure the control block
        current->control.aio_fildes = fileDescriptor;
        current->control.aio_buf = const_cast<char *>(current->buffer.data());
        current->control.aio_nbytes = current->buffer.length();
        current->control.aio_sigevent.sigev_notify = SIGEV_NONE;

        // Enqueue the write
        int result = aio_write(&current->control);

        // Verify that the write didn't immediately fail
        if (result == -1) {
            std::cout<< "AIO write enqueue failed with error " << strerror(errno)
                     << std::endl;
        }

#ifdef DEBUG_LOGGING
        std::cout << "Enqueued a message for writing. There are "
                  << ongoing.size() << " ongoing writes to " << fileName
                  << std::endl;
#endif
    }

    // Simply writes the header defined at the beginning of this file
    void writeHeader()
    {
        std::cout << "Writing header to " << fileName << std::endl;
        writeCharBuffer(HEADER.data(), HEADER.length());
    }

protected:
    // Implements the Module run_ method
    virtual void run_()
    {
        input.latch();

        // Open the file and write the header if it hasn't been done
        if (!fileOpen) {
            try {
                openFile();
            } catch (io_exception& io_exception) {
                std::cout << io_exception.what() << std::endl;
                return;
            }
            this->writeHeader();
        }

        // Check for and remove finished writes from the list
        checkWrites();

        // Start a new write for the current message
        writeMessage(input.message());
    }

    portals::InPortal<T> input;
};

}
}
