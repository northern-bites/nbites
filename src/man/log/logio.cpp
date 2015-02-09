#include "logio.h"

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <inttypes.h>


namespace logio {
    
#define WRITE_STUB write
    
    ssize_t send_stub(int sck, const void * data, size_t nbytes) {
#ifndef __APPLE__
        return send(sck, data, nbytes, MSG_NOSIGNAL);
#else
        return send(sck, data, nbytes, 0);
#endif
    }
    
#define SEND_STUB send_stub
    
    ssize_t recv_stub(int sck, void * data, size_t nbytes) {
#ifndef __APPLE__
        return recv(sck, data, nbytes, MSG_NOSIGNAL);
#else
        return recv(sck, data, nbytes, 0);
#endif
    }
    
#define RECV_STUB recv_stub
    
    int put_exact(ssize_t (* pstub)(int, const void *, size_t),
                  int sofd,
                  size_t nb,
                  uint8_t * data)
    {
        NBLassert(pstub && data && nb);
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
                    char buf[256];
                    strerror_r(saved_err, buf, 256);
                    printf("\n\n****put_exact****:  %s\n", buf);
                    return 2;
                }
            } else {
                written += ret;
            }
        }
        
        NBLassert(written == nb);
        return 0;
    }
    
    int get_exact(ssize_t (*gstub)(int, void *,size_t),
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
                    char buf[256];
                    strerror_r(err_saved, buf, 256);
                    printf("\n\n****get_exact****:  %s\n", buf);
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
    
    int write_exact(int fd, size_t nbytes, const void * data) {
        return put_exact(&WRITE_STUB, fd, nbytes, (uint8_t *) data);
    }
    
    int send_exact(int sck, size_t nbytes, const void * data) {
        return put_exact(&SEND_STUB, sck, nbytes, (uint8_t *) data);
    }
    
    int recv_exact(int sck, size_t nbytes, void * buffer, double max_wait) {
        return get_exact(&RECV_STUB, sck, nbytes, buffer, max_wait);
    }
    
    
    
    //stubs
    //then put_exact/get_exact
    //then put_log/get_log
    //then use all of above to implement from header
    
    /*
     STUBS
     */
    //read as exact
    //write as exact
    
    
    ssize_t send_stub(int sck, const void * data, size_t nbytes) {
#ifndef __APPLE__
        return send(sck, data, nbytes, MSG_NOSIGNAL);
#else
        return send(sck, data, nbytes, 0);
#endif
    }
    
    ssize_t recv_stub(int sck, const void * data)
    
    
    int _put_exactly(ssize_t (* wfunc)(int, const void *, size_t), int sofd, size_t nb, uint8_t * data) {
        NBLassert(wfunc && data && nb);
        NBLassert(sofd >= 0);
        
        size_t written = 0;
        while (written < nb) {
            ssize_t ret = wfunc(sofd,
                                data + written,
                                nb - written);
            if (ret == 0) {
                usleep(USLEEP_EXPECTING);
            } else if (ret < 0) {
                int saved_err = errno;
                
                if (saved_err == EAGAIN) {
                    usleep(USLEEP_EXPECTING);
                } else {
                    char buf[256];
                    strerror_r(saved_err, buf, 256);
                    printf("\n\n****_put_exactly****:  %s\n", buf);
                    return 2;
                }
            } else {
                written += ret;
            }
        }
        
        assert(written == nb);
        return 0;
    }
    
    int write_exactly(int fd, size_t nbytes, void * adata) {
        return _put_exactly(&write, fd, nbytes, (uint8_t *) adata);
    }
    
    ssize_t send_stub(int sck, const void * data, size_t nbytes) {
#ifndef __APPLE__
        return send(sck, data, nbytes, MSG_NOSIGNAL);
#else
        return send(sck, data, nbytes, 0);
#endif
    }
    
    int send_exactly(int socket, size_t nbytes, void * adata) {
        return _put_exactly(&send_stub, socket, nbytes, (uint8_t *) adata);
    }
    
    int recv_exactly(int sck, size_t nbytes, void * abuffer, double max_wait) {
        uint8_t * buffer = (uint8_t *) abuffer;
        
        NBLassert(max_wait >= 1);
        NBLassert(buffer);
        
        time_t last = time(NULL);
        
        size_t rbytes = 0;
        while (rbytes < nbytes) {
            if (difftime(time(NULL), last) >= max_wait)
                return 1;
#ifndef __APPLE__
            ssize_t ret = recv(sck, buffer + rbytes, nbytes - rbytes, MSG_NOSIGNAL);
#else
            ssize_t ret = recv(sck, buffer + rbytes, nbytes - rbytes, 0);
#endif
            
            if (ret == 0) {
                usleep(USLEEP_EXPECTING);
            } else if (ret < 0) {
                int err_saved = errno;
                if (err_saved == EAGAIN) {
                    usleep(USLEEP_EXPECTING);
                } else {
                    char buf[256];
                    strerror_r(err_saved, buf, 256);
                    printf("\n\n****recv_exactly****:  %s\n", buf);
                    return 2;
                }
            } else {
                rbytes += ret;
                last = time(NULL);
            }
        }
        
        return 0;
    }
    
    int _put_log( int (*exacter)(int, size_t, void *), int sofd, log_object_t * log) {
        NBLassert(sofd >= 0);
        NBLassert(log);
        
        char desc[MAX_LOG_DESC_SIZE];
        description(desc, MAX_LOG_DESC_SIZE, log);
        uint32_t desc_hlen = strlen(desc);
        uint32_t data_hlen = log->n_bytes;
        
        uint32_t desc_nlen = htonl(desc_hlen);
        uint32_t data_nlen = htonl(data_hlen);
        
        if (exacter(sofd, 4, &desc_nlen)) {
            return 1;
        }
        if (exacter(sofd, desc_hlen, desc)) {
            return 2;
        }
        if (exacter(sofd, 4, &data_nlen)) {
            return 3;
        }
        if (log->n_bytes) {
            if (exacter(sofd, data_hlen, log->data)) {
                return 4;
            }
        }
        
        return 0;
    }
    
    int send_log(int sock, log_object_t * log) {
        return _put_log(&send_exactly, sock, log);
    }
    
    int write_log(int fd, log_object_t * log) {
        return _put_log(&write_exactly, fd, log);
    }
    
}