/*
 * @class BulkIO
 *
 * Keeps track of a bunch of FDProviders stored in a map that's indexed by
 * the template
 *
 * There is a 1-1 correspondance between the FDProvider and its subscriber/owner
 *
 * @author Octavian Neamtu <oneamtu89@gmail.com>
 */

#ifndef _BulkIO_h_
#define _BulkIO_h_

#include <map>
#include <boost/shared_ptr.hpp>
#include "IOProvider.h"
#include "ClassHelper.h"

namespace common {
namespace io {

template <class T>
class BulkIO {

    ADD_SHARED_PTR(BulkIO)
    ADD_NULL_INSTANCE(BulkIO)

public:
    typedef std::pair< T, IOProvider::ptr > IOProviderPair;
    typedef std::map< T, IOProvider::ptr > IOProviderMap;

public:
    BulkIO(){}
    virtual ~BulkIO() {};

public:
    virtual void addIOProvider(T index, IOProvider::ptr ioProvider) {
        ioProviderMap.insert(IOProviderPair(index, ioProvider));
    }

protected:
    virtual IOProvider::ptr tempIOProvider(T index) const {
        typename std::map< T, IOProvider::ptr >::const_iterator it =
                ioProviderMap.find(index);

        if (it != ioProviderMap.end()) {
            return it->second;
        } else {
            return IOProvider::ptr();
        }
    }


protected:
    IOProviderMap ioProviderMap;
};

}
}

#endif
