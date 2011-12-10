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

namespace man {
namespace memory {
namespace log {

class OutputProviderFactory {

public:
    static void AllFileOutput(LoggingBoard::ptr loggingBoard);
    static void AllSocketOutput(LoggingBoard::ptr loggingBoard);

};

}
}
}
