//
//  log_serverio.c
//  NB_log_server
//
//  Created by Philip Koch on 10/4/14.
//

#include "log_header.h"
#include "log_sf.h"
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
        LOGDEBUG(1, "log_serverio_init()\n");
        
        log_main->log_serverio_thread = (pthread_t *) malloc(sizeof(pthread_t));
        pthread_create((log_main->log_serverio_thread), NULL, &server_io_loop, NULL);
    }
    
    //Block thread unti client has attempted to connect.
    /*
     NOTE: the cnc server uses this function as well, so editors should ensure their changes do not break that code.
     */
    int block_accept(int lfd) {
        int connfd = -1;
        
        for (; connfd < 0; usleep(SERVER_USLEEP_WAITING))
            connfd = accept(lfd, (struct sockaddr*)NULL, NULL);
        
        //Set the socket to non-blocking.
        int flags = fcntl(connfd, F_GETFL, 0);
        fcntl(connfd, F_SETFL, flags | O_NONBLOCK);
        
#ifdef __APPLE__
        int set = 1;
        setsockopt(connfd, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));
#endif
        
        return connfd;
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
    
    void * server_io_loop(void * arg) {
        int listenfd = -1, connfd = -1;
        struct sockaddr_in serv_addr;
        
        //Network socket, TCP streaming protocol, default options
        listenfd = socket(AF_INET, SOCK_STREAM, 0);
        LOGDEBUG(1, "log_servio listenfd=[%i]\n", listenfd);
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
        
        LOGDEBUG(2, "log_serverio listening...port=%i\n", LOG_PORT);
        nbsf::flags[nbsf::serv_connected] = false;
        
        for (;;) {
            connfd = block_accept(listenfd);
            log_object_t * obj = NULL;
            
            LOGDEBUG(3, "log_serverio FOUND CLIENT [%i]\n", connfd);
            memcpy(nbsf::cio_start, nbsf::total, sizeof(nbsf::io_state_t) * NUM_LOG_BUFFERS);
            nbsf::cio_upstart = time(NULL);
            nbsf::flags[nbsf::serv_connected] = true;
            
            uint32_t version = htonl(LOG_VERSION);
            uint32_t seq_num = 0;
            uint32_t recvd;
            
            CHECK_RET(send_exactly(connfd, 4, (uint8_t *) &seq_num))
            
            CHECK_RET(recv_exactly(connfd, 4, (uint8_t *) &recvd, IO_SEC_TO_BREAK));
            
            if (recvd != 0) {
                LOGDEBUG(1, "log_serverio got bad ping initiation: %u\n", ntohl(recvd));
                goto connection_died;
            }
            
            CHECK_RET(send_exactly(connfd, 4, (uint8_t *) &version))
            
            CHECK_RET(recv_exactly(connfd, 4, (uint8_t *) &recvd, IO_SEC_TO_BREAK));
            
            LOGDEBUG(1, "log_serverio starting connection; server version: %u, client version: %u\n", ntohl(version), ntohl(recvd));
            
            ++seq_num;
            LOGDEBUG(1, "log_servio client connection ready...\n");
            for (;;) {
                if (!(nbsf::flags[nbsf::servio])) {
                    uint32_t ping = 0;
                    
                    CHECK_RET(send_exactly(connfd, 4, (uint8_t *) &ping))
                    
                    CHECK_RET(recv_exactly(connfd, 4, (uint8_t *) &recvd, IO_SEC_TO_BREAK));
                    
                    if (recvd != 0) {
                        LOGDEBUG(1, "log_serverio got bad ping while waiting: %u\n", ntohl(recvd));
                        goto connection_died;
                    }
                    
                    usleep(SERVER_USLEEP_WAITING);
                    continue;
                }
                
                //we're writing.
                uint8_t ws = false;
                
                for (int i = 0; i < NUM_LOG_BUFFERS; ++i) {
                    for (int r = 0; r < LOG_RATIO[i]; ++r) {
                        if (!(nbsf::flags[nbsf::servio])) {
                            break;
                        }
                        
                        obj = acquire(i, &(log_main->buffers[i]->servio_nextr));
                        
                        if (obj) {
                            ws = true;
                            
                            uint32_t msg_seq_n = htonl(seq_num++);
                            
                            CHECK_RET(send_exactly(connfd, 4, (uint8_t *) &msg_seq_n));
                            
                            CHECK_RET(send_log(connfd, obj));
                            
                            release(obj, true);
                        } else {
                            break;
                        }
                    }
                }
                
                if (!ws) {
                    usleep(SERVER_USLEEP_WAITING);
                }
            }
            
            //Acts like a c exception.
        connection_died:
            close(connfd);
            LOGDEBUG(1, "log_serverio loop broken, connection closed.\n");
            nbsf::flags[nbsf::serv_connected] = false;
            if (obj)
                release(obj, true);
        }
        
        return NULL;
    }
}

