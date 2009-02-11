/* 
   Constants for the most basic of things (and things that don't fit elsewhere)
*/

#ifndef Common_h_DEFINED
#define Common_h_DEFINED

#include <math.h> // for PI

#include "ifdefs.h"

// ROBOT TYPES
#define AIBO_ERS7 0
#define AIBO_220  1
#define AIBO      2
#define NAO_RL    3
#define NAO_SIM   4
#define NAO       5
#ifndef ROBOT_TYPE
#  define ROBOT_TYPE AIBO_ERS7
#endif
#define ROBOT(t) ( \
    (t == AIBO_ERS7 && ROBOT_TYPE == AIBO_ERS7) || \
    (t == AIBO_220  && ROBOT_TYPE == AIBO_220) || \
    (t == NAO_RL    && ROBOT_TYPE == NAO_RL) || \
    (t == NAO_SIM   && ROBOT_TYPE == NAO_SIM) || \
    (t == AIBO      && \
      (ROBOT_TYPE == AIBO || ROBOT_TYPE == AIBO_ERS7 || \
       ROBOT_TYPE == AIBO_220) ) || \
    (t == NAO       && \
      (ROBOT_TYPE == NAO_RL || ROBOT_TYPE == NAO_SIM) ) \
    )

// DOGS
#define NEO      1
#define TRINITY  2
#define MORPHEUS 3
#define DOZER    4
#define RAPH     5
#define MIKE     6
#define LEO      7
#define DON      8
#define FRODO    9
#define SAM      10
#define MERRY    11
#define PIPPIN   12
#define R2D2     13
#define HAN      14
#define CHEWIE   15
#define C3PO     16

// CAMERA SETTINGS
// WHITE BALANCES
#define INDOOR_MODE 1
#define OUTDOOR_MODE 2
#define FL_MODE 3
// GAINS
#define LOW_GAIN 4
#define MID_GAIN 5
#define HIGH_GAIN 6
// SHUTTER SPEEDS
#define SLOW_SHUTTER 7
#define MID_SHUTTER 8
#define FAST_SHUTTER 9

// SERIES
#define ERS7 0
#define ERS220 1

#ifndef NUM_PLAYERS_PER_TEAM
# define NUM_PLAYERS_PER_TEAM 5
#endif

#ifdef OFFLINE
# define FAKE_PLAYER_NO 3 //for cortex debugging
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

