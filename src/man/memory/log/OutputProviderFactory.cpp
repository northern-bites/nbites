
#include <string>

#include <iostream>

#include "OutputProviderFactory.h"
#include "io/FileOutProvider.h"
#include "io/SocketOutProvider.h"
#include "memory/MemoryCommon.h"
#include "CommDef.h"

using namespace std;
using namespace common::io;
using namespace common::paths;

namespace man {
namespace memory {
namespace log {

void OutputProviderFactory::AllFileOutput(LoggingBoard* loggingBoard,
        string log_folder_path) {
	for (MObject_ID id = FIRST_OBJECT_ID; id != LAST_OBJECT_ID; id++) {
		string file_name = log_folder_path + "/" +
				MObject_names[static_cast<int>(id)] + NAO_LOG_EXTENSION;
		loggingBoard->newOutputProvider(
				OutProvider::ptr(new FileOutProvider(file_name)), id);
	}
}

void OutputProviderFactory::AllSocketOutput(const Memory* memory,
                                            LoggingBoard* loggingBoard) {
        for (Memory::const_iterator it = memory->begin(); it != memory->end(); it++) {
            loggingBoard->newOutputProvider(
                OutProvider::ptr(new SocketOutProvider(
                                     static_cast<unsigned short>(it->second->getID()) + STREAMING_PORT_BASE)), it->second->getID());
        }
    }
}
}
}
