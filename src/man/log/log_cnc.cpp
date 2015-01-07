//
//  log_cnc.c
//  NB_log_server
//
//  Created by Philip Koch on 12/26/14.
//

#include "log_header.h"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <limits.h>
#include <signal.h>
#include <time.h>

#include <map>
#include <string>
#include <vector>
#include <sstream>

/*
 cnc functions follow prototype:
    uint32_t (*)(std::string, size_t, uint8_t *)
 
 the return value is transmitted to the calling client.
 the arguments are the size of the data sent, and a pointer to it.
 
 Note: cnc functions should not free the data, it is freed after the function returns (and so if it needs to be kept around, it must be copied).
 Note2: cnc functions are called in a different thread than any diagrams.  Proper precautions must be taken.
 
 ---------------------------------
 
 actual network transmissions follow the following protocol:
 
 in intervals of SERVER_USLEEP_WAITING:
    server pings, reads response.
        if the client sends 0 (ping) nothing happens
        if the client sends 1, the server reads a command (same format as a log, but first key in description must be cmnd=<name_of_command>)

 */

namespace nblog {
    
    uint32_t cnc_test(std::string s, size_t u, uint8_t * p) {
        printf("\ttest:[%s] %lu %lu %p\n", s.data(), s.size(), u, p);
        return 0;
    }
    
    //Notably DOES NOT include the serv_connected flag.
    uint8_t * flag_ptrs[] = {
        &(log_flags->fileio),
        &(log_flags->servio),
        
        &(log_flags->STATS),
        
        &(log_flags->SENSORS),
        &(log_flags->GUARDIAN),
        &(log_flags->COMM),
        &(log_flags->LOCATION),
        &(log_flags->ODOMETRY),
        &(log_flags->OBSERVATIONS),
        &(log_flags->LOCALIZATION),
        &(log_flags->BALLTRACK),
        &(log_flags->IMAGES),
        &(log_flags->VISION)
    };
    //expects two bytes of data:
    //flag index
    //new flag value
    uint32_t cnc_setFlag(std::string s, size_t u, uint8_t * p) {
        if (u != 2) return 1;
        if (p[0] > 12) return 1;
        
        switch (p[0]) {
            case 0: //setting fileio
                if (p[1]) {
                    memcpy(log_stats->fio_start, log_stats->current, sizeof(bufstate_t) * NUM_LOG_BUFFERS);
                    log_stats->ts.fio_upstart = time(NULL);
                }
                
                log_flags->fileio = p[1];
                break;
            case 1: //setting servio
                if (p[1]) {
                    log_stats->ts.sio_upstart = time(NULL);
                }
                
                log_flags->servio = p[1];
                break;
                
            default:
                uint8_t * fp = flag_ptrs[p[0]];
                *fp = p[1];
                break;
        }
        
        return 0;
    }
    
    std::map<std::string, uint32_t (*)(std::string, size_t, uint8_t *)> init_fmap() {
        std::map<std::string, uint32_t (*)(std::string, size_t, uint8_t *)> ret;
        
        ret["test"] = &cnc_test;
        ret["setFlag"] = &cnc_setFlag;
        
        return ret;
    }
    std::map<std::string, uint32_t (*)(std::string, size_t, uint8_t *)> fmap = init_fmap();

    void * cnc_loop(void * cntxt);
    
    void log_cnc_init() {
        LOGDEBUG(1, "log_cnc_init() with %i functions.\n", fmap.size());
        log_main->log_cnc_thread = (pthread_t *) malloc(sizeof(pthread_t));
        
        pthread_create(log_main->log_cnc_thread, NULL, &cnc_loop, NULL);
    }
    
    std::vector<std::string> split(const std::string &s, char delim) {
        std::vector<std::string> elems;
        std::stringstream ss(s);
        std::string item;
        
        while (std::getline(ss, item, delim)) {
            if (!item.empty())
                elems.push_back(item);
        }
        
        return elems;
    }
    
