//
//  log_cnc.c
//  NB_log_server
//
//  Created by Philip Koch on 12/26/14.
//

#include "log_header.h"

namespace nblog {

    void * cnc_loop(void * cntxt);
    
    void log_cnc_init() {
        log_main->log_cnc_thread = (pthread_t *) malloc(sizeof(pthread_t));
        
        pthread_create(log_main->log_cnc_thread, NULL, &cnc_loop, NULL);
    }

    void * cnc_loop(void * cntxt) {
        
        
        return NULL;
    }
}