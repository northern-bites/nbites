//
//  log_main.c
//  NB_log_server
//
//  Created by Philip Koch on 10/4/14.
//

#include "logging.h"
#include "../control/control.h"
#include "nbdebug.h"

#include "../../share/include/Camera.h"

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

namespace nblog {
    
    uint64_t fio_upstart;
    
    uint64_t cio_upstart;
    uint64_t cnc_upstart;
    
    uint64_t main_upstart;
    
    io_state_t fio_start[NUM_LOG_BUFFERS];
    io_state_t cio_start[NUM_LOG_BUFFERS];
    io_state_t total[NUM_LOG_BUFFERS];
    
    int nlog_assoc[NUM_LOG_BUFFERS];
    
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
        NBLassert(!log_running);
        
        //...
        /*
        SExpr aString("thisstring");
        printf("%s\n", aString.serialize().c_str()); */
        
        NBDEBUG("sane.\n");
    }
    
    void log_main_init() {
        NBDEBUG("log_main_init()\n\t"
                "nbuffers=%i version=%i\n", NUM_LOG_BUFFERS, LOG_VERSION);
        
        main_upstart = time(NULL);
        
        bzero(fio_start, sizeof(io_state_t) * NUM_LOG_BUFFERS);
        bzero(cio_start, sizeof(io_state_t) * NUM_LOG_BUFFERS);
        bzero(total, sizeof(io_state_t) * NUM_LOG_BUFFERS);
        
        bzero(nlog_assoc, sizeof(int) * NUM_LOG_BUFFERS);
        
        bzero(&log_main, sizeof(log_main_t));
        
        for (int i = 0; i < NUM_LOG_BUFFERS; ++i) {
            log_main.buffers[i].objects = (Log **) malloc(LOG_BUFFER_SIZES[i] * sizeof(Log *));
            bzero(log_main.buffers[i].objects, LOG_BUFFER_SIZES[i] * sizeof(Log *));
            
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
    
    SExpr makeBufManage(int bi) {
        std::vector<SExpr> vals = {
            SExpr("servio_nextr", (int) log_main.buffers[bi].servio_nextr),
            SExpr("fileio_nextr", (int) log_main.buffers[bi].fileio_nextr),
            SExpr("next_write", (int) log_main.buffers[bi].next_write),
            
            SExpr("nl_assoc", (int) nlog_assoc[bi])
        };
        return SExpr(vals);
    }
    
    SExpr makeBufState(io_state_t * start, io_state_t * end) {
        std::vector<SExpr> vals = {
            SExpr("l_given", (long) (end->l_given - start->l_given)),
            SExpr("b_given", (long) (end->b_given - start->b_given)),
            
            SExpr("l_freed", (long) (end->l_freed - start->l_freed)),
            SExpr("l_lost", (long) (end->l_lost - start->l_lost)),
            SExpr("b_lost", (long) (end->b_lost - start->b_lost)),
            
            SExpr("l_writ", (long) (end->l_writ - start->l_writ)),
            SExpr("b_writ", (long) (end->b_writ - start->b_writ)),
        };
        
        return SExpr(vals);
    }
    
    void * log_main_loop(void * context) {
        
        log_serverio_init();
        log_fileio_init();

        while (1) {
            sleep(1);
            
            //Log state.
            std::vector<SExpr> fields;
            fields.push_back(SExpr(CONTENT_TYPE_S, "STATS"));
            
            std::vector<SExpr> fvector = {
                SExpr("flags"),
                SExpr("serv_connected", control::serv_connected,
                      control::flags[control::serv_connected]),
                SExpr("control_connected", control::control_connected,
                      control::flags[control::control_connected]),
                SExpr("fileio", control::fileio,
                      control::flags[control::fileio]),
                SExpr("SENSORS", control::SENSORS,
                      control::flags[control::SENSORS]),
                SExpr("GUARDIAN", control::GUARDIAN,
                      control::flags[control::GUARDIAN]),
                SExpr("COMM", control::COMM,
                      control::flags[control::COMM]),
                SExpr("LOCATION", control::LOCATION,
                      control::flags[control::LOCATION]),
                SExpr("ODOMETRY", control::ODOMETRY,
                      control::flags[control::ODOMETRY]),
                SExpr("OBSERVATIONS", control::OBSERVATIONS,
                      control::flags[control::OBSERVATIONS]),
                SExpr("LOCALIZATION", control::LOCALIZATION,
                      control::flags[control::LOCALIZATION]),
                SExpr("BALLTRACK", control::BALLTRACK,
                      control::flags[control::BALLTRACK]),
                
                SExpr("VISION", control::VISION,
                      control::flags[control::VISION]),
                
                SExpr("tripoint", control::tripoint,
                      control::flags[control::tripoint]),

                SExpr("thumbnail", control::thumbnail,
                      control::flags[control::thumbnail])
            };
            fields.push_back(SExpr(fvector));
            
            
            fields.push_back(SExpr("num_buffers", NUM_LOG_BUFFERS));
            fields.push_back(SExpr("num_cores", (int) NUM_CORES));
            
            SExpr ratios;
            ratios.append(SExpr("ratio"));
            for (int i = 0; i < NUM_LOG_BUFFERS; ++i) {
                ratios.append(SExpr(LOG_RATIO[i]));
            }
            fields.push_back(ratios);
            
            SExpr sizes;
            sizes.append(SExpr("size"));
            for (int i = 0; i < NUM_LOG_BUFFERS; ++i) {
                sizes.append(SExpr(LOG_BUFFER_SIZES[i]));
            }
            fields.push_back(sizes);
            
            time_t NOW = time(NULL);
            
            fields.push_back(SExpr("con_uptime", control::flags[control::serv_connected] ? difftime(NOW, cio_upstart) : 0));
            fields.push_back(SExpr("cnc_uptime", control::flags[control::control_connected] ? difftime(NOW, cnc_upstart) : 0));
            fields.push_back(SExpr("fio_uptime", control::flags[control::fileio] ? difftime(NOW, fio_upstart) : 0));
            
            fields.push_back(SExpr("log_uptime", difftime(NOW, main_upstart)));
            
            SExpr manages;
            manages.append(SExpr("bufmanage"));
            for (int i = 0; i < NUM_LOG_BUFFERS; ++i) {
                manages.append(makeBufManage(i));
            }
            fields.push_back(manages);
            
            /*
             This system of grabbing io state is subject to multi-threading accuracy drift.
             It is therefore only for estimates.
             */
            io_state_t zerostate;
            bzero(&zerostate, sizeof(io_state_t));
            
            SExpr state_total;
            state_total.append(SExpr("total-state"));
            for (int i = 0; i < NUM_LOG_BUFFERS; ++i) {
                state_total.append(makeBufState(&zerostate, total + i));
            }
            fields.push_back(state_total);
            
            if (control::flags[control::fileio]) {
                SExpr state_file;
                state_file.append(SExpr("file-state"));
                for (int i = 0; i < NUM_LOG_BUFFERS; ++i) {
                    state_file.append(makeBufState(fio_start + i, total + i));
                }
                fields.push_back(state_file);
            }
            
            if (control::flags[control::serv_connected]) {
                SExpr state_serv;
                state_serv.append(SExpr("serv-state"));
                for (int i = 0; i < NUM_LOG_BUFFERS; ++i) {
                    state_serv.append(makeBufState(cio_start + i, total + i));
                }
                fields.push_back(state_serv);
            }
            
            std::vector<SExpr> contents = {SExpr(fields)};
            
            //NBDEBUG("logged state...");
            NBLog(NBL_SMALL_BUFFER, "main_loop", contents, "");
        }
        
        return NULL;
    }
    
    /*
     Definitions for log lib functions.
     */
    
    void releaseWrapper(int bi, Log * lg, bool lock) {
        NBLassert(bi >= 0 && bi < NUM_LOG_BUFFERS);
        NBLassert(lg);
        
        if (lock) pthread_mutex_lock(&(log_main.buffers[bi].lock));
        
        if (lg->release()) {
            
            total[bi].l_freed += 1;
            if (!lg->written()) {
                total[bi].l_lost += 1;
                total[bi].b_lost += lg->fullSize();
            }
            
            --nlog_assoc[bi];
            NBLassert(nlog_assoc[bi] >= 0);
            
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
        
        if (old) {
            releaseWrapper(bi, old, false);
        }
        
        buf.objects[(old_i % LOG_BUFFER_SIZES[bi])] = nstored;
        
        /*If IO is very slow, we need to update IO 'next_*' */
        
        //fileio
        NBLassert(buf.fileio_nextr <= buf.next_write);
        uint32_t dif = buf.next_write - buf.fileio_nextr;
        NBLassert(dif <= LOG_BUFFER_SIZES[bi]);
        if (dif == LOG_BUFFER_SIZES[bi]) {
            ++(buf.fileio_nextr);
        }
        //servio
        NBLassert(buf.servio_nextr <= buf.next_write);
        dif = buf.next_write - buf.servio_nextr;
        NBLassert(dif <= LOG_BUFFER_SIZES[bi]);
        if (dif == LOG_BUFFER_SIZES[bi]) {
            ++(buf.servio_nextr);
        }
        
        ++(buf.next_write);
        
        ++nlog_assoc[bi];
        //1 log could possibly be in the hands of each io.
        NBLassert(nlog_assoc[bi] <= LOG_BUFFER_SIZES[bi] + 2);
        
        NBDEBUGs(SECTION_LOGM, "\t\tfilenr=%i servnr=%i nextw=%i\n", buf.fileio_nextr, buf.servio_nextr, buf.next_write);
    }
    
    void NBLog(int buffer_index, Log * log) {
        NBDEBUGs(SECTION_LOGM, "NBlog(buffer_index=%i)\n", buffer_index);
        NBLassert(buffer_index < NUM_LOG_BUFFERS);
        //Can't log if the server's not running...
        if (!log_running) {
            NBDEBUG("NBlog returning because !log_running\n");
            delete log;
            return;
        }
        
        if (buffer_index >= NUM_LOG_BUFFERS || buffer_index < 0) {
            printf("ERROR: NBlog(...) called with INVALID buffer index!  Deleting log and returning...\n");
            delete log;
            return;
        }
        
        pthread_mutex_lock(&(log_main.buffers[buffer_index].lock));
        put(log, buffer_index);
        pthread_mutex_unlock(&(log_main.buffers[buffer_index].lock));
    }
    
    void NBLog(int buffer_index, const std::string& where_called,
               const std::vector<SExpr>& items, const std::string& data ) {
        if (!log_running) {
            NBDEBUG("NBlog returning because !log_running\n");
            return;
        }
        
        Log * newl = new Log(LOG_FIRST_ATOM_S, where_called, time(NULL), LOG_VERSION,
                             items, data);
        
        
        NBLog(buffer_index, newl);
    }
    
    void NBLog(int buffer_index, const std::string& where_called,
               const std::vector<SExpr>& items, const void * buffer, size_t nbytes ) {
        if (!log_running) {
            NBDEBUG("NBlog returning because !log_running\n");
            return;
        }
        
        Log * newl = new Log(LOG_FIRST_ATOM_S, where_called, time(NULL), LOG_VERSION, items, buffer, nbytes);
        
        NBLog(buffer_index, newl);
    }
}
