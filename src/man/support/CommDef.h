#pragma once

#include <string.h>
#include <string>

#include "Common.h"

#define IP_TARGET "255.255.255.255"
#define TEAM_PORT 11400

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

static const int NUM_ROBOTS = 13;    // Total number of robots we have

typedef struct robot_ip_pair_t
{
    std::string name;
    std::string ip;
}robot_ip_pair;

// Bowdoin IPs.
static const robot_ip_pair wash    = {"wash"   , "10.0.14.9" };
static const robot_ip_pair river   = {"river"  , "10.0.14.10"};
static const robot_ip_pair jayne   = {"jayne"  , "10.0.14.11"};
static const robot_ip_pair simon   = {"simon"  , "10.0.14.12"};
static const robot_ip_pair inara   = {"inara"  , "10.0.14.13"};
static const robot_ip_pair kaylee  = {"kaylee" , "10.0.14.14"};
static const robot_ip_pair vera    = {"vera"   , "10.0.14.15"};
static const robot_ip_pair mal     = {"mal"    , "10.0.14.16"};
static const robot_ip_pair zoe     = {"zoe"    , "10.0.14.17"};

static const robot_ip_pair ringo   = {"ringo"  , "10.0.14.18"};
static const robot_ip_pair beyonce = {"beyonce", "10.0.14.19"};
static const robot_ip_pair ozzy    = {"ozzy"   , "10.0.14.20"};
static const robot_ip_pair avril   = {"avril"  , "10.0.14.21"};
static const robot_ip_pair batman  = {"batman" , "10.0.14.22"};
static const robot_ip_pair she-hulk= {"she-hulk" , "10.0.14.23"};

static const robot_ip_pair robotIPs[NUM_ROBOTS] = {wash, river, jayne,
                                                   simon, inara, kaylee,
                                                   vera, mal, zoe, ringo, beyonce,
                                                   ozzy, avril, batman, she-hulk};

//
// Comm constants and MACRO definitions
//

#define UNIQUE_ID "B" // keep this as define so it stays 2 bytes, not 4.
static const int NUM_HEADER_BYTES = 16;

static const long PACKETS_PER_SECOND = 5;
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
static const long long TEAMMATE_DEAD_THRESHOLD = 10 * MICROS_PER_SECOND;
static const int       RESET_SEQ_NUM_THRESHOLD = 30 * PACKETS_PER_SECOND;

static const unsigned int MAX_MESSAGE_MEMORY = 20;
