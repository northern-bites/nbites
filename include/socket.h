/*
  Edited by Jeremy Fishman, Winter 2006
  for use by Bowdoin College's 2006 Northern Bites RoboCup Team
*/
/* Declarations of socket constants, types, and functions.
   Copyright (C) 1991,92,1994-2001,2003 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#ifndef SOCKET_H
#define SOCKET_H   1

#define __need_size_t

#include <stddef.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>

#include <OPENR/OObject.h>
#include <EndpointTypes.h>
#include <TCPEndpointMsg.h>
#include <UDPEndpointMsg.h>
#include <DNSEndpointMsg.h>
#include <DNSEndpointErr.h>

//  ADDED  JFISHMAN -- Changable operational constants

const int DEF_SEND_BUF = 1024;
const int DEF_RECV_BUF = 1024;
const int ENDPOINT_SIZE = 8192;

/* This operating system-specific header file defines the SOCK_*, PF_*,
   AF_*, MSG_*, SOL_*, and SO_* constants, and the `struct sockaddr',
   `struct msghdr', and `struct linger' types.  */
//#include <bits/socket.h>
//  ADDED  JFISHMAN  --  Above mentioned constants/types defined here  //
//    see http://www.opengroup.org/pubs/online/7908799/xns/syssocket.h.html  //

//typedef int socklen_t;
#define socklen_t int
typedef unsigned int sa_family_t;

struct in_addr {
  unsigned long s_addr;  // load with inet_aton()
};

struct sockaddr_in
{
  short            sin_family;   // e.g. AF_INET
  unsigned short   sin_port;     // e.g. htons(3490)
  struct in_addr   sin_addr;     // see struct in_addr, below
  char             sin_zero[8];  // zero this if you want to
};

struct msghdr
{
  void *msg_name;
  socklen_t msg_namelen;
  struct iovec *msg_iov;
  int msg_iovlen;
  void *msg_control;
  socklen_t msg_controllen;
  int msg_flags;
};
  
struct cmsghdr
{
  socklen_t cmsg_len;
  int cmsg_level;
  int cmsg_type;
};

//If the argument is a pointer to a cmsghdr structure, this macro returns an
//unsigned character pointer to the data array associated with the cmsghdr
//structure.
#define CMSG_DATA(cmsg) { \
}
//If the first argument is a pointer to a msghdr structure and the second
//argument is a pointer to a cmsghdr structure in the ancillary data, pointed
//to by the msg_control field of that msghdr structure, this macro returns a
//pointer to the next cmsghdr structure, or a null pointer if this structure is
//the last cmsghdr in the ancillary data.
#define CMSG_NXTHDR(mhdr,cmsg) { \
}
//If the argument is a pointer to a msghdr structure, this macro returns a
//pointer to the first cmsghdr structure in the ancillary data associated with
//this msghdr structure, or a null pointer if there is no ancillary data
//associated with the msghdr structure.
#define CMSG_FIRSTHDR(mhdr) { \
}

struct linger
{
  int l_onoff;
  int l_linger;
};

enum
{
  SOCK_STREAM = 0,
#define SOCK_STREAM     SOCK_STREAM
  SOCK_DGRAM,
#define SOCK_DGRAM      SOCK_DGRAM
  SOCK_SEQPACKET
#define SOCK_SEQPACKET  DOCK_SEQPACKET
};

#define SOL_SOCKET 65535


enum
{
  SO_ACCEPTCONN = 2,
#define SO_ACCEPTCONN   SO_ACCEPTCONN
  SO_BROADCAST = 32,
#define SO_BROADCAST    SO_BROADCAST
  SO_DEBUG = 1,
#define SO_DEBUG        SO_DEBUG
  SO_DONTROUTE = 16,
#define SO_DONTROUTE    SO_DONTROUTE
  SO_ERROR = 4103,
#define SO_ERROR        SO_ERROR
  SO_KEEPALIVE = 8,
#define SO_KEEPALIVE    SO_KEEPALIVE
  SO_LINGER = 128,
#define SO_LINGER       SO_LINGER
  SO_OOBLINLINE = 256,
#define SO_OOBINLINE    SO_OOBINLINE
  SO_RCVBUF = 4098,
#define SO_RCVBUF       SO_RCVBUF
  SO_RCVLOWAT = 4100,
#define SO_RCVLOWAT     SO_RCVLOWAT
  SO_RCVTIMEO = 4102,
#define SO_RCVTIMEO     SO_RCVTIMEO
  SO_REUSEADDR = 4,
#define SO_REUSEADDR    SO_REUSEADDR
  SO_SNDBUF = 4097,
#define SO_SNDBUF       SO_SNDBUF
  SO_SNDLOWAT = 4099,
#define SO_SNDLOWAT     SO_SNDLOWAT
  SO_SNDTIMEO = 4101,
#define SO_SNDTIMEO     SO_SNDTIMEO
  SO_TYPE = 4104
#define SO_TYPE         SO_TYPE
};

