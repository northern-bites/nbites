#include "LogModule.h"
<<<<<<< HEAD
#include "VisionDef.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace portals;
using namespace std;
=======
>>>>>>> 71579888c32df3f4b58dfbbd6ecaf60da9adbc1f

namespace man {
namespace log {

<<<<<<< HEAD
LogModule::LogModule() : Module(),
                         saved_frames(1)
{
}

void LogModule::run_()
{
    topImageIn.latch();
    writeFrame();
}

void LogModule::writeFrame()
{
    int MAX_FRAMES = 5000;
    if (saved_frames > MAX_FRAMES)
        return;

    stringstream num;
    num << saved_frames;
    string filename = FILEPATH + num.str() + EXT;


    fstream fout(filename.c_str(), fstream::out);
    fout.write(reinterpret_cast<char*>(topImageIn.message().get_image()),
               NAO_IMAGE_BYTE_SIZE);

    fout.close();
    cout << "Saved frame #" << saved_frames++ << endl;
=======
LogBase::LogBase(std::string name) : fileOpen(false),
                                     fileName(PATH+name)
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
    // Add a new Write struct
    ongoing.push_back(Write());
    Write* current = &ongoing.back();

    // Copy in the buffer
    current->buffer = buffer;

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

    // And let the list know it's done
    return true;
}

// Removes all finished writes from the list of ongoing writes
void LogBase::checkWrites()
{
    ongoing.remove_if(finished);
>>>>>>> 71579888c32df3f4b58dfbbd6ecaf60da9adbc1f
}

}
}
