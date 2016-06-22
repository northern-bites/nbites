//
//  nblogio.cpp
//  tool8-separate
//
//  Created by Philip Koch on 3/28/16.
//

#include <string>
#include <string>
#include <algorithm>
#include <sstream>
#include <stdexcept>

#include <netinet/in.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <err.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/time.h>

#include "nblogio.h"

#include "Log.hpp"

#include "utilities.hpp"
#define NBL_LOGGING_LEVEL NBL_WARN_LEVEL

namespace nbl {
    namespace io {

        const char LOCAL_HOST_ADDR[] = "127.0.0.1";

        FileMonitor::FileMonitor(const char * file, bool trueOnFirst) :
            fd(0), last(0)
        {
            fd = open(file, O_RDONLY);
            if (!fd) {
                int err = errno;
                throw std::runtime_error{utilities::format("could not open file: %s: %s",
                                                           file,
                                                           utilities::get_error(err).c_str()
                                                           )};
            }

            if (!trueOnFirst) {
                update();
            }
        }

        FileMonitor::~FileMonitor() {
            close();
        }

        bool FileMonitor::update() {
            if (fd) {
                struct stat buf;
                fstat(fd, &buf);
                double diff = difftime(buf.st_mtime, last);
                if (diff > 0) {
                    last = buf.st_mtime;
                    return true;
                } else return false;
            } else {
                throw std::runtime_error("FileMonitor: cannot call update() after close()");
            }


        }

        void FileMonitor::close() {
            if (fd) {
                ::close(fd);
                fd = 0;
            }
        }

        const iotime_t IO_EXPECTING_ST  = 1000;
        const iotime_t IO_NOT_EXPECTING_ST = 500000;

        iotime_t IO_MAX_DELAY() {
            return SharedConstants::REMOTE_HOST_TIMEOUT();
        }

#define MICROSECONDS_MOD_SECONDS(us) (us % 1000000)
        struct timespec io_get_abs_ts(iotime_t fromNow) {
            struct timeval tv;
            struct timespec ts;
            int r1 = gettimeofday(&tv, NULL);
            NBL_ASSERT_EQ(r1, 0);

            long long total_ns = tv.tv_sec * 1e9 + tv.tv_usec * 1000;
            total_ns += fromNow * 1000;

            long long int seconds = total_ns / 1e9;
            long long int nsec = total_ns - (1e9 * seconds);

            ts.tv_sec = seconds;
            ts.tv_nsec =  nsec;

            NBL_ASSERT_GT(ts.tv_sec, 0);
            NBL_ASSERT_GT(ts.tv_nsec, 0);
            NBL_ASSERT_LT(ts.tv_nsec, 1e9);

            return ts;
        }

        inline ioret exact(iostub_ref stub, int sofd,
                           size_t nb, void * data_,
                           iotime_t maxWaitS, const char * stub_name) {
            NBL_ASSERT(stub);
            NBL_ASSERT(data_);
            NBL_ASSERT_GT(sofd, 0);

            size_t used = 0;
            uint8_t * data = (uint8_t *) data_;
            time_t last = time(NULL);

            while (used < nb) {
                ssize_t ret = stub(sofd,
                                   data + used,
                                   nb - used);
                int saved_err = errno;

                if ( (ret == 0) ||
                    (ret < 0 && saved_err == EAGAIN) ) {

                    if (difftime(last, time(NULL)) > io_to_s(maxWaitS)) {
                        return TIMEOUT;
                    } else {
                        usleep(io_to_us(IO_EXPECTING_ST));
                    }

                } else if (ret < 0) {

                    std::string error = utilities::get_error(saved_err);
                    NBL_WARN( "***%s*** got error: %s", (stub_name) ? stub_name : "exactio",
                            error.c_str());
                    return ERROR;

                } else {
                    used += ret;
                    last = time(NULL);
                }
            }

            NBL_ASSERT_EQ(used, nb)
            return SUCCESS;
        }

        ioret write_exact(int fd, size_t nb, const void * data) {
            return exact( (iostub_ref) &write, fd, nb, const_cast<void*>(data), std::numeric_limits<iotime_t>::max(), "write_exact");
        }

        ioret read_exact(int fd, size_t nb, void * data) {
            return exact( &read, fd, nb, data, std::numeric_limits<iotime_t>::max(), "read_exact");
        }

        size_t file_size(int fd) {
            struct stat buf;
            if ( fstat(fd, &buf) ) {
                int esaved = errno;
                std::string error = utilities::get_error(esaved);

                NBL_ERROR("fstat() error: %s", error.c_str());

                throw std::runtime_error( utilities::format("fstat() error: %s", error.c_str()) );
            }

            return buf.st_size;
        }

        void readFileToString(std::string& buf, const std::string& path) {
            int fd = open( path.c_str(), O_RDONLY | O_CREAT, S_IRWXG | S_IRWXU);

            if (fd < 0) {
                std::string msg = utilities::format("could not open file '%s' for reading!", path.c_str());
                NBL_ERROR("%s", msg.c_str());
                throw std::runtime_error(msg);
            }

            readFileToString(buf, fd);
            close(fd);
        }

