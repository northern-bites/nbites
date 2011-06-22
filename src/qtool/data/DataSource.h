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
#include <vector>

namespace qtool {
namespace data{

class DataSource {

    typedef std::pair< int, man::include::io::FDProvider::ptr > FDProviderPair;
    typedef std::map< int, man::include::io::FDProvider::ptr > FDProviderMap;

public:
    typedef boost::shared_ptr<DataSource> ptr;
    enum Type {
        offline = 1,
        online,
        old
    };

public:
    DataSource(Type type);

    std::vector<int> getFileDescriptors();
    void addProvider(man::include::io::FDProvider::ptr fdprovider);

    Type getType() { return type; }

private:
    FDProviderMap fdProviderMap;
    Type type;
};

}
}
