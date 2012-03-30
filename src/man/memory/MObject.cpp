
#include "MObject.h"

namespace man {
namespace memory {

using namespace std;

MObject::MObject(MObject_ID id, ProtoMessage_ptr protoMessage)
    : SpecializedNotifier<MObject_ID>(id), my_id(id), protoMessage(protoMessage),
      objectMutex(MObject_names[id]), birth_time(time_stamp()) {
}

void MObject::update() {
    objectMutex.lock();
    this->updateData();
    objectMutex.unlock();
    this->notifySubscribers();
}

void MObject::serializeToString(string* write_buffer) const {
    if (protoMessage.get()) {
        objectMutex.lock();
        protoMessage->SerializeToString(write_buffer);
        objectMutex.unlock();
    }
}

void MObject::parseFromBuffer(const char* read_buffer, uint32_t buffer_size) {
    if (protoMessage.get()) {
        objectMutex.lock();
        protoMessage->ParseFromArray(read_buffer, buffer_size);
        objectMutex.unlock();
        this->notifySubscribers();
    }
}

unsigned MObject::byteSize() const {
    if (protoMessage.get()) {
        return protoMessage->ByteSize();
    }
    return 0;
}

}
}
