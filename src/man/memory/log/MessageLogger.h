/**
 * MessageLogger.hpp
 *
 * @class MessageLogger
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
#include <google/protobuf/message.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "FDLogger.h"

namespace man {
namespace memory {
namespace log {

//TODO: remove this
using namespace google::protobuf::io;
typedef ::google::protobuf::Message ProtoMessage;

class MessageLogger : public FDLogger {

public:
    /**
     * Opens the file fileName and writes to its head
     *
     * @param fileName : the name of the file we log to
     * @param logTypeID : an ID written to the head identifying the log
     * @param m : the proto message we will log
     * @return
     */
    MessageLogger(FDProvider::const_ptr fdp,
    		int logTypeID, boost::shared_ptr<const ProtoMessage> m);

    /**
     * Closes the file, which will flush the output buffer
     * to ensure that the file on disk is in sync with the buffer
     * @return
     */
    virtual ~MessageLogger();
    void writeToLog();
    void writeHead();

private:
    int logID;
    boost::shared_ptr<const ProtoMessage> message;
    std::string write_buffer;
   /**
    * @var raw_output : a ZeroCopyOutputStream, an abstract I/O interface
    * which will minimize the amount of copying to the buffer
    *
    * @var coded_output : a CodedOutputStream which wraps the ZeroCopyOutputStream and
    * adds some nice features such as being able to output variable size integers
    * useful for recording things like file IDs and message sizes
    */
    FileOutputStream* raw_output;
    CodedOutputStream* coded_output;

};
}
}
}
