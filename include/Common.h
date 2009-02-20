/*
 *Constants for the most basic of things (and things that don't fit elsewhere)
 */

#ifndef Common_h_DEFINED
#define Common_h_DEFINED

#include <math.h> // for PI

#include "ifdefs.h"

// ROBOT TYPES
#define NAO_RL    1
#define NAO_SIM   2
#define NAO       3
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

static const double PI = M_PI;
static const double DEG_OVER_RAD = 180.0 / M_PI;
static const double RAD_OVER_DEG = M_PI / 180.0;

//
// Define oft-used short-hand or clarifying user-defined types

#ifndef true
#  define true  1
#  define false 0
#endif

#ifndef byte
typedef unsigned char byte;
#endif

inline static int ROUND(float x) {
  if ((x-static_cast<int>(x)) >= 0.5) return (static_cast<int>(x)+1);
  if ((x-static_cast<int>(x)) <= -0.5) return (static_cast<int>(x)-1);
  else return (int)x;
}

#include <time.h>
#include <sys/time.h>

static const long long MICROS_PER_SECOND = 1000000;
static long long
micro_time (void)
{
    // Needed for microseconds which we convert to milliseconds
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return tv.tv_sec * MICROS_PER_SECOND + tv.tv_usec;
}

#endif // Common_h_DEFINED

