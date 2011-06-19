/**
 * ImageFDLogger.cpp
 *
 *  The structure for a log file:
 *  -- ID number for the type of object logged
 *  -- the birth_time timestamp of the man process that parents the logger
 *
 *  -- for each message
 *  ---- size of serialized message
 *  ---- serialized message
 *
 *      Author: Octavian Neamtu
 *      E-mail: oneamtu@bowdoin.edu
 */

#include <fcntl.h>
#include <unistd.h>
#include <iostream>

#include "Common.h"
#include "ImageFDLogger.h"
#include "ImageAcquisition.h"

namespace memory {

extern long long birth_time;

namespace log {

using namespace std;

ImageFDLogger::ImageFDLogger(const FDProvider* fdp,
        int logTypeID,
        const RoboImage* roboImage) :
        FDLogger(fdp),
        current_buffer(new (void*)),
        current_buffer_size(1),
        bytes_written(0),
        logID(logTypeID),
        roboImage(roboImage)
{
    raw_output = new FileOutputStream(file_descriptor,
            roboImage->getByteSize() + sizeof(roboImage->getTimestamp()));
    cout << raw_output->GetErrno() << endl;
    writeHead();
}

void ImageFDLogger::writeHead() {
    this->getNextBuffer();
    uint32_t bytes_written = 0;
    // this helps us ID the log
    this->writeValue<int32_t>(logID, &bytes_written);
    // this time stamps the log
    this->writeValue<int64_t>(birth_time, &bytes_written);
    this->writeValue<uint32_t>(roboImage->getWidth(), &bytes_written);
    this->writeValue<uint32_t>(roboImage->getHeight(), &bytes_written);
    this->writeValue<uint32_t>(roboImage->getByteSize(), &bytes_written);
    //the buffer isn't full yet - back up will make sure only the used part
    //is read
    raw_output->BackUp(current_buffer_size - bytes_written);
}

void ImageFDLogger::write() {
    uint32_t bytes_written = 0;
    this->getNextBuffer();
    this->writeValue<int64_t>(process_micro_time() - birth_time, &bytes_written);
    _copy_image(const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(roboImage->getImage())),
            reinterpret_cast<uint8_t*> (*current_buffer) + bytes_written);
}

const uint8_t* ImageFDLogger::getCurrentImage() const {
    return reinterpret_cast<uint8_t*> (*current_buffer);
}

void ImageFDLogger::getNextBuffer() {

    raw_output->Next(current_buffer, &current_buffer_size);
    //we're not guaranteed a non-empty buffer
    //but we're guaranteed one eventually
    while (current_buffer_size == 0) {
        raw_output->Next(current_buffer, &current_buffer_size);
    }
}

ImageFDLogger::~ImageFDLogger() {

    raw_output->Close();
    delete raw_output;
    close(file_descriptor);
}
}
}
