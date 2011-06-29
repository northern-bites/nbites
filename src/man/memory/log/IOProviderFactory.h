/**
 * @class IOProviderFactory
 *
 * Provides some static methods that create different types of IOPoviders
 * useful in logging
 *
 * @author Octavian Neamtu <oneamtu89@gmail.com>
 */
#pragma once

#include "memory/MemoryIOBoard.h"
#include "memory/MObject.h"

namespace man {
namespace memory {
namespace log {

class IOProviderFactory {

public:
    static IOProvider::const_ptr newAllObjectsProvider();

};

}
}
}
