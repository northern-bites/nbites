/**
 * @class WifiAngel
 *
 * extends ConnectionAngel with means to reconnecting to wifi
 *
 * @author Octavian Neamtu <oneamtu89@gmail.com>
 */

#ifndef _WIFI_ANGEL_H_
#define _WIFI_ANGEL_H_

#include "ConnectionAngel.h"
#include "ConnectionConstants.h"

#include "Common.h"

#include <string>

namespace man {
namespace corpus {
namespace guardian {

namespace conn_consts = connection_constants;

class WifiAngel : public ConnectionAngel {

public:
    WifiAngel(std::string connection_name =
                      conn_consts::CONNECTION_NAME_DEFAULT,
              std::string connection_pswd =
                      conn_consts::CONNECTION_PSWD_DEFAULT);

    virtual ~WifiAngel(){}

    /**
     * Will check on the status of the wifi.
     * If we don't have any kind of address, then we try to reconnect to wifi
     *
     * !! We assume that we have already connected to that WiFi connection before
     * (or else it won't know the password)
     *
     * @return true if wifi was up, false if it was down
     */
    bool check_on_wifi();
    bool reset_soft();
    bool reset_hard();

private:
    std::string connection_name;
    std::string connection_pswd;
    unsigned frames_with_no_wifi;
    unsigned wifi_reconnect_attempts;

};

}
}
}

#endif //_WIFI_ANGEL_H_
