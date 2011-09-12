
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
        TemplatedParser<proto::Message>(fdProvider, message)
{
    initStreams();
    readHeader();
}

MessageParser::~MessageParser() {

    delete coded_input;
    delete raw_input;
}

void MessageParser::readHeader() {

    coded_input->ReadLittleEndian32(&(log_header.log_id));
    cout << "Log ID: " << log_header.log_id << endl;

    coded_input->ReadLittleEndian64(&(log_header.birth_time));
    cout << "Birth time: " << log_header.birth_time << endl;
}

const LogHeader MessageParser::getHeader() {
    return log_header;
}

bool MessageParser::getNext() {

    proto::uint32 size;
    uint64_t byte_count = raw_input->ByteCount();

    coded_input->ReadLittleEndian32(&size);
    cout << size << endl;
    //current_size = size;

    CodedInputStream::Limit l = coded_input->PushLimit(size);
    finished = container->ParseFromCodedStream(coded_input);
    coded_input->PopLimit(l);

    //current_size = raw_input->ByteCount() - byte_count;
    //TODO: fixo - return
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

    raw_input = new FileInputStream(fdProvider->getFileDescriptor());
    coded_input = new CodedInputStream(raw_input);
    coded_input->SetTotalBytesLimit(2000000000, 2000000000);


}

}
}
}
