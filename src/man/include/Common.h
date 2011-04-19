/*
 *Constants for the most basic of things (and things that don't fit elsewhere)
 */

#ifndef Common_h_DEFINED
#define Common_h_DEFINED

#include <math.h> // for PI
#include "NBMath.h"
#include "manconfig.h"
#include "visionconfig.h"
#include <time.h>

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
#include <sys/time.h>

static const long long MICROS_PER_SECOND = 1000000;

const static float MOTION_FRAME_LENGTH_S = 0.01f;
// 1 second * 1000 ms/s * 1000 us/ms
const float MOTION_FRAME_LENGTH_uS = 1000.0f * 1000.0f * MOTION_FRAME_LENGTH_S;
const float MOTION_FRAME_RATE = 1.0f / MOTION_FRAME_LENGTH_S;


static long long micro_time (void)
{
#ifdef OFFLINE

    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * MICROS_PER_SECOND + tv.tv_usec;

#else

    // Needed for microseconds which we convert to milliseconds
    struct timespec tv;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tv);
    return tv.tv_sec * MICROS_PER_SECOND + tv.tv_nsec / 1000;

#endif
}

#endif // Common_h_DEFINED

