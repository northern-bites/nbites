/*
 *Constants for the most basic of things (and things that don't fit elsewhere)
 */

#ifndef Common_h_DEFINED
#define Common_h_DEFINED

#include <math.h> // for PI
#include "NBMath.h"
//TODO: (Octavian) THIS IS A REALLY SHITTY WAY TO ENSURE THAT THE CONFIG DEFINES
// ARE DEFINED EVERYWHERE. BEWARE!
#ifndef NO_MANCONFIG
#include "manconfig.h"
#include "nameconfig.h"
#include "visionconfig.h"
#endif //NO_MANCONFIG
#include <time.h>
#include <assert.h>
#include <stdint.h>

// ROBOT TYPES
#define NAO_RL    3
#define NAO_SIM   4
#define NAO       5
#define NAO_RL_33 6  // longer arms, new heads

namespace angle {
    typedef float radians;
    typedef float degrees;
}

// we set ROBOT_TYPE here for now, not in cmake anymore
// Nathan 4/18/11
// @TODO Make this less stupid
#undef ROBOT_TYPE
#ifdef ROBOT_NAME_slarti
  #define ROBOT_TYPE NAO_RL
#else
#ifdef ROBOT_NAME_trillian
  #define ROBOT_TYPE NAO_RL
#else
#ifdef ROBOT_NAME_marvin
  #define ROBOT_TYPE NAO_RL
#else
#ifdef ROBOT_NAME_zaphod
  #define ROBOT_TYPE NAO_RL
#else
  #define ROBOT_TYPE NAO_RL_33
#endif
#endif
#endif
#endif

#define ROBOT(t) ( \
    (t == NAO_RL_33 && ROBOT_TYPE == NAO_RL_33) || \
    (t == NAO_SIM   && ROBOT_TYPE == NAO_SIM) || \
    (t == NAO_RL && (ROBOT_TYPE == NAO_RL_33 || ROBOT_TYPE == NAO_RL) ) || \
    (t == NAO  && \
      (ROBOT_TYPE == NAO_RL || ROBOT_TYPE == NAO_RL_33 || ROBOT_TYPE == NAO_SIM) ))

static const int NUM_PLAYERS_PER_TEAM = 4;

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

#ifndef uint
typedef unsigned int uint;
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

/**
 * Copies a struct using memcpy
 *
 * @returns the number of bytes copied
 */
template <class S, class D>
int copy_struct(const S* source, D* destination) {
    memcpy(source, destination, sizeof(S));
    return sizeof(S);
}

//TODO: if we ever want to time stuff offline proper, then fix the ifdefs
static long long thread_micro_time (void)
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

static long long process_micro_time(void)
{
#ifdef OFFLINE

    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * MICROS_PER_SECOND + tv.tv_usec;

#else

    // Needed for microseconds which we convert to milliseconds
    struct timespec tv;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tv);

    return tv.tv_sec * MICROS_PER_SECOND + tv.tv_nsec / 1000;
#endif
}

static long long monotonic_micro_time(void)
{
#ifdef OFFLINE

    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * MICROS_PER_SECOND + tv.tv_usec;

#else
    // Needed for microseconds which we convert to milliseconds
    struct timespec tv;
    clock_gettime(CLOCK_MONOTONIC, &tv);

    return tv.tv_sec * MICROS_PER_SECOND + tv.tv_nsec / 1000;
#endif
}

static long long realtime_micro_time(void) {
#ifdef OFFLINE

    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * MICROS_PER_SECOND + tv.tv_usec;

#else
    // Needed for microseconds which we convert to milliseconds
    struct timespec tv;
    clock_gettime(CLOCK_REALTIME, &tv);

    return tv.tv_sec * MICROS_PER_SECOND + tv.tv_nsec / 1000;
#endif
}

#endif // Common_h_DEFINED
