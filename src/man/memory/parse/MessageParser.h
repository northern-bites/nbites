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

#include "memory/MObject.h"
#include "Parser.h"

namespace man {
namespace memory {
namespace parse {

namespace proto_io = google::protobuf::io;
namespace proto = google::protobuf;

class MessageParser : public Parser {

public:
    MessageParser(include::io::FDProvider::const_ptr fdProvider,
            ProtoMessage_ptr message);

    virtual ~MessageParser();

    void initStreams();

    bool getNext();
    bool getPrev();

private:
    void readHeader();
    void increaseBufferSizeTo(uint32_t new_size);

private:
    ProtoMessage_ptr objectToParseTo;

    uint32_t current_message_size;
    char* current_buffer;
    uint32_t current_buffer_size;

    bool finished;

};

}
}
}
