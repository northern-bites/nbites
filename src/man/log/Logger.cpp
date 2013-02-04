#include "Logger.h"
#include <string.h>
#include <errno.h>

namespace man{
namespace log{

LoggerBase::LoggerBase(std::string name) : Thread(name),
                                           file_name(PATH + name),
                                           is_open(false)
{
    memset(&control_block, 0, sizeof(control_block));
}

LoggerBase::~LoggerBase() {
    this->stop();
    this->waitForThreadToFinish();
}

void LoggerBase::openCommunicationChannel() throw (file_exception) {

    file_descriptor = open(file_name.c_str(), NEW, PERMISSIONS_ALL);

    if (file_descriptor < 0) {
        throw file_exception(file_exception::CREATE_ERR);
    }

    is_open = true;
}

bool LoggerBase::fileOpened() const {
    return is_open;
}

void LoggerBase::closeCommunicationChannel() {
    close(file_descriptor);
    file_descriptor = -1;
    is_open = false;
}

bool LoggerBase::writingInProgress() const {
    return aio_error(&control_block) == EINPROGRESS;
}

void LoggerBase::waitForWriteToFinish() throw (std::runtime_error) {

    const struct aiocb* cblist[] = { &control_block };
    int result = aio_suspend(cblist, 1, NULL);

    if (result != 0) {
        throw_errno(errno);
    }
}

//yields before the other write is done!
void LoggerBase::writeCharBuffer(const char* buffer, uint32_t size) {
    if (!fileOpened()) {
        std::cout<<"Cannot write to an unopened channel!" << std::endl;
        return;
    }

    //const_casting is bad(!!!) but aio_write demands a non-const buffer
    control_block.aio_fildes = file_descriptor;
    control_block.aio_buf = const_cast<char *>(buffer);
    control_block.aio_nbytes = size;
    control_block.aio_sigevent.sigev_notify = SIGEV_NONE;
    int result = aio_write(&control_block);

    if (result == -1) {
        std::cout<<"AIO write enqueue failed with error " << strerror(errno) << std::endl;
    }

    while(writingInProgress()) {
        pthread_yield();
    }

    int bytes_written = aio_return(&control_block);

    if (bytes_written == -1) {
        std::cout<< "AIO write failed with error " << strerror(aio_error(&control_block)) << std::endl;
        this->closeCommunicationChannel();
    } else {
        if ((uint32_t) (bytes_written) < size) {
            writeCharBuffer(buffer + bytes_written, size - bytes_written);
        }
    }
}
}
}
