/**
 * Logger.hpp
 *
 * @class Logger
 *
 * An abstract class that provides the basic interface for
 * a proto message logger
 *
 *      Author: Octavian Neamtu
 *      E-mail: oneamtu@bowdoin.edu
 */

#pragma once

#include <iostream>
#include <google/protobuf/message.h>
#include "Logger.hpp"
//TODO: remove this
#include <fcntl.h>

namespace memory {
namespace log {

typedef ::google::protobuf::Message ProtoMessage;

class FileLogger : public Logger {

public:
    /**
     * @param m : the proto message to log
     * @return
     */
    FileLogger(const char* output_file_descriptor, const ProtoMessage* m) :
        Logger(m) {//file_descriptor(output_file_descriptor) {
        file_descriptor = open(output_file_descriptor,
                                       O_WRONLY | O_CREAT | O_TRUNC,
                                       S_IRWXU | S_IRWXG | S_IRWXO);
        if (file_descriptor < 0) {
            std::cout << "Invalid file descriptor passed for logging "
                    << m->GetTypeName() << std::endl;
        }
        std::cout << "File descriptor : " << file_descriptor << std::endl;
    }
    /**
     * The write() method should write the message to some
     * sort of output buffer implemented in the respective
     * Logger subclass
     */
    virtual void write() = 0;

private:
    /**
     * This method should write to the head of the file; this is only done
     * once, versus the other logging which happens in a loop
     */
    virtual void writeHead() = 0;


protected:
    int file_descriptor;
};

}
}
