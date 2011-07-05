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

static const std::string CONNECTION_NAME_DEFAULT = "SPL-D";
static const std::string CONNECTION_PSWD_DEFAULT = "a1b0a1b0a1";

// the threshold is the number of checks that return no wifi
// before the wifi resets
// we check the connection every CONNECTION_CHECK_RATE frames
// of roboguardian
static const unsigned WIFI_SOFT_RESET_THRESHOLD = 1;
static const unsigned WIFI_HARD_RESET_THRESHOLD = 3;

}
}
}
}

#endif //_ConnectionConstants_h_
