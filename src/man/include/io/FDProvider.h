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

protected:
    virtual void openFileDescriptor() = 0;

protected:
    int file_descriptor;

};

}
}
}
