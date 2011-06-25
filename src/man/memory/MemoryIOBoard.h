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

namespace man {
namespace memory {

template <class IOobject>
class MemoryIOBoard {

public:
    typedef std::pair< MObject_ID,
            boost::shared_ptr<IOobject> > ObjectParserPair;
    typedef std::map< MObject_ID,
            boost::shared_ptr<IOobject> > ObjectParserMap;

protected:
    typedef include::io::BulkIO<MObject_ID> IOProvider;

public:
    MemoryIOBoard() { }

    virtual void newIOProvider(IOProvider::const_ptr ioProvider) = 0;

protected:
    ObjectParserMap objectParserMap;
};

}
}
