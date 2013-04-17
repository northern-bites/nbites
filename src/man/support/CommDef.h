#pragma once

#include <string.h>
#include <string>

#include "Common.h"

#define TEAM_PORT 4000

#define UDP_BUF_SIZE 1024
#define TCP_BUF_SIZE 1048576 // 1MB for the Nao's

#ifndef llong
typedef long long llong;
#endif
#ifndef byte
typedef unsigned char byte;
#endif

#define ERROR_NO_INPUT "Input unavailable on socket."
#define ERROR_NO_OUTPUT "Output unavailable on socket."
#define ERROR_DATATYPE "Unexpected data type read from socket connection."
#define ERROR_DATASIZE "Unexpected amount of data read from socket connection."


//
// Multicast information
//

static const int NUM_ROBOTS = 9;    // Total number of robots we have

typedef struct robot_ip_pair_t
{
    std::string name;
    std::string ip;
}robot_ip_pair;

static const robot_ip_pair wash    = {"wash"   , "139.140.218.9" };
static const robot_ip_pair river   = {"river"  , "139.140.218.10"};
static const robot_ip_pair jayne   = {"jayne"  , "139.140.218.11"};
static const robot_ip_pair simon   = {"simon"  , "139.140.218.12"};
static const robot_ip_pair inara   = {"inara"  , "139.140.218.13"};
static const robot_ip_pair kaylee  = {"kaylee" , "139.140.218.14"};
static const robot_ip_pair vera    = {"vera"   , "139.140.218.15"};
static const robot_ip_pair mal     = {"mal"    , "139.140.218.16"};
static const robot_ip_pair zoe     = {"zoe"    , "139.140.218.17"};


static const robot_ip_pair robotIPs[NUM_ROBOTS] = {wash, river, jayne,
                                                   simon, inara, kaylee,
                                                   vera, mal, zoe};

//
// Comm constants and MACRO definitions
//

#define UNIQUE_ID "B" // keep this as define so it stays 2 bytes, not 4.
static const int NUM_HEADER_BYTES = 16;

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
