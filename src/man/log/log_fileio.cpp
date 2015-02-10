//
//  log_fileio.c
//  NB_log_server
//
//  Created by Philip Koch on 10/4/14.
//

#include "log_header.h"
#include "log_sf.h"
#include <string>
#include <algorithm>
#include <netinet/in.h>


namespace nblog {
    
    static const char * LOG_FOLDER = "/home/nao/nbites/log/";
    //const char * LOG_FOLDER = "/Users/pkoch/Desktop/LOGS/";
    
    void * file_io_loop(void * context);
    
    void log_fileio_init() {
        LOGDEBUG(1, "log_fileio_init()\n");
        
        pthread_create(&(log_main->log_fileio_thread), NULL, &file_io_loop, NULL);
        pthread_detach(log_main->log_fileio_thread);
    }
    
    int write_to_fs(log_object_t * obj);
    
    void * file_io_loop(void * context) {
        uint8_t wrote_something = 0;
        log_object_t * obj = NULL;
        
        for (;;
             (wrote_something) ? (wrote_something = 0) : usleep(FILE_USLEEP_ON_NTD)
             )
        {
            if (!(nbsf::flags[nbsf::fileio])) {continue;}
            
            for (int i = 0; i < NUM_LOG_BUFFERS; ++i) {
                
                for (int r = 0; r < LOG_RATIO[i]; ++r) {
                    obj = acquire(i, &(log_main->buffers[i]->fileio_nextr));
                    
                    if (obj) {
                        wrote_something = 1;
                        write_to_fs(obj);
                        
                        release(obj, true);
                    } else {
                        break;
                    }
                }
            }
        }
        
        return NULL;
    }
    
    /*
     fileio
     */
    
    int write_to_fs(log_object_t * obj) {
        int fd;
        char buf[MAX_LOG_DESC_SIZE];
        
        int nw = strlen(obj->log.desc);
        
        char * ss;
        int len;
        
        //240 is ~ what we can put in a file name
        if (nw < 240) {
            len = nw;
            ss = buf;
        } else {
            len = 240;
            ss = buf + (nw - 240);
        }
        
        std::string cpp_s(ss);
        std::replace(cpp_s.begin(), cpp_s.end(), ' ', '_');
        cpp_s.append(".nblog");
        std::string path(LOG_FOLDER);
        path.append(cpp_s);
        
        fd = open(path.c_str(), O_WRONLY | O_TRUNC | O_CREAT, S_IRWXG | S_IRWXU);
        
        if (fd < 0) {
            printf("*************NB_Log file_io COULD NOT OPEN LOG FILE \n\t%s\n\n", path.c_str());
            
            return 1;
        }
        
        if (logio::write_log(fd, &(obj->log))) {
            printf("*************NB_Log file_io COULD NOT WRITE LOG \n\t%s\n\n", path.c_str());
            
            return 2;
        }
        
        return 0;
    }
    
}
