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

#include <google/protobuf/message.h>

namespace memory {
namespace log {

typedef ::google::protobuf::Message ProtoMessage;

class Logger {

public:
    /**
     * @param m : the proto message to log
     * @return
     */
    Logger(const ProtoMessage* m) : message(m) {
    }
    /**
     * The write() method should write the message to some
     * sort of output buffer implemented in the respective
     * Logger subclass
     */
    virtual void write() = 0;

protected:
    const ProtoMessage *message;
};


}
}
