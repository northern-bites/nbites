/*
 *Constants for the most basic of things (and things that don't fit elsewhere)
 */

#ifndef Common_h_DEFINED
#define Common_h_DEFINED

#include <math.h> // for PI
#include "ifdefs.h"
#include "NBMath.h"

// ROBOT TYPES
#define NAO_RL    3
#define NAO_SIM   4
#define NAO       5
#ifndef ROBOT_TYPE
#  define ROBOT_TYPE NAO_RL
#endif
#define ROBOT(t) ( \
    (t == NAO_RL    && ROBOT_TYPE == NAO_RL) || \
    (t == NAO_SIM   && ROBOT_TYPE == NAO_SIM) || \
    (t == NAO       && \
      (ROBOT_TYPE == NAO_RL || ROBOT_TYPE == NAO_SIM) ) \
    )

#ifndef NUM_PLAYERS_PER_TEAM
# define NUM_PLAYERS_PER_TEAM 4
#endif

// game controller constants for structure information
#define GAME_CONTROLLER_LIST_SIZE 11
#define GAME_CONTROLLER_TEAMS_LIST_SIZE 2
#define GAME_CONTROLLER_TEAM_LIST_SIZE 4
#define GAME_CONTROLLER_PLAYERS_LIST_SIZE NUM_PLAYERS_PER_TEAM


//
// Define oft-used short-hand or clarifying user-defined types

#ifndef true
#  define true  1
#  define false 0
#endif

#ifndef byte
typedef unsigned char byte;
#endif

#include <time.h>
#ifndef _WIN32
#include <sys/time.h>
#else
#include <sys/timeb.h>
#endif
static const long long MICROS_PER_SECOND = 1000000;

static long long micro_time (void)
{
#ifndef _WIN32
    // Needed for microseconds which we convert to milliseconds
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return tv.tv_sec * MICROS_PER_SECOND + tv.tv_usec;
#else
    _timeb timebuffer;
    time_t secondsSince1970;
    unsigned short millis;

    _ftime64_s( &timebuffer);
    secondsSince1970 = timebuffer.time;
    millis = timebuffer.millitm;
    return (secondsSince1970 * 1000ul + millis) * 1000ul;
#endif
}

#endif // Common_h_DEFINED

