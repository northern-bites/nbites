/*
 * Logger.hpp
 *
 *      Author: oneamtu
 */

#pragma once

#include <google/protobuf/message.h>

typedef ::google::protobuf::Message ProtoMessage;

class Logger;

class Logger {

public:
    Logger(const ProtoMessage* m) : message(m) {
    }
    virtual void write() = 0;

protected:
    const ProtoMessage *message;
};
