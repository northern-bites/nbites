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

/*
#ifndef __STDCPP_THREADS__
#error NB_log_server REQUIRES threads.  Do not compile/link into a non-threading environment!
#endif
 */

namespace nblog {
    
    //colored output macros.
#define NBlog_DB_color "\x1B[36m"
#define NBlog_DB_cnrm  "\x1B[0m"
#define NBlog_DB_asf   "\x1B[31m" //assertion failed
    
    //NBlog debug level.  undef to never hear from log code,
    //set higher to hear A LOT from log code
#define NBlog_DB 5
    
//We have NO guarantee assertions will be on... want some form of yelp if something goes wrong
    
    
#ifdef NBlog_DB
#define NBLassert(val) {if(!(val)) {printf(NBlog_DB_asf \
"*************NBLassert FAILED:[%s][%i] (%s)\n\n" NBlog_DB_cnrm, \
    __FILE__, __LINE__, #val); assert((val));}}
#else
#define NBLassert(val)
#endif
    
#ifdef NBlog_DB
    static inline void debug_printf(unsigned int lev, const char * format, ...) {
        
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
#define LOGDEBUG(lev, ...) debug_printf(lev, __VA_ARGS__)
#else
#define LOGDEBUG(lev, ...) 
#endif
    
    /*
     Data is given to the log_process thread via the NBlog function.
     
     Each call COPIES all relevant data, using mutexes to modify the log data ring buffers.
     
     Since the logging system keeps its own copies of all data, memory usage is an issue.  The log_process
     keeps a limited number of each type of log data, low
     density and high density.  Low-density refers to large objects such as images.  High density refers to
     messages with high information density, such as proto-buffs.
     
     generally, the larger a log object the higher index buffer it should be placed in.  Right now,
     
     index 0 = high density
     index 1 = low density
     
     The log_process immediately overwrites old objects as the buffer fills up.  If no io system is currently using the object (references == 0), it is immediately free'd.  If an io system is using the object, log_process removes it from the ring buffer.  It is now up to the last io system to stop using the object to free it.
     */
    
    /*
     NEEDS WORK:
     
     Should have all threads use attributes on creation, to make it easier to change priorities.  We might want higher priorities (trying to get as much data logged as possible) or very low priorities (not interfereing with host system).
     
     Better system for sending object attributes?
     */
    
    /*
     A logging object/structure.  log-owned data waiting to be thoroughly logged and then free'd.
     */
    
    /*
     <type> specifications
     
     required specs
     fpl: file prefix length.  Since file names are a limited size, if we one day need long type strings we'll add the first fpl bytes of the file to the name to determine file specifications.  So ALL LOG FILES MUST INCLUDE THIS SPEC.  Just set it to 0 for now since we aren't using all of the max file name length.
     type: type (image, protobuf type, stdout)
     index: assocaited image index
     time: creation time
     
     optionals...
     from: from (camera top, localization)
     
     image specs
     width:
     height:
     
     
     name: 
        human readable name
     */
    
    typedef struct _log_object_s {
        size_t image_index;     //associated image id
        //time_t creation_time;   //time(NULL) when object created
        clock_t creation_time;
        const char * type;      //variable length string encoding data specifics.
        
        size_t n_bytes;
        uint8_t * data;
        
        //For memory management.
        uint8_t references;
        
        //log objects know what buffer they're on (makes free/release easier)
        int buffer;
        
        //For completeness tests.  Not accessed in a thread safe manner, so only check for 0 or 1.
        uint8_t was_written;
    } log_object_t;
    
    //lock is for modifying the buffer, OR modifying (i.e. deleting/releasing) an object.
    typedef struct _log_buffer_s {
        uint32_t last_written;
        pthread_mutex_t lock;
        log_object_t * objects[];
    } log_buffer_t;
    
#define NUM_LOG_BUFFERS 2
    
    //declared in log_process.cpp
    const extern int LOG_BUFFER_SIZES[NUM_LOG_BUFFERS];
    
    /*
     The central log_process thread structure.
     
     Logging io threads should lock a buffer, take a reference and mark it as being written (increment ref count), then unlock.
     */
    
    typedef struct _log_process_s {
        //Not to be modified except by serverio thread.
#define SERVER_CONNECTED 0x01
        uint8_t flags;
        
        clock_t creation_time;
        
        log_buffer_t * buffers[NUM_LOG_BUFFERS];
        
        pthread_t * log_process_thread;
        pthread_t * log_serverio_thread;
        pthread_t * log_fileio_thread;
    } log_process_t;
    
    //global reference to the (singleton) log process object
    //declared in log_process.cpp
    extern log_process_t * log_process;
    
#define LOG_PORT 32000

#define SERVER_USLEEP_EXPECTING 1000
#define SERVER_USLEEP_WAITING 100000
    
#define SERVER_SECONDS_TILL_BREAK 5.0
    
    //If this is not defined, nothing will be written to disk.
    //If this is defined, EVERYTHING will be written to disk – barring io time problems.
//#define FILE_LOGGING
#define FILE_USLEEP_ON_NTD (100000)
    
    /**********************************************************
     This is the ONLY thing code outside of this log library should call.
     
     Thread safe.
     
     buffer_index: which buffer to put data on (buffers might have different sizes to restrict log system memory usage)
     image_index: image that this log's encapsulated information was derived from.
        0 means n/a or unknown.
     
     creation_time: relative to nao process start time.  Fairly good at making all logs from a given log session distinct.
     
     type: specs about this log data.  Pretty important.  Fill 'er in.
     
     nbytes/data: encapsulated opaque log data.
     */
    void NBlog(int buffer_index, size_t image_index, clock_t creation_time, const char * type, size_t n_bytes, uint8_t * data);
    
    //Thread safe (uses obj->buffer_index's lock.
    void log_object_release(log_object_t * obj);
    
    //Only call this directly in rare occaisons.  Not thread safe.  Just call release outside the context of a lock. (because release locks)
    void log_object_free(log_object_t * obj);
    
    //constructor
    log_object_t * log_object_create(size_t image_index, clock_t creation_time, const char * type, size_t bytes, uint8_t * data);
    
    //init log_process thread
    void log_process_init();
    
    //get next log from buffer[buffer_index].  Update last_read.
    //Returns null if no logs available.
    log_object_t * get_log(int buffer_index, int * last_read);
    
    static inline void write_exactly(int fd, size_t nbytes, void * data) {
        size_t written = 0;
        while (written < nbytes) {
            written += write(fd, ((uint8_t *) data) + written, nbytes - written);
        }
    }
    
    //Descriptions can be longer than 255... but right now we truncate them for file sizes.
    
    //Maybe need to change
    static inline char * generate_type_specs(log_object_t * obj) {
        int32_t checksum = 0;
        for (int i = 0; i < obj->n_bytes; ++i)
            checksum += obj->data[i];
        char * f_path = (char *) malloc(1024);
        int n_written = snprintf(f_path, 1024, "type=%s checksum=%i index=%li time=%lu", obj->type, checksum, obj->image_index, obj->creation_time);
        NBLassert(n_written < 1024);
        LOGDEBUG(8, "generate_type_specs() [%i] %s\n", n_written, f_path);
        return f_path;
    }
    
}//namespace NBlog

#endif
