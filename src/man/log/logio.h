#ifndef NBlog_logio
#define NBlog_logio

#include <sys/types.h>
#include <stdint.h>
#include "log_header.h"

namespace logio {
    
    typedef struct {
        char * desc;
        int dlen;
        uint8_t * data;
    } log_t;
    
    //Intended for use with file descriptors
    int write_exact(int fd, size_t nbytes, void * data);
    int write_log(int fd, log_t * log);
    
    //Intended for use with net sockets
    int recv_exact(int sck, size_t nbytes, void * buffer, double max_wait);
    int recv_log(int sck, log_t * log, double max_wait);
    int send_exact(int sck, size_t nbytes, void * buffer);
    int send_log(int sck, log_t * log);
    
    /*
     For parsing log descriptions
     */
    
    
}

#endif //NBlog_logio