/**
 *
 * @class MessageInterface
 *
 *  Interface class for a Message-type object that can be serialized and parsed back
 *  Also has a header
 *
 *  @author Octavian Neamtu
 *
**/

#pragma once

#include <string>
#include <cstring>
#include <stdint.h>
#include <iostream>

#include "ClassHelper.h"

namespace common {
namespace io {

/**
 * A header that uniquely identifies the message
 */
struct MessageHeader {
    char name[56];
    int64_t birth_time;
    char padding[64];
    char more_padding[128]; //in case we need it for other stuff in the future

    friend std::ostream& operator<< (std::ostream &o, const MessageHeader &h) {
        return o << "(" << h.name << "," << h.birth_time << ")";
    }
};



class MessageInterface {

    ADD_SHARED_PTR(MessageInterface)

public:
    MessageInterface(std::string name, int64_t birth_time) {
        assert(name.size() < 56); //name limit
        strcpy(header.name, name.c_str());
        header.birth_time = birth_time;
    }
    virtual ~MessageInterface() {};

    virtual const MessageHeader& getHeader() const { return header; }
    virtual void setHeader(MessageHeader& _header) { header = _header; }

    virtual std::string getName() const { return std::string(header.name); }

    virtual void serializeToString(std::string* write_buffer) const = 0;
    virtual void parseFromString(const char* read_buffer, uint32_t buffer_size) = 0;
    virtual unsigned byteSize() const = 0;

protected:
    MessageHeader header;
};

}
}
