//
//  log_main.c
//  NB_log_server
//
//  Created by Philip Koch on 10/4/14.
//

#include "log_header.h"
#include "log_sf.h"
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <inttypes.h>

//#include <unistd.h>
//#include <fcntl.h>

//Declared in order of size, largest to smallest.
namespace nbsf {
    /*
     64 bit fields
     */
    uint64_t fio_upstart;
    uint64_t sio_upstart;
    
    uint64_t cio_upstart;
    uint64_t cnc_upstart;
    
    uint64_t main_upstart;
    
    //Effectively 64 bit fields
    io_state_t fio_start[NUM_LOG_BUFFERS];
    io_state_t cio_start[NUM_LOG_BUFFERS];
    io_state_t total[NUM_LOG_BUFFERS];
    
    /*
     32 bit fields
     */
    
    const uint32_t NUM_CORES = (uint32_t) sysconf(_SC_NPROCESSORS_ONLN);
    
    volatile uint8_t flags[num_flags];
}

namespace nblog {
    log_main_t _log_main;
    log_main_t * log_main = &_log_main;
    
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
    
    void sanity_checks() {
        LOGDEBUG(1, "sanity_checks()...\n");
        
        NBLassert(sizeof(nbsf::io_state_t) == 40);
        NBLassert(sizeof(nbsf::buf_state_t) == 12);
        
        NBLassert(nbsf::cnc_connected == 1);
        NBLassert(nbsf::fileio == 2);
        //...
        
        LOGDEBUG(1, "sane.\n");
    }
    
    void log_main_init() {
        LOGDEBUG(1, "log_main_init()\n");
        
        /*
         stats and flags
         */
        bzero( (uint8_t *) nbsf::flags, nbsf::num_flags);
        nbsf::flags[nbsf::servio] = true;
        nbsf::flags[nbsf::STATS] = true;
        
        nbsf::main_upstart = time(NULL);
        nbsf::sio_upstart = time(NULL);
        
        bzero(nbsf::fio_start, sizeof(nbsf::io_state_t) * NUM_LOG_BUFFERS);
        bzero(nbsf::cio_start, sizeof(nbsf::io_state_t) * NUM_LOG_BUFFERS);
        bzero(nbsf::total, sizeof(nbsf::io_state_t) * NUM_LOG_BUFFERS);
        
        bzero(log_main, sizeof(log_main_t));
        
        for (int i = 0; i < NUM_LOG_BUFFERS; ++i) {
            size_t buffer_size = sizeof(log_buffer_t) + LOG_BUFFER_SIZES[i] * sizeof(log_object_t *);
            log_main->buffers[i] = (log_buffer_t *) malloc(buffer_size);
            
            bzero(log_main->buffers[i], buffer_size);
            pthread_mutex_init(&(log_main->buffers[i]->lock), NULL);
        }
        
        sanity_checks();
        
        log_main->log_main_thread = (pthread_t *) malloc(sizeof(pthread_t));
        
        pthread_create(log_main->log_main_thread, NULL, &log_main_loop, NULL);
        //server thread is live...
        LOGDEBUG(1, "log_main thread running...\n");
    }
    
    
    inline uint64_t net_time(time_t start, time_t end) {
        double dt = difftime(end, start);
        uint64_t hval = dt; // difftime has granulatiy of 1 sec, cast does not lose info
        return htonll(hval);
    }
    
