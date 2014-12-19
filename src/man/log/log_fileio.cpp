//
//  log_fileio.c
//  NB_log_server
//
//  Created by Philip Koch on 10/4/14.
//

#include "log_header.h"
#include <string>
#include <algorithm>
#include <netinet/in.h>


namespace nblog {
    
    
    const char * LOG_FOLDER = "/home/nao/nbites/log/";
    //const char * LOG_FOLDER = "/Users/pkoch/Desktop/LOGS/";
    
    int fileio_last_read[NUM_LOG_BUFFERS];
    void * file_io_loop(void * context);
    
    void log_fileio_init() {
        LOGDEBUG(1, "log_fileio_init()\n");
        log_process->log_fileio_thread = (pthread_t *) malloc(sizeof(pthread_t));
        
        //First accessed index with be 0.
        for (int i = 0; i < NUM_LOG_BUFFERS; ++i) {
            fileio_last_read[i] = -1;
        }
        
        //Do we want to be writing data to disk on the robot?
#ifdef FILE_LOGGING
        pthread_create(log_process->log_fileio_thread, NULL, &file_io_loop, NULL);
#endif
    }
    
    void * file_io_loop(void * context) {
        while (1) {
            uint8_t wrote_something = 0;
            log_object_t * obj = NULL;
            
            //Log one from each buffer.
            for (int i = 0; i < NUM_LOG_BUFFERS; ++i) {
                //log_buffer_t * buf = log_process->buffers[i];
                obj = get_log(i, &(fileio_last_read[i]));
                if (obj) {
                    wrote_something = 1;
                    
                    char * f_name = generate_type_specs(obj);
                    std::string cps(f_name);
                    std::replace(cps.begin(), cps.end(), ' ', '_');
                    int spos = cps.length() - 240;
                    if (spos < 0) spos = 0;
                    cps = cps.substr(spos, std::string::npos);
                    cps.append(".nblog");
                    size_t path_len = cps.length() + strlen(LOG_FOLDER) + 1;
                    char * f_path = (char *) malloc(path_len);
                    
                    snprintf(f_path, path_len, "%s%s", LOG_FOLDER, cps.c_str());
                    f_path[path_len - 1] = '\0';
                    
                    int fd = open(f_path, O_WRONLY | O_TRUNC | O_CREAT, S_IRWXG | S_IRWXU);
                    
                    if (fd < 0) {
                        printf("*************NB_Log file_io COULD NOT OPEN LOG FILE \n\t%s\n\n", f_path);
                        log_object_release(obj);
                        continue;
                    }
                    
                    uint32_t hord = strlen(f_name);
                    uint32_t nlen = htonl(hord);
                    write_exactly(fd, 4, &nlen);
                    write_exactly(fd, hord, f_name);
                    write_exactly(fd, obj->n_bytes, obj->data);
                    close(fd);
                    
                    LOGDEBUG(6, "log_fileio WROTE SOMETHING! (%s)\n", f_path);
                    free(f_name);
                    free(f_path);
                    
                    //pthread_mutex_lock(&(buf->lock));
                    log_object_release(obj);
                    //pthread_mutex_unlock(&(buf->lock));
                }
            }
            
            //No buffers had available data, sleep
            if (!wrote_something){
                LOGDEBUG(11, "log_fileio sleeping on NTD...\n");
                usleep(FILE_USLEEP_ON_NTD);
                //Don't want to be grinding if there's nothing to write
               
            } else {
                //we wrote something! woo
            }
        }
        
        return NULL;
    }
    
}
