/*
   Created by Jeremy Fishman, Winter 2006
   for use by Bowdoin College's Northern Bites RoboCup Team
*/

#ifndef SOCKET_MODULE
#define SOCKET_MODULE 1

#include <Python.h>
#ifndef NO_SOCKET
#  include "socket.h"
#endif

#ifndef PyMODINIT_FUNC
#  define PyMODINIT_FUNC void
#endif

PyMODINIT_FUNC init_socket(void);

#ifndef SOCKET_H

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

#endif /* socket.h */

typedef struct {
  PyObject_HEAD
  int sockn;
  /* other stuff */
} Socket;

extern PyObject * Socket_New(int family, int type, int protocol);
extern PyObject * Socket_accept(Socket *self, PyObject *args);
extern PyObject * Socket_bind(Socket *self, PyObject *args);
extern PyObject * Socket_close(Socket *self, PyObject *args);
extern PyObject * Socket_connect(Socket *self, PyObject *args);
extern PyObject * Socket_connect_ex(Socket *self, PyObject *args);
//extern PyObject * Socket_dup(Socket *self, PyObject *args);
extern PyObject * Socket_fileno(Socket *self, PyObject *args);
extern PyObject * Socket_getpeername(Socket *self, PyObject *args);
extern PyObject * Socket_getsockname(Socket *self, PyObject *args);
extern PyObject * Socket_getsockopt(Socket *self, PyObject *args);
extern PyObject * Socket_gettimeout(Socket *self, PyObject *args);
extern PyObject * Socket_listen(Socket *self, PyObject *args);
//extern PyObject * Socket_makefile(Socket *self, PyObject *args);
extern PyObject * Socket_recv(Socket *self, PyObject *args);
extern PyObject * Socket_recvfrom(Socket *self, PyObject *args);
extern PyObject * Socket_sendall(Socket *self, PyObject *args);
extern PyObject * Socket_send(Socket *self, PyObject *args);
extern PyObject * Socket_sendto(Socket *self, PyObject *args);
extern PyObject * Socket_setblocking(Socket *self, PyObject *args);
extern PyObject * Socket_setsockopt(Socket *self, PyObject *args);
extern PyObject * Socket_settimeout(Socket *self, PyObject *args);
extern PyObject * Socket_shutdown(Socket *self, PyObject *args);


extern PyObject * socket_gethostbyname(PyObject *self, PyObject *args);
extern PyObject * socket_gethostbyaddr(PyObject *self, PyObject *args);
extern PyObject * socket_getservers(PyObject *self, PyObject *args);
extern PyObject * socket_setservers(PyObject *self, PyObject *args);

#endif /* _socketmodule.h */
