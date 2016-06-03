//
//  utilities.cpp
//  tool8-separate
//
//  Created by Philip Koch on 4/17/16.
//  Copyright © 2016 pkoch. All rights reserved.
//

#include <stdio.h>
#include "utilities.hpp"

namespace nblog {
    namespace utilities {

#define NBL_LEVEL_M(NAME) NBL_STRINGIFY(NAME) ,

        const char * LevelStrings[] = {
            NBL_LEVEL_SET
        };

#undef NBL_LEVEL_M

        bool mayUseColor() {
            const char * term = getenv("TERM");

            if (!term) {
                return false;
            } else {
                std::string str(term);

                return  str.find("xterm") != std::string::npos ||
                str.find("color") != std::string::npos;
            }
        }

        bool useColor = mayUseColor();

        const char * ColorStrings[] = {
            "\x1b[31m", "\x1b[32m", "\x1b[33m",
            "\x1b[34m", "\x1b[35m", "\x1b[36m",
            "\x1b[0m"
        };

        const char * LevelColors[] = {
            ColorStrings[NBL_GREEN],
            ColorStrings[NBL_YELLOW],
            ColorStrings[NBL_RED],
            ColorStrings[NBL_CYAN]
        };

        void nbl_print( LogLevel curLev, LogLevel msgLevel, const char * file, int line, const char * format,
                       ...) {
            va_list vaargs;
            va_start(vaargs, format);

            if (msgLevel >= curLev) {
                const char * prefixColor = (useColor) ?
                LevelColors[msgLevel] : "";
                const char * postfixColor = (useColor) ?
                    ColorStrings[NBL_RESET] : "";

                std::string stlFile(file);
                size_t slash = stlFile.find_last_of('/') + 1;

                //just get first char of LevelString
                printf("%s[%c][%s@%d]: ",
                       prefixColor, *(LevelStrings[msgLevel]), file + slash, line);
                vprintf(format, vaargs);
                //use std::cout for std::endl flushing
                std::cout << postfixColor << std::endl;
            }
            
            va_end(vaargs);
        }
        
        std::string format(const char * format, ...) {
            va_list args;
            va_start(args, format);

            //+1 for \0
            ssize_t len = (ssize_t) vsnprintf(NULL, 0, format, args) + 1;
            if (len < 0) throw std::runtime_error("could not format string!");

            va_end(args);
            va_start(args, format);

            char buffer[ len ];
            vsnprintf(buffer, len, format, args);
            va_end(args);

            return std::string(buffer);
        }

        std::string get_error(const int errsaved) {
            static const size_t max_msg_size = 256;

#ifdef __APPLE__
            char buf[max_msg_size];
            buf[0] = '\0';
            strerror_r(errsaved, buf, max_msg_size);
#else
            char optbuf[max_msg_size];
            optbuf[0] = '\0';
            const char * buf = strerror_r(errsaved, optbuf, max_msg_size);
#endif

            NBL_CHECK(buf);
            NBL_CHECK( strlen(buf) );

            return std::string(buf);
        }

        void safe_perror(const int err) {
            printf("ERROR: %s\n", get_error(err).c_str());
        }
    }
}