
#include <string>

#include <iostream>

#include "IOProviderFactory.h"
#include "include/NaoPaths.h"
#include "include/io/FileFDProvider.h"

namespace man {
namespace memory {
namespace log {

using namespace std;
using namespace include::io;
using namespace include::paths;

IOProvider::const_ptr IOProviderFactory::newAllObjectsProvider() {
    IOProvider::ptr ioProvider(new IOProvider());
    for (MObject_ID id = FIRST_OBJECT; id != LAST_OBJECT; id++) {
        string file_name = NAO_LOG_DIR + MObject::NameFromID(id) + NAO_LOG_EXTENSION;
        ioProvider->addFDProvider(id,
                FDProvider::ptr(new FileFDProvider(file_name, NEW)));
    }
    return ioProvider;
}

}
}
}
