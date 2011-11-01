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
#include "FDProvider.h"

namespace man {
namespace include {
namespace io {

template <class T>
class BulkIO {

public:
    typedef boost::shared_ptr<BulkIO> ptr;
    typedef boost::shared_ptr<const BulkIO> const_ptr;
    typedef std::pair< T, FDProvider::ptr > FDProviderPair;
    typedef std::map< T, FDProvider::ptr > FDProviderMap;

public:
    BulkIO(){}
    static const_ptr NullBulkIO() {
        static const_ptr NullPtr(new BulkIO());
        return NullPtr;
    }
    virtual ~BulkIO() {};

    virtual void addFDProvider(T index, FDProvider::ptr fdProvider) {
        fdProviderMap.insert(FDProviderPair(index, fdProvider));
    }

    const FDProviderMap* getMap() const { return &fdProviderMap; }


protected:
    FDProviderMap fdProviderMap;
};

}
}
}

#endif
