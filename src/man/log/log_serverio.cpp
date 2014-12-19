//
//  log_serverio.c
//  NB_log_server
//
//  Created by Philip Koch on 10/4/14.
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


/*
 Protocol: first byte is ALWAYS message type
 
 0 is ping
 1 is log data
 
 followed by:
 
 32 bit network order string length
 <string bytes.
 32 bit network order data length
 <data bytes>
 
 the string encodes any useful information about the data.
 that means type, source, associated index, creation time, etc.
 
 In general, new information should be added to the END of the string.  Always include the fpl field.  See header.
 */

/*
 Mix relying on TCP for lossless traffic with having the client iteratively request data so that we don't overflow buffers (and lose data) on lower network levels.
 
 Protocol:  
 Client sent                Server sent
 0 (init)                   0 (init)
 1 (data request)           1 (sending log with sequence 1)
                            strlen(description)
                            description bytes
                            length(data)
                            data
 2 (data request)           2 (sending log with sequence 2)
 
 etc...
 */


namespace nblog {
    
    int serverio_last_read[NUM_LOG_BUFFERS];
    void * server_io_loop(void * context);
    
    void log_serverio_init() {
        LOGDEBUG(1, "log_serverio_init()\n");
        log_process->flags = 0;
        
        //memset(serverio_last_read, 0, sizeof(int) * NUM_LOG_BUFFERS);
        
        //First request will be for index 0.
        for (int i = 0; i < NUM_LOG_BUFFERS; ++i) {
            serverio_last_read[i] = -1;
        }
        
        log_process->log_serverio_thread = (pthread_t *) malloc(sizeof(pthread_t));
        pthread_create((log_process->log_serverio_thread), NULL, &server_io_loop, NULL);
    }
    
    //time() of last recved data
    //if recv_exactly finds this to differ from current time() by SERVER_SECONDS_TILL_BREAK seconds
    //it returns 1 and the connection will be closed.
    time_t last_heard;
    //returns 1 on too much silence, 2 on error, 0 success
    static inline int recv_exactly(int fd, size_t nbytes, void * buffer, int flags) {
        size_t read = 0;
        while (read < nbytes) {
            if (difftime(time(NULL), last_heard) >= SERVER_SECONDS_TILL_BREAK)
                return 1;
            
            int ret = (int) recv(fd,
                           ((uint8_t *) buffer) + read,
                           nbytes - read, flags);
            
            //Treat negative return (which normally indicates error)
            //as 0 data available.  This is because socket is non-blocking and so 0 data could well return EAGAIN or something.
            
            //Might be good to actually check the error code tho...
            if (ret == 0) {
                //shorter wait because we think data should be otw.
                usleep(SERVER_USLEEP_EXPECTING);
            } else if (ret < 0) {
                int err_saved = errno;
                if (err_saved == EAGAIN) {
                    usleep(SERVER_USLEEP_EXPECTING);
                } else {
                    char buf[1000];
                    strerror_r(err_saved, buf, 1000);
                    printf("%s\n", buf);
                    return 2;
                }
            } else
                read += ret;
        }
        
        //completed recv'ing.
        time(&last_heard);
        return 0;
    }
    
    //Send guaranteed exactly.
    //Possibility that if socket is broken while this function is running we could infinite loop.
    //Need to check ret errors more carefully.
    static inline int send_exactly(int fd, size_t nbytes, void * data, int flags) {
        
        int written = 0;
        while (written < nbytes) {
            
            int ret = (int) send(fd, ((uint8_t *) data) + written, nbytes - written, flags);
            
            
            if (ret == 0) {
                //shorter wait because we think data should be otw.
                usleep(SERVER_USLEEP_EXPECTING);
            } else if (ret < 0) {
                int err_saved = errno;
                if (err_saved == EAGAIN) {
                    usleep(SERVER_USLEEP_EXPECTING);
                } else {
                    char buf[1000];
                    strerror_r(err_saved, buf, 1000);
                    printf("%s\n", buf);
                    return 2;
                }
            } else
                written += ret;
        }
        
        return 0;
    }
    
    //Block thread unti client has attempted to connect.
    int block_accept(int lfd) {
        int connfd = -1;
        
        for (; connfd < 0; usleep(50))
            connfd = accept(lfd, (struct sockaddr*)NULL, NULL);
        
        //The below code is saved for future generations as how to NEVER
        //EVER check the return of accept.
        
        //Or, at least make damn sure your parenthesis are in the right spots.
        //                  < has precedence over =
        /*
        while ( (connfd = accept(lfd, (struct sockaddr*)NULL, NULL)) < 0 ) {
            usleep(50);
        } */
        
        //Set the socket to non-blocking.
        int flags = fcntl(connfd, F_GETFL, 0);
        fcntl(connfd, F_SETFL, flags | O_NONBLOCK);
        
        return connfd;
    }
    
