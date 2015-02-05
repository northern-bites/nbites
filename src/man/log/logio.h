#ifndef NBlog_logio
#define NBlog_logio

#include <sys/types.h>
#include <stdint.h>
#include "log_header.h"

namespace logio {
    
    //Intended for use with file descriptors
    int write_exact(int fd, size_t nbytes, void * data);
    int write_log(int fd, nblog::log_object_t * log);
    
    //Intended for use with net sockets
    int recv_exact(int sck, size_t nbytes, void * buffer, double max_wait);
    int send_exact(int sck, size_t nbytes, void * buffer);
    int send_log(int sck, nblog::log_object_t * log);
    
    /*
     For parsing log descriptions
     */
    
    
}

#endif //NBlog_logio