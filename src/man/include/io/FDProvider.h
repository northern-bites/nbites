/**
 * @class FDProvider
 *
 * An abstract class that has the only role of providing a file descriptor
 * (somehow).
 *
 * On unix a file descriptor could represent anything from an actual file to
 * a socket or some shared memory stuff.
 *
 * So each child class of this class should implement some way of getting a
 * file descriptor.
 */


#pragma once

#include <boost/shared_ptr.hpp>
#include <string>
#include "include/Common.h"

namespace man {
namespace include {
namespace io {

class FDProvider {

public:
    typedef boost::shared_ptr<FDProvider> ptr;
    typedef boost::shared_ptr<const FDProvider> const_ptr;

public:
    FDProvider() : file_descriptor(0) {}
    virtual ~FDProvider() {};
    int getFileDescriptor() const { return file_descriptor; }

    virtual std::string debugInfo() const = 0;

    virtual uint64_t getCurrentPosition() const {
        return lseek64(file_descriptor, 0, SEEK_CUR);
    }

    virtual bool rewind(uint32_t offset) const { return true; }

protected:
    virtual void openFileDescriptor() = 0;

protected:
    int file_descriptor;

};

}
}
}
