/**
 * ZeroCopyFileLogger.hpp
 *
 * @class ZeroCopyFileLogger
 *
 * This class provides a way to serialize a proto message to a file
 * in a sequential manner.
 * It uses the google/protobuf/io stuff.
 *
 * read more:
 * http://code.google.com/apis/protocolbuffers/docs/reference/cpp/google.protobuf.io.coded_stream.html
 *
 * some of the code is inspired from the example provided in the link
 *
 *      Author: Octavian Neamtu
 *      E-mail: oneamtu@bowdoin.edu
 */

#pragma once

#include <string>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "FDLogger.h"

namespace memory {
namespace log {

using namespace google::protobuf::io;

class ZeroCopyFileLogger : public FDLogger {

public:
    /**
     * Opens the file fileName and writes to its head
     *
     * @param logTypeID : an ID written to the head of the log identifying the log
     * @return
     */
    ZeroCopyFileLogger(std::string output_file_descriptor,
                       int logTypeID, ProtoMessage* m);

    /**
     * Closes the file, which will flush the output buffer
     * to ensure that the file on disk is in sync with the buffer
     * @return
     */
    ~ZeroCopyFileLogger();

    void write();

private:
    void getNextBuffer();
    void writeHead();

private:
    void** current_buffer;
    int current_buffer_size;
    unsigned long long bytes_written;
    int logID;

   /**
    * @var raw_output : a ZeroCopyOutputStream, an abstract I/O interface
    * which will minimize the amount of copying to the buffer
    */
    FileOutputStream* raw_output;

};
}
}
