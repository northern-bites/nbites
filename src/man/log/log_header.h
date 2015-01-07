//
//  log_header.h
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
#include <stdarg.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <err.h>
#include <errno.h>
#include <pthread.h>

#include <assert.h>

#include "log_sf.h"

namespace nblog {
    
    //colored output macros.
#define NBlog_DB_color "\x1B[36m"
#define NBlog_DB_cnrm  "\x1B[0m"
#define NBlog_DB_asf   "\x1B[31m" //assertion failed
    
    //NBlog debug level.  undef to (almost) never hear from log code,
    //set higher to hear A LOT from log code
#define NBlog_DB 5
    
//Logging code has no guarantee assertions will be on... want some form of yelp if something goes wrong
#ifdef NBlog_DB
#define NBLassert(val) {if(!(val)) {printf(NBlog_DB_asf \
"*************NBLassert FAILED:[%s][%i] (%s)\n\n" NBlog_DB_cnrm, \
    __FILE__, __LINE__, #val); assert((val));}}
#else
#define NBLassert(val)
#endif
    
#ifdef NBlog_DB
    static inline void NBLdebug_printf(unsigned int lev, const char * format, ...) {
        
        va_list arguments;
        va_start(arguments, format);
        
       
        if (NBlog_DB >= lev) {
            printf(NBlog_DB_color "[NBlog]: ");
            vprintf(format, arguments);
            printf(NBlog_DB_cnrm "");
        }
        
        va_end(arguments);
    }
#endif
    
#ifdef NBlog_DB
#define LOGDEBUG(lev, ...) NBLdebug_printf(lev, __VA_ARGS__)
#else
#define LOGDEBUG(lev, ...) 
#endif
    
    /*
     Data is given to the logging system via the thread-safe NBlog function.
     
     Each call COPIES all relevant data (string, bytes, etc), using mutexes to modify the log data ring buffers.
     
     Since the logging system keeps its own copies of all data, memory usage is an issue.
     Larger logs are written to a separate, smaller, buffer to reduce footprint and give priority to smaller logs.
     
     The log_main immediately overwrites old objects as the buffer fills up.  If no io system is currently using the object (references == 0), it is immediately free'd.  If an io system is using the object, log_main removes it from the ring buffer.  It is now up to the last io system to stop using the object to free it.
     */
    
#define NUM_IO_TYPES 2
    typedef enum {SERVER = 0, FILE = 1, NONE = -1} IO_TAG;
    
    /*
     A logging object/structure.
     Data referenced in object must be owned by the logging system.
     
     type refers to a string of key/value pairs (e.g., type=YUVImage)
        See log file formatting.
     */
    
    typedef struct _log_object_s {
        size_t image_index;     //associated image id
        clock_t creation_time;
        const char * type;      //variable length string encoding data specifics.
        
        size_t n_bytes;
        uint8_t * data;
        
        //For memory management.
        uint8_t references;
        
        //log objects know what buffer they're on (makes free/release easier)
        int buffer;
        
        //For completeness tests.
        uint8_t was_written;
    } log_object_t;
    
#define MAX_LOG_DESC_SIZE 1024
    
    //Lock is for any modification of the buffer or its stored logs.
    typedef struct _log_buffer_s {
        uint32_t fileio_nextr;
        uint32_t servio_nextr;
        uint32_t next_write;
        
        pthread_mutex_t lock;
        log_object_t * objects[];
    } log_buffer_t;
    
#define NUM_LOG_BUFFERS 2
    //Not final... but used for now to make code clearer.
#define NBL_IMAGE_BUFFER 0
#define NBL_SMALL_BUFFER 1
    
    //declared in log_main.cpp
    const extern int LOG_BUFFER_SIZES[NUM_LOG_BUFFERS];
    extern int LOG_RATIO[NUM_LOG_BUFFERS];
    
    /*
     Logging stats functions and structures
     */
    typedef struct {
        uint32_t servnr;
        uint32_t filenr;
        
        uint32_t nextw;
    } bufmanage_t;
    
