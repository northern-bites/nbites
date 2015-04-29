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

#include "log_header.h"

namespace nbsf {
    
    /*
     FLAGS
     */
    typedef enum {
        serv_connected,
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
        
        //Num_flags must be last!  it has int value (previous + 1) which, if it is last,
        //is equivalent to the number of flags previous listed.
        num_flags
    } flag_e;
    
    extern volatile uint8_t flags[num_flags];
    
    
    /*
     Logging STATISTICS data structures
     */
    typedef struct {
        uint32_t servnr;
        uint32_t filenr;
        uint32_t nextw;
    } buf_state_t;
    
    /*
     even number of 32 bit fields should result in 64 bit alignment w/o padding.
     */
    typedef struct {
        uint64_t b_given;
        uint64_t b_lost;
        uint64_t b_writ;
        
        uint32_t l_given;
        
        uint32_t l_freed;
        uint32_t l_lost;
        
        uint32_t l_writ;
    } io_state_t;
    
    extern io_state_t fio_start[NUM_LOG_BUFFERS];
    extern io_state_t cio_start[NUM_LOG_BUFFERS];
    extern io_state_t total[NUM_LOG_BUFFERS];
    
    extern uint64_t fio_upstart;
    extern uint64_t sio_upstart;
    
    extern uint64_t cio_upstart;
    extern uint64_t cnc_upstart;
    
    extern uint64_t main_upstart;
    
    const extern uint32_t NUM_CORES;
    
    /*
     typedef struct {
     //Set when fileio flag set.
     //char smagic1[4];
     bufstate_t fio_start[NUM_LOG_BUFFERS];
     //Set when connection established
     bufstate_t cio_start[NUM_LOG_BUFFERS];
     
     //acquire, release, nblog
     bufstate_t current[NUM_LOG_BUFFERS];
     
     //Copied right before stats sent.
     bufmanage_t manage[NUM_LOG_BUFFERS];
     
     //char smagic2[5];
     
     //Upstart set on flag or connection.
     time_stats_t ts;
     
     //Copied right before stats sent.
     uint32_t ratio[NUM_LOG_BUFFERS];
     
     //Constants
     uint32_t size[NUM_LOG_BUFFERS];
     uint32_t num_cores;
     //then flags.
     } log_stats_t;
     */
}

#endif //NB_log__STATS_n_FLAGS_h