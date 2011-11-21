
#include <string>

#include <iostream>

#include "IOProviderFactory.h"
#include "include/NaoPaths.h"
#include "include/io/FileFDProvider.h"

namespace man {
namespace memory {
namespace log {

using namespace std;
using namespace common::paths;

//BulkFDProvider::const_ptr IOProviderFactory::createAllFileFDProviders() {
//    BulkFDProvider::ptr ioProvider(new BulkFDProvider());
//    for (MObject_ID id = FIRST_OBJECT; id != LAST_OBJECT; id++) {
//        string file_name = NAO_LOG_DIR +
//                MObject_names[static_cast<int>(id)] + NAO_LOG_EXTENSION;
//        ioProvider->addFDProvider(id,
//                IOProvider::ptr(new FileFDProvider(file_name, NEW)));
//    }
//    return ioProvider;
//}

}
}
}
