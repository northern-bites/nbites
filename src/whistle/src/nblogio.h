//
//  exactio.h
//
//  Created by Philip Koch on 3/19/16.
//

#ifndef nblogio_h
#define nblogio_h

#include <string>
#include <fstream>
#include <sstream>

#include <fcntl.h>

namespace nblog {
    namespace io {

        /* exact io on file descriptors */

        //pointer to function for individual io call.
        //declaration is same for read and write
        typedef ssize_t (*iostub_ref)(int, void *, size_t);

        typedef enum {
            SUCCESS = 0,
            TIMEOUT = 1,
            ERROR = 2
        } ioret;

        typedef useconds_t iotime_t;

        static inline time_t io_to_s(iotime_t t) { return t / 1000000; }
        static inline useconds_t io_to_us(iotime_t t) { return t; }

        struct timespec io_get_abs_ts(iotime_t fromNow);
    
        //Same code for input/output, direction determined by stub.
        //stub_name may be NULL.  It is for debugging.
        ioret exact(iostub_ref stub, int sofd,
                    size_t nb, void * data,
                    iotime_t maxWaitS, const char * stub_name);

        /* File io */
        /* fd is file descriptor (as in, return from open() )*/

        size_t file_size(int fd);

        //Success in return value, will not return TIMEOUT
        ioret write_exact(int fd, size_t nb, const void * data);
        ioret read_exact(int fd, size_t nb, void * data);

        //Throws runtime_error on failure
        extern void readFileToString(std::string& buf, const std::string& path);
        extern void readFileToString(std::string& buf, int fd);

        extern void writeStringToFile(const std::string& buf, const std::string& path);

        /* Network io & connections */

        //Success in return value, nz return indicates failure
        ioret send_exact(int sock, size_t nb, const void * data, iotime_t mws);
        ioret recv_exact(int sock, size_t nb, void * data, iotime_t mws);

        //sockets & connecting

        //typedef just to make function declarations clearer.
        typedef int server_socket_t;
        typedef int client_socket_t;

        //Success in return value, ss set to descriptor on success.
        extern ioret server_socket(server_socket_t& ss, int port, int queuesize);
        //Blocks until client connects,
        extern ioret poll_accept(server_socket_t serverSocket, client_socket_t& retSock);

        typedef enum {
            NONBLOCKING = O_NONBLOCK
        } sock_opt_mask;

        //set options on socket, also #ifdef __APPLE__ NOSIGPIPE
        extern void config_socket(client_socket_t socket, sock_opt_mask options);

        extern ioret connect_to( client_socket_t& sock, int port, const char * host );

#ifdef __APPLE__
#define NETIO_FLAGS 0
#else
#define NETIO_FLAGS MSG_NOSIGNAL
#endif

        extern const iotime_t IO_EXPECTING_ST;
        extern const iotime_t IO_NOT_EXPECTING_ST;

        extern const char LOCAL_HOST_ADDR[];

        //function because references CONSTANTS object (Log.h)
        iotime_t IO_MAX_DELAY();
    }
}


#endif /* nblogio_h */
