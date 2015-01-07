//
//  log_sf.h
//  nbsf & nblog
//
//  Created by Philip Koch on 6.1.2014
//

#ifndef NB_log__STATS_n_FLAGS_h
#define NB_log__STATS_n_FLAGS_h

#include <sys/types.h>
#include <stdint.h>

namespace nbsf {
    
    /*
     FLAGS
     */
    typedef enum {
        serv_connected = 0,
        cnc_connected,
        
        fileio,
        servio,
        
        STATS,
        SENSORS,
        GUARDIAN,
        COMM,
        LOCATION,
        ODOMETRY,
        OBSERVATIONS,
        LOCALIZATION,
        BALLTRACK,
        IMAGES,
        VISION,
        
        num_flags
    } flag_e;
    
    extern uint8_t flags[num_flags];
    
    
    
}

#endif //NB_log__STATS_n_FLAGS_h