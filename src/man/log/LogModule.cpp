#include "LogModule.h"

namespace man {
namespace log {

LogBase::LogBase(std::string name) : fileOpen(false),
                                     fileName(PATH+name),
                                     maxWrites(DEFAULT_MAX_WRITES)
{
}

LogBase::~LogBase()
{
    closeFile();
}

// Opens the file that we'll write to and gets its FD
void LogBase::openFile() throw (file_exception)
{
    fileDescriptor = open(fileName.c_str(), NEW_FLAG, ALL_PERMISSIONS);

    if (fileDescriptor < 0) {
        throw file_exception(file_exception::CREATE_ERR);
    }

    fileOpen = true;
}

// Close the file
void LogBase::closeFile()
{
    close(fileDescriptor);
    fileDescriptor = -1;
    fileOpen = false;
}

// Takes any char buffer, copies it into a Write struct, and
// enqueues the IO
void LogBase::writeCharBuffer(const char* buffer, uint32_t size)
{
    // Don't enqueue any more writes if we already have too many!
    if (ongoing.size() == maxWrites)
    {
#ifdef DEBUG_LOGGING
        std::cout << "Dropped a char buffer because there are already "
                  << maxWrites << " ongoing writes to " << fileName
                  << std::endl;
#endif
        return;
    }

    // Add a new Write struct
    ongoing.push_back(Write());
    Write* current = &ongoing.back();

    // Copy in the buffer
    current->buffer = std::string(buffer, size);

    // Recommended by aio--zeroes the CB
    memset(&current->control, 0, sizeof(current->control));

    // Set up the CB
    current->control.aio_fildes = fileDescriptor;
    current->control.aio_buf = const_cast<char *>(current->buffer.data());
    current->control.aio_nbytes = size;
    current->control.aio_sigevent.sigev_notify = SIGEV_NONE;

    // Enqueue the write
    int result = aio_write(&current->control);

    // Make sure the write didn't immediately fail
    if (result == -1)
    {
        std::cout<< "AIO write enqueue failed with error " << strerror(errno)
                 << std::endl;
    }

#ifdef DEBUG_LOGGING
        std::cout << "Enqueued a char buffer for writing. There are "
                  << ongoing.size() << " ongoing writes to " << fileName
                  << std::endl;
#endif
}

// The Predicate for remove_if
// See http://www.cplusplus.com/reference/list/list/remove_if/
bool finished(Write& write)
{
    // Check on the write
    int busy = aio_error(&(write.control));

    // If it's still going, let it do its thing
    if(busy == EINPROGRESS) return false;

    // If it's done, check for an error
    if(busy != 0)
    {
        std::cout<< "AIO write failed with error " << strerror(errno)
                 << std::endl;
    }

#ifdef DEBUG_LOGGING
    std::cout << "A write finished successfully."
                  << std::endl;
#endif

    // And let the list know it's done
    return true;
}

// Removes all finished writes from the list of ongoing writes
void LogBase::checkWrites()
{
    ongoing.remove_if(finished);
}

}
}
