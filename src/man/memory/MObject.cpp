
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

unsigned MObject::byteSize() const {
    return protoMessage->ByteSize();
}

}
}
