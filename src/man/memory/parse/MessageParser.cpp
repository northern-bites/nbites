
#include <iostream>
#include <fcntl.h>

#include "MessageParser.h"

namespace man {
namespace memory {
namespace parse {

using namespace std;
using namespace google::protobuf::io;
using boost::shared_ptr;
using namespace include::io;

MessageParser::MessageParser(FDProvider::const_ptr fdProvider,
        boost::shared_ptr<proto::Message> message) :
        Parser(fdProvider),
        objectToParseTo(message),
        current_message_size(0),
        current_buffer(NULL), current_buffer_size(0)
{
    readHeader();
}

MessageParser::~MessageParser() {

    if (current_buffer) {
        free(current_buffer);
    }
}

void MessageParser::readHeader() {

    this->readValue<int32_t>(log_header.log_id);
    cout << "Log ID: " << log_header.log_id << endl;

    this->readValue<int64_t>(log_header.birth_time);
    cout << "Birth time: " << log_header.birth_time << endl;
}

void MessageParser::increaseBufferSizeTo(uint32_t new_size) {
    void* new_buffer = realloc(current_buffer, new_size);

    assert(new_buffer != NULL);
    current_buffer = reinterpret_cast<char*>(new_buffer);
    current_buffer_size = new_size;
}

bool MessageParser::getNext() {

    this->readValue<uint32_t>(current_message_size);

    if (current_message_size > current_buffer_size) {
        increaseBufferSizeTo(current_message_size);
    }

    this->readCharBuffer(current_buffer, current_message_size);
    objectToParseTo->ParseFromArray(current_buffer, current_message_size);
}

bool MessageParser::getPrev() {


//    raw_input->BackUp(current_size);
//
//    proto::uint32 size;
//    coded_input->ReadVarint32(&size);
//
//    CodedInputStream::Limit l = coded_input->PushLimit(size);
//    finished = container->ParseFromCodedStream(coded_input);
//    coded_input->PopLimit(l);
}

}
}
}
