#include "Sound.h"

namespace nbsound {
    
    void * thread_function(void * _handler) {
        printf("thread_function()\n");
        Handler * handler = (Handler *) _handler;
        
        handler->main();
        
        return NULL;
    }
    
    void Handler::start_new_thread(pthread_t& thread, pthread_attr_t * attr) {
        pthread_create(&thread, attr, thread_function, this);
        pthread_detach(thread);
    }

}