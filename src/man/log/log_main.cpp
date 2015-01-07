//
//  log_main.c
//  NB_log_server
//
//  Created by Philip Koch on 10/4/14.
//

#include "log_header.h"
#include <string.h>
#include <sys/types.h>

//#include <unistd.h>
//#include <fcntl.h>

namespace nbsf {
    uint8_t flags[num_flags];
}

namespace nblog {
    
    log_main_t _log_main;
    log_stats_t _log_stats;
    log_flags_t _log_flags;
    
    log_main_t * log_main = &_log_main;
    log_stats_t * log_stats = &_log_stats;
    log_flags_t * log_flags = &_log_flags;
    
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
        
        bzero(nbsf::flags, nbsf::num_flags);
        nbsf::flags[nbsf::servio] = true;
        nbsf::flags[nbsf::STATS] = true;
        
        signal(SIGPIPE, SIG_IGN);
        
        bzero(log_main, sizeof(log_main_t));
        bzero(log_stats, sizeof(log_stats_t));
        bzero(log_flags, sizeof(log_flags));
        log_flags->servio = true;
        log_flags->STATS = true;
        
        log_stats->ts.start = time(NULL);
        log_stats->ts.sio_upstart = time(NULL);
        log_stats->num_cores = sysconf(_SC_NPROCESSORS_ONLN);
        
        for (int i = 0; i < NUM_LOG_BUFFERS; ++i) {
            size_t buffer_size = sizeof(log_buffer_t) + LOG_BUFFER_SIZES[i] * sizeof(log_object_t *);
            log_main->buffers[i] = (log_buffer_t *) malloc(buffer_size);
            
            bzero(log_main->buffers[i], buffer_size);
            pthread_mutex_init(&(log_main->buffers[i]->lock), NULL);
            
            log_stats->size[i] = LOG_BUFFER_SIZES[i];
        }
        
        log_main->log_main_thread = (pthread_t *) malloc(sizeof(pthread_t));
        
