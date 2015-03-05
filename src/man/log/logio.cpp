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

#include <sstream>

#include "log_header.h"

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
    
    int put_log( int (*exacter)(int, size_t, const void *), int sofd, log_t * log) {
        NBLassert(sofd >= 0);
        NBLassert(log);
        NBLassert(log->desc);
        
        uint32_t desc_hlen = (uint32_t) strlen(log->desc) + 1;
        uint32_t data_hlen = (uint32_t) log->dlen;
        
        uint32_t desc_nlen = htonl(desc_hlen);
        uint32_t data_nlen = htonl(data_hlen);
        
        if (exacter(sofd, 4, &desc_nlen)) {
            return 1;
        }
        if (exacter(sofd, desc_hlen, log->desc)) {
            return 2;
        }
        if (exacter(sofd, 4, &data_nlen)) {
            return 3;
        }
        if (log->data) {
            if (exacter(sofd, data_hlen, log->data)) {
                return 4;
            }
        }
        
        return 0;
    }
    
    int get_log( int (*exacter)(int, size_t, void *, double),
                int sofd,
                log_t * dest,
                double mw
                ) {
        NBLassert(sofd >= 0);
        NBLassert(dest);
        
        uint32_t ndesc;
        uint32_t hdesc;
        
        uint32_t ndata;
        uint32_t hdata;
        
        if (exacter(sofd, 4, &ndesc, IO_SEC_TO_BREAK)) {
            return 1;
        }
        
        hdesc = ntohl(ndesc);
        dest->desc = (char *) malloc(hdesc + 1);
        bzero(dest->desc, hdesc + 1);
        
        if (exacter(sofd, hdesc, dest->desc, IO_SEC_TO_BREAK)) {
            free(dest->desc);
            dest->desc = NULL;
            return 2;
        }
        
        if (exacter(sofd, 4, &ndata, IO_SEC_TO_BREAK)) {
            free(dest->desc);
            dest->desc = NULL;
            return 3;
        }
        
        hdata = ntohl(ndata);
        dest->dlen = hdata;
        
        if (hdata) {
            dest->data = (uint8_t *) malloc(hdata);
            
            if (exacter(sofd, hdata, dest->data, IO_SEC_TO_BREAK)) {
                free(dest->desc);
                free(dest->data);
                
                dest->desc = NULL;
                dest->data = NULL;
                
                return 4;
            }
        } else {
            dest->data = NULL;
        }
        
        return 0; //success
    }
    
    int write_log(int fd, log_t * log) {
        return put_log(write_exact, fd, log);
    }
    
    int send_log(int sck, log_t * log) {
        return put_log(send_exact, sck, log);
    }
    
    int recv_log(int sck, log_t * log, double max_wait) {
        return get_log(recv_exact, sck, log, max_wait);
    }
    
    ///=============================================
    
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
    
    std::vector<std::string> pairs(const char * desc) {
        std::string d(desc);
        size_t nullstart = d.find('\0');
        if (nullstart != d.npos)
            d = d.substr(0, nullstart);
        
        return split(d, ' ');
    }
    
    std::map<std::string, std::string> kvp(const char * desc) {
        std::map<std::string, std::string> ret;
        
        std::vector<std::string> kp = pairs(desc);
        
        for (int i = 0; i < kp.size(); ++i) {
            assert(kp[i].find('=') != std::string::npos);
            std::vector<std::string> splitted = split(kp[i], '=');
            assert(splitted.size() == 2);
            
            ret[splitted[0]] = splitted[1];
        }
        
        return ret;
    }
    
    int widthOf(log_t log) {
        std::map<std::string, std::string> keys = kvp(log.desc);
        
        if (keys.find("width") != keys.end()) {
            return atoi(keys["width"].c_str());
        } else {
            return -1;
        }
    }
    
    int heightOf(log_t log) {
        std::map<std::string, std::string> keys = kvp(log.desc);
        
        if (keys.find("height") != keys.end()) {
            return atoi(keys["height"].c_str());
        } else {
            return -1;
        }
    }
    
    bool isType(log_t * log, const char * type) {
        std::string need = "type=";
        need.append(type, strlen(type));
        
        std::string d(log->desc);
        if (d.find(need) == std::string::npos) return false;
        else return true;
    }
    
    log_t copyLog(log_t * log) {
        log_t newl;
        newl.desc = (char *) malloc(strlen(log->desc));
        strcpy(newl.desc, log->desc);
        newl.dlen = log->dlen;
        newl.data = (uint8_t *) malloc(newl.dlen);
        bcopy(log->data, newl.data, newl.dlen);
        
        return newl;
    }
    
    log_t heapLog(const char * desc, size_t dlen, void * data) {
        log_t ret;
        size_t desclen = strlen(desc) + 1;
        ret.desc = (char *) malloc(desclen);
        ret.data = (uint8_t *) malloc(dlen);
        ret.dlen = dlen;
        
        memcpy(ret.desc, desc, desclen + 1);
        memcpy(ret.data, data, dlen);
        
        return ret;
    }
    
}