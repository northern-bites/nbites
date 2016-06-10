#ifndef WHISTLE_SERVER_SHARE_H
#define WHISTLE_SERVER_SHARE_H

#include "whistle.hpp"

namespace whistle {

    extern bool listening;
    extern bool was_heard;

    static inline void * server_loop(void * unused) {


        return NULL;
    }

    static inline void create_server() {
        NBL_WARN("whistle::create_server()");
        pthread_t server_thread
        pthread_create(&server_thread, NULL, server_loop, NULL);
        pthread_detach(server_thread);
    }
    
}

#endif