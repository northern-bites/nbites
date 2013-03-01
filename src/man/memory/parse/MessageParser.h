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

#include "io/MessageInterface.h"
#include "ThreadedParser.h"
#include "ClassHelper.h"
#include "Notifier.h"

namespace man {
namespace memory {
namespace parse {

struct LogHeader {

    int32_t log_id;
    int64_t birth_time;

};

class MessageParser : public ThreadedParser {

    ADD_SHARED_PTR(MessageParser)

    typedef common::io::MessageInterface Message;

public:
    static const uint32_t TOO_BIG_THRESHOLD = 2000000; // ~2MB

public:
    MessageParser(common::io::InProvider::ptr in_provider,
                  Message::ptr objectToParseTo);

    virtual ~MessageParser();

    void readHeader();
    void openFile();
    bool readNextMessage();
    bool getPrev(uint32_t n);
    bool getPrev();

    virtual void run();

    uint32_t sizeOfLastNumMessages(uint32_t n) const;

    void setObjectToParseTo(Message::ptr newObject) {
        objectToParseTo = newObject;
    }

    void signalToParseNext() {
        this->signalToResume();
    }

private:
    void increaseBufferSizeTo(uint32_t new_size);
    uint32_t truncateNumberOfFramesToRewind(uint32_t n) const;

    template <typename T>
    T readValue() {
        T value;
        readIntoBuffer((char *)(&value), sizeof(value));
        return value;
    }

    bool readIntoBuffer(char* buffer, uint32_t num_bytes);

private:
    Message::ptr objectToParseTo;

    uint32_t current_message_size;
    //history of message sizes read, useful for rewinding
    std::vector<uint32_t> message_sizes;

    char* current_buffer;
    uint32_t current_buffer_size;

};

}
}
}