enum
{
  MSG_CTRUNC = 8,
#define MSG_CTRUNC    MSG_CTRUNC
  MSG_DONTROUTE = 4,
#define MSG_DONTROUTE MSG_DONTROUTE
  MSG_EOR = 128,
#define MSG_EOR       MSG_EOR
  MSG_OOB = 1,
#define MSG_OOB       MSG_OOB
  MSG_PEEK = 2,
#define MSG_PEEK      MSG_PEEK
  MSG_TRUNC = 32,
#define MSG_TRUNC     MSG_TRUNC
  MSG_WAITALL = 256
#define MSG_WAITALL   MSG_WAITALL
};

enum
{
  AF_UNSPEC = 0,
#define AF_UNSPEC   AF_UNSPEC
  AF_UNIX,
#define AF_UNIX     AF_UNIX
  AF_INET
#define AF_INET     AF_INET
};

enum
{
  SHUT_RD = 0,          /* No more receptions.  */
#define SHUT_RD         SHUT_RD
  SHUT_WR,              /* No more transmissions.  */
#define SHUT_WR         SHUT_WR
  SHUT_RDWR             /* No more receptions or transmissions.  */
#define SHUT_RDWR       SHUT_RDWR
};


//  END ADDITIONS  //

//  Begin my Socket class  //

#define MAX_SOCKETS 10

//enum ConnectionState
//{
//  CONNECTION_CLOSED,
//  CONNECTION_BOUND,
//  CONNECTION_LISTENING,
//  CONNECTION_SENDING,
//  CONENCTION_RECEIVEING,
//  CONNECTION_CLOSING,
//  CONNECTION_ERROR
//}

