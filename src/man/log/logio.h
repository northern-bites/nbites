#ifndef NBlog_logio
#define NBlog_logio

#include <sys/types.h>
#include <stdint.h>

#include <string>
#include <vector>
#include <map>

namespace logio {
    
    typedef struct {
        char * desc;
        size_t dlen;
        uint8_t * data;
    } log_t;
    
    //Intended for use with file descriptors
    int write_exact(int fd, size_t nbytes, void * data);
    int write_log(int fd, log_t * log);
    
    //Intended for use with net sockets
    int recv_exact(int sck, size_t nbytes, void * buffer, double max_wait);
    int recv_log(int sck, log_t * log, double max_wait);
    int send_exact(int sck, size_t nbytes, const void * buffer);
    int send_log(int sck, log_t * log);
    
    /*
     For parsing log descriptions
     */
    
    //Split desc into key-value pairs.
    std::vector<std::string> pairs(const char * desc);
    //Key-value pairs in a map.
    std::map<std::string, std::string> kvp(const char * desc);
    
    //Returns -1 if key not found.
    int widthOf(log_t log);
    int heightOf(log_t log);
    
    //true if log has key 'type' with value equal to passed type.
    bool isType(log_t * log, const char * type);
    
    //Copy log, result's pointers are on heap.
    log_t copyLog(log_t * log);
    //return log struct with pointers copied to heap.
    log_t heapLog(const char * desc, size_t dlen, void * data);
}

#endif //NBlog_logio