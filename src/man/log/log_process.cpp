//
//  log_process.c
//  NB_log_server
//
//  Created by Philip Koch on 10/4/14.
//

#include "log_header.h"
#include <string.h>

//#include <unistd.h>
//#include <fcntl.h>

namespace nblog {
    
    log_process_t * log_process = NULL;
    log_stats_t * log_stats = NULL;
    
    const int LOG_BUFFER_SIZES[NUM_LOG_BUFFERS] = {
        200,
        20
    };
    
    //io thread init functions defined in their own files.
    void log_serverio_init();
    void log_fileio_init();
    
    void * log_process_loop(void * context);
    
    void log_process_init() {
        LOGDEBUG(1, "log_process_init()\n");
        log_process =(log_process_t *) malloc(sizeof(log_process_t));
        log_stats = (log_stats_t *) malloc(sizeof(log_stats_t));
        
        memset(log_stats, 0, sizeof(log_stats_t));
        log_stats->start_time = time(NULL);
        pthread_mutex_init(&(log_stats->lock), NULL);
        
        memset(log_process, 0, sizeof(log_process_t));
        for (int i = 0; i < NUM_LOG_BUFFERS; ++i) {
            size_t buffer_size = sizeof(log_buffer_t) + LOG_BUFFER_SIZES[i] * sizeof(log_object_t *);
            log_process->buffers[i] = (log_buffer_t *) malloc(buffer_size);
            
            memset(log_process->buffers[i], 0, buffer_size);
            pthread_mutex_init(&(log_process->buffers[i]->lock), NULL);
        }
        
        log_process->log_process_thread = (pthread_t *) malloc(sizeof(pthread_t));
        
        pthread_create(log_process->log_process_thread, NULL, &log_process_loop, NULL);
        //server thread is live...
        LOGDEBUG(1, "log_process thread running...\n");
    }
    
    void * log_process_loop(void * context) {
        log_serverio_init();
        log_fileio_init();
        
        while (1) {
            sleep(1);
            
            //TODO
            /*
            if (log_process->flags & SERVER_CONNECTED) {
                //Try to log some sort of stats message.
                char buf[1024];
                
                int size = snprintf(buf, 1024, "bytes_logged=%llu logs_given=%llu logs_freed=%llu logs_lost=%llu logs_written=%llu logs_sent=%llu c_logs_sent=%llu c_bytes_sent=%llu c_logs_lost=%llu server_uptime=%f con_uptime=%f",
                         log_stats->bytes_logged,
                         log_stats->logs_given,
                         log_stats->logs_freed,
                         log_stats->logs_lost,
                         log_stats->logs_written,
                         log_stats->logs_sent,
                         log_stats->c_logs_sent,
                         log_stats->c_bytes_sent,
                         log_stats->c_logs_lost,
                         difftime(time(NULL), log_stats->start_time ),
                         difftime(time(NULL), log_stats->connect_start));
                
                assert(size > 0);
                assert(size < 1024);
                NBlog(SMALL_BUFFER, 0, clock(), "log_stats_t", (size_t) size, (uint8_t *) buf);
            } */
        }
        
        return NULL;
    }
    
    //thread safe
    void log_object_release(log_object_t * obj) {
        int buffer_index = obj->buffer;
        pthread_mutex_lock(&(log_process->buffers[buffer_index]->lock));
        
        --obj->references;
        
        NBLassert(obj->references >= 0);
        if (!obj->references) log_object_free(obj);
        
        pthread_mutex_unlock(&(log_process->buffers[buffer_index]->lock));
    }
    
    //Not thread safe.
    void log_object_free(log_object_t * obj) {
        NBLassert(obj->references == 0);
        //NBLassert(obj->was_written);
        /*if (!obj->was_written) {
            printf("X");
        }*/
        
        free((char *) obj->type);
        free(obj->data);
        
        free(obj);
    }
    
    log_object_t * log_object_create(size_t index, clock_t creation_time, const char * type, size_t bytes, uint8_t * data) {
        NBLassert(data && type);
        log_object_t * newp = (log_object_t *) malloc(sizeof(log_object_t));
        memset(newp, 0, sizeof(log_object_t));
        
        newp->image_index = index;
        newp->creation_time = creation_time;
        
        size_t type_s = strlen(type) + 1;
        newp->type = (char *) malloc(type_s);
        //memcpy(type, newp->type, type_s);
        memcpy((void *) newp->type, type, type_s);
        
        newp->n_bytes = bytes;
        newp->data = (uint8_t *) malloc(bytes);
        memcpy(newp->data, data, bytes);
        
        newp->references = 1;
        
        //newp->was_written is 0 as part of memset
        
        return newp;
    }
    
    void NBlog_statInc(size_t bytes) {
        pthread_mutex_lock(&(log_stats->lock));
        
        log_stats->bytes_logged += bytes;
        log_stats->logs_given += 1;
        
        log_stats->c_bytes_sent += bytes;
        log_stats->c_logs_sent +=1;
        
        pthread_mutex_unlock(&(log_stats->lock));
    }
    
    void NBlog(int buffer_index, size_t image_index, clock_t creation_time, const char * type, size_t n_bytes, uint8_t * data) {
        LOGDEBUG(8, "NBlog(buffer_index=%i, image_index=%li, type=%s, bytes=%li)\n", buffer_index, image_index, type, n_bytes);
        
        NBlog_statInc(n_bytes);
        
        NBLassert(buffer_index < NUM_LOG_BUFFERS);
        //Can't log if the server's not running...
        if (!log_process || !log_process->log_process_thread) {
            LOGDEBUG(1, "NBlog returning because !log_process || !log_process->log_process_thread\n");
            return;
        }
        log_object_t * newp = log_object_create( image_index,  creation_time,  type, n_bytes,  data);
        newp->buffer = buffer_index;
        
        log_buffer_t * buf = log_process->buffers[buffer_index];
        
        pthread_mutex_lock(&(buf->lock));
        buf->last_written = (buf->last_written + 1) % LOG_BUFFER_SIZES[buffer_index];
        
        log_object_t * old = buf->objects[buf->last_written];
        buf->objects[buf->last_written] = newp;
        if (old) {
            //CANNOT USE RELEASE (we're in a mutex already!)
            --old->references;
            if ((old->references) == 0) log_object_free(old);
        }
        
        pthread_mutex_unlock(&(buf->lock));
    }
    
    /*
     Return a reference to the next loggable object, or NULL if no such object exists.
     
     Since the logging threads don't actually maintain references to logged objects (or mark them as such),
     if exactly **_RING_BUFFER_SIZE is written to a buffer before the next get_**() call is made, the below methods cannot detect the update and will return NULL....
     
     solutions?
     */
    
    log_object_t * get_log(int buffer_index, int * last_read) {
        LOGDEBUG(11, "get_log(buffer_index=%i, last_read=%i)\n", buffer_index, *last_read);
        log_buffer_t * buf = log_process->buffers[buffer_index];
        pthread_mutex_lock(&(buf->lock));
        
        if (*last_read == buf->last_written) {
            pthread_mutex_unlock(&buf->lock);
            return NULL;
        }
        
        *last_read = (*last_read + 1) % LOG_BUFFER_SIZES[buffer_index];
        
        log_object_t * ret = buf->objects[*last_read];
        
        if (ret) {
            ret->was_written = 1;
            ++ret->references;
        }
        
        pthread_mutex_unlock(&(buf->lock));
        
        return ret;
    }
}
