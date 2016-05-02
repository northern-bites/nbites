//
//  Filesystem.cpp
//  tool8-separate
//
//  Created by Philip Koch on 4/7/16.
//  Copyright © 2016 pkoch. All rights reserved.
//

#include "Filesystem.hpp"

#include <string>

#include "utilities.hpp"
#include "Control.hpp"
#include "Log.hpp"

#include <unistd.h>

#define NBL_LOGGING_LEVEL NBL_WARN_LEVEL

namespace nblog {
    namespace filesystem {

        const size_t MAX_WRITTEN_BYTES = (1<<28);
        size_t total_written = 0;

        std::string makeFileName(logptr log);
        bool checkMayWriteString(const std::string& str);

        void Filelogger::threadLoop() {

            NBL_WARN("FileLogger::threadLoop() starting...");

            for (;running;) {

                if (!control::check(control::flags::logToFilesystem)) {
                    sleep(FILE_THREAD_SLEEP_SECS);
                    continue;
                }

                logptr log = provider.blockForLog({Q_FILESYSTEM}, std::numeric_limits<io::iotime_t>::max());

                if (log) {
                    std::string buffer;
                    log->serialize(buffer);

                    std::string file = makeFileName(log);

                    if (checkMayWriteString(buffer)) {
                        io::writeStringToFile(buffer, file);
                    } else {
                        NBL_WARN("FileLogger::threadLoop() cannot write log (%zu bytes) because it has already written %zu bytes (max is %zu)",
                                 buffer.size(),
                                 total_written,
                                 MAX_WRITTEN_BYTES);
                    }
                }
            }
        }

        const std::string suffix(".nblog");
        std::string makeFileName(logptr log) {
            std::string type;
            if (log->blocks.empty()) {
                type = "notype";
            } else {
                type = log->blocks[0].type;
            }

            return utilities::format("%s/%s_%lu_%li%s",
                                     CONSTANTS.ROBOT_LOG_PATH_PREFIX().c_str(),
                                     type.c_str(),
                                     log->createdWhen,
                                     clock(),
                                     suffix.c_str());
        }

        bool checkMayWriteString(const std::string& str) {
            if (total_written + str.size() > MAX_WRITTEN_BYTES) {
                return false;
            } else {
                total_written += str.size();
                return true;
            }
        }

    }
}