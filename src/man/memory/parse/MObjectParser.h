/**
 * File parser implementation
 * @author Octavian Neamtu
 */

#pragma once

#include <string>

#include "memory/MObject.h"
#include "Parser.h"

namespace man {
namespace memory {
namespace parse {

namespace proto_io = google::protobuf::io;
namespace proto = google::protobuf;

class MObjectParser : public Parser {

public:
    MObjectParser(include::io::FDProvider::const_ptr fdProvider,
            MObject::ptr objectToParseTo);

    virtual ~MObjectParser();

    void initStreams();

    bool getNext();
    bool getPrev();

    uint32_t sizeOfLastNumMessages(uint32_t n) const;

private:
    void readHeader();
    void increaseBufferSizeTo(uint32_t new_size);

private:
    MObject::ptr objectToParseTo;

    uint32_t current_message_size;
    std::vector<uint32_t> message_sizes;
    char* current_buffer;
    uint32_t current_buffer_size;

    bool finished;

};

}
}
}