    //defined in log_servio.cpp
    int block_accept(int lfd);
    
#define CHECK_RET(r) {if (r) goto connection_died;}

    void * cnc_loop(void * cntxt) {
        int listenfd = -1, connfd = -1;
        struct sockaddr_in serv_addr;
        
        //Network socket, TCP streaming protocol, default options
        listenfd = socket(AF_INET, SOCK_STREAM, 0);
        LOGDEBUG(1, "log_cnc listenfd=[%i]\n", listenfd);
        //memset(&serv_addr, '0', sizeof(serv_addr));
        memset(&serv_addr, 0, sizeof(serv_addr));
        
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port = htons(CNC_PORT);
        
        bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        
        //We don't want to crash the server if the pipe dies...
        //signal(SIGPIPE, SIG_IGN);
        
        //Accepting connections on this socket, backqueue of size 1.
        listen(listenfd, 1);
        
        LOGDEBUG(2, "log_cnc listening...\n");
        
        for (;;) {
            connfd = block_accept(listenfd);
            LOGDEBUG(3, "log_cnc FOUND CLIENT [%i]\n", connfd);
            log_stats->ts.cnc_upstart = time(NULL);
            log_flags->cnc_connected = 1;
            
            for (;; usleep(SERVER_USLEEP_WAITING)) {
                uint32_t ping = 0;
                uint32_t resp;
                
                CHECK_RET(write_exactly(connfd, 4, (uint8_t *) &ping) )
                CHECK_RET(read_exactly(connfd, 4, (uint8_t *) &resp, IO_SEC_TO_BREAK))
                
                if (ntohl(resp) == 1) {
                    //cnc call coming in:
                    CHECK_RET(read_exactly(connfd, 4, (uint8_t *) &resp, IO_SEC_TO_BREAK))
                    size_t desc_len = ntohl(resp);
                    char cbuf[desc_len];
                    CHECK_RET(read_exactly(connfd, desc_len, (uint8_t *) cbuf, IO_SEC_TO_BREAK))
                    
                    CHECK_RET(read_exactly(connfd, 4, (uint8_t *) &resp, IO_SEC_TO_BREAK))
                    size_t data_len = ntohl(resp);
                    uint8_t dbuf[data_len];
                    
                    uint8_t * dptr = (data_len) ? dbuf : NULL;
                    
                    //Parse description
                    std::string desc(cbuf, desc_len);
                    std::vector<std::string> kvps = split(desc, ' ');
                    if (kvps.size() == 0) {
                        LOGDEBUG(1, "log_cnc got bad desc string: %s\n", desc.data());
                        goto connection_died;
                    }
                    
                    std::vector<std::string> cmnd = split(kvps[0], '=');
                    if (cmnd.size() != 2) {
                        LOGDEBUG(1, "log_cnc got bad desc string: %s\n", desc.data());
                        goto connection_died;
                    }
                    
                    if (cmnd[0].compare("cmnd") != 0) {
                        LOGDEBUG(1, "log_cnc got bad desc string: %s\n", desc.data());
                        goto connection_died;
                    }
                    
                    if (cmnd[1].empty()) {
                        LOGDEBUG(1, "log_cnc got bad desc string: %s\n", desc.data());
                        goto connection_died;
                    }
                    
                    uint32_t ret = htonl(fmap[cmnd[1]](desc, data_len, dptr));
                    
                    CHECK_RET(write_exactly(connfd, 4, &ret))
                    
                    LOGDEBUG(1, "log_cnc called %s with data_len:%lu\n", cmnd[1].data(), data_len);
                } else {
                    if (resp != 0) {
                        LOGDEBUG(1, "log_cnc got bad ping reply! %x\n", resp);
                        goto connection_died;
                    }
                }
            }
            
            //Acts like an exception.
        connection_died:
            log_flags->cnc_connected = 0;
            close(connfd);
            LOGDEBUG(1, "log_cnc loop broken, connection closed.\n");
        }
        
        return NULL;
    }
}