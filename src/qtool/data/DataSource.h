/**
 * @class DataSource
 *
 * Represents a data source and provides file descriptors
 * By itself it will just initialize an empty map and always return 0
 * The classes that extend it should populate it with FDProviders
 *
 * @author Octavian Neamtu <oneamtu89@gmail.com>
 *
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "man/memory/Memory.h"
#include "man/include/io/BulkIO.h"

namespace qtool {
namespace data{

typedef man::memory::MObject_ID MObject_ID;

class DataSource : public man::include::io::BulkIO<MObject_ID> {

public:
    typedef boost::shared_ptr<DataSource> ptr;

    enum Type {
        offline = 1,
        online,
        old,
        null
    };

public:
    DataSource(Type type) : type(type) {}
    virtual ~DataSource() {}

    Type getType() { return type; }

private:
    Type type;
};

}
}
