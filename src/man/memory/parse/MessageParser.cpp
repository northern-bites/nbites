
#include <iostream>

#include "MessageParser.h"

namespace man {
namespace memory {
namespace parse {

using namespace std;
using boost::shared_ptr;
using namespace common::io;

MessageParser::MessageParser(InProvider::ptr in_provider,
                             MessageInterface::ptr objectToParseTo) :
        ThreadedParser(in_provider, objectToParseTo->getName() + "_parser"),
        objectToParseTo(objectToParseTo),
        current_message_size(0),
        current_buffer(NULL), current_buffer_size(0)
{

}

MessageParser::~MessageParser() {
    if (current_buffer) {
        free(current_buffer);
    }
    in_provider->closeChannel();
    this->stop();
    this->waitForThreadToFinish();
}

void MessageParser::run() {

    while (running) {
        if (!in_provider->opened()) {
            //blocking for socket fds, (almost) instant for other ones
            try {
                in_provider->openCommunicationChannel();
            } catch (io_exception& io_exception) {
                cout << io_exception.what() << endl;
                return;
            }
            this->readHeader();
        }
        //the order here matters; if getNext is put after waitForSignal
        //then when the thread tries to stop it will call getNext
        //and that will throw a pure virtual call error
        this->readNextMessage();
        //in streaming we get messages continuously,
        //so there's no need to wait
        if (!in_provider->isOfTypeStreaming()) {
            this->waitForSignal();
        }
    }
}

void MessageParser::readHeader() {

    MessageHeader header = this->readValue<MessageHeader>();

    cout << header << endl;
    objectToParseTo->setHeader(header);
}

void MessageParser::increaseBufferSizeTo(uint32_t new_size) {
    void* new_buffer = realloc(current_buffer, new_size);

    assert(new_buffer != NULL);
    current_buffer = reinterpret_cast<char*>(new_buffer);
    current_buffer_size = new_size;
}

bool MessageParser::readNextMessage() {

    if (in_provider->reachedEnd()) {
        return false;
    }

    current_message_size = this->readValue<uint32_t>();
    message_sizes.push_back(current_message_size);

    if (current_message_size > TOO_BIG_THRESHOLD) {
        cout << "Message size is too big! Cannot read in "
             << current_message_size << " bytes" << endl;
        return false;
    }

    if (current_message_size > current_buffer_size) {
        increaseBufferSizeTo(current_message_size);
    }

    bool result = readIntoBuffer(current_buffer, current_message_size);

    if (result == true) {
        objectToParseTo->parseFromString(current_buffer, current_message_size);
        return true;
    }
    return false;
}

bool MessageParser::readIntoBuffer(char* buffer, uint32_t num_bytes) {

    uint32_t bytes_read = 0;

    while (bytes_read < num_bytes) {

        try {
            in_provider->readCharBuffer(
                    buffer + bytes_read, num_bytes - bytes_read);
            in_provider->waitForReadToFinish();
            bytes_read += in_provider->bytesRead();
        } catch (read_exception& read_exception) {
            cout << read_exception.what() << " " << in_provider->debugInfo() << endl;
            return false;
        }
    }
    return true;
}

uint32_t MessageParser::sizeOfLastNumMessages(uint32_t n) const {
    uint32_t total_size = 0;
    for (uint i = message_sizes.size() - n; i < message_sizes.size(); i++) {
        total_size += message_sizes[i];
    }
    //also add the size taken up by the message size informa-ion themselves
    total_size += n*sizeof(uint32_t);
    return total_size;
}

uint32_t MessageParser::truncateNumberOfFramesToRewind(uint32_t n) const {
    if (n >= message_sizes.size()) {
        return message_sizes.size() - 1;
    } else {
        return n;
    }
}

bool MessageParser::getPrev(uint32_t n) {
    n = truncateNumberOfFramesToRewind(n);
    //we can't read backwards; that's why we rewind n+1 messages
    //then go forward one
    bool success = in_provider->rewind(sizeOfLastNumMessages(n+1));
    //rewind the message_sizes read
    for (uint i = 0; i < n+1; i++) {
        message_sizes.pop_back();
    }
    // A step back is sometimes a step forward too - the Tao of Octavian
    if (success) {
        this->signalToParseNext();
        return true;
    }
    return false;
}

bool MessageParser::getPrev() {
    return getPrev(1);
}

}
}
}
