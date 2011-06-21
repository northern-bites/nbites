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

typedef memory::MObject_ID Data_ID;

class DataSource {

    typedef pair< Data_ID, FDProvider* > DataFDProviderPair;
    typedef map< Data_ID, FDProvider* > DataFDProviderMap;

public:
    typedef boost::shared_ptr<DataSource> ptr;

    DataSource() {}
    int getFileDescriptor(Data_ID data_ID) {

        DataFDProviderMap::iterator it = dataFDProviderMap.find(data_ID);

        if (it != dataFDProviderMap.end()) {
            //we found a legitimate object
            return it->second->getFileDescriptor();
        }
        return 0;
    }

private:
    DataFDProviderMap dataFDProviderMap;
};

}
}
