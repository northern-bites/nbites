/**
 * @class ConnectionAngel
 *
 * Handles IP discovery
 *
 * @author Octavian Neamtu <oneamtu89@gmail.com>
 *
 */
#ifndef _CONNECTION_ANGEL_
#define _CONNECTION_ANGEL_


#include <string>
#include <stdint.h>

namespace man {
namespace corpus {
namespace guardian {

class ConnectionAngel {


public:
    ConnectionAngel();
    virtual ~ConnectionAngel();

    unsigned long get_ip() const;
    const char* get_ip_cstring() const;
    std::string get_ip_string() const;
    bool connected() const {return get_ip();}

private:
    inline bool is_loopback_if(const char* if_name) const;
    inline bool is_wired_if(const char* if_name) const;

private:
    //address_buffer - contains the last address returned by get_ip
    char* address_buffer;
};

}
}
}

#endif //_CONNECTION_ANGEL_
