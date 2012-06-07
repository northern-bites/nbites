
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

void OutputProviderFactory::AllFileOutput(const Memory* memory, LoggingBoard* loggingBoard,
        string log_folder_path) {

    for (Memory::const_iterator it = memory->begin(); it != memory->end(); it++) {
        string file_name = log_folder_path + "/" + it->first + NAO_LOG_EXTENSION;
		loggingBoard->newOutputProvider(OutProvider::ptr(new FileOutProvider(file_name)), it->first);
	}
}

void OutputProviderFactory::AllSocketOutput(const Memory* memory,
        LoggingBoard* loggingBoard) {

    for (Memory::const_iterator it = memory->begin(), unsigned short i = 0; it != memory->end(); it++, i++) {
        loggingBoard->newOutputProvider(OutProvider::ptr(new SocketOutProvider(
                static_cast<unsigned short>(i + STREAMING_PORT_BASE)), it->first);
    }

}
}
}
}
