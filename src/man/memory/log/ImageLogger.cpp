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

ImageLogger::ImageLogger(FDProvider::const_ptr fdp,
        int logTypeID,
        boost::shared_ptr<const RoboImage> roboImage) :
        FDLogger(fdp),
        bytes_written(0),
        logID(logTypeID),
        roboImage(roboImage),
        exceeded(false)
{
    writeHead();
}

void ImageLogger::writeHead() {

    // this helps us ID the log
    this->writeValue<int32_t>(logID);
    // this time stamps the log
    this->writeValue<int64_t>(birth_time);
    this->writeValue<uint32_t>(roboImage->getWidth());
    this->writeValue<uint32_t>(roboImage->getHeight());
    this->writeValue<uint32_t>(roboImage->getByteSize());
}

void ImageLogger::writeToLog() {
    if (!exceededByteLimit()) {
        writeValue<int64_t>(process_micro_time() - birth_time);
        write(file_descriptor,
                roboImage->getImage(), roboImage->getByteSize());
    } else {
        cout << "Stopped Image logging because we reached size limit!" << endl;
        exceeded = true;
    }
}

bool ImageLogger::exceededByteLimit() {
    return exceeded || bytes_written > BYTES_MAX;
}


ImageLogger::~ImageLogger() {
    cout << "Done, written " << bytes_written <<
            " to " << file_descriptor_provider->debugInfo() << endl;
}
}
}
}
