#pragma once

#include <string.h>
#include <string>

#include "Common.h"

// Bowdoin port
#define TEAM_PORT 4500
//#define TEAM_PORT 11450

#ifndef llong
typedef long long llong;
#endif
#ifndef byte
typedef unsigned char byte;
#endif

//
// Multicast information
//

static const int NUM_ROBOTS = 9;    // Total number of robots we have

typedef struct robot_ip_pair_t
{
    std::string name;
    std::string ip;
}robot_ip_pair;

// Bowdoin IPs.
static const robot_ip_pair wash    = {"wash"   , "139.140.218.9" };
static const robot_ip_pair river   = {"river"  , "139.140.218.10"};
static const robot_ip_pair jayne   = {"jayne"  , "139.140.218.11"};
static const robot_ip_pair simon   = {"simon"  , "139.140.218.12"};
static const robot_ip_pair inara   = {"inara"  , "139.140.218.13"};
static const robot_ip_pair kaylee  = {"kaylee" , "139.140.218.168"};
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
static const long PACKETS_PER_SECOND = 6;
// The minimum delay between sending and receiving a packet (packet "travel time").
static const long long MIN_PACKET_DELAY = 0;

static const long long TEAMMATE_DEAD_THRESHOLD = 4 * MICROS_PER_SECOND;
// Number of sequence numbers before we consider old ones still valid.
// should be at least    MAX PING_TIME(s) * packets per second (~15)
static const int       RESET_SEQ_NUM_THRESHOLD = 180;
