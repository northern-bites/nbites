/*
 * @class BulkIn
 *
 * Keeps track of a bunch of InProviders stored in a map
 *
 * There is a 1-1 correspondance between the FDProvider and its subscriber/owner
 *
 * @author Octavian Neamtu <oneamtu89@gmail.com>
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include "ClassHelper.h"
#include "BulkIO.h"
#include "InProvider.h"

namespace common {
namespace io {

template <class T>
class BulkIn : public BulkIO<T> {

    ADD_SHARED_PTR(BulkIn)
    ADD_NULL_INSTANCE(BulkIn)

public:
    BulkIn(){}
    virtual ~BulkIn() {};

    virtual void addFDProvider(T index, InProvider::ptr fdProvider) {
        BulkIO<T>::addIOProvider(index, fdProvider);
    }

    virtual InProvider::ptr getIOProvider(T index) const {
        return boost::dynamic_pointer_cast<InProvider, IOProvider>(
                BulkIO<T>::tempIOProvider(index));
    }
};

}
}
