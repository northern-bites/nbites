#ifndef DEBUG_H_DEFINED
#define DEBUG_H_DEFINED

#include "ifdefs.h"
#include "Common.h"

#if !defined(OFFLINE) && ROBOT(AIBO)

//#include <OPENR/OSyslog.h>

/*
 * If you define DEBUG in a module, use DEBUG_PRINT(whatever) to print out
 * values.  You may give DEBUG_PRINT as many arguments as you'd like.
 */
#ifdef DEBUG
//#include <OPENR/OObject.h> // for OSYSPRINT
///#define DEBUG_PRINT(...) (OSYSPRINT((__VA_ARGS__)) & OSYSPRINT(("\n")))
#else
//#define DEBUG_PRINT(...)
#endif
#endif //OFFLINE

#if ROBOT(NAO) || defined(OFFLINE)
#define print(...) (printf(__VA_ARGS__) & printf("\n"))
#else
//#define print(...) (OSYSPRINT((__VA_ARGS__)) & OSYSPRINT(("\n")))
#endif

#endif // DEBUG_H_DEFINED