    /*
     basically a static for loop, so that we can loop through buffers getting logs over multiple server loops.
     */
    int last_read_buffer = -1;
    log_object_t * sio_dequeue() {
        log_object_t * obj;
        int i = 1;
        
        for (; i <= NUM_LOG_BUFFERS; ++i) {
            int bi = (last_read_buffer + i) % NUM_LOG_BUFFERS;
            obj = get_log(bi, &(serverio_last_read[bi]));
            
            if (obj) {
                last_read_buffer = bi;
                return obj;
            }
        }
        
        return NULL;
    }
    
    //Available bytes on socket fd.
    //deprecated?
    static inline int available(int fd) {
        int a;
        ioctl(fd, FIONREAD, &a);
        return a;
    }
    
#define CHECK_RET(r) {if (r) goto connection_died;}
    
    void * server_io_loop(void * arg) {
        
        int listenfd = 0, connfd = 0;
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
        signal(SIGPIPE, SIG_IGN);
        
        //Accepting connections on this socket, backqueue of size 1.
        listen(listenfd, 1);
        
        LOGDEBUG(2, "log_serverio listening...\n");
        log_process->flags &= ~SERVER_CONNECTED;
        //Continue accepting connections until we exit.
        while (1) {
            connfd = block_accept(listenfd);
            
            LOGDEBUG(3, "log_serverio FOUND CLIENT [%i]\n", connfd);
            log_process->flags |= SERVER_CONNECTED;
            last_heard = time(NULL);
            uint32_t seq_num = 0;
            uint32_t recvd;
            
            //init connection
            CHECK_RET(send_exactly(connfd, 4, &seq_num, 0));
            if (recv_exactly(connfd, 4, &recvd, 0)) {
                 LOGDEBUG(1, "log_serverio BREAKING CONNECTION DUE TO SILENCE (initiation) [s: %f]\n", difftime(time(NULL), last_heard));
                goto connection_died;
            }
            
            if (recvd != 0) {
                LOGDEBUG(1, "log_serverio got bad ping initiation: %u\n", ntohl(recvd));
                goto connection_died;
            }
            
            ++seq_num;
            LOGDEBUG(1, "log_servio client connection ready...\n");
            while (1) {
                if (recv_exactly(connfd, 4, &recvd, 0)) {
                    LOGDEBUG(1, "log_serverio BREAKING CONNECTION DUE TO SILENCE [s: %f]\n", difftime(time(NULL), last_heard));
                    goto connection_died;
                }
                
                recvd = ntohl(recvd);
                if (recvd != seq_num) {
                    LOGDEBUG(1, "log_serverio got bad request: %u\n", recvd);
                    goto connection_died;
                }
                
                log_object_t * obj = sio_dequeue();
                while (!obj) {
                    usleep(SERVER_USLEEP_WAITING);
                    obj = sio_dequeue();
                }
                
                char * desc = generate_type_specs(obj);
                //uint32_t msg_seq = seq_num;
                uint32_t msg_seq_n = htonl(seq_num);
                
                uint32_t desc_len = htonl(strlen(desc));
                NBLassert(obj->n_bytes < ULONG_MAX);
                uint32_t data_len = htonl(obj->n_bytes);
                
                CHECK_RET(send_exactly(connfd, 4, &msg_seq_n, 0));
                CHECK_RET(send_exactly(connfd, 4, &desc_len, 0));
                CHECK_RET(send_exactly(connfd, strlen(desc), desc, 0));
                CHECK_RET(send_exactly(connfd, 4, &data_len, 0));
                CHECK_RET(send_exactly(connfd, obj->n_bytes, obj->data, 0));
                
                //pthread_mutex_lock(&(buf->lock));
                log_object_release(obj);
                //pthread_mutex_unlock(&(buf->lock));
                
                free(desc);
                LOGDEBUG(3, "log_serverio sent %u (desc_len=%u, data_len=%u)\n", seq_num, ntohl(desc_len), ntohl(data_len));
                
                ++seq_num;
                
                //reset silence
                last_heard = time(NULL);
            }
            
        connection_died:
            close(connfd);
            LOGDEBUG(1, "log_serverio loop broken, connection closed.\n");
        }
        
        return NULL;
    }
    
}


