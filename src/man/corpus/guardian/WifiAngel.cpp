
#include "WifiAngel.h"

#include <stdio.h> //printf()
#include <cstdlib> //system()
#include <cstring> //strcat()

namespace man {
namespace corpus {
namespace guardian {

using std::string;

WifiAngel::WifiAngel(string connection_name,
        int wifi_reconnects_max) :
        connection_name(connection_name),
        wifi_reconnects_max(wifi_reconnects_max),
        wifi_reconnect_attempts(0) {
}

bool WifiAngel::check_on_wifi() {
    if (this->connected()) {
        wifi_reconnect_attempts = 0;
        return true;
    } else {
        if (wifi_reconnect_attempts < wifi_reconnects_max) {
            wifi_reconnect_attempts++;
            this->try_to_reconnect();
        }
        return false;
    }
}

//TODO: this returns false always - is there a way to check if we're successful?
//Octavian
bool WifiAngel::try_to_reconnect() {

    printf("Attempting to reconnect to wifi\n");

    FILE * f3 = popen(("connman services | awk '/" + connection_name +
                      "/ {print $4}'").c_str(), "r");
    char service[100] = "";
    fscanf(f3,"%s\n", service);
    pclose(f3);

    if (service[0] != ' ') {
        //TODO: make this a call to roboguardian
        //system((sout+str+" &").c_str());
        char command[100] = "";
        strcat(command, "su -c \" connman connect ");
        strcat(command, service);
        strcat(command, " \" & ");
        system(command);
    } else {
        printf("Couldn't find specified wifi network to reconnect to\n");
    }

    return false;
}

}
}
}
