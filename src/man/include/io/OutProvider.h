/**
 * @class OutProvider
 *
 * An abstract class that has the only role of providing means of writing
 * to a file descriptor
 *
 * @author Octavian Neamtu
 */


#pragma once

#include <boost/shared_ptr.hpp>
#include <string>
#include <unistd.h>
#include "ClassHelper.h"
#include "IOProvider.h"

namespace man {
namespace common {
namespace io {

class InProvider : public IOProvider {

ADD_SHARED_PTR(InProvider);

public:
    InProvider() {}
    virtual ~InProvider() {};

    virtual std::string debugInfo() const = 0;
    virtual bool rewind(uint64_t offset) const = 0;
    virtual void openCommunicationChannel() = 0;

    virtual void writeCharBuffer(const char* buffer, uint32_t size) {
        uint32_t result = 0;
        result = write(file_descriptor, buffer, size);

        if (result != size) {
            std::cout<<"Could not write out " << size <<" bytes for "
                     << debugInfo() << std::endl;
        }
    }

    template <class T>
    void writeValue(T &value) {
        writeCharBuffer(reinterpret_cast<char *>(&value), sizeof(value));
    }

};

}
}
}
