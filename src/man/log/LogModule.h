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
#include "Images.h"
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
static const int DEFAULT_MAX_WRITES = 5;

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

// Alternate version for images
struct ImageWrite
{
    aiocb control;
    messages::YUVImage image;
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
    void checkSizeWrites();
    static bool finished(Write& write);
    static bool imageFinished(ImageWrite& write);

    void writeCharBuffer(const char* buffer, uint32_t size);

    void writeSize(uint32_t value)
    {
        writeCharBuffer(reinterpret_cast<const char *>(&value), sizeof(value));
    }

    // Keeps track of all writes that haven't finished yet
    std::list<Write> ongoingSizes;
    // Has the file been opened?
    bool fileOpen;
    // The file that we've opened/are writing to
    int fileDescriptor;
    // The full path of the file
    std::string fileName;
    // Stores the maximum number of writes that should happen concurrently
    unsigned int maxWrites;
    // Stores how much we've written to this file to avoid huge files
    unsigned int bytesWritten;
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

    // Writes out a header protobuf
    void writeHeader()
    {
        Header head;
        head.set_name(input.message().GetTypeName());
        head.set_version(CURRENT_VERSION);
        head.set_timestamp(42);

        std::string buf;
        head.SerializeToString(&buf);
        writeSize(buf.length());
        writeCharBuffer(buf.data(), buf.length());

        std::cout << "Writing header to " << fileName << std::endl;
    }

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

        // Don't write if the file has gotten too huge
        if (bytesWritten >= FILE_MAX_SIZE)
        {
#ifdef DEBUG_LOGGING
            std::cout << "Dropped a message because the file "
                      << fileName << " has reached " << bytesWritten << " bytes "
                      << std::endl;
#endif
            ongoing.pop_back();
            return;
        }

        writeInternal(msg);
    }

protected:
    // Handles all of the message-specific writing actions. Kept as a
    // separate helper method so that it can be specialized for images
    // or any other non-proto types
    void writeInternal(T msg)
    {
        // Add a new write to the list of current writes
        ongoing.push_back(Write());
        Write* current = &ongoing.back();

        // Serialize directly into the Write's buffer to avoid a copy
        msg.SerializeToString(&(current->buffer));

        bytesWritten += current->buffer.length();

        // Write ths size of the message that will be written
        writeSize(current->buffer.length());

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
        std::cout << "Enqueued a message for writing."
                  << std::endl;
#endif

    }

    void checkMessageWrites()
    {
        ongoing.remove_if(LogBase::finished);
    }

    // Implements the Module run_ method
    virtual void run_()
    {
        /* Only possible to log 1 in every 5 images, do same thing with
           all unlogers to stay synced with images */
        frameCounter++;
        // EPIC HACK: 10-second delay
        if (frameCounter < 300) return;
        // EPIC HACK: don't try to log every image
        if (frameCounter%5 != 0) return;

        input.latch();

        // Open the file and write the header if it hasn't been done
        if (!fileOpen) {
            try {
                openFile();
            } catch (io_exception& io_exception) {
                std::cout << io_exception.what() << std::endl;
                return;
            }
            writeHeader();
        }

        // Check for and remove finished writes from the list
        checkSizeWrites();
        checkMessageWrites();

        // Start a new write for the current message
        writeMessage(input.message());
    }

    portals::InPortal<T> input;
    std::list<Write> ongoing;
    int frameCounter;
};

// Special for Images
template<class T>
class ImageLogModule : public LogBase {
public:
    /*
     * @brief Takes an OutPortal and wires it to this new module so that
     *        we can log its output.
     */
    ImageLogModule(portals::OutPortal<T>* out, std::string name) : LogBase(name)
    {
        input.wireTo(out);
    }

    // Writes out a header protobuf
    void writeHeader()
    {
        Header head;
        // Specialized to YUVImage, fixme
        head.set_name("messages.YUVImage");
        head.set_version(CURRENT_VERSION);
        head.set_timestamp(42);

        if(getIdFromPath(fileName) == "top")
        {
            head.set_top_camera(true);
        }
        else if(getIdFromPath(fileName) == "bottom")
        {
            head.set_top_camera(false);
        }
        else
        {
            std::cout << "Warning: Unexpected camera type specified by file name."
                      << std::endl;
        }

        std::string buf;
        head.SerializeToString(&buf);
        writeSize(buf.length());
        writeCharBuffer(buf.data(), buf.length());

        std::cout << "Writing header to " << fileName << std::endl;
    }

    void writeImage(T msg)
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

        // Don't write if the file has gotten too huge
        if (bytesWritten >= FILE_MAX_SIZE)
        {
#ifdef DEBUG_LOGGING
            std::cout << "Dropped a message because the file "
                      << fileName << " has reached " << bytesWritten << " bytes "
                      << std::endl;
#endif
            ongoing.pop_back();
            return;
        }

        writeInternal(msg);
    }

protected:
    // Handles all of the image-specific writing actions. Kept as a
    // separate helper method so that it can be specialized for images
    // or any other non-proto types
    void writeInternal(T msg)
    {
        // Add a new write to the list of current writes
        ongoing.push_back(ImageWrite());
        ImageWrite* current = &ongoing.back();
        current->image = msg;

        // We know the width, height of the image and what each pixel holds
        int size = msg.width() * msg.height() * sizeof(unsigned char);
        bytesWritten += size;

        // We write width and height as well as size for clarity on unlogging side
        writeSize(size);
        writeSize(msg.width());
        writeSize(msg.height());

        // Configure the control block
        current->control.aio_fildes = fileDescriptor;
        // Note we don't use the Write's string field here.
        current->control.aio_buf = current->image.pixelAddress(0, 0);
        current->control.aio_nbytes = size;
        current->control.aio_sigevent.sigev_notify = SIGEV_NONE;

        // Enqueue the write
        int result = aio_write(&current->control);

        // Verify that the write didn't immediately fail
        if (result == -1) {
            std::cout<< "AIO write enqueue failed with error " << strerror(errno)
                     << std::endl;
        }

#ifdef DEBUG_LOGGING
        std::cout << "Enqueued an image for writing."
                  << std::endl;
#endif
    }

    void checkImageWrites()
    {
        ongoing.remove_if(LogBase::imageFinished);
    }

    // Implements the Module run_ method
    virtual void run_()
    {
        // Check for and remove finished writes from the list
        checkSizeWrites();
        checkImageWrites();

        frameCounter++;

        // EPIC HACK: 10-second delay
        if (frameCounter < 300) return;

        // EPIC HACK: don't try to log every image
        if (frameCounter%5 != 0) return;

        input.latch();

        // Open the file and write the header if it hasn't been done
        if (!fileOpen) {
            try {
                openFile();
            } catch (io_exception& io_exception) {
                std::cout << io_exception.what() << std::endl;
                return;
            }
            writeHeader();
        }

        // Start a new write for the current message
        writeImage(input.message());
    }

    portals::InPortal<T> input;
    std::list<ImageWrite> ongoing;
    int frameCounter;
};
}
}
