#ifndef nb_exactio_h
#define nb_exactio_h

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>

#include <fcntl.h>

#include <inttypes.h>

#include "nbdebug.h"

#define USLEEP_EXPECTING (1000)
#define IO_SEC_TO_BREAK (5.0)

static void nbperror(const char * prefix, int errsaved) {
#ifdef __APPLE__
    char buf[256];
    strerror_r(errsaved, buf, 256);
    printf("%s %s\n", prefix, buf);
#else
    char buf[256];
    char * buf2 = strerror_r(errsaved, buf, 256);
    printf("%s %s\n", prefix, buf2);
#endif
    
}

//fileio can just use the write function
#define WRITE_STUB write

//specify system-dependent args to send function
static inline ssize_t send_stub(int sck, const void * data, size_t nbytes) {
#ifndef __APPLE__
    return send(sck, data, nbytes, MSG_NOSIGNAL);
#else
    return send(sck, data, nbytes, 0);
#endif
}

//network output io uses the above stub.
#define SEND_STUB send_stub

//specify system dependent args to recv function
static inline ssize_t recv_stub(int sck, void * data, size_t nbytes) {
#ifndef __APPLE__
    return recv(sck, data, nbytes, MSG_NOSIGNAL);
#else
    return recv(sck, data, nbytes, 0);
#endif
}

//network input io uses the above stub.
#define RECV_STUB recv_stub

//NOTE: this framework currently does not support file input.
//So no 4th stub.

//Uses a supplied put stub to write exactly nb bytes of data to sofd
//i.e., socket-or-filedescriptor
static inline int put_exact(ssize_t (* pstub)(int, const void *, size_t),
              int sofd,
              size_t nb,
              uint8_t * data)
{
    NBLassert(pstub && data && (nb >= 0) );
    NBLassert(sofd >= 0);
    
    size_t written = 0;
    while (written < nb) {
        ssize_t ret = pstub(sofd,
                            data + written,
                            nb - written);
        if (ret == 0) {
            usleep(USLEEP_EXPECTING);
        } else if (ret < 0) {
            int saved_err = errno;
            
            if (saved_err == EAGAIN) {
                usleep(USLEEP_EXPECTING);
            } else {
                /*
                char buf[256];
                strerror_r(saved_err, buf, 256);
                printf("\n\n****put_exact****:  %s\n", buf); */
                
                nbperror("\n\n****put_exact****: ", saved_err);
                return 2;
            }
        } else {
            written += ret;
        }
    }
    
    NBLassert(written == nb);
    return 0;
}

static inline int get_exact(ssize_t (*gstub)(int, void *,size_t),
              int sck,
              size_t nbytes,
              void * abuffer,
              double max_wait)
{
    uint8_t * buffer = (uint8_t *) abuffer;
    
    NBLassert(max_wait >= 1);
    NBLassert(buffer);
    
    time_t last = time(NULL);
    
    size_t rbytes = 0;
    while (rbytes < nbytes) {
        if (difftime(time(NULL), last) >= max_wait)
            return 1;
        
        ssize_t ret = gstub(sck, buffer + rbytes, nbytes - rbytes);
        
        if (ret == 0) {
            usleep(USLEEP_EXPECTING);
        } else if (ret < 0) {
            int err_saved = errno;
            if (err_saved == EAGAIN) {
                usleep(USLEEP_EXPECTING);
            } else {
                /*
                char buf[256];
                strerror_r(err_saved, buf, 256);
                printf("\n\n****get_exact****:  %s\n", buf); */
                
                nbperror("\n\n****get_exact****: ", err_saved);
                return 2;
            }
        } else {
            rbytes += ret;
            last = time(NULL);
        }
    }
    
    NBLassert(rbytes == nbytes);
    return 0;
}

//put exactly using write_stub which currently translates to simple write()
static inline int write_exact(int fd, size_t nbytes, const void * data) {
    return put_exact(&WRITE_STUB, fd, nbytes, (uint8_t *) data);
}

//send exactly using SEND_STUB
static inline int send_exact(int sck, size_t nbytes, const void * data) {
    return put_exact(&SEND_STUB, sck, nbytes, (uint8_t *) data);
}

//recv exactly using RECV_STUB
static inline int recv_exact(int sck, size_t nbytes, void * buffer, double max_wait) {
    return get_exact(&RECV_STUB, sck, nbytes, buffer, max_wait);
}

//Block thread until client has attempted to connect.
static inline int block_accept(int lfd, int timeout) {
    int connfd = -1;
    
    for (; connfd < 0; usleep(timeout))
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

#endif //nb_exactio_h