
/*
 utilities to ease debugging, printing, etc.
 created by Philip Koch 29-3-2016
 */

/* define NBL_NO_DEBUG to prevent this header from generating almost any code in used macros */

/* define NBL_LOGLEVEL to set level for the compilation unit this header is included into.
 INFO WARN ERROR

 ****** MAY BE SET / OVERRIDDEN AT LAST SECOND EVEN IF HEADER INCLUDED THIS FILE ;) ******
 */

#ifndef nbl_utilities_h
#define nbl_utilities_h

#ifndef __cplusplus
#error "These utilities require c++!"
#endif

#ifndef NBL_LOGGING_LEVEL
#define NBL_LOGGING_LEVEL NBL_INFO_LEVEL
#endif

#if defined(NBL_UTILITIES_VERSION) && NBL_UTILITIES_VERSION != 2
#error "wrong NBL_UTILITIES_VERSION"
#endif

#define NBL_UTILITIES_VERSION 2

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

#include "utilities-pp.hpp"

/* constants and functions needing namespace protection */
namespace nbl {
    namespace utilities {

#define NBL_LEVEL_SET InfoLevel, WarnLevel, ErrorLevel, AlwaysLevel
        
        NBL_MAKE_ENUM(LogLevel, NBL_LEVEL_SET)

        NBL_DECLARE_STRINGS(LevelStrings)

        typedef const char * color_string;
        extern color_string color_red, color_green, color_yellow, color_blue, color_magenta, color_cyan, color_bold_yellow, color_bold_red, color_color_reset;

        extern bool mayUseColor();
        extern const char * get_path_suffixes(const char * filePath, int elements);

        extern void nbl_print( LogLevel curLev, LogLevel msgLevel, const char * file, int line, const char * format, ...);

        extern void print_divider( int pre, int post, color_string color );
        extern const char * indent(int spaces);

        template<typename T>
        static inline const std::string describe(const char * name, T var) {
            std::ostringstream buffer;
            buffer << "var '" << name << "' == " << var;
            return buffer.str();
        }

        /* formats args according to format, returns result as std::string */
        extern std::string format(const char * format, ...);

        extern std::string get_error(const int errsaved);
        extern void safe_perror(const int err);

        extern bool safe_perror_describe(const int err, const char * file, int line, const char * description);

    }
}

#endif  //nbl_utilities_h
