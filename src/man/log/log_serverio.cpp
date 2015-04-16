//
//  log_streamio.c
//  NB_log_server
//
//  Created by Philip Koch on 10/4/14.
//

#include "logging.h"
#include "control.h"
#include "nbdebug.h"
#include "exactio.h"

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <limits.h>
#include <signal.h>
#include <time.h>

namespace nblog {
    
    void * server_io_loop(void * context);
    
    void log_serverio_init() {
        NBDEBUG("log_serverio_init()\n");
        
        pthread_create(&(log_main.log_serverio_thread), NULL, &server_io_loop, NULL);
        pthread_detach(log_main.log_serverio_thread);
    }
    
    /*
    //Available bytes on socket fd.
    //deprecated?
    static inline int available(int fd) {
        int a;
        ioctl(fd, FIONREAD, &a);
        return a;
    } */
    
#define CHECK_RET(r) {if (r) goto connection_died;}
//#define CHECK_RET(r) {if (r) {printf("failed at line %d\n", __LINE__); goto connection_died;}}
    
    void * server_io_loop(void * arg) {
        int listenfd = -1, connfd = -1;
        struct sockaddr_in serv_addr;
        
        //Network socket, TCP streaming protocol, default options
        listenfd = socket(AF_INET, SOCK_STREAM, 0);
        NBDEBUG( "log_streamio listenfd=[%i]\n", listenfd);
        //memset(&serv_addr, '0', sizeof(serv_addr));
        memset(&serv_addr, 0, sizeof(serv_addr));
        
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port = htons(LOG_PORT);
        
        bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        
        //We don't want to crash the server if the pipe dies...
        //signal(SIGPIPE, SIG_IGN);
        
        //Accepting connections on this socket, backqueue of size 1.
        listen(listenfd, 1);
        
        NBDEBUG("log_streamio listening...port=%i\n", LOG_PORT);
        control::flags[control::serv_connected] = false;
        
        for (;;) {
            connfd = block_accept(listenfd, STREAM_USLEEP_WAITING);
            log::Log * obj = NULL;
            int obj_bi = -1;
            
            NBDEBUG("log_streamio FOUND CLIENT [%i]\n", connfd);
            memcpy(cio_start, total, sizeof(io_state_t) * NUM_LOG_BUFFERS);
            cio_upstart = time(NULL);
            control::flags[control::serv_connected] = true;
            
            uint32_t version = htonl(LOG_VERSION);
            uint32_t seq_num = 0;
            uint32_t recvd;
            
            CHECK_RET(send_exact(connfd, 4, (uint8_t *) &seq_num))
            
            CHECK_RET(recv_exact(connfd, 4, (uint8_t *) &recvd, IO_SEC_TO_BREAK));
            
            if (recvd != 0) {
                NBDEBUG( "log_streamio got bad ping initiation: %u\n", ntohl(recvd));
                goto connection_died;
            }
            
            CHECK_RET(send_exact(connfd, 4, (uint8_t *) &version))
            
            CHECK_RET(recv_exact(connfd, 4, (uint8_t *) &recvd, IO_SEC_TO_BREAK));
            
            NBDEBUG( "log_streamio starting connection; server version: %u, client version: %u\n", ntohl(version), ntohl(recvd));
            
            ++seq_num;
            for (;;) {
                //we're writing.
                uint8_t ws = false;
                
                for (int i = 0; i < NUM_LOG_BUFFERS; ++i) {
                    obj_bi = i;
                    
                    for (int r = 0; r < LOG_RATIO[i]; ++r) {
                        
                        obj = acquire(i, &(log_main.buffers[i].servio_nextr));
                        
                        if (obj) {
                            ws = true;
                            
                            uint32_t msg_seq_n = htonl(seq_num++);
                            
                            CHECK_RET(send_exact(connfd, 4, (uint8_t *) &msg_seq_n));
                            
                            CHECK_RET(!obj->send(connfd));
                            
                            releaseWrapper(i, obj, true);
                        } else {
                            break;
                        }
                        
                        obj = NULL;
                    }
                }
                
                printf("looped\n");
                
                if (!ws) {
                    printf("waiting...\n");
                    usleep(STREAM_USLEEP_WAITING);
                }
            }
            
            //Acts like a c exception.
        connection_died:
            close(connfd);
            NBDEBUG( "log_streamio loop broken, connection closed.\n");
            control::flags[control::serv_connected] = false;
            
            if (obj)
                releaseWrapper(obj_bi, obj, true);
            obj = NULL;
        }
        
        return NULL;
    }
}

