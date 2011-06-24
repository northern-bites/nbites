/**
 * File parser implementation
 * @author Octavian Neamtu
 */

#pragma once

#include <unistd.h>
#include <iostream>
#include <string>
#include <google/protobuf/message.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "Parser.h"

namespace man {
namespace memory {

namespace parse {

namespace proto_io = google::protobuf::io;
namespace proto = google::protobuf;

class MessageParser : public TemplatedParser <proto::Message>{

public:
    MessageParser(include::io::FDProvider::ptr fdProvider,
            boost::shared_ptr<proto::Message> message);

    virtual ~MessageParser();

    void initStreams();

    const LogHeader getHeader();
    bool getNext();
    boost::shared_ptr<const proto::Message> getPrev();
    boost::shared_ptr<const proto::Message> getCurrent();

private:
    void readHeader();

private:
    int current_size;

    bool finished;

    proto_io::FileInputStream* raw_input;
    proto_io::CodedInputStream* coded_input;

};

}
}
}
