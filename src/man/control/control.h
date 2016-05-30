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
     The following code uses X macros.  If you're not sure what those are
     or how they work, google them.
     */
    
    //Construct flag enum.
#define XTEMP(n) n
    typedef enum {
#include "control_flags.defs.h"
    } flag_e;
#undef XTEMP
    
//The above used to generate:
    /*
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
    } flag_e; */
    
    extern volatile uint8_t flags[];
    extern const char * string_flags[];
    
#ifndef __APPLE__
    static messages::CameraParams receivedParams;
#endif
    
    void control_init();
    void control_destroy();
    
    extern pthread_t control_thread;
}

#endif //nbcontrol_h
