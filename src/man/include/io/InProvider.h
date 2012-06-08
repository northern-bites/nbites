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
    virtual void closeChannel() = 0;
    virtual bool opened() const = 0;
    virtual bool readInProgress() const = 0;
    virtual bool isOfTypeStreaming() const = 0;
    virtual bool reachedEnd() const = 0;
    virtual void peekAt(char* buffer, uint32_t size) const throw (read_exception) = 0;

    /* does a read from the source of the input
     * might be blocking or might be asynchronous (in which case
     * readInProgress() is going to return true as the read is running)
     *
     * once the read is done, readInProgress will return false
     * and bytesRead() will return the number of bytes read
     *
     * WARNING: it might legitimately read less bytes than the buffer size,
     * so don't bank on it always reading an entire buffer!
     */
    virtual void readCharBuffer(char* buffer, uint32_t size) const
                                        throw (read_exception) = 0;

    // returns the number of bytes actually read in the last read
    // call this after making sure a read is done
    virtual uint32_t bytesRead() const throw (read_exception) = 0;

    // peeks at the stream and interprets the next bytes as a structure of type
    // T; might block
    template<class T>
    T peekAndGet() const throw (read_exception) {
        T value;
        peekAt(reinterpret_cast<char *>(&value), sizeof(value));
        return value;
    }

};

}
}