    nbsf::io_state_t zero_state[NUM_LOG_BUFFERS];
    inline void neterize(nbsf::io_state_t * dest, nbsf::io_state_t * start, nbsf::io_state_t * cur) {
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
        
        bzero(zero_state, sizeof(nbsf::io_state_t) * NUM_LOG_BUFFERS);
        
        while (1) {
            sleep(1);
            
            if (nbsf::flags[nbsf::STATS]) {
                
                /*
                 We're sending this over the network to a (presumably) java program.  So if the
                 compiler puts padding in, it will screw up our parsing.  __packed__ *should* 
                 prevent the compiler from putting padding/alignment stuff in.
                 
                 But a thought-out size check after is still a good idea.
                 */
                
                struct __attribute__((__packed__)) {
                    uint64_t fio_upstart;
                    uint64_t sio_upstart;
                    
                    uint64_t cio_upstart;
                    uint64_t cnc_upstart;
                    
                    uint64_t main_upstart;
                    
                    nbsf::io_state_t fio_start[NUM_LOG_BUFFERS];
                    nbsf::io_state_t cio_start[NUM_LOG_BUFFERS];
                    nbsf::io_state_t total[NUM_LOG_BUFFERS];
                    
                    nbsf::buf_state_t state[NUM_LOG_BUFFERS];
                    
                    uint32_t ratio[NUM_LOG_BUFFERS];
                    uint32_t size[NUM_LOG_BUFFERS];
                    
                    uint32_t cores;
                    
                    uint8_t flags[nbsf::num_flags];
                } contig;
                
                // 40 + (3 * n * 40) + (n * 12) + (2 * n * 4) + 4 + (nbsf::num_flags)
                int packed_size = 40 + (3 * NUM_LOG_BUFFERS * 40) + (NUM_LOG_BUFFERS * 12) + (2 * NUM_LOG_BUFFERS * 4) + 4 + (nbsf::num_flags);
                NBLassert(sizeof(contig) == packed_size);
                LOGDEBUG(7, "stat struct size: %i\n", packed_size);
                
                const time_t CURRENT = time(NULL);
                
                //Copy volatile items as close to simultaneously as possible to minimize drift
                contig.fio_upstart = nbsf::fio_upstart;
                contig.sio_upstart = nbsf::sio_upstart;
                contig.cio_upstart = nbsf::cio_upstart;
                contig.cnc_upstart = nbsf::cnc_upstart;
                contig.main_upstart = nbsf::main_upstart;
                
                memcpy(contig.fio_start, nbsf::fio_start, sizeof(nbsf::io_state_t) * NUM_LOG_BUFFERS);
                memcpy(contig.cio_start, nbsf::cio_start, sizeof(nbsf::io_state_t) * NUM_LOG_BUFFERS);
                memcpy(contig.total, nbsf::total, sizeof(nbsf::io_state_t) * NUM_LOG_BUFFERS);
                
                for (int i = 0; i < NUM_LOG_BUFFERS; ++i) {
                    //memcpy(contig.state + i, log_main->buffers[i], sizeof(nbsf::buf_state_t));
                    log_buffer_t * buf = log_main->buffers[i];
                    
                    contig.state[i].filenr = buf->fileio_nextr;
                    contig.state[i].servnr = buf->servio_nextr;
                    contig.state[i].nextw  = buf->next_write;
                    
                    contig.ratio[i] = LOG_RATIO[i];
                    contig.size[i] = LOG_BUFFER_SIZES[i];
                }
                
                memcpy( contig.flags,  (uint8_t *) nbsf::flags, nbsf::num_flags);
                
                //Set non-volatile items, convert everything to network order.
                contig.cores = htonl(nbsf::NUM_CORES);
                
                if (contig.flags[nbsf::fileio]) {
                    neterize(contig.fio_start, contig.fio_start, contig.total);
                    contig.fio_upstart = net_time(contig.fio_upstart, CURRENT);
                } else {
                    bzero(contig.fio_start, sizeof(nbsf::io_state_t) * NUM_LOG_BUFFERS);
                    contig.fio_upstart = 0;
                }
                
                if (contig.flags[nbsf::serv_connected]) {
                    neterize(contig.cio_start, contig.cio_start, contig.total);
                    contig.cio_upstart = net_time(contig.cio_upstart, CURRENT);
                } else {
                    bzero(contig.cio_start, sizeof(nbsf::io_state_t) * NUM_LOG_BUFFERS);
                    contig.cio_upstart = 0;
                }
                
                neterize(contig.total, zero_state, contig.total);
                
                for (int i = 0; i < NUM_LOG_BUFFERS; ++i) {
                    LOGDEBUG(6, "\t\tstat(%i): filenr=%i servnr=%i nextw=%i\n",
                             i, contig.state[i].filenr, contig.state[i].servnr,
                             contig.state[i].nextw);
                    contig.state[i].filenr = htonl(contig.state[i].filenr);
                    contig.state[i].servnr = htonl(contig.state[i].servnr);
                    contig.state[i].nextw = htonl(contig.state[i].nextw);
                    
                    contig.size[i] = htonl(contig.size[i]);
                    contig.ratio[i] = htonl(contig.ratio[i]);
                }
                
                contig.sio_upstart = (contig.flags[nbsf::servio]) ? net_time(contig.sio_upstart, CURRENT) : 0;
                contig.cnc_upstart = (contig.flags[nbsf::cnc_connected]) ? net_time(contig.cnc_upstart, CURRENT) : 0;
                
                contig.main_upstart = net_time(contig.main_upstart, CURRENT);
                
                char cbuf[100];
                snprintf(cbuf, 100, "stats nbuffers=%i", NUM_LOG_BUFFERS);
                NBlog(NBL_SMALL_BUFFER, 0, clock(), cbuf, packed_size, (uint8_t *) &contig);
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
    
    int _put_exactly(ssize_t (* wfunc)(int, const void *, size_t), int sofd, size_t nb, uint8_t * data) {
        NBLassert(wfunc && data && nb);
        NBLassert(sofd >= 0);
        
        size_t written = 0;
        while (written < nb) {
            ssize_t ret = wfunc(sofd,
                                  data + written,
                                  nb - written);
            if (ret == 0) {
                usleep(USLEEP_EXPECTING);
            } else if (ret < 0) {
                int saved_err = errno;
                
                if (saved_err == EAGAIN) {
                    usleep(USLEEP_EXPECTING);
                } else {
                    char buf[256];
                    strerror_r(saved_err, buf, 256);
                    printf("\n\n****_put_exactly****:  %s\n", buf);
                    return 2;
                }
            } else {
                written += ret;
            }
        }
        
        assert(written == nb);
        return 0;
    }
    
    int write_exactly(int fd, size_t nbytes, void * adata) {
        return _put_exactly(&write, fd, nbytes, (uint8_t *) adata);
    }
    
    ssize_t send_stub(int sck, const void * data, size_t nbytes) {
#ifndef __APPLE__
        return send(sck, data, nbytes, MSG_NOSIGNAL);
#else
        return send(sck, data, nbytes, 0);
#endif
    }
    
    int send_exactly(int socket, size_t nbytes, void * adata) {
        return _put_exactly(&send_stub, socket, nbytes, (uint8_t *) adata);
    }
    
    int recv_exactly(int sck, size_t nbytes, void * abuffer, double max_wait) {
        uint8_t * buffer = (uint8_t *) abuffer;
        
        NBLassert(max_wait >= 1);
        NBLassert(buffer);
        
        time_t last = time(NULL);
        
        size_t rbytes = 0;
        while (rbytes < nbytes) {
            if (difftime(time(NULL), last) >= max_wait)
                return 1;
#ifndef __APPLE__
            ssize_t ret = recv(sck, buffer + rbytes, nbytes - rbytes, MSG_NOSIGNAL);
#else
            ssize_t ret = recv(sck, buffer + rbytes, nbytes - rbytes, 0);
#endif
            
            if (ret == 0) {
                usleep(USLEEP_EXPECTING);
            } else if (ret < 0) {
                int err_saved = errno;
                if (err_saved == EAGAIN) {
                    usleep(USLEEP_EXPECTING);
                } else {
                    char buf[256];
                    strerror_r(err_saved, buf, 256);
                    printf("\n\n****recv_exactly****:  %s\n", buf);
                    return 2;
                }
            } else {
                rbytes += ret;
                last = time(NULL);
            }
        }
        
        return 0;
    }
    
    int _put_log( int (*exacter)(int, size_t, void *), int sofd, log_object_t * log) {
        NBLassert(sofd >= 0);
        NBLassert(log);
        
        char desc[MAX_LOG_DESC_SIZE];
        description(desc, MAX_LOG_DESC_SIZE, log);
        uint32_t desc_hlen = strlen(desc);
        uint32_t data_hlen = log->n_bytes;
        
        uint32_t desc_nlen = htonl(desc_hlen);
        uint32_t data_nlen = htonl(data_hlen);
        
        if (exacter(sofd, 4, &desc_nlen)) {
            return 1;
        }
        if (exacter(sofd, desc_hlen, desc)) {
            return 2;
        }
        if (exacter(sofd, 4, &data_nlen)) {
            return 3;
        }
        if (log->n_bytes) {
            if (exacter(sofd, data_hlen, log->data)) {
                return 4;
            }
        }
        
        return 0;
    }
    
    int send_log(int sock, log_object_t * log) {
        return _put_log(&send_exactly, sock, log);
    }
    
    int write_log(int fd, log_object_t * log) {
        return _put_log(&write_exactly, fd, log);
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
            
            nbsf::total[bi].l_freed += 1;
            if (!obj->was_written) {
                nbsf::total[bi].l_lost += 1;
                nbsf::total[bi].b_lost += obj->n_bytes;
            }
            
            log_object_free(obj);
        }
        
        if (lock) pthread_mutex_unlock(&(log_main->buffers[bi]->lock));
    }
    
    log_object_t * acquire(int buffer_index, uint32_t * relevant_nextr)
    {
        
        log_buffer_t * buf = log_main->buffers[buffer_index];
        LOGDEBUG(6, "acq(%i):\t%i [filenr=%i servnr=%i nextw=%i]\n", buffer_index, *relevant_nextr,
                 buf->fileio_nextr, buf->servio_nextr, buf->next_write);
        
        pthread_mutex_lock(&(buf->lock));
        assert(*relevant_nextr <= buf->next_write);
        log_object_t * ret;
        
        if (*relevant_nextr != buf->next_write) {
            uint32_t rindex = (*relevant_nextr) % LOG_BUFFER_SIZES[buffer_index];
            
            ret = buf->objects[rindex];
            assert(ret);
            
            ++(*relevant_nextr);
        } else {
            ret = NULL;
        }
        
        if (ret) {
            ret->was_written = 1;
            ++(ret->references);
            nbsf::total[buffer_index].l_writ += 1;
            nbsf::total[buffer_index].b_writ += ret->n_bytes;
        }
        
        LOGDEBUG(6, "\t\t%i [filenr=%i servnr=%i nextw=%i]\n", *relevant_nextr,
                 buf->fileio_nextr, buf->servio_nextr, buf->next_write);
        
        pthread_mutex_unlock(&(buf->lock));
        return ret;
    }
    
    /*
     NBlog and helper functions.
     */
    
    log_object_t * put(log_object_t * newp, log_buffer_t * buf, int bi) {
        
        NBLassert(newp);
        LOGDEBUG(6, "put(%i):\tfilenr=%i servnr=%i nextw=%i\n", bi, buf->fileio_nextr, buf->servio_nextr, buf->next_write);
        
        nbsf::total[bi].l_given += 1;
        nbsf::total[bi].b_given += newp->n_bytes;
        
        uint32_t old_i = buf->next_write;
        log_object_t * old = buf->objects[(old_i % LOG_BUFFER_SIZES[bi])];
        
        buf->objects[(old_i % LOG_BUFFER_SIZES[bi])] = newp;
        
        NBLassert(buf->fileio_nextr <= buf->next_write);
        uint32_t dif = buf->next_write - buf->fileio_nextr;
        NBLassert(dif <= LOG_BUFFER_SIZES[bi]);
        if (dif == LOG_BUFFER_SIZES[bi]) {
            ++(buf->fileio_nextr);
        }
        
        NBLassert(buf->servio_nextr <= buf->next_write);
        dif = buf->next_write - buf->servio_nextr;
        NBLassert(dif <= LOG_BUFFER_SIZES[bi]);
        if (dif == LOG_BUFFER_SIZES[bi]) {
            ++(buf->servio_nextr);
        }
        
        ++(buf->next_write);
        
        LOGDEBUG(6, "\t\tfilenr=%i servnr=%i nextw=%i\n", buf->fileio_nextr, buf->servio_nextr, buf->next_write);
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

        //Extra byte is to ensure null character.
        size_t ts = strlen(type) + 1;
        newp->type = (char *) malloc(ts);
        memcpy((void *) newp->type, type, ts);
        
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
        log_object_t * newp = log_object_create( image_index,  creation_time, type, n_bytes,  data);
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
