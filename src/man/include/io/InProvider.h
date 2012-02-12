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
#include "ClassHelper.h"
#include "IOProvider.h"
#include "IOExceptions.h"

namespace common {
namespace io {

class InProvider : public IOProvider {

ADD_SHARED_PTR(InProvider);

public:
    InProvider() {}
    virtual ~InProvider() {};

    virtual std::string debugInfo() const = 0;
    virtual bool rewind(long int offset) const = 0;
    virtual void openCommunicationChannel() throw (std::exception) = 0;
    virtual bool opened() const = 0;
    //return false because read() blocks until it finishes
    virtual bool readInProgress() const {return false;}
    virtual bool isOfTypeStreaming() const = 0;
    virtual bool reachedEnd() const = 0;
    virtual void peekAt(char* buffer, uint32_t size) const throw (read_exception) = 0;

    template<class T>
    T peekAndGet() const throw (read_exception) {
        T value;
        peekAt(reinterpret_cast<char *>(&value), sizeof(value));
        return value;
    }

    // returns the number of bytes actually read
    virtual uint32_t readCharBuffer(char* buffer, uint32_t size) const
                                        throw (read_exception) = 0;

    // only works if the size of T is small enough so it fits in one
    // read; we don't handle the possibility that it might not
    template <class T>
    T readValue() const throw (read_exception) {
        T value;
        readCharBuffer(reinterpret_cast<char *>(&value), sizeof(value));
        return value;
    }

};

}
}
