/**
 * @class IOProviderFactory
 *
 * Provides some static methods that create different types of IOPoviders
 * useful in logging
 *
 * @author Octavian Neamtu <oneamtu89@gmail.com>
 */
#pragma once

#include "LoggingBoard.h"
#include "NaoPaths.h"

namespace man {
namespace memory {
namespace log {

class OutputProviderFactory {

public:
    static void AllFileOutput(const Memory* memory, LoggingBoard* loggingBoard,
            std::string log_folder_path = common::paths::NAO_LOG_DIR);
    static void AllSocketOutput(const Memory* memory, LoggingBoard* loggingBoard);

};

}
}
}
