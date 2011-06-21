
#include "ConnectionAngel.h"

#include <unistd.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

#include "ConnectionConstants.h"

namespace man {
namespace corpus {
namespace guardian {

using std::string;
using namespace connection_constants;

ConnectionAngel::ConnectionAngel() :
        address_buffer(new char[INET_ADDRSTRLEN]){
}

ConnectionAngel::~ConnectionAngel() {
    delete address_buffer;
}

/**
 * this is the fastest way to do it at around 144 us - Octavian
 */
unsigned long ConnectionAngel::get_ip() const {

    unsigned long ip_address = 0;
    struct ifaddrs * if_list_start = NULL;
    struct ifaddrs * current_if = NULL;
    bool is_wired_connected = false;

    getifaddrs(&if_list_start);

    for (current_if = if_list_start; current_if != NULL;
            current_if = current_if->ifa_next) {

        short family = current_if->ifa_addr->sa_family;
        if (family == AF_INET && !is_loopback_if(current_if->ifa_name)) {
            //extract the address as an unsigned long
            unsigned long if_address =
                    reinterpret_cast<struct sockaddr_in *> (current_if->ifa_addr)->
                    sin_addr.s_addr;
            //we want to give the wired if precedence over wifi
            //since it is the one that is active if both are on
            if (!is_wired_connected) {
                ip_address = if_address;
            }
            if (is_wired_if(current_if->ifa_name) && if_address != 0) {
                is_wired_connected = true;
            }
        }
    }

    // since the interface list is dynamically allocated we need to make sure
    // to free it after we're done
    freeifaddrs(if_list_start);
    return ip_address;
}

const char* ConnectionAngel::get_ip_cstring() const {
    unsigned long int_address = this->get_ip();
    inet_ntop(AF_INET, &int_address, address_buffer, INET_ADDRSTRLEN);
    return address_buffer;
}

std::string ConnectionAngel::get_ip_string() const {
    return std::string(address_buffer);
}

inline bool ConnectionAngel::is_loopback_if(const char* if_name) const {
    return !strcmp(if_name, LOOPBACK_IF_NAME);
}

inline bool ConnectionAngel::is_wired_if(const char* if_name) const {
    return !strcmp(if_name, WIRED_IF_NAME);
}

}
}
}
