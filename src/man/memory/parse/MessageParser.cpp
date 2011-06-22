
#include <iostream>
#include <fcntl.h>

#include "MessageParser.h"

namespace man {
namespace memory {
namespace parse {

using namespace std;
using namespace google::protobuf::io;
using boost::shared_ptr;

//TODO: use file descriptor providers
MessageParser::MessageParser(boost::shared_ptr<proto::Message> message,
                       int _file_descriptor) :
        Parser<proto::Message>(message),
        file_descriptor(_file_descriptor)
{

    initStreams();
    readHeader();

}

MessageParser::MessageParser(boost::shared_ptr<proto::Message> message,
                       const char* _file_name) :
       Parser<proto::Message>(message) {

    file_descriptor = open(_file_name, O_RDONLY);

    initStreams();
    readHeader();
}

MessageParser::~MessageParser() {

    delete coded_input;
    delete raw_input;
    close(file_descriptor);
}

void MessageParser::readHeader() {


    cout << "Log ID: " << log_header.log_id << endl;

    coded_input->ReadLittleEndian64(&(log_header.birth_time));
    cout << "Birth time: " << log_header.birth_time << endl;
}

const LogHeader MessageParser::getHeader() {
    return log_header;
}

shared_ptr<const proto::Message> MessageParser::getNext() {

    proto::uint32 size;
    uint64_t byte_count = raw_input->ByteCount();
    coded_input->ReadVarint32(&size);

    //current_size = size;

    CodedInputStream::Limit l = coded_input->PushLimit(size);
    finished = container->ParseFromCodedStream(coded_input);
    coded_input->PopLimit(l);

    //current_size = raw_input->ByteCount() - byte_count;

    return container;
}

shared_ptr<const proto::Message> MessageParser::getPrev() {


    raw_input->BackUp(current_size);

    proto::uint32 size;
    coded_input->ReadVarint32(&size);

    CodedInputStream::Limit l = coded_input->PushLimit(size);
    finished = container->ParseFromCodedStream(coded_input);
    coded_input->PopLimit(l);
}


void MessageParser::initStreams() {

    raw_input = new FileInputStream(file_descriptor);
    coded_input = new CodedInputStream(raw_input);
    coded_input->SetTotalBytesLimit(2000000000, 2000000000);


}

}
}
}
