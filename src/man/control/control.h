#ifndef nbcontrol_h
#define nbcontrol_h

#include <pthread.h>
#include <CameraParams.pb.h>
#include <google/protobuf/message_lite.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#define CONTROL_USLEEP_WAITING (100000)

#define CONTROL_PORT (30001)

namespace control {
    
    /*
     FLAGS
     */
    typedef enum {
        serv_connected,
        control_connected,
        
        fileio,
        
        SENSORS,
        GUARDIAN,
        COMM,
        LOCATION,
        ODOMETRY,
        OBSERVATIONS,
        LOCALIZATION,
        BALLTRACK,
        VISION,
        
        tripoint,
        thumbnail,
        
        //Num_flags must be last!  it has int value ( 'previous' + 1)
        //which, if it is last,
        //is equivalent to the number of flags previous listed.
        num_flags
    } flag_e;
    
    extern volatile uint8_t flags[num_flags];
    static messages::CameraParams receivedParams;
    
    void control_init();
    
    extern pthread_t control_thread;
}

#endif //nbcontrol_h