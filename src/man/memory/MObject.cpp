
#include "MObject.h"

namespace man {
namespace memory {

using namespace std;

MObject::MObject(MObject_ID id, ProtoMessage_ptr protoMessage)
    : SpecializedNotifier<MObject_ID>(id),
      ProtobufMessage(protoMessage, MObject_names[id], (int32_t) id),
      my_id(id) {
}

void MObject::update() {
    ProtobufMessage::update();
    this->notifySubscribers();
}

void MObject::parseFromBuffer(const char* read_buffer, uint32_t buffer_size) {
    ProtobufMessage::parseFromBuffer(read_buffer, buffer_size);
    this->notifySubscribers();
}

}
}
