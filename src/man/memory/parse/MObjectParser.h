/**
 * @class MObjectParser
 *
 * Parser a MObject-stream type log
 *
 * First reads the log header, then calls to getNext() and getPrev()
 * advance or rewind the log
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <string>

#include "memory/MObject.h"
#include "Parser.h"
#include "ClassHelper.h"

namespace man {
namespace memory {
namespace parse {

struct LogHeader {

    MObject_ID log_id;
    int64_t birth_time;

};

class MObjectParser : public Parser {

    ADD_SHARED_PTR(MObjectParser)

public:
    MObjectParser(common::io::InProvider::const_ptr fdProvider,
            MObject::ptr objectToParseTo = MObject::NullInstanceSharedPtr());

    virtual ~MObjectParser();

    void initStreams();

    bool getNext();
    bool getPrev(uint32_t n);
    bool getPrev();

    virtual LogHeader getHeader() const {return log_header;}

    uint32_t sizeOfLastNumMessages(uint32_t n) const;

    void setObjectToParseTo(MObject::ptr newObject) {
        objectToParseTo = newObject;
    }

private:
    void readHeader();
    void increaseBufferSizeTo(uint32_t new_size);
    uint32_t truncateNumberOfFramesToRewind(uint32_t n) const;

private:
    MObject::ptr objectToParseTo;

    LogHeader log_header;

    uint32_t current_message_size;
    //history of message sizes read, useful for rewinding
    std::vector<uint32_t> message_sizes;

    char* current_buffer;
    uint32_t current_buffer_size;

};

}
}
}
