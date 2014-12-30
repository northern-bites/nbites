//
//  log_main.c
//  NB_log_server
//
//  Created by Philip Koch on 10/4/14.
//

#include "log_header.h"
#include <string.h>

//#include <unistd.h>
//#include <fcntl.h>

namespace nblog {
    
    log_main_t * log_main = NULL;
    log_stats_t * log_stats = NULL;
    log_flags_t * log_flags = NULL;
    
    const int LOG_BUFFER_SIZES[NUM_LOG_BUFFERS] = {
        1 << 8,
        1 << 5
    };
    
    int LOG_RATIO[NUM_LOG_BUFFERS] = {
        10,
        1
    };
    
    //io thread init functions defined in their own files.
    void log_serverio_init();
    void log_fileio_init();
    void log_cnc_init();
    
    void * log_main_loop(void * context);
    
    void log_main_init() {
        LOGDEBUG(1, "log_main_init()\n");
        log_main = (log_main_t *) malloc(sizeof(log_main_t));
        log_stats = (log_stats_t *) malloc(sizeof(log_stats_t));
        log_flags = (log_flags_t *) malloc(sizeof(log_flags_t));
        
        bzero(log_main, sizeof(log_main_t));
        bzero(log_stats, sizeof(log_stats_t));
        bzero(log_flags, sizeof(log_flags));
        log_flags->servio = true;
        log_flags->STATS = true;
        
        log_stats->start_time = time(NULL);
        pthread_mutex_init(&(log_stats->lock), NULL);
        
        for (int i = 0; i < NUM_LOG_BUFFERS; ++i) {
            size_t buffer_size = sizeof(log_buffer_t) + LOG_BUFFER_SIZES[i] * sizeof(log_object_t *);
            log_main->buffers[i] = (log_buffer_t *) malloc(buffer_size);
            
            bzero(log_main->buffers[i], buffer_size);
            pthread_mutex_init(&(log_main->buffers[i]->lock), NULL);
        }
        
        log_main->log_main_thread = (pthread_t *) malloc(sizeof(pthread_t));
        
        pthread_create(log_main->log_main_thread, NULL, &log_main_loop, NULL);
        //server thread is live...
        LOGDEBUG(1, "log_main thread running...\n");
    }
    
