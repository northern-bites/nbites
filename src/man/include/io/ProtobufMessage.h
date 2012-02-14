/**
 * @class ProtobufMessage
 *
 * Basic wrapper to the MessageInterface of a google protocol buffer message
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <string>
#include <stdint.h>
#include <google/protobuf/message.h>

#include "io/MessageInterface.h"
#include "Common.h"
#include "ClassHelper.h"

namespace common {
namespace io {

class ProtobufMessage: public common::io::MessageInterface {

ADD_SHARED_PTR(ProtobufMessage)

public:
    typedef google::protobuf::Message ProtoMessage;
    typedef boost::shared_ptr<ProtoMessage> ProtoMessage_ptr;
    typedef boost::shared_ptr<const ProtoMessage> ProtoMessage_const_ptr;

public:
    static const int32_t DEFAULT_ID_TAG = 42;
    static const std::string DEFAULT_NAME = "ProtobufMessage";

public:
    static long long int time_stamp() {
        return realtime_micro_time();
    }

    ProtobufMessage(ProtoMessage_ptr protoMessage) :
        protoMessage(protoMessage), birth_time(time_stamp()) {
    }

public:
    virtual ~ProtobufMessage() {
    }

    virtual std::string getName() const {
        return DEFAULT_NAME;
    }
    virtual int32_t getIDTag() const {
        return DEFAULT_ID_TAG;
    }
    virtual long long getBirthTime() const {
        return birth_time;
    }
    virtual void serializeToString(std::string* write_buffer) const {
        protoMessage->SerializeToString(write_buffer);
    }
    virtual void parseFromBuffer(const char* read_buffer, uint32_t buffer_size) {
        protoMessage->ParseFromArray(read_buffer, buffer_size);
    }
    virtual unsigned byteSize() const {
        return protoMessage->ByteSize();
    }

    ProtoMessage_const_ptr getProtoMessage() const {
        return protoMessage;
    }
    ProtoMessage_ptr getMutableProtoMessage() {
        return protoMessage;
    }

protected:
    ProtoMessage_ptr protoMessage;
    long long int birth_time;

};

}
}
