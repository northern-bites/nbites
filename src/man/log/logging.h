//
//  logging.h
//  NB_log_server
//
//  Created by Philip Koch on 10/4/14.
//

#ifndef NB_log_server_log_header_h
#define NB_log_server_log_header_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <err.h>
#include <errno.h>
#include <pthread.h>

#include <assert.h>

#include "Log.h"

namespace nblog {
    
    /*
     Data is given to the logging system via the thread-safe NBlog function.
     
     Each call COPIES all relevant data (string, bytes, etc), using mutexes to modify the log data ring buffers.
     
     Since the logging system keeps its own copies of all data, memory usage is an issue.
     Larger logs are written to a separate, smaller, buffer to reduce footprint and give priority to smaller logs.
     */
    
    //Lock is for any modification of the buffer or its stored logs.
    typedef struct _log_buffer_s {
        uint32_t fileio_nextr;
        uint32_t servio_nextr;
        uint32_t next_write;
        
        pthread_mutex_t lock;
        Log ** objects;
    } log_buffer_t;
    
#define NUM_LOG_BUFFERS     2
    //Not final... but used for now to make code clearer.
#define NBL_IMAGE_BUFFER    0
#define NBL_SMALL_BUFFER    1
    
    //declared in log_main.cpp
    const extern int LOG_BUFFER_SIZES[NUM_LOG_BUFFERS];
    extern int LOG_RATIO[NUM_LOG_BUFFERS];
    
    /*
     The central logging structure.
     
     Logging io threads should lock a buffer, take a reference and mark it as being written (increment ref count), then unlock.
     */
    typedef struct _log_main_s {
        log_buffer_t buffers[NUM_LOG_BUFFERS];
        
        //The logging code doesn't use these IDs at the moment – but they do contain the valid IDs of
        //the logging threads if other code needs them.
        pthread_t log_main_thread;
        pthread_t log_serverio_thread;
        pthread_t log_fileio_thread;
    } log_main_t;
    
    //global reference to the (singleton) log process object
    //declared in log_main.cpp
    extern log_main_t log_main;
    
#define LOG_VERSION 6
    
#define STREAM_PORT (30000)
    
#define STREAM_USLEEP_WAITING (100000)
    
#define FILEIO_USLEEP_ON_NTD (100000)
    
    /**********************************************************
     This is the ONLY thing code outside of this log library should call.
     
     Thread safe.
     
     buffer_index: which buffer to put data on
     image_index: image that this log's encapsulated information was derived from.
        0 means n/a or unknown.
     
     creation_time: relative to nao process start time.  Fairly good at making all logs from a given log session distinct.
     
     type: specs about this log data.  Pretty important.  Fill 'er in.
     
     nbytes/data: encapsulated opaque log data.
     */
    
    void NBLog(int buffer_index, Log * log);
    void NBLog(int buffer_index, const std::string& where_called,
               const std::vector<SExpr>& items, const std::string& data );
    
    //Same as above, but only copies buffer once.
    void NBLog(int buffer_index, const std::string& where_called,
               const std::vector<SExpr>& items, const void * buffer, size_t nbytes );
    
    
    //init log_main thread
    void log_main_init();
    
    //for io threads.
    Log * acquire(int bi, uint32_t * relevant_nextr);
    void releaseWrapper(int bi, Log * lg, bool lock);
    
    /*
     STATISTICS
     */
    
    typedef struct {
        uint64_t b_given;
        uint64_t b_lost;
        uint64_t b_writ;
        
        uint32_t l_given;
        
        uint32_t l_freed;
        uint32_t l_lost;
        
        uint32_t l_writ;
    } io_state_t;
    
    extern io_state_t fio_start[NUM_LOG_BUFFERS];
    extern io_state_t cio_start[NUM_LOG_BUFFERS];
    extern io_state_t total[NUM_LOG_BUFFERS];
    
    extern uint64_t fio_upstart;
    extern uint64_t sio_upstart;
    
    extern uint64_t cio_upstart;
    extern uint64_t cnc_upstart;
    
    extern uint64_t main_upstart;
    
    const extern uint32_t NUM_CORES;
    
}//namespace NBlog

#endif
