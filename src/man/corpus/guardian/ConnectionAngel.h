#ifndef _WIFI_ANGEL_
#define _WIFI_ANGEL_

/**
 * @class WifiAngel
 *
 * Guards against mishaps with wifi.
 *
 * @author Octavian Neamtu <oneamtu89@gmail.com>
 *
 */

#include <string>
#include <stdint.h>

namespace man {
namespace corpus {
namespace guardian {

class ConnectionAngel {


public:
    ConnectionAngel(const char* connection_name = CONNECTION_NAME_DEFAULT,
            int wifi_reconnects_max = WIFI_RECONNECTS_MAX_DEFAULT,
            int connection_check_rate = CONNECTION_CHECK_RATE_DEFAULT);

    ~ConnectionAngel();

    unsigned long get_ip() const;
    const char* get_ip_string() const;

public:
    static const char* CONNECTION_NAME_DEFAULT;
    static const int WIFI_RECONNECTS_MAX_DEFAULT;
    static const int CONNECTION_CHECK_RATE_DEFAULT;

    static const char* LOOPBACK_IF_NAME;
    static const char* WIRED_IF_NAME;

private:
    inline bool is_loopback_if(const char* if_name) const;
    inline bool is_wired_if(const char* if_name) const;

private:
    const char* connection_name;
    int wifi_reconnects_max;
    int connection_check_rate;

    //and the last address polled for
    char* address_buffer;
};

}
}
}

#endif //_WIFI_ANGEL_