namespace AiboSocket {
  extern void init();
  extern void closeAll();
  extern void destroyAll();
  
# define __SOCKADDR_ARG         struct sockaddr *__restrict
# define __CONST_SOCKADDR_ARG   __const struct sockaddr *

#define __THROW 


/* Create a new socket of type TYPE in domain DOMAIN, using
   protocol PROTOCOL.  If PROTOCOL is zero, one is chosen automatically.
   Returns a file descriptor for the new socket, or -1 for errors.  */
extern int socket (int __domain, int __type, int __protocol);

/* Create two new sockets, of type TYPE in domain DOMAIN and using
   protocol PROTOCOL, which are connected to each other, and put file
   descriptors for them in FDS[0] and FDS[1].  If PROTOCOL is zero,
   one will be chosen automatically.  Returns 0 on success, -1 for errors.  */
extern int socketpair (int __domain, int __type, int __protocol,
                       int __fds[2]);

/* Give the socket FD the local address ADDR and port PORT.  */
extern int bind (int __fd, IPAddress __addr, Port __port);

/* Open a connection on socket FD to peer at ADDR and port PORT.
   For connectionless socket types, just set the default address to send to
   and the only address from which to accept transmissions.
   Return 0 on success, -1 for errors.

   This function is a cancellation point and therefore not marked with
   __THROW.  */
extern int connect (int __fd, IPAddress __addr, Port __port);

/* Put the address and port of the peer connected to socket FD into *ADDR and
   *PORT, respectively.  */
extern int getpeername (int __fd, IPAddress *__addr, Port *__port);

/* Put the local address and port of FD into *ADDR and *PORT, respectively.  */
extern int getsockname (int __fd, IPAddress *address, Port *port);

/* Send N bytes of BUF to socket FD.  Returns the number sent or -1.

   This function is a cancellation point and therefore not marked with
   __THROW.  */
extern ssize_t send (int __fd, __const void *__buf, size_t __n, int __flags);

/* Send N bytes of BUF to socket FD, asynchronously.  
   
   Function returns immediately. If a pre-execution error is detected, returns 
   -1.  SELECTOR is notified of result of IO operation, which includes amount of 
   data sent.  */
extern int send_asynch (int _fd, __const void *__buf, size_t __n, int __flags,
    int32 __selector);

/* Read N bytes into BUF from socket FD.
   Returns the number read or -1 for errors.

   This function is a cancellation point and therefore not marked with
   __THROW.  */
extern ssize_t recv (int __fd, void *__buf, size_t __n, int __flags);

/* Recv N bytes from socket FD, asynchronously.
   
   Function returns immediately.  If a pre-execution error is detected, returns
   -1.  SELECTOR is notified of result of IO operation, which includes received
   data.  */
extern int recv_asynch (int _fd, size_t __n, int __flags, int32 __selector);

/* Send N bytes of BUF on socket FD to peer at address ADDR (which is
   ADDR_LEN bytes long).  Returns the number sent, or -1 for errors.

   This function is a cancellation point and therefore not marked with
   __THROW.  */
extern ssize_t sendto (int __fd, __const void *__buf, size_t __n,
                       int __flags, IPAddress __addr, Port __port);

/* Send N bytes of BUF to socket FD to peer at address ADDR and port PORT, 
   asynchronously.  
   
   Function returns immediately. If a pre-execution error is detected, returns 
   -1.  SELECTOR is notified of result of IO operation, which includes amount of
   data sent.  */
extern int sendto_asynch (int _fd, __const void *__buf, size_t __n,
                           int __flags, IPAddress __addr, Port __port,
                           int32 __selector);

/* Read N bytes into BUF through socket FD.
   If ADDR is not NULL and/or PORT is not NULL, store the ADDR and PORT,
   respectively, of the sender.  Returns the number of bytes read or -1 for
   errors.

   This function is a cancellation point and therefore not marked with
   __THROW.  */
extern ssize_t recvfrom (int __fd, void *__restrict __buf, size_t __n,
                         int __flags, IPAddress *address, Port *port);

/* Send a message described MESSAGE on socket FD.
   Returns the number of bytes sent, or -1 for errors.

   This function is a cancellation point and therefore not marked with
   __THROW.  */
//extern ssize_t sendmsg (int __fd, __const struct msghdr *__message,
//                        int __flags);

/* Receive a message as described by MESSAGE from socket FD.
   Returns the number of bytes read or -1 for errors.

   This function is a cancellation point and therefore not marked with
   __THROW.  */
//extern ssize_t recvmsg (int __fd, struct msghdr *__message, int __flags);


/* Put the current value for socket FD's option OPTNAME at protocol level LEVEL
   into OPTVAL (which is *OPTLEN bytes long), and set *OPTLEN to the value's
   actual length.  Returns 0 on success, -1 for errors.  */
extern int getsockopt (int __fd, int __level, int __optname,
                       void *__restrict __optval,
                       socklen_t *__restrict __optlen);

/* Set socket FD's option OPTNAME at protocol level LEVEL
   to *OPTVAL (which is OPTLEN bytes long).
   Returns 0 on success, -1 for errors.  */
extern int setsockopt (int __fd, int __level, int __optname,
                       __const void *__optval, socklen_t __optlen);

/* Set socket PD's blocking mode to BLOCKING.
   Returns 0 on success, -1 for error.   */
extern int setblocking (int __fd, bool __blocking);


/* Prepare to accept connections on socket FD.
   N connection requests will be queued before further requests are refused.
   Returns 0 on success, -1 for errors.  */
extern int listen (int __fd, int __n);

/* Await a connection on socket FD.
   When a connection arrives, open a new socket to communicate with it,
   set *ADDR (which is *ADDR_LEN bytes long) to the address of the connecting
   peer and *ADDR_LEN to the address's actual length, and return the
   new socket's descriptor, or -1 for errors.

   This function is a cancellation point and therefore not marked with
   __THROW.  */
extern int accept (int __fd);

/* Shut down all or part of the connection open on socket FD.
   HOW determines what to shut down:
     SHUT_RD   = No more receptions;
     SHUT_WR   = No more transmissions;
     SHUT_RDWR = No more receptions or transmissions.
   Returns 0 on success, -1 for errors.  */
extern int shutdown (int __fd, int __how);

/* Close the connection on a given socket completely
   */
extern int close (int __fd);

/* Perfrom a DNS query to determine the host information specified by name.
     On success, 'address' contains the IPAddress of the host, 'n_name' contains
     the number of alias specific to this host, and 'n_addr' contains the number
     of addresses associated with this host. If the name specified is not the 
     official name of the host, 'name' will contain the official name.  
     Otherwise, it will be truncated to the maximum name allowed on this system.
   Returns 0 on success, -1 for error.  */
extern int gethostbyname(char **name, IPAddress *address, int *n_name,
    int *n_addr);

/* Perfrom a DNS query to determine the host information specified by address.
     On success, 'name' contains the official name of the host, 'n_name' 
     contains the number of alias specific to this host, and 'n_addr' contains
     the number of addresses associated with this host.
   Returns 0 on success, -1 for error.  */
extern int gethostbyaddr(IPAddress *address, char **name, int *n_name,
    int *n_addr);

/* Get the DNS server addresses
     On success, ADDRESSES will point to a valid array of type IPAddress, and
     NSCOUNT will be set to the length of this array.
   Returns 0 on success, -1 for error.  */
extern int getservers(IPAddress **addresses, int *nscount);

/* Set the DNS server addresses
     On success, the list of IPAddresses, to the given number of servers, will
     be used in order for all future DNS queries.
   Return 0 on success, -1 for error.  */
extern int setservers(IPAddress *addresses, int nscount);

} // AiboSocket namespace

#endif /* socket.h */