    void * log_main_loop(void * context) {
        log_serverio_init();
        log_fileio_init();
        log_cnc_init();
        
        while (1) {
            sleep(1);
            
            //TODO
            /*
            if (log_main->flags & SERVER_CONNECTED) {
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
    
    /*
     Definitions for log lib functions.
     */
    
    inline int description(char * buf, size_t size, log_object_t * obj)
    {
        int32_t checksum = 0;
        if (obj->data) {
            for (int i = 0; i < obj->n_bytes; ++i)
                checksum += obj->data[i];
        }
        
        int n_written = snprintf(buf, size, "type=%s checksum=%i index=%li time=%lu", obj->type, checksum, obj->image_index, obj->creation_time);
        NBLassert(n_written < size);
        
        return n_written;
    }
    
    inline int write_exactly(int sck_or_fd, size_t nbytes, uint8_t * data) {
        
        size_t written = 0;
        while (written < nbytes) {
            int ret = (int) write(sck_or_fd,
                                  data + written,
                                  nbytes - written);
            if (ret == 0) {
                usleep(USLEEP_EXPECTING);
            } else if (ret < 0) {
                int saved_err = errno;
                
                if (saved_err == EAGAIN) {
                    usleep(USLEEP_EXPECTING);
                } else {
                    char buf[256];
                    strerror_r(saved_err, buf, 256);
                    printf("\n\n********************%s\n", buf);
                    return 2;
                }
            } else {
                written += ret;
            }
        }
        
        assert(written == nbytes);
        return 0;
    }
    
    inline int read_exactly(int sck_or_fd, size_t nbytes, uint8_t * buffer, double max_wait) {
        NBLassert(max_wait >= 1);
        NBLassert(buffer);
        
        time_t last = time(NULL);
        
        size_t rbytes = 0;
        while (rbytes < nbytes) {
            if (difftime(time(NULL), last) >= max_wait)
                return 1;
            
            int ret = (int) read(sck_or_fd, buffer + rbytes, nbytes - rbytes);
            
            if (ret == 0) {
                usleep(USLEEP_EXPECTING);
            } else if (ret < 0) {
                int err_saved = errno;
                if (err_saved == EAGAIN) {
                    usleep(USLEEP_EXPECTING);
                } else {
                    char buf[256];
                    strerror_r(err_saved, buf, 256);
                    printf("\n\n********************%s\n", buf);
                    return 2;
                }
            } else {
                rbytes += ret;
                last = time(NULL);
            }
        }
        
        return 0;
    }
    
    int write_log(int sock_or_fd, log_object_t * log) {
        assert(sock_or_fd >= 0);
        assert(log);
        
        char desc[MAX_LOG_DESC_SIZE];
        description(desc, MAX_LOG_DESC_SIZE, log);
        uint32_t desc_hlen = strlen(desc);
        uint32_t data_hlen = log->n_bytes;
        
        uint32_t desc_nlen = htonl(desc_hlen);
        uint32_t data_nlen = htonl(data_hlen);
        
        if (write_exactly(sock_or_fd, 4, (uint8_t *) &desc_nlen)) {
            return 1;
        }
        if (write_exactly(sock_or_fd, desc_hlen, (uint8_t *) desc)) {
            return 2;
        }
        if (write_exactly(sock_or_fd, 4, (uint8_t *) &data_nlen)) {
            return 3;
        }
        if (log->n_bytes) {
            if (write_exactly(sock_or_fd, data_hlen, log->data)) {
                return 4;
            }
        }
        
        return 0;
    }
    
    //Not thread safe.
    void log_object_free(log_object_t * obj) {
        NBLassert(obj);
        NBLassert(obj->references == 0);
        
        free((char *) obj->type);
        
        if (obj->data)
            free(obj->data);
        
        free(obj);
    }
    
    void release(log_object_t * obj, bool lock) {
        int bi = obj->buffer;
        if (lock) pthread_mutex_lock(&(log_main->buffers[bi]->lock));
        
        NBLassert(obj->references > 0);
        --obj->references;
        if (obj->references == 0)
            log_object_free(obj);
        
        if (lock) pthread_mutex_unlock(&(log_main->buffers[bi]->lock));
    }
    
    log_object_t * acquire(int buffer_index, uint32_t * relevant_last_read)
    {
        log_buffer_t * buf = log_main->buffers[buffer_index];
        
        pthread_mutex_lock(&(buf->lock));
        assert(*relevant_last_read <= buf->next_write);
        log_object_t * ret;
        
        if (*relevant_last_read != buf->next_write) {
            uint32_t rindex = (*relevant_last_read) % LOG_BUFFER_SIZES[buffer_index];
            
            ret = buf->objects[rindex];
            
            ++(*relevant_last_read);
        } else {
            ret = NULL;
        }
        
        ret->was_written = 1;
        
        pthread_mutex_unlock(&(buf->lock));
        return ret;
    }
    
    /*
     NBlog and helper functions.
     */
    
    log_object_t * put(log_object_t * newp, log_buffer_t * buf, int bi) {
        uint32_t old_i = buf->next_write;
        log_object_t * old = buf->objects[(old_i % LOG_BUFFER_SIZES[bi])];
        
        buf->objects[(old_i % LOG_BUFFER_SIZES[bi])] = newp;
        
        assert(buf->fileio_nextr <= buf->next_write);
        uint32_t dif = buf->next_write - buf->fileio_nextr;
        assert(dif <= LOG_BUFFER_SIZES[bi]);
        if (dif == LOG_BUFFER_SIZES[bi]) {
            ++(buf->fileio_nextr);
        }
        
        assert(buf->servio_nextr <= buf->next_write);
        dif = buf->next_write - buf->servio_nextr;
        assert(dif <= LOG_BUFFER_SIZES[bi]);
        if (dif == LOG_BUFFER_SIZES[bi]) {
            ++(buf->servio_nextr);
        }
        
        ++(buf->next_write);
        return old;
    }
    
    //Does not set buffer index.
    log_object_t * log_object_create(size_t index, clock_t creation_time, const char * type, size_t bytes, uint8_t * data)
    {
        NBLassert(type);
        log_object_t * newp = (log_object_t *) malloc(sizeof(log_object_t));
        memset(newp, 0, sizeof(log_object_t));
        
        newp->image_index = index;
        newp->creation_time = creation_time;
        
        size_t type_s = strlen(type) + 1;
        newp->type = (char *) malloc(type_s);
        memcpy((void *) newp->type, type, type_s);
        
        if (data) {
            newp->n_bytes = bytes;
            newp->data = (uint8_t *) malloc(bytes);
            memcpy(newp->data, data, bytes);
        } else {
            newp->data = NULL;
            newp->n_bytes = 0;
        }
        
        newp->references = 1;
        //newp->was_written is 0 as part of memset
        
        return newp;
    }
    
    
    void NBlog(int buffer_index, size_t image_index, clock_t creation_time, const char * type, size_t n_bytes, uint8_t * data)
    {
        LOGDEBUG(8, "NBlog(buffer_index=%i, image_index=%li, type=%s, bytes=%li)\n", buffer_index, image_index, type, n_bytes);
        
        NBLassert(buffer_index < NUM_LOG_BUFFERS);
        //Can't log if the server's not running...
        if (!log_main || !(log_main->log_main_thread)) {
            LOGDEBUG(1, "NBlog returning because !log_main || !log_main->log_main_thread\n");
            return;
        }
        log_object_t * newp = log_object_create( image_index,  creation_time,  type, n_bytes,  data);
        newp->buffer = buffer_index;
        
        log_buffer_t * buf = log_main->buffers[buffer_index];
        
        pthread_mutex_lock(&(buf->lock));
        
        log_object_t * old = put(newp, buf, buffer_index);
        if (old) {
            release(old, false);
        }
        
        pthread_mutex_unlock(&(buf->lock));
    }
}
