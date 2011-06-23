/**
 * Common constants for connection manipulation
 *
 * @author Octavian Neamtu <oneamtu89@gmail.com>
 */

#ifndef _ConnectionConstants_h_
#define _ConnectionConstants_h_

#include <string>

namespace man {
namespace corpus {
namespace guardian {
namespace connection_constants {

static const char* LOOPBACK_IF_NAME = "lo";
static const char* WIRED_IF_NAME = "eth0";

static std::string CONNECTION_NAME_DEFAULT = "ROBOTICS";
static const int   WIFI_RECONNECTS_MAX_DEFAULT = 3;

}
}
}
}

#endif //_ConnectionConstants_h_
