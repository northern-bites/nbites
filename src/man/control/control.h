#ifndef nbcontrol_h
#define nbcontrol_h

#include <pthread.h>

#ifndef __APPLE__
#include <CameraParams.pb.h>
#include <google/protobuf/message_lite.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#endif

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
        
        sensors,
        guardian,
        comm,
        localization,
        balltrack,
        vision,
        tripoint,
        multiball,
        thumbnail,
        
        //Num_flags must be last!  it has int value ( 'previous' + 1)
        //which, if it is last,
        //is equivalent to the number of flags previous listed.
        num_flags
    } flag_e;
    
    extern volatile uint8_t flags[num_flags];
    
#ifndef __APPLE__
    static messages::CameraParams receivedParams;
#endif
    
    void control_init();
    
    extern pthread_t control_thread;
}

#endif //nbcontrol_h
