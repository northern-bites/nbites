/**
 * @class InProvider
 *
 * An abstract class that has the only role of providing means of reading
 * from a file descriptor
 *
 * @author Octavian Neamtu
 */


#pragma once

#include <boost/shared_ptr.hpp>
#include <iostream>
#include <unistd.h>
#include "ClassHelper.h"
#include "IOProvider.h"

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

    virtual bool readCharBuffer(char* buffer, uint32_t size)  const {
        uint32_t result = 0;
        result = read(file_descriptor, buffer, size);

        if (result != size) {
            std::cout<<"Could not read in " << size <<" bytes for "
                     << debugInfo() << std::endl;
            return false;
        }
        return true;
    }

    template <class T>
    void readValue(T &value) const {
        readCharBuffer(reinterpret_cast<char *>(&value), sizeof(value));
    }

};

}
}