        void readFileToString(std::string& buf, int fd) {
            size_t size = file_size(fd);
            NBL_ASSERT_GE(size, 0)
            buf.resize(size);

            ioret ret = read_exact(fd, size, &buf[0]);
            NBL_ASSERT( !ret );
        }

        void writeStringToFile(const std::string& buf, const std::string& path) {
            int fd = open( path.c_str(), O_WRONLY | O_TRUNC | O_CREAT,
                          S_IRWXG | S_IRWXU);

            if (fd < 0) {
                std::string msg = utilities::format("could not open file '%s' for writing!", path.c_str());
                NBL_ERROR("%s", msg.c_str());
                throw std::runtime_error(msg);
            }

            size_t size = buf.size();
            ioret ret = write_exact(fd, size, &buf[0]);
            NBL_ASSERT(!ret)
            close(fd);
        }

        ssize_t send_stub(int sck, void * data, size_t bytes) {
            return send(sck, data, bytes, NETIO_FLAGS);
        }

        ioret send_exact(int sock, size_t nb, const void * data, iotime_t mws) {
            return exact(send_stub, sock, nb, const_cast<void*>(data), mws, "send_exact");
        }

        ssize_t recv_stub(int sck, void * data, size_t bytes) {
            return recv(sck, data, bytes, NETIO_FLAGS);
        }

        ioret recv_exact(int sock, size_t nb, void * data, iotime_t mws) {
            return exact(recv_stub, sock, nb, data, mws, "recv_exact");
        }

#define PERROR_AND_FAIL_IF( condition, msg ) \
if ( condition ) {      \
int err = errno;    \
NBL_ERROR("failure: %s: %s", msg, utilities::get_error(err).c_str());  \
return ERROR; }

        ioret server_socket(server_socket_t& ss, int port, int qs) {
            struct sockaddr_in serv_addr;
            memset(&serv_addr, 0, sizeof(serv_addr));

            ss = socket(AF_INET, SOCK_STREAM, 0);
            PERROR_AND_FAIL_IF(ss < 0, "could not create server socket")

            serv_addr.sin_family = AF_INET;
            serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
            serv_addr.sin_port = htons(port);

            /* we need the socket to use these two options so that we may re-establish dead servers quickly */
            int enable = 1;
            
            int ru_addr_ret = setsockopt(ss, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
            PERROR_AND_FAIL_IF(ru_addr_ret, "could not set SO_REUSEADDR !");

            int ru_port_ret = setsockopt(ss, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int));
            PERROR_AND_FAIL_IF(ru_port_ret, "could not set SO_REUSEPORT !");

            int bret = bind(ss, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
            PERROR_AND_FAIL_IF(bret, "could not bind server socket!");

            int lret = listen(ss, qs);
            PERROR_AND_FAIL_IF(lret, "could not listen() server socket!");

            return SUCCESS;
        }

        ioret poll_accept(server_socket_t serverSocket, client_socket_t& retSock) {
            //all programs should have 0,1,2 used for stdio
            NBL_ASSERT_GT(serverSocket, STDERR_FILENO);

            for (client_socket_t sock; ;) {
                sock = accept(serverSocket, (struct sockaddr*)NULL, NULL);

                if (sock < 0 && errno == EAGAIN) {
                    usleep(IO_NOT_EXPECTING_ST);
                } else if (sock < 0) {
                    PERROR_AND_FAIL_IF(true, "poll_accept() got non-EAGAIN error");
                } else {
                    retSock = sock;
                    return SUCCESS;
                }
            }
        }

        void config_socket(client_socket_t socket, sock_opt_mask options) {
            int flags = fcntl(socket, F_GETFL, 0);
            flags |= options;
            int ret = fcntl(socket, F_SETFL, flags);
            NBL_LOG_IF(NBL_ERROR_LEVEL, ret, "config_socket(): %s",
                       utilities::get_error(errno).c_str());

#if defined(__APPLE__) && !defined(OFFLINE)
            int set = 1;
            setsockopt(socket, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));
#endif
        }

        ioret connect_to(client_socket_t& sock, int port, const char * host )
        {
            struct sockaddr_in server;
            bzero(&server, sizeof(server));

            sock = socket(AF_INET, SOCK_STREAM, 0);
            PERROR_AND_FAIL_IF(sock < 0, "could not create client socket!")

            server.sin_family = AF_INET ;
            server.sin_port = htons(port);
            
            struct hostent * hent = gethostbyname(host);
            PERROR_AND_FAIL_IF(!hent, utilities::format("could not get host ip '%s'", host).c_str() )
            
            bcopy(hent->h_addr, &server.sin_addr.s_addr, hent->h_length);
            
            int ret = connect(sock, (struct sockaddr *) &server, (socklen_t) sizeof(struct sockaddr_in) );
            
            PERROR_AND_FAIL_IF(ret, "could not connect client socket!")
            
            return SUCCESS;
        }
        
    }
}