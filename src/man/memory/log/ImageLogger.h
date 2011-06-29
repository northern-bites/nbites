/**
 * ImageLogger.h
 *
 * @class ImageLogger
 *
 * This class provides a way to serialize a robo image to a file
 * in a sequential manner.
 * It uses the google/protobuf/io stuff.
 *
 * It provides an empty buffer of an appropriate size in which an image
 * can be copied into for writing out.
 *
 * read more:
 * http://code.google.com/apis/protocolbuffers/docs/reference/cpp/google.protobuf.io.coded_stream.html
 *
 *
 *      Author: Octavian Neamtu
 *      E-mail: oneamtu@bowdoin.edu
 */

#pragma once

#include <string>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <stdint.h>

#include "FDLogger.h"
#include "memory/RoboImage.h"

namespace man {
namespace memory {
namespace log {

using namespace google::protobuf::io;

class ImageLogger : public FDLogger {

public:
    typedef boost::shared_ptr<ImageLogger> ptr;
    typedef boost::shared_ptr<const ImageLogger> const_ptr;

public:
    /**
     * Opens the file fileName and writes to its head
     *
     * @param logTypeID : an ID written to the head of the log identifying the log
     */
    ImageLogger(FDProvider::const_ptr fdp,
                int logTypeID,
                boost::shared_ptr<const RoboImage> roboImage);

    /**
     * Closes the file, which will flush the output buffer
     * to ensure that the file on disk is in sync with the buffer
     */
    virtual ~ImageLogger();

    void writeToLog();

public:
    static const unsigned long long BYTES_MAX = 1200000000; //1.2 GB

private:
    void writeHead();
    /*
     * Writes a value of type T to the current buffer.
     * It writes it at the specified offset and then
     * increments the offset with the size of the value written.
     */
    template <class T>
    unsigned int writeValue(T value) {
        //TODO: assert if we actually write everything
        return write(file_descriptor, &value, sizeof(value));
    }


private:
    unsigned long long bytes_written;
    int logID;
    boost::shared_ptr<const RoboImage> roboImage;
    bool exceeded;
};
}
}
}
