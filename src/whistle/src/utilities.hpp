
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

#define NBL_MAKELEVEL(LEVEL) ( nblog::utilities::LOGLEVEL_ ## LEVEL )
#define NBL_MAKELEVEL_EXPAND(LEVEL) NBL_MAKELEVEL(LEVEL)

#ifndef NBL_LOGLEVEL
#define NBL_LOGLEVEL INFO
#endif

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

/* constants and functions needing namespace protection */
namespace nblog {
    namespace utilities {

#define NBL_LEVEL_SET    \
    NBL_LEVEL_M(INFO)   \
    NBL_LEVEL_M(WARN)   \
    NBL_LEVEL_M(ERROR)  \
    NBL_LEVEL_M(ALWAYS) \

        //create enumeration
#undef NBL_LEVEL_M
#define NBL_LEVEL_M(NAME) LOGLEVEL_ ## NAME ,
        enum LogLevel {
            NBL_LEVEL_SET
        };
#undef NBL_LEVEL_M

        //Stringified levels
        extern const char * LevelStrings[];

        enum {
            NBL_RED, NBL_GREEN, NBL_YELLOW, NBL_BLUE,
            NBL_MAGENTA, NBL_CYAN, NBL_RESET
        };

        extern const char * ColorStrings[];

        extern bool mayUseColor();

        extern void nbl_print( LogLevel curLev, LogLevel msgLevel, const char * file, int line, const char * format, ...);

        template<typename T>
        static inline const std::string describe(const char * name, T var) {
            std::ostringstream buffer;
            buffer << "var '" << name << "' == " << var;
            return buffer.str();
        }

        /* formats args according to format, returns result as std::string */
        extern std::string format(const char * format, ...);
    }
}

/* #defines */

#define NBL_PRINT(format, ...) NBL_PRINT_(ALWAYS, format, ## __VA_ARGS__)

#define NBL_PRINT_(LEVEL, format, ...)              \
    do { nbl_print(                                 \
    NBL_MAKELEVEL_EXPAND(NBL_LOGLEVEL),            \
    NBL_MAKELEVEL_EXPAND(LEVEL),                    \
    __FILE__,                                       \
    __LINE__,                                       \
    format, ## __VA_ARGS__ );                       \
    } while(0);

//Return (const char *) s.t. NBL_CSTR_DESCRIBE(false) => "var 'false' == 0"
//#define NBL_CSTR_DESCRIBE(VAR) (nblog::utilities::combine("var '", #VAR, "' == ", VAR).c_str())
#define NBL_CSTR_DESCRIBE(VAR) ( nblog::utilities::describe(#VAR, VAR).c_str() )

#define NBL_STRINGIFY(SYM) ( #SYM )
#define NBL_STRINGIFY_2(SYM) NBL_STRINGIFY(SYM)

#ifndef NBL_NO_DEBUG

//Formatted and colored output conditioned on (LEVEL >= current_level)
#define NBL_LOG(LEVEL, format, ...) NBL_PRINT_(LEVEL, format, ## __VA_ARGS__)

//Formatted and colored ouput conditioned on (LEVEL >= current_level) && CONDITION
#define NBL_LOG_IF(LEVEL, CONDITION, format, ...)   \
    if( CONDITION ) { NBL_LOG(LEVEL, format, ## __VA_ARGS__) }

//Print out state of VAR (always)
#define NBL_WHATIS(VAR) NBL_LOG(ALWAYS, "%s", NBL_CSTR_DESCRIBE(VAR) )

//Warn if EXPRESSION evaluates to false
#define NBL_CHECK(EXPRESSION) NBL_LOG_IF(WARN, !(EXPRESSION), "CHECK of '%s' FAILED (0)!", #EXPRESSION )

//Warn if (A OP B) evaluates to false, print values
#define NBL_CHECK_TWO(A, B, OP) NBL_LOG_IF(WARN, !((A) OP (B)), "CHECK of '%s %s %s' FAILED!\n\t%s\n\t%s",  \
    #A, #OP, #B, NBL_CSTR_DESCRIBE(A), NBL_CSTR_DESCRIBE(B) )

//Assert EXPRESSION
#define NBL_ASSERT(EXPRESSION) NBL_LOG_IF(ERROR, !(EXPRESSION), "ASSERT of '%s' FAILED (0)!\n", #EXPRESSION ) assert(EXPRESSION);

//Assert (A OP B), print values
#define NBL_ASSERT_TWO(A, B, OP) NBL_LOG_IF(ERROR, !((A) OP (B)), "ASSERT of '%s %s %s' FAILED!\n\t%s\n\t%s\n",  \
    #A, #OP, #B, NBL_CSTR_DESCRIBE(A), NBL_CSTR_DESCRIBE(B) ) assert(((A) OP (B)));

#else   //NBL_NO_DEBUG
//Define as blank space.
#define NBL_LOG(LEVEL, format, ...)
#define NBL_LOG_IF(LEVEL, CONDITION, format, ...)
#define NBL_WHATIS(VAR)
#define NBL_CHECK(EXPRESSION)
#define NBL_CHECK_TWO(A, B, OP)
#define NBL_ASSERT(EXPRESSION)
#define NBL_ASSERT_TWO(A, B, OP)

#endif  //NBL_NO_DEBUG

//Common values for OP
#define NBL_CHECK_EQ(A, B) NBL_CHECK_TWO(A, B, ==)
#define NBL_CHECK_NE(A, B) NBL_CHECK_TWO(A, B, !=)
#define NBL_CHECK_LT(A, B) NBL_CHECK_TWO(A, B, <)
#define NBL_CHECK_LE(A, B) NBL_CHECK_TWO(A, B, <=)
#define NBL_CHECK_GT(A, B) NBL_CHECK_TWO(A, B, >)
#define NBL_CHECK_GE(A, B) NBL_CHECK_TWO(A, B, >=)

//Common values for OP
#define NBL_ASSERT_EQ(A, B) NBL_ASSERT_TWO(A, B, ==)
#define NBL_ASSERT_NE(A, B) NBL_ASSERT_TWO(A, B, !=)
#define NBL_ASSERT_LT(A, B) NBL_ASSERT_TWO(A, B, <)
#define NBL_ASSERT_LE(A, B) NBL_ASSERT_TWO(A, B, <=)
#define NBL_ASSERT_GT(A, B) NBL_ASSERT_TWO(A, B, >)
#define NBL_ASSERT_GE(A, B) NBL_ASSERT_TWO(A, B, >=)

#define NBL_INFO(format, ...) NBL_LOG(INFO, format, ## __VA_ARGS__)
#define NBL_WARN(format, ...) NBL_LOG(WARN, format, ## __VA_ARGS__)
#define NBL_ERROR(format, ...) NBL_LOG(ERROR, format, ## __VA_ARGS__)

namespace nblog {
    namespace utilities {
        extern std::string get_error(const int errsaved);
        extern void safe_perror(const int err);
    }
}

#define IF_NZ_PERROR_AND(expr)  \
    if (expr) { nblog::utilities::safe_perror(errno); \

#define NBL_DISALLOW_COPY(cname)                 \
    public:                                      \
        cname(cname const&)          = delete;   \
        void operator=(cname const&) = delete;   \

#endif  //nbl_utilities_h
