
#include "MObject.h"

namespace man {
namespace memory {

using namespace std;

MObject::MObject(MObject_ID id, ProtoMessage_ptr protoMessage)
    : id(id), protoMessage(protoMessage){
}

void MObject::serializeToString(string* write_buffer) const {
    protoMessage->SerializeToString(write_buffer);
}

void MObject::parseFromBuffer(const char* read_buffer, uint32_t buffer_size) {
    protoMessage->ParseFromArray(read_buffer, buffer_size);
}

unsigned MObject::byteSize() const {
    return protoMessage->ByteSize();
}

}
}
