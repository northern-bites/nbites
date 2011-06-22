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
#include <man/memory/Memory.h>

namespace qtool {
namespace data{

typedef man::memory::MObject_ID Data_ID;

class DataSource {

    typedef std::pair< Data_ID, FDProvider* > DataFDProviderPair;
    typedef std::map< Data_ID, FDProvider* > DataFDProviderMap;

public:
    typedef boost::shared_ptr<DataSource> ptr;
    struct Type {
        enum {
            offline = 1,
            online,
            old
        };
    };

public:
    DataSource(Type type) : type(type){}

    int getFileDescriptor(Data_ID data_ID) {

        DataFDProviderMap::iterator it = dataFDProviderMap.find(data_ID);

        if (it != dataFDProviderMap.end()) {
            //we found a legitimate object
            return it->second->getFileDescriptor();
        }
        return 0;
    }

    Type getType() { return type; }

private:
    DataFDProviderMap dataFDProviderMap;
    Type type;
};

}
}
