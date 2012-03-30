/**
 * @class IOProvider
 *
 * An abstract class that has the only role of providing means of IO through
 * a file descriptor
 *
 * On unix a file descriptor could represent anything from an actual file to
 * a socket or some shared memory stuff.
 *
 * So each child class of this class should implement some way of getting a
 * file descriptor
 *
 * Note: the IOProvider will not open the file descriptor automatically;
 * Instead, one has to call openCommunicationChannel for that to happen
 */


#pragma once

#include <boost/shared_ptr.hpp>
#include <string>
#include "include/Common.h"
#include "ClassHelper.h"
#include <unistd.h>

namespace common {
namespace io {

class IOProvider {

ADD_SHARED_PTR(IOProvider);

public:
    IOProvider() {}
    virtual ~IOProvider() {};

    virtual std::string debugInfo() const = 0;

    virtual void openCommunicationChannel() = 0;
    virtual bool opened() const = 0;
};

}
}
