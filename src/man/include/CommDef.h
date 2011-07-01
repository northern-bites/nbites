
#ifndef CommDef_H
#define CommDef_H

#include <string.h>

#include "Common.h"

//
// DataSerializer constants and MACRO definitions
//

// Comment out to turn off listening (will not bind to the local UDP port)
//   Usefull if you're running in the simulator with multiple robots, or want
//   to run the EKF localization debugger
#if !defined(COMM_LISTEN) && !defined(COMM_DEAF)
#  define COMM_LISTEN 1
#endif
#if !defined(COMM_SEND) && !defined(COMM_MUTE)
#  define COMM_SEND 1
#endif

#define UDP_PORT  4000
#define TCP_PORT  4001
#define TOOL_PORT 4002

#define UDP_BUF_SIZE 1024

#if ROBOT(NAO)
#  define TCP_BUF_SIZE 1048576 // 1MB for the Nao's
#else
#  define TCP_BUF_SIZE 204800  // 200KB for the Aibo's, and simulator
#endif

typedef long long llong;
#ifndef byte
typedef unsigned char byte;
#endif


#undef SIZEOF_INT
#define SIZEOF_INT        4
#undef SIZEOF_BYTE
#define SIZEOF_BYTE       1
#undef SIZEOF_FLOAT
#define SIZEOF_FLOAT      SIZEOF_INT
#undef SIZEOF_LLONG
#define SIZEOF_LLONG      8
#undef SIZEOF_DOUBLE
#define SIZEOF_DOUBLE     SIZEOF_LLONG

#define TYPE_INT          0
#define TYPE_BYTE         1
#define TYPE_FLOAT        2
#define TYPE_LONG         3
#define TYPE_DOUBLE       4
#define TYPE_INT_ARRAY    5
#define TYPE_BYTE_ARRAY   6
#define TYPE_FLOAT_ARRAY  7
#define TYPE_LONG_ARRAY   8
#define TYPE_DOUBLE_ARRAY 9

#define ERROR_NO_INPUT "Input unavailable on socket."
#define ERROR_NO_OUTPUT "Output unavailable on socket."
#define ERROR_DATATYPE "Unexpected data type read from socket connection."
#define ERROR_DATASIZE "Unexpected amount of data read from socket connection."


//
// TOOLConnect constants and MACRO definitions
//

#define TOOL_COMMANDING 0
#define TOOL_REQUESTING 1

#define COMMAND_MSG 0
#define REQUEST_MSG 1
#define DISCONNECT  2

#define CMD_TABLE      0
#define CMD_MOTION     1
#define CMD_HEAD       2
#define CMD_JOINTS     3


static const char *TOOL_REQUEST_MSG = "TOOL:request";
static const int TOOL_REQUEST_LEN = strlen(TOOL_REQUEST_MSG);
static const char *TOOL_ACCEPT_MSG = "TOOL:accept";
static const int TOOL_ACCEPT_LEN = strlen(TOOL_ACCEPT_MSG);


//
// Comm constants and MACRO definitions
//

#define PACKET_HEADER "ilikeyoulots"

static const long PACKETS_PER_SECOND = 6;
static const long SLEEP_MILLIS = 5000;
//static const long long MICROS_PER_SECOND = 1000000; // defined in Common.h
static const long long MICROS_PER_PACKET = MICROS_PER_SECOND /
                                              PACKETS_PER_SECOND;
static const long long PACKET_GRACE_PERIOD = 3 * MICROS_PER_SECOND;
static const long long PACKET_TIME_DISRUPTED = 5 * MICROS_PER_SECOND;
static const long long DEBUG_REC_E_DISRUPTED = 5 * MICROS_PER_SECOND;
static const long long GAME_INITIAL_TIMESTAMP = -1;
static const long long PENALIZED_TIMESTAMP = -2;
static const long long SOS_TIMESTAMP = -666;
static const long long USE_TEAMMATE_BALL_REPORT_FRAMES_OFF = 2;
// The minimum delay between sending and receiving a packet (packet "travel time").
static const long long MIN_PACKET_DELAY = 0;
// The minimum and maximum number of packets we can send per second.
static const long long MAX_PACKETS_PER_SECOND = 10;  // 10 packets per second.
static const long long MIN_PACKETS_PER_SECOND = 4;   // 4 packets pers second.
static const long long TEAMMATE_DEAD_THRESHOLD = 3 * MICROS_PER_SECOND;

static const unsigned int MAX_MESSAGE_MEMORY = 20;

typedef struct CommPacketHeader_t
{
    char header[sizeof(PACKET_HEADER)];
    llong timestamp;
    int number;
    int team;
    int player;
    int color;
} CommPacketHeader;

typedef struct CommTeammatePacketInfo_t 
{
    CommTeammatePacketInfo_t()
    : timestamp(0), lastNumber(0)
	{ }

    llong timestamp;       // Timestamp of last received packet.
    int lastNumber;        // (Unique) number of last packet received.
} CommTeammatePacketInfo;

#endif /* CommDef.h */
