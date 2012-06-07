/**
 *
 * @class MessageInterface
 *
 *  Interface class for a Message-type object that can be serialized and parsed back
 *
 *  @author Octavian Neamtu
 *
**/

#pragma once

#include <string>
#include <stdint.h>

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
};

class MessageInterface {

    ADD_SHARED_PTR(MessageInterface)

public:
    MessageInterface() {}
    virtual ~MessageInterface() {};

    virtual std::string getName() const = 0;
    virtual int32_t getIDTag() const = 0;
    virtual long long getBirthTime() const = 0;

    virtual void update() = 0;
    virtual void serializeToString(std::string* write_buffer) const = 0;
    virtual void parseFromBuffer(const char* read_buffer, uint32_t buffer_size) = 0;
    virtual unsigned byteSize() const = 0;

protected:
    MessageHeader header;
};

}
}
