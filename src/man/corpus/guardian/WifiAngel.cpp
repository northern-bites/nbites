
#include "WifiAngel.h"

#include <stdio.h> //printf()
#include <iostream>
#include <cstdlib> //system()

#include "SoundPaths.h"

namespace man {
namespace corpus {
namespace guardian {

using std::string;
using namespace sound_paths;
using namespace conn_consts;

#define NO_WIFI_RESET_HARD

WifiAngel::WifiAngel(string connection_name, string connection_pswd) :
        connection_name(connection_name), connection_pswd(connection_pswd),
        frames_with_no_wifi(0) {
}

bool WifiAngel::check_on_wifi() {
    if (this->connected()) {
        frames_with_no_wifi = 0;
        return true;
    } else {
        frames_with_no_wifi++;
        switch (frames_with_no_wifi) {
        case WIFI_SOFT_RESET_THRESHOLD :
            reset_soft();
            break;
        case WIFI_HARD_RESET_THRESHOLD :
#ifndef NO_WIFI_RESET_HARD
            reset_hard();
#else
            reset_soft();
#endif
            break;
        default:
            break;
        }
    }
    return false;
}

//TODO: #FunProject not essential, but it could be a fun little project to
//use the dbus to communicate with connman
//that will make it faster, and also give us more control over our connection
//Octavian
bool WifiAngel::reset_soft() {

    printf("Attempting to do a soft reset of wifi\n");

    FILE * f3 = popen(("connman services | awk '/" + connection_name +
                      "/ {print $4}'").c_str(), "r");
    char service[100] = "";
    if(fscanf(f3,"%s\n", service) != EOF)
        printf("Warning: reading service failed on soft wifi reset.\n");
    pclose(f3);

    if (service[0] != ' ') {
        std::string connman_service(service);
        //TODO: make this a call to roboguardian
        if(system((sout+wifi_restart_wav+" &").c_str()) != 0)
            printf("Warning: problem with soft wifi reset.\n");
        if(system(("su -c \" connman connect " + connman_service +
                   " \" & ").c_str()) != 0)
            printf("Warning: problem with soft wifi reset.\n");
        std::cout << "su -c \" connman connect " << connman_service
                  << " \" & " << std::endl;
        return true;
    } else {
        printf("Couldn't find specified wifi network to reconnect to\n");
        return false;
    }
}

bool WifiAngel::reset_hard() {
    printf("Attempting to do a hard reset of wifi\n");

    std::string command;
    command += "su -c /etc/init.d/wireless restart";
    command += " && su -c /etc/init.d/connman restart";
    command += " && su -c connman scan";
    FILE * f3 = popen(("connman services | awk '/" + connection_name +
            "/ {print $3}'").c_str(), "r");
    char service[100] = "";
    if(fscanf(f3,"%s\n", service) != EOF)
        printf("Warning: reading service failed on hard wifi reset.\n");
    pclose(f3);

    if (service[0] != ' ') {
        std::string connman_service(service);
        //TODO: make this a call to roboguardian
        if(system((sout+wifi_restart_wav+" &").c_str()) != 0)
            printf("Warning: problem with hard wifi reset.\n");
        command += " && su -c \" connman passphrase " + connman_service +
                " " + connection_pswd + "\" " +
                " && su -c \" connman autoconnect " + connman_service +
                                " true" + "\" " +
                " && su -c \" connman connect " + connman_service + "\" &";
        std::cout << command.c_str() << std::endl;
        if(system(command.c_str()) != 0)
            printf("Warning: problem with hard wifi reset.\n");

        return true;
    } else {
        printf("Couldn't find specified wifi network to reconnect to\n");
        return false;
    }
}

}
}
}
