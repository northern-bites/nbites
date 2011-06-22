/**
 * ImageFDLogger.hpp
 *
 * @class ImageFDLogger
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

class ImageFDLogger : public FDLogger {

public:
    /**
     * Opens the file fileName and writes to its head
     *
     * @param logTypeID : an ID written to the head of the log identifying the log
     */
    ImageFDLogger(const FDProvider* fdp,
                  int logTypeID,
                  const RoboImage* roboImage);

    /**
     * Closes the file, which will flush the output buffer
     * to ensure that the file on disk is in sync with the buffer
     */
    ~ImageFDLogger();

    void write();
    const uint8_t* getCurrentImage() const;


private:
    void getNextBuffer();
    void writeHead();
    /*
     * Writes a value of type T to the current buffer.
     * It writes it at the specified offset and then
     * increments the offset with the size of the value written.
     */
    template <class T>
    void writeValue(T value, uint32_t* offset) {
        uint8_t* byte_ptr_to_write_to =
                reinterpret_cast<uint8_t*> (*current_buffer) + *offset;
    	*(reinterpret_cast<T*>(byte_ptr_to_write_to)) = value;
    	*offset += sizeof(T);
    }


private:
    void** current_buffer;
    int current_buffer_size;
    unsigned long long bytes_written;
    int logID;
    const RoboImage* roboImage;

   /**
    * @var raw_output : a ZeroCopyOutputStream, an abstract I/O interface
    * which will minimize the amount of copying to the buffer
    */
    FileOutputStream* raw_output;

};
}
}
}
