/**
 * MessageLogger.hpp
 *
 * @class MessageLogger - provides the methods to write serialized MObject data
 * to an OutProvider
 *
 * @author Octavian Neamtu
 *
 */

#pragma once

#include <string>
#include "io/MessageInterface.h"
#include "ThreadedLogger.h"

namespace man {
namespace memory {
namespace log {

class MessageLogger : public ThreadedLogger {

public:
    typedef boost::shared_ptr<MessageLogger> ptr;
    typedef boost::shared_ptr<const MessageLogger> const_ptr;
    typedef common::io::MessageInterface Message;

public:
    MessageLogger(OutProvider::ptr out_provider,
                  Message::const_ptr objectToLog);

    virtual ~MessageLogger();

    void writeToLog();
    void writeHead();

    void run();

private:
    Message::const_ptr messageToLog;
    std::string write_buffer;

};
}
}
}
