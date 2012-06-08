/**
 * @class ProtobufMessage
 *
 * Basic wrapper to the MessageInterface of a google protocol buffer message
 * To make it thread-safe a mutex-locked update function is introduced
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <string>
#include <stdint.h>
#include <iostream>
#include <google/protobuf/message.h>

#include "io/MessageInterface.h"
#include "Common.h"
#include "ClassHelper.h"

#include "Notifier.h"
#include "synchro/mutex.h"

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

public:
    static long long int time_stamp() {
        return realtime_micro_time();
    }

    ProtobufMessage(ProtoMessage_ptr protoMessage,
                    std::string name,
                    int32_t id_tag = DEFAULT_ID_TAG) :
        protoMessage(protoMessage), objectMutex(name + "mutex"),
        birth_time(time_stamp()), name(name), id_tag(id_tag) {
    }

public:
    virtual ~ProtobufMessage() {
    }

    // this method should update the fields in the protocol buffer
    // in some meaningful way
    virtual void updateData() = 0;

    virtual void update() {
        objectMutex.lock();
        this->updateData();
        objectMutex.unlock();
    }

    virtual void serializeToString(std::string* write_buffer) const {
        if (protoMessage.get()) {
            objectMutex.lock();
            protoMessage->SerializeToString(write_buffer);
            objectMutex.unlock();
        } else {
            std::cout << "Warning - trying to serialize NULL protoMessage"
                      << __FILE__ << " : " << __LINE__ << std::endl;
        }
    }

    virtual void parseFromBuffer(const char* read_buffer, uint32_t buffer_size) {
        if (protoMessage.get()) {
            objectMutex.lock();
            protoMessage->ParseFromArray(read_buffer, buffer_size);
            objectMutex.unlock();
        } else {
            std::cout << "Warning - trying to parse into NULL protoMessage"
                      << __FILE__ << " : " << __LINE__ << std::endl;
        }
    }

    virtual unsigned byteSize() const {
        unsigned byteSize = 0;
        if (protoMessage.get()) {
            objectMutex.lock();
            byteSize = protoMessage->ByteSize();
            objectMutex.unlock();
        }
        return byteSize;
    }

    ProtoMessage_const_ptr getProtoMessage() const { return protoMessage; }
    ProtoMessage_ptr getMutableProtoMessage() { return protoMessage; }

    virtual std::string getName() const { return name; }
    virtual int32_t getIDTag() const { return id_tag; }
    virtual long long getBirthTime() const { return birth_time; }

protected:
    ProtoMessage_ptr protoMessage;
    mutex objectMutex;
    long long int birth_time;
    std::string name;
    int32_t id_tag;

};

class NotifyingProtobufMessage : public ProtobufMessage, public Notifier {

public:
    NotifyingProtobufMessage(ProtoMessage_ptr protoMessage,
            std::string name) :
                ProtobufMessage(protoMessage, name) {
    }

    virtual ~NotifyingProtobufMessage() {
    }

    void update() {
        ProtobufMessage::update();
        this->notifySubscribers();
    }

    void parseFromBuffer(const char* read_buffer, uint32_t buffer_size) {
        ProtobufMessage::parseFromBuffer(read_buffer, buffer_size);
        this->notifySubscribers();
    }

};

}
}
