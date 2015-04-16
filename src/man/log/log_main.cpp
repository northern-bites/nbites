//
//  log_main.c
//  NB_log_server
//
//  Created by Philip Koch on 10/4/14.
//

#include "logging.h"
#include "control.h"
#include "nbdebug.h"

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

using log::Log;
using log::SExpr;

namespace nblog {
    
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

    bool log_running = false;
    log_main_t log_main;
    
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
    
    void * log_main_loop(void * context);
    
    void sanity_checks() {
        NBDEBUG("sanity_checks()...\n");
        
        NBLassert(control::control_connected == 1);
        NBLassert(control::fileio == 2);
        //...
        
        NBDEBUG("sane.\n");
    }
    
    void log_main_init() {
        NBDEBUG("log_main_init()\n");
        
        main_upstart = time(NULL);
        sio_upstart = time(NULL);
        
        bzero(fio_start, sizeof(io_state_t) * NUM_LOG_BUFFERS);
        bzero(cio_start, sizeof(io_state_t) * NUM_LOG_BUFFERS);
        bzero(total, sizeof(io_state_t) * NUM_LOG_BUFFERS);
        
        bzero(&log_main, sizeof(log_main_t));
        
        for (int i = 0; i < NUM_LOG_BUFFERS; ++i) {
            log_main.buffers[i].objects = (Log **) malloc(LOG_BUFFER_SIZES[i] * sizeof(Log *));
            
            pthread_mutex_init(&(log_main.buffers[i].lock), NULL);
        }
        
        sanity_checks();
        
        pthread_create(&(log_main.log_main_thread), NULL, &log_main_loop, NULL);
        pthread_detach(log_main.log_main_thread);
        //server thread is live...
        
        log_running = true;
        NBDEBUG("log_main thread running...\n");
    }
    
    /*
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
    } */
    
    void * log_main_loop(void * context) {
        
        log_serverio_init();
        log_fileio_init();

        while (1) {
            sleep(1);
            
            //Log state.
            
            std::vector<SExpr> fvector = {
                SExpr("flags"),
                SExpr("serv_connected", control::flags[control::serv_connected]),
                SExpr("control_connected", control::flags[control::control_connected]),
                SExpr("fileio", control::flags[control::fileio]),
                SExpr("SENSORS", control::flags[control::SENSORS]),
                SExpr("GUARDIAN", control::flags[control::GUARDIAN]),
                SExpr("COMM", control::flags[control::COMM]),
                SExpr("LOCATION", control::flags[control::LOCATION]),
                SExpr("ODOMETRY", control::flags[control::ODOMETRY]),
                SExpr("OBSERVATIONS", control::flags[control::OBSERVATIONS]),
                SExpr("LOCALIZATION", control::flags[control::LOCALIZATION]),
                SExpr("BALLTRACK", control::flags[control::BALLTRACK]),
                SExpr("IMAGES", control::flags[control::IMAGES]),
                SExpr("VISION", control::flags[control::VISION]),
            };
            
            SExpr flags(fvector);
            
            //... all the other stats stuff...
            
            std::vector<SExpr> cvector = { flags };
            SExpr contents = SExpr(cvector);
            
            
            //printf("stats...\n");
            NBLog(NBL_SMALL_BUFFER, "log_main", time(NULL), contents, std::string());   //no data with it.
        }
        
        return NULL;
    }
    
    /*
     Definitions for log lib functions.
     */
    
    int32_t checksum(const std::string& data) {
        int32_t sum = 0;
        for (int i = 0; i < data.size(); ++i)
            sum += (uint8_t) data[i];
        
        return sum;
    }
    
    void releaseWrapper(int bi, log::Log * lg, bool lock) {
        NBLassert(bi >= 0 && bi < NUM_LOG_BUFFERS);
        NBLassert(lg);
        
        if (lock) pthread_mutex_lock(&(log_main.buffers[bi].lock));
        
        if (lg->release()) {
            
            total[bi].l_freed += 1;
            if (!lg->written()) {
                total[bi].l_lost += 1;
                total[bi].b_lost += lg->fullSize();
            }
            
            delete lg;
        }
        
        if (lock) pthread_mutex_unlock(&(log_main.buffers[bi].lock));
    }
    
