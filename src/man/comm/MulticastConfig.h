/**
 * File used to hold static IPs for robots. Used in multicasting
 * @author Josh Zalinger and Wils Dawson 5/11/12
 */

#include <string>

static const int NUM_ROBOTS = 9;    // Total number of robots we have

typedef struct robot_ip_pair_t
{
    std::string name;
    std::string ip;
}robot_ip_pair;

static const robot_ip_pair wash   = {"wash"  , "139.140.218.9" };
static const robot_ip_pair river  = {"river" , "139.140.218.10"};
static const robot_ip_pair jayne  = {"jayne" , "139.140.218.11"};
static const robot_ip_pair scotty = {"scotty", "139.140.218.12"};
static const robot_ip_pair dax    = {"dax"   , "139.140.218.13"};
static const robot_ip_pair annika = {"annika", "139.140.218.14"};
static const robot_ip_pair data   = {"data"  , "139.140.218.15"};
static const robot_ip_pair mal    = {"mal"   , "139.140.218.16"};
static const robot_ip_pair zoe    = {"zoe"   , "139.140.218.17"};

static const robot_ip_pair robotIPs[NUM_ROBOTS] = {wash, river, jayne, scotty,
                                                    dax, annika, data, mal, zoe};