    typedef struct {
        uint32_t l_given;
        uint64_t b_given;
        
        uint32_t l_freed;
        uint32_t l_lost;
        uint64_t b_lost;
        
        uint32_t l_writ;
        uint64_t b_writ;
    } bufstate_t;
    
    typedef struct {
        //set by flags (or log initiation)
        uint64_t fio_upstart;
        uint64_t sio_upstart;
        
        //set by connection
        uint64_t cio_upstart;
        uint64_t cnc_upstart;
        
        uint64_t start;
    } time_stats_t;
    
    typedef struct {
        //Set when fileio flag set.
        //char smagic1[4];
        bufstate_t fio_start[NUM_LOG_BUFFERS];
        //Set when connection established
        bufstate_t cio_start[NUM_LOG_BUFFERS];
        
        //acquire, release, nblog
        bufstate_t current[NUM_LOG_BUFFERS];
        
        //Copied right before stats sent.
        bufmanage_t manage[NUM_LOG_BUFFERS];
        
        //char smagic2[5];
        
        //Upstart set on flag or connection.
        time_stats_t ts;
        
        //Copied right before stats sent.
        uint32_t ratio[NUM_LOG_BUFFERS];
        
        //Constants
        uint32_t size[NUM_LOG_BUFFERS];
        uint32_t num_cores;
        //then flags.
    } log_stats_t;
    
    /*
     flags for the logging system.
     
     Generally, non-mutex written in one place,
        read in multiple places.
     */
    
    typedef struct {
        //char fmagic1[5];
        uint8_t serv_connected;
        uint8_t cnc_connected;
        
        //log to
        uint8_t fileio;
        uint8_t servio;
        
        //what to log
        uint8_t STATS;
        
        //SPECIFIC modules
        uint8_t SENSORS;
        uint8_t GUARDIAN;
        uint8_t COMM;
        uint8_t LOCATION;
        uint8_t ODOMETRY;
        uint8_t OBSERVATIONS;
        uint8_t LOCALIZATION;
        uint8_t BALLTRACK;
        uint8_t IMAGES;
        uint8_t VISION;
        //char fmagic2[5];
    } log_flags_t;
    
    /*
     The central logging structure.
     
     Logging io threads should lock a buffer, take a reference and mark it as being written (increment ref count), then unlock.
     */
    typedef struct _log_main_s {
        log_buffer_t * buffers[NUM_LOG_BUFFERS];
        
        pthread_t * log_main_thread;
        pthread_t * log_serverio_thread;
        pthread_t * log_fileio_thread;
        pthread_t * log_cnc_thread;
    } log_main_t;
    
    //global reference to the (singleton) log process object
    //declared in log_main.cpp
    extern log_main_t * log_main;
    extern log_stats_t * log_stats;
    extern log_flags_t * log_flags;
    
#define LOG_VERSION 3
    
#define LOG_PORT (32000)
#define CNC_PORT (32001)

#define USLEEP_EXPECTING (1000)
#define IO_SEC_TO_BREAK (5.0)
    
#define SERVER_USLEEP_WAITING (100000)
    
#define FILE_USLEEP_ON_NTD (100000)
    
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
    void NBlog(int buffer_index, size_t image_index, clock_t creation_time, const char * type, size_t n_bytes, uint8_t * data);
    
    //init log_main thread
    void log_main_init();
    
    
    /**
     logging lib common functions.
     */
    
    log_object_t * acquire(int buffer_index, uint32_t * relevant_last_read);
    void release(log_object_t * obj, bool lock);
    
    /*
     Netio could use send/recv, but a.t.m. we're not using flags so this approach allows file and net io to use the same functions.
     */
    int write_exactly(int sck_or_fd, size_t nbytes, void * data);
    
    /*
     time() has granularity of 1 second, so while max_wait
     is a float, fractions are irrelevant.
     */
    int read_exactly(int sck_or_fd, size_t nbytes, void * buffer, double max_wait);
    
    /*
     Write log (using write_exactly) to sock_or_fd.
     
     0 on success, + on failure
     */
    int write_log(int sock_or_fd, log_object_t * log);
    
    //Generates a string w/ generic type specs encoded.
    int description(char * buf, size_t size, log_object_t * obj);
    
}//namespace NBlog

#endif
