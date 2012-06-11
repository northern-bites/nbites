/**
 * @class ProtobufMessage
 *
 * Basic wrapper to the MessageInterface of a google protocol buffer message
 * To make it thread-safe a mutex-locked update function is introduced
 *
 * By default it uses the underlying ProtobufMessage wrapper object to implement the serialization/
 * de-serialization of the data
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

class ProtobufMessage: public MessageInterface, public Notifier {

ADD_SHARED_PTR(ProtobufMessage)

public:
    typedef google::protobuf::Message ProtoMessage;

public:
    static long long int time_stamp() {
        return realtime_micro_time();
    }

    ProtobufMessage(std::string name, ProtoMessage* protoMessage) :
        MessageInterface(name, time_stamp()),
        protoMessage(protoMessage), objectMutex(getName() + "mutex") {
    }

public:
    virtual ~ProtobufMessage() {
    }

    // thread-safety locks; while the lock is on, the object won't serialize
    // or parse back in
    // use them with care!

    // TODO: we might need a read lock; what if we're accessing values from the memory object
    // and it just gets updated?
    void lock() {
        objectMutex.lock();
    }

    void release() {
        objectMutex.unlock();
    }

    virtual void serializeToString(std::string* write_buffer) const {
        if (protoMessage) {
            objectMutex.lock();
            protoMessage->SerializeToString(write_buffer);
            objectMutex.unlock();
        } else {
            std::cout << "Warning - trying to serialize NULL protoMessage"
                      << __FILE__ << " : " << __LINE__ << std::endl;
        }
    }

    virtual void parseFromString(const char* read_buffer, uint32_t buffer_size) {
        if (protoMessage) {
            objectMutex.lock();
            protoMessage->ParseFromArray(read_buffer, buffer_size);
            objectMutex.unlock();
            this->notifySubscribers();
        } else {
            std::cout << "Warning - trying to parse into NULL protoMessage"
                      << __FILE__ << " : " << __LINE__ << std::endl;
        }
    }

    //TODO: the byte size might change between calling this method and calling
    // serializeToString; maybe we should just use the byte size of the serializeToString
    // buffer that gets returned instead of using this!
    virtual unsigned byteSize() const {
        unsigned byteSize = 0;
        if (protoMessage) {
            objectMutex.lock();
            byteSize = protoMessage->ByteSize();
            objectMutex.unlock();
        }
        return byteSize;
    }

    const ProtoMessage* getProtoMessage() const { return protoMessage; }
    ProtoMessage* getMutableProtoMessage() { return protoMessage; }

protected:
    ProtoMessage* protoMessage;
    mutex objectMutex;
};

// the template must always be a Protobuf Message class
template <class ProtoType>
class TemplatedProtobufMessage : public ProtobufMessage {

public:
    TemplatedProtobufMessage(std::string name, ProtoType* protoMessage = new ProtoType) :
                                                 //HACK: make the protoMessage in
                                                 //ProtobufMessage also a regular
                                                 //pointer
            ProtobufMessage(name, protoMessage), data(protoMessage) {   }

public:
    virtual ~TemplatedProtobufMessage(){ delete protoMessage; }

    const ProtoType* get() const { return data; }
    ProtoType* get() { return data; }

protected:
    //TODO: get rid of boost::shared_ptr crap; this could be a regular object (or pointer)
    //passed by reference
    ProtoType* data;
};

}
}
