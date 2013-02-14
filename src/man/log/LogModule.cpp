#include "LogModule.h"

namespace man {
namespace log {

LogBase::LogBase(std::string name) : fileOpen(false),
                                     fileName(PATH+name)
{
}

void LogBase::openFile() throw (file_exception)
{
    fileDescriptor = open(fileName.c_str(), NEW_FLAG, ALL_PERMISSIONS);

    if (fileDescriptor < 0) {
        throw file_exception(file_exception::CREATE_ERR);
    }

    fileOpen = true;
}

void LogBase::closeFile()
{
    close(fileDescriptor);
    fileDescriptor = -1;
    fileOpen = false;
}

void LogBase::writeCharBuffer(const char* buffer, uint32_t size)
{
    ongoing.push_back(Write());
    Write* current = &ongoing.back();
    current->buffer = buffer;
    memset(&current->control, 0, sizeof(current->control));
    current->control.aio_fildes = fileDescriptor;
    current->control.aio_buf = const_cast<char *>(current->buffer.data());
    current->control.aio_nbytes = size;
    current->control.aio_sigevent.sigev_notify = SIGEV_NONE;

    int result = aio_write(&current->control);

    if (result == -1)
    {
        std::cout<< "AIO write enqueue failed with error " << strerror(errno)
                 << std::endl;
    }
}

bool finished(Write& write)
{
    int busy = aio_error(&(write.control));

    if(busy == EINPROGRESS) return false;

    if(busy != 0)
    {
        std::cout<< "AIO write failed with error " << strerror(errno)
                 << std::endl;
    }

    return true;
}

void LogBase::checkWrites()
{
    ongoing.remove_if(finished);
}

}
}
