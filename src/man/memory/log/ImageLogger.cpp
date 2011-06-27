/**
 * ImageLogger.cpp
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

#include "include/Common.h"
#include "ImageLogger.h"
#include "corpus/ImageAcquisition.h"

namespace man {
namespace memory {

extern long long birth_time;

namespace log {

using namespace std;

ImageLogger::ImageLogger(const FDProvider* fdp,
        int logTypeID,
        const RoboImage* roboImage) :
        FDLogger(fdp),
        bytes_written(0),
        logID(logTypeID),
        roboImage(roboImage)
{
    writeHead();
}

void ImageLogger::writeHead() {

    // this helps us ID the log
    bytes_written += this->writeValue<int32_t>(logID);
    // this time stamps the log
    bytes_written += this->writeValue<int64_t>(birth_time);
    bytes_written += this->writeValue<uint32_t>(roboImage->getWidth());
    bytes_written += this->writeValue<uint32_t>(roboImage->getHeight());
    bytes_written += this->writeValue<uint32_t>(roboImage->getByteSize());
}

void ImageLogger::writeToLog() {
    bytes_written += writeValue<int64_t>(process_micro_time() - birth_time);
    bytes_written += write(file_descriptor,
            roboImage->getImage(), roboImage->getByteSize());
}


ImageLogger::~ImageLogger() {
    cout << "Done, written " << bytes_written <<
            " to " << file_descriptor_provider->debugInfo() << endl;
}
}
}
}
