
#include <iostream>
#include <fcntl.h>

#include "MObjectParser.h"

namespace man {
namespace memory {
namespace parse {

using namespace std;
using namespace google::protobuf::io;
using boost::shared_ptr;
using namespace include::io;

MObjectParser::MObjectParser(FDProvider::const_ptr fdProvider,
        MObject::ptr objectToParseTo) :
        Parser(fdProvider),
        objectToParseTo(objectToParseTo),
        current_message_size(0),
        current_buffer(NULL), current_buffer_size(0)
{
    readHeader();
}

MObjectParser::~MObjectParser() {

    if (current_buffer) {
        free(current_buffer);
    }
}

void MObjectParser::readHeader() {

    this->readValue<int32_t>(log_header.log_id);
    cout << "Log ID: " << log_header.log_id << endl;

    this->readValue<int64_t>(log_header.birth_time);
    cout << "Birth time: " << log_header.birth_time << endl;
}

void MObjectParser::increaseBufferSizeTo(uint32_t new_size) {
    void* new_buffer = realloc(current_buffer, new_size);

    assert(new_buffer != NULL);
    current_buffer = reinterpret_cast<char*>(new_buffer);
    current_buffer_size = new_size;
}

bool MObjectParser::getNext() {

    this->readValue<uint32_t>(current_message_size);

    if (current_message_size > current_buffer_size) {
        increaseBufferSizeTo(current_message_size);
    }

    bool success = this->readCharBuffer(current_buffer, current_message_size);
    if (success) {
        objectToParseTo->parseFromBuffer(current_buffer, current_message_size);
        return true;
    }
    return false;
}

bool MObjectParser::getPrev() {

    bool success = fdProvider->rewind(current_message_size);
    // A step back is sometimes a step forward too - the Tao of Octavian
    if (success) {
        return this->getNext();
    }
    return false;
}

}
}
}
