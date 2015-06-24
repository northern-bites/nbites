//
//  log_fileio.c
//  NB_log_server
//
//  Created by Philip Koch on 10/4/14.
//

#include "logging.h"
#include "nbdebug.h"
#include "../control/control.h"

#include <string>
#include <algorithm>
#include <sstream>
#include <netinet/in.h>

namespace nblog {
    
    const char * LOG_FOLDER = "/home/nao/nbites/log/";
    //const char * LOG_FOLDER = "/Users/pkoch/Desktop/LOGS/";
    bool STARTED = false;
    
    //TODO need more experimenting for optimal size.
    const uint64_t MAX_WRITTEN_BYTES = 5 * (1<<25);
    uint64_t total_written = 0;
    
    void * file_io_loop(void * context);
    
    void log_fileio_init() {
        NBLassert(!STARTED);
        STARTED = true;
        
        NBDEBUG("log_fileio_init()\n");
        
        pthread_create(&(log_main.log_fileio_thread), NULL, &file_io_loop, NULL);
        pthread_detach(log_main.log_fileio_thread);
    }
    
    int write_to_fs(Log * obj);
    
    void * file_io_loop(void * context) {
        uint8_t wrote_something = 0;
        Log * obj = NULL;
        int cur_bi;
        
        for (;;
             (wrote_something) ? (wrote_something = 0) : usleep(FILEIO_USLEEP_ON_NTD)
             )
        {
            if (!(control::flags[control::fileio])) {continue;}
            
            for (cur_bi = 0; cur_bi < NUM_LOG_BUFFERS; ++cur_bi) {
                
                for (int r = 0; r < LOG_RATIO[cur_bi]; ++r) {
                    
                    if (total_written > MAX_WRITTEN_BYTES) {
                        printf("\n\n\n**********************\nWARNING:\n"
                               "\tfileio SHUTTING DOWN AFTER WRITING %llu BYTES"
                               "\n**********************\n\n\n",
                               total_written);
                        return NULL;
                    }
                    
                    obj = acquire(cur_bi, &(log_main.buffers[cur_bi].fileio_nextr));
                    
                    if (obj) {
                        wrote_something = 1;
                        total_written += obj->fullSize();
                        if (write_to_fs(obj) > 0) {
                            printf("**********\nERROR!\n**********\n"
                                   "could not write log to file!\n\n");
                            releaseWrapper(cur_bi, obj, true);
                            return NULL;
                        } else {
                            releaseWrapper(cur_bi, obj, true);
                        }
                        
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
    
    int write_index = 0;
    int write_to_fs(Log * obj) {
        int fd;
        
        std::ostringstream ss;
        
        std::string cr_loc = obj->tree().find(LOG_CREATED_S)->get(1)->value();
        std::string cr_date = obj->tree().find(LOG_CREATED_S)->get(2)->value();
        std::string type = obj->tree().find(LOG_CONTENTS_S)->get(1)->find(CONTENT_TYPE_S)->get(1)->value();
        
        ss << cr_loc << "_" << cr_date << "_" << type << "_i"
            << write_index++ << ".nblog";
        
        /*
        std::string desc = obj->description();
        desc = desc.substr(0, 100);
        std::replace(desc.begin(), desc.end(), ' ', '_');
        desc.append(".nblog"); */
        
        std::string path(LOG_FOLDER);
        std::string name = ss.str();
        std::replace(name.begin(), name.end(), ' ', '_');
        std::replace(name.begin(), name.end(), '/', '-');
        std::replace(name.begin(), name.end(), ':', '-');
        path.append(name);
        
        fd = open(path.c_str(), O_WRONLY | O_TRUNC | O_CREAT, S_IRWXG | S_IRWXU);
        
        if (fd < 0) {
            printf("*************NB_Log file_io COULD NOT OPEN LOG FILE \n\t%s\n\n", path.c_str());
            
            return 1;
        }
        
        if (!obj->write(fd)) {
            printf("*************NB_Log file_io COULD NOT WRITE LOG \n\t%s\n\n", path.c_str());
            
            close(fd);
            return 2;
        }
        
        close(fd);
        return 0;
    }
    
}
