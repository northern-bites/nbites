/*
 *Constants for the most basic of things (and things that don't fit elsewhere)
 */

#ifndef Common_h_DEFINED
#define Common_h_DEFINED

#include <math.h> // for PI
#include "NBMath.h"
#include "DebugConfig.h"
#include <time.h>
#include <assert.h>
#include <stdint.h>

namespace angle {
    typedef float radians;
    typedef float degrees;
}

typedef int pixels;
typedef float cms;
typedef float mms;

static const int NUM_PLAYERS_PER_TEAM = 5;

// Location of pathnames of color tables on robot
#define TOP_TABLE_PATHNAME "/home/nao/nbites/lib/table/top_table.mtb"
#define BOTTOM_TABLE_PATHNAME "/home/nao/nbites/lib/table/bottom_table.mtb"

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

#define CHECK_SUCCESS(x) {\
        int result; \
        if( (result = (x)) < 0) { \
            printf("Problem with " #x ", returned %i\n", result); \
            perror("Error message"); \
        } \
}

#include <stdexcept>
#include <cstring>
#include <string>

static void throw_errno(int err_no) throw (std::runtime_error) {
    throw std::runtime_error(std::string(strerror(err_no)));
}

#include <time.h>
#include <sys/time.h>

static const long long MICROS_PER_SECOND = 1000000;

const static float MOTION_FRAME_LENGTH_S = 0.01f;
// 1 second * 1000 ms/s * 1000 us/ms
const float MOTION_FRAME_RATE = 1.0f / MOTION_FRAME_LENGTH_S;
const int GUARDIAN_FRAME_RATE = MOTION_FRAME_RATE;

/*
 * FRAME LENGTHS
 * A note about why these are what they are.
 * Motion = 100 fps = 10000 uS per frame
 *     This has to synchronize with the DCM.
 * Cognition = 30 fps = 34000 uS (ish) per frame
 *     This needs to run once per camera frame, which we get at 30 fps
 * Guardian = same as motion
 *     This needs to check often for falls and problems
 * Sensors = same as motion
 *     This needs to run fast so that it stays accurate when motion gets data
 * Comm = same as cognition
 *     Tried running this at the motion framerate, and it took too long
 *     almost every frame. Also, it makes sense to run it with cognition bc
 *     that's what it mainly interacts with.
 */
const long long MOTION_FRAME_LENGTH_uS = 1000 * 1000 / MOTION_FRAME_RATE;
const long long COGNITION_FRAME_LENGTH_uS = 34000;
const long long GUARDIAN_FRAME_LENGTH_uS = MOTION_FRAME_LENGTH_uS;
const long long SENSORS_FRAME_LENGTH_uS = MOTION_FRAME_LENGTH_uS;
const long long COMM_FRAME_LENGTH_uS = COGNITION_FRAME_LENGTH_uS;

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
