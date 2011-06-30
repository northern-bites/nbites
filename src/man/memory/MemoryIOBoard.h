/**
 * @class MemoryIOBoard
 *
 * Abstract class that will define some way to associate parsers or loggers
 * with memory objects
 *
 * @author Octavian Neamtu <oneamtu89@gmail.com>
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include "include/io/BulkIO.h"
#include "MObject.h"

namespace man {
namespace memory {

typedef include::io::BulkIO<MObject_ID> IOProvider;

template <class IOobject>
class MemoryIOBoard {

public:
    typedef std::pair< MObject_ID,
            boost::shared_ptr<IOobject> > ObjectIOPair;
    typedef std::map< MObject_ID,
            boost::shared_ptr<IOobject> > ObjectIOMap;

public:
    MemoryIOBoard() { }
    // this function should create new IO Objects based on the
    // what the IOProvider is providing us with
    // Note: this function should also save the instance of IOProvider
    // passed to it - since we want to keep ownership of the IOProvider
    // until we get a new one (if we don't it might get destroyed
    // and then we are screwed
    virtual void newIOProvider(IOProvider::const_ptr ioProvider) = 0;

protected:
    IOProvider::const_ptr ioProvider;
    ObjectIOMap objectIOMap;
};

}
}