    Log * acquire(int buffer_index, uint32_t * relevant_nextr)
    {
        
        log_buffer_t& buf = log_main.buffers[buffer_index];
        
        NBDEBUGs(SECTION_LOGM, "acq(%i):\t%i [filenr=%i servnr=%i nextw=%i]\n", buffer_index, *relevant_nextr,
                 buf.fileio_nextr, buf.servio_nextr, buf.next_write);
        
        pthread_mutex_lock(&(buf.lock));
        assert(*relevant_nextr <= buf.next_write);
        Log * ret;
        
        if (*relevant_nextr != buf.next_write) {
            uint32_t rindex = (*relevant_nextr) % LOG_BUFFER_SIZES[buffer_index];
            
            ret = buf.objects[rindex];
            assert(ret);
            
            ++(*relevant_nextr);
        } else {
            ret = NULL;
        }
        
        if (ret) {
            ret->acquire();
            total[buffer_index].l_writ += 1;
            total[buffer_index].b_writ += ret->fullSize();
        }
        
        NBDEBUGs(SECTION_LOGM, "\t\t%i [filenr=%i servnr=%i nextw=%i]\n", *relevant_nextr,
                 buf.fileio_nextr, buf.servio_nextr, buf.next_write);
        
        pthread_mutex_unlock(&(buf.lock));
        return ret;
    }
    
    /*
     NBlog and helper functions.
     */
    
    void put(Log * nstored, int bi) {
        
        NBLassert(nstored);
        log_buffer_t& buf = log_main.buffers[bi];
        
        NBDEBUGs(SECTION_LOGM, "put(%i):\tfilenr=%i servnr=%i nextw=%i\n", bi, buf.fileio_nextr, buf.servio_nextr, buf.next_write);
        
        total[bi].l_given += 1;
        total[bi].b_given += nstored->fullSize();
        nstored->acquire(); //One reference for the buffer.
        
        uint32_t old_i = buf.next_write;
        Log * old = buf.objects[old_i % LOG_BUFFER_SIZES[bi]];
        
        if (old && old->release()) {
            delete old;
        }
        
        buf.objects[(old_i % LOG_BUFFER_SIZES[bi])] = nstored;
        
        //If IO is very slow, we need to update IO 'next_*'
        NBLassert(buf.fileio_nextr <= buf.next_write);
        uint32_t dif = buf.next_write - buf.fileio_nextr;
        NBLassert(dif <= LOG_BUFFER_SIZES[bi]);
        if (dif == LOG_BUFFER_SIZES[bi]) {
            ++(buf.fileio_nextr);
        }
        
        NBLassert(buf.servio_nextr <= buf.next_write);
        dif = buf.next_write - buf.servio_nextr;
        NBLassert(dif <= LOG_BUFFER_SIZES[bi]);
        if (dif == LOG_BUFFER_SIZES[bi]) {
            ++(buf.servio_nextr);
        }
        
        ++(buf.next_write);
        
        NBDEBUGs(SECTION_LOGM, "\t\tfilenr=%i servnr=%i nextw=%i\n", buf.fileio_nextr, buf.servio_nextr, buf.next_write);
    }
    
    /*
    void NBlog(int buffer_index, size_t image_index, clock_t creation_time, const char * type, size_t n_bytes, uint8_t * data)
    {
        LOGDEBUG(8, "NBlog(buffer_index=%i, image_index=%li, type=%s, bytes=%li)\n", buffer_index, image_index, type, n_bytes);
        
        NBLassert(buffer_index < NUM_LOG_BUFFERS);
        //Can't log if the server's not running...
        if (!log_main || !(log_main->log_main_thread)) {
            NBDEBUG( "NBlog returning because !log_main || !log_main->log_main_thread\n");
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
    } */
    
    void NBLog(int BI, log::SExpr& desc, const std::string& data) {
        
        NBDEBUGs(SECTION_LOGM, "NBlog(buffer_index=%i)\n", BI);
        NBLassert(BI < NUM_LOG_BUFFERS);
        //Can't log if the server's not running...
        if (!log_running) {
            NBDEBUG("NBlog returning because !log_running\n");
            return;
        }
        
        Log * log = new Log(desc);
        log->setData(data);
        
        pthread_mutex_lock(&(log_main.buffers[BI].lock));
        put(log, BI);
        pthread_mutex_unlock(&(log_main.buffers[BI].lock));
    }
    
    void NBLog(int BI, const std::string where_made, time_t when_made, log::SExpr& content_list, const std::string& data) {
        
        time_t now = time(NULL);
        tm * ptm = localtime(&now);
        char buffer[100];
        strftime(buffer, 100, "%d.%m.%Y %H:%M:%S", ptm);
        std::string time(buffer);
        
        std::vector<SExpr> made_list = {
            SExpr("created"),
            SExpr(where_made),
            SExpr(time)
        };
        
        std::vector<SExpr> top_list =  {
            SExpr("nblog"),
            SExpr(made_list),
            SExpr("version", LOG_VERSION),
            SExpr("checksum", checksum(data)),
            SExpr("contents", content_list)
        };
        
        SExpr desc(top_list);
        
        //printf("%s\n", desc.print().c_str());
        
        NBLog(BI, desc, data);
    }
}
