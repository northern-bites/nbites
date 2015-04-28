#ifndef nbdebug_h
#define nbdebug_h

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


// Debug sections
#define SECTION_ALL     (~0     )
#define SECTION_SEXPR   (0x01   )
#define SECTION_CONTROL (0x02   )
#define SECTION_FILEIO  (0x04   )
#define SECTION_STREAM  (0x08   )
#define SECTION_LOGM    (0x010  )
#define SECTION_SOUND   (0x020  )

// if NB_DEBUG isn't defined to anything, no debug output results.

//Print everything
//#define NB_DEBUG  SECTION_ALL
//Print only what isn't tied to a section
//#define NB_DEBUG  0

#define NB_DEBUG    SECTION_SEXPR

#ifndef __APPLE__
#define NBDB_USECOLOR
#endif

//colored output macros.
#ifdef NBDB_USECOLOR
#define NBDB_color "\x1B[36m"
#define NBDB_cnrm  "\x1B[0m"
#define NBDB_asf   "\x1B[31m" //assertion failed
#else
#define NBDB_color
#define NBDB_cnrm
#define NBDB_asf
#endif

#ifdef NB_DEBUG
//Logging code has no guarantee assertions will be on... want some form of yelp if something goes wrong
#define NBLassert(val) {if(!(val)) {printf(NBDB_asf \
"* * * * * * * * * * * * *\nNBLassert FAILED: [%s][%i] (%s)\n" NBDB_cnrm, \
__FILE__, __LINE__, #val); assert((val));}}

static inline void NBDB_printf(bool always, unsigned int section, const char * format, ...) {
    
    va_list arguments;
    va_start(arguments, format);
    
    if (always || (NB_DEBUG & section)) {
        printf(NBDB_color "[debug]: ");
        vprintf(format, arguments);
        printf(NBDB_cnrm "");
    }
    
    va_end(arguments);
}

#define NBDEBUG(...) NBDB_printf(true, 0, __VA_ARGS__)
#define NBDEBUGs(s, ...) NBDB_printf(false, s, __VA_ARGS__)

#else
//Define to blankspace.

#define NBLassert(val)
#define NBDEBUG(...)
#define NBDEBUGs(s, ...)

#endif //NB_DEBUG


#endif //nbdebug_h