        pthread_create(log_main->log_main_thread, NULL, &log_main_loop, NULL);
        //server thread is live...
        LOGDEBUG(1, "log_main thread running...\n");
    }
    
    inline uint64_t net_time(time_t start, time_t end) {
        double dt = difftime(end, start);
        uint64_t hval = dt;
        return htonll(hval);
    }
    
    inline void net_prep(bufstate_t * dest, bufstate_t * cur, bufstate_t * _start) {
        bufstate_t * start;
        bufstate_t buf[NUM_LOG_BUFFERS];
    
        if (_start) start = _start;
        else {
            start = buf;
            bzero(buf, sizeof(bufstate_t) * NUM_LOG_BUFFERS);
        }
        
        for (int i = 0; i < NUM_LOG_BUFFERS; ++i) {
            dest[i].l_given = htonl(cur[i].l_given - start[i].l_given);
            dest[i].b_given = htonll(cur[i].b_given - start[i].b_given);
            
            dest[i].l_freed = htonl(cur[i].l_freed - start[i].l_freed);
            dest[i].l_lost = htonl(cur[i].l_lost - start[i].l_lost);
            dest[i].b_lost = htonll(cur[i].b_lost - start[i].b_lost);
            
            dest[i].l_writ = htonl(cur[i].l_writ - start[i].l_writ);
            dest[i].b_writ = htonll(cur[i].b_writ - start[i].b_writ);
        }
    }
    
    void * log_main_loop(void * context) {
        log_serverio_init();
        log_fileio_init();
        log_cnc_init();
        
        while (1) {
            sleep(1);
            
            if (log_flags->STATS) {
                const time_t CURRENT = time(NULL);
                
                struct contig_s {
                    log_stats_t ls;
                    log_flags_t lf;
                } contig;
                
                int stat_length = sizeof(log_stats_t) + sizeof(log_flags_t);
                log_stats_t * ls = &contig.ls;
                log_flags_t * lf = &contig.lf;
                
                printf("%lu vs. %d vs. %lu\n", sizeof(contig), stat_length, sizeof(contig_s));
                NBLassert(sizeof(contig) == stat_length);
                
                //Try to accumulate out data as quickly as possible, to minimize potential drift.
                *ls = *log_stats;
                *lf = *log_flags;
                for (int i = 0; i < NUM_LOG_BUFFERS; ++i) {
                    memcpy(&(ls->manage[i]), log_main->buffers[i], sizeof(uint32_t) * 3);
                    
                    ls->ratio[i] = LOG_RATIO[i];
                }
                
                /*
                strncpy(ls->smagic1, "stat", 4);
                strncpy(ls->smagic2, "magic", 5);
                
                strncpy(lf->fmagic1, "flags", 5);
                strncpy(lf->fmagic2, "endof", 5); */
                
                printf("sizes:\n\tmanage: %lu\n\tstate: %lu\n\ttime: %lu\n\tlog_stats: %lu\n\tlog_flags: %lu\n\ttotal: %lu\n", sizeof(bufmanage_t), sizeof(bufstate_t), sizeof(time_stats_t), sizeof(log_stats_t), sizeof(log_flags_t), sizeof(contig));
                
                
                
                //Data copied, now:
                //  Set non-valid fields 0, convert to network endian-ness
                
                if (lf->fileio) {
                    net_prep(ls->fio_start, ls->current, ls->fio_start);
                    ls->ts.fio_upstart = net_time(ls->ts.fio_upstart, CURRENT);
                } else {
                    bzero(ls->fio_start, sizeof(bufstate_t) * NUM_LOG_BUFFERS);
                    ls->ts.fio_upstart = 0;
                }
                
                if (lf->serv_connected) {
                    net_prep(ls->cio_start, ls->current, ls->cio_start);
                    ls->ts.cio_upstart = net_time(ls->ts.cio_upstart, CURRENT);
                } else {
                    bzero(ls->cio_start, sizeof(bufstate_t) * NUM_LOG_BUFFERS);
                    ls->ts.cio_upstart = 0;
                }
                
                net_prep(ls->current, ls->current, NULL);
                
                for (int i = 0; i < NUM_LOG_BUFFERS; ++i) {
                    ls->manage[i].filenr = htonl(ls->manage[i].filenr);
                    ls->manage[i].servnr = htonl(ls->manage[i].servnr);
                    ls->manage[i].nextw = htonl(ls->manage[i].nextw);
                }
                
                if (lf->servio) {
                    ls->ts.sio_upstart = net_time(ls->ts.sio_upstart, CURRENT);
                } else {
                    ls->ts.sio_upstart = 0;
                }
                
                if (lf->cnc_connected) {
                    ls->ts.cnc_upstart = net_time(ls->ts.cnc_upstart, CURRENT);
                } else {
                    ls->ts.cnc_upstart = 0;
                }
                
                ls->ts.start = net_time(ls->ts.start, CURRENT);
                
                //Flags can stay as they are.
                printf("stat_length=%i\n", stat_length);
                
                char cbuf[100];
                snprintf(cbuf, 100, "stats nbuffers=%i", NUM_LOG_BUFFERS);
                NBlog(0, 0, clock(), cbuf, stat_length, (uint8_t *) &contig);
            }
        }
        
        return NULL;
    }
    
    /*
     Definitions for log lib functions.
     */
    
    int description(char * buf, size_t size, log_object_t * obj)
    {
        int32_t checksum = 0;
        if (obj->data) {
            for (int i = 0; i < obj->n_bytes; ++i)
                checksum += obj->data[i];
        }
        
        int n_written = snprintf(buf, size, "type=%s checksum=%i index=%li time=%lu version=%i", obj->type, checksum, obj->image_index, obj->creation_time, LOG_VERSION);
        NBLassert(n_written < size);
        
        return n_written;
    }
    
    int write_exactly(int sck_or_fd, size_t nbytes, void * adata) {
        uint8_t * data = (uint8_t *) adata;
        
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
    
    int read_exactly(int sck_or_fd, size_t nbytes, void * abuffer, double max_wait) {
        uint8_t * buffer = (uint8_t *) abuffer;
        
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
        if (obj->references == 0) {
            
            log_stats->current[bi].l_freed += 1;
            if (!obj->was_written) {
                log_stats->current[bi].l_lost += 1;
                log_stats->current[bi].b_lost += obj->n_bytes;
            }
            
            log_object_free(obj);
        }
        
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
            assert(ret);
            
            ++(*relevant_last_read);
        } else {
            ret = NULL;
        }
        
        if (ret) {
            ret->was_written = 1;
            ++(ret->references);
            log_stats->current[buffer_index].l_writ += 1;
            log_stats->current[buffer_index].b_writ += ret->n_bytes;
        }
        
        pthread_mutex_unlock(&(buf->lock));
        return ret;
    }
    
    /*
     NBlog and helper functions.
     */
    
    log_object_t * put(log_object_t * newp, log_buffer_t * buf, int bi) {
        
        assert(newp);
        log_stats->current[bi].l_given += 1;
        log_stats->current[bi].b_given += newp->n_bytes;
        
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
