
#include <errno.h>      // errno
#include <string.h>     // strerror(), memcpy()
#include <stdio.h>
#include <Common.h>
#include "CommDef.h"

#include <sys/socket.h> // socket(), connect(), send(), recv(), setsockopt()
#include <unistd.h>     // close()
#include <arpa/inet.h>  // inet_aton(), htonl(), htons()
#include <netdb.h>      // gethostbyname()d

//#include <fcntl.h>      // fcntl()

#include "DataSerializer.h"


using namespace std;

DataSerializer::DataSerializer () throw(socket_error&)
  : bind_sockn(-1), sockn(-1), blocking(true)
{
}

DataSerializer::~DataSerializer()
{
  closeAll();
}

void
DataSerializer::accept () throw(socket_error&)
{
  sockn = SOCKETNS::accept(bind_sockn, NULL, NULL);
  if (sockn == -1) {
    close();
    socket_error e(__FILE__, __LINE__, errno);
    throw e;
    //throw SOCKET_ERROR(errno);
  }
}

void
DataSerializer::bind () throw(socket_error&)
{

  // create socket
  bind_sockn = SOCKETNS::socket(AF_INET, SOCK_STREAM, 0);
  if (bind_sockn == -1) {
    close();
    throw SOCKET_ERROR(errno);
  }


  // set bind address parameters
  struct sockaddr_in bind_addr;
  bind_addr.sin_family = AF_INET;
  bind_addr.sin_port = htons(TCP_PORT);
  bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  // bind socket to address
  if (SOCKETNS::bind(bind_sockn, (const struct sockaddr*)&bind_addr,
        sizeof(bind_addr)) == -1 ||
      SOCKETNS::listen(bind_sockn, 10) == -1) {
    closeAll();
    throw SOCKET_ERROR(errno);
  }
}

/*
void
DataSerializer::connect (const char *addr) throw(socket_error&)
{
  struct sockaddr_in raw_addr;
  int result;

  // convert 'addr' to in_addr struct
  result = inet_aton(addr, &raw_addr.sin_addr);
  if (result == 0) {
    // 'addr' is not a valid address, attempt to resolve hostname
    hostent *host_info = gethostbyname(addr);
    if (host_info->h_addr_list == NULL || *(host_info->h_addr_list) != NULL)
      error(errno);

    // resolving worked, convert to in_addr struct
    addr = *(host_info->h_addr_list);
    result = inet_aton(addr, &raw_addr.sin_addr);
    if (result == 0) {
      close();
      throw SOCKET_ERROR(errno);
    }
  }

  // create socket
  sockn = SOCKETNS::socket(AF_INET, SOCK_STREAM, 0);
  if (sockn == -1) {
    close();
    throw SOCKET_ERROR(errno);
  }

  // set socket send and receive buffer sizes
  if (SOCKETNS::setsockopt(sockn, SOL_SOCKET, SO_SNDBUF, (void*)BUF_SIZE,
        sizeof(int)) != 0 ||
      SOCKETNS::setsockopt(sockn, SOL_SOCKET, SO_RCVBUF, (void*)BUF_SIZE,
        sizeof(int)) != 0) {
    close();
    throw SOCKET_ERROR(errno);
  }

  // connect socket to address
  raw_addr.sin_family = AF_INET;
  raw_addr.sin_port = TCP_PORT;
  if (SOCKETNS::connect(sockn, (struct sockaddr *)&raw_addr, sizeof raw_addr) == -1)
    error(errno);
}
*/

void
DataSerializer::close () throw()
{
  // attempt to close if we haven't already
  if (sockn != -1)
    SOCKETNS::close(sockn);
  // invalidate file descriptor
  sockn = -1;
}

void
DataSerializer::closeAll () throw()
{
  close();

  if (bind_sockn != -1)
    if (SOCKETNS::close(bind_sockn) == -1)
      fprintf(stderr, "Error closing socket: #%i - %s\n", errno,
          strerror(errno));
  bind_sockn = -1;
}

void
DataSerializer::setblocking (bool toBlock) throw(socket_error&)
{
  if (toBlock ^ blocking) {
      // TODO: WTF is this?
    fprintf(stderr, "Non-blocking sockets currently not supported on non-Aibo "
        "platforms\n");
  }
  blocking = toBlock;
}

bool
DataSerializer::bound () const
{
  return bind_sockn != -1;
}

bool
DataSerializer::connected () const
{
  return sockn != -1;
}

void
DataSerializer::write (const void *data, int len) throw(socket_error&)
{
  int wrote = 0, result;

  while (wrote < len) {
    result = SOCKETNS::send(sockn, ((byte*)data + wrote) , len - wrote, 0);

    if (result == -1) {
      if (blocking || errno != EAGAIN)
        close();
      throw SOCKET_ERROR(errno);
    }else if (result == 0) {
      close();
      throw SOCKET_ERROR(ERROR_NO_OUTPUT);
    }

    wrote += result;
  }
}

void
DataSerializer::read (void *data, int len) throw(socket_error&)
{
  int nread = 0, result;

  while (nread < len) {
    result = SOCKETNS::recv(sockn, ((byte*)data + nread), len - nread, 0);

    if (result == -1) {
      if (blocking || errno != EAGAIN)
        close();
      throw SOCKET_ERROR(errno);
    }else if (result == 0) {
      close();
      throw SOCKET_ERROR(ERROR_NO_INPUT);
    }

    nread += result;
  }
}


//
// Writing methods
//

void
DataSerializer::raw_write_int (int val) throw(socket_error&)
{
    buf[0] = byte((val >> 24) & 0xff);
    buf[1] = byte((val >> 16) & 0xff);
    buf[2] = byte((val >>  8) & 0xff);
    buf[3] = byte(val        & 0xff);

  write(&buf[0], SIZEOF_INT);
}

void
DataSerializer::raw_write_long (llong val) throw(socket_error&)
{
  buf[0] = (val >> 56) && 0xff;
  buf[1] = (val >> 48) && 0xff;
  buf[2] = (val >> 40) && 0xff;
  buf[3] = (val >> 32) && 0xff;
  buf[4] = (val >> 24) && 0xff;
  buf[5] = (val >> 16) && 0xff;
  buf[6] = (val >>  8) && 0xff;
  buf[7] = (val      ) && 0xff;

  write(&buf[0], SIZEOF_LLONG);
}

void
DataSerializer::write_array_header (byte type, int length) throw(socket_error&)
{
  write(&type, SIZEOF_BYTE);
  raw_write_int(length);
}

void
DataSerializer::write_int (int value) throw(socket_error&)
{
  write_array_header(TYPE_INT, value);
}

void
DataSerializer::write_byte (byte value) throw(socket_error&)
{
  buf[0] = TYPE_BYTE;
  buf[1] = value;

  write(&buf[0], 2 * SIZEOF_BYTE);
}

void
DataSerializer::write_float (float value) throw(socket_error&)
{
    int val = (int)value;
    buf[0] = TYPE_FLOAT;
    buf[1] = byte((val >> 24) & 0xff);
    buf[2] = byte((val >> 16) & 0xff);
    buf[3] = byte((val >>  8) & 0xff);
    buf[4] = byte(val        & 0xff);

    write(&buf[0], SIZEOF_BYTE + SIZEOF_FLOAT);
}

void
DataSerializer::write_double (double value) throw(socket_error&)
{
  llong val = (llong)value;
  buf[0] = TYPE_DOUBLE;
  buf[1] = (val >> 56) && 0xff;
  buf[2] = (val >> 48) && 0xff;
  buf[3] = (val >> 40) && 0xff;
  buf[4] = (val >> 32) && 0xff;
  buf[5] = (val >> 24) && 0xff;
  buf[6] = (val >> 16) && 0xff;
  buf[7] = (val >>  8) && 0xff;
  buf[8] = (val      ) && 0xff;

  write(&buf[0], SIZEOF_BYTE + SIZEOF_DOUBLE);
}

void
DataSerializer::write_ints (const int *data, int len) throw(socket_error&)
{
  write_array_header(TYPE_INT_ARRAY, len * SIZEOF_INT);

  for (int i = 0; i < len; i++)
    raw_write_int(data[i]);
}

void
DataSerializer::write_bytes (const byte *data, int len) throw(socket_error&)
{
  write_array_header(TYPE_BYTE_ARRAY, len * SIZEOF_BYTE);

  write(data, len * SIZEOF_BYTE);
}

void
DataSerializer::write_floats (const float *data, int len) throw(socket_error&)
{
  write_array_header(TYPE_FLOAT_ARRAY, len * SIZEOF_FLOAT);

  for (int i = 0; i < len; i++)
    raw_write_int(*((int*)&data[i]));
}

void
DataSerializer::write_doubles (const double *data, int len) throw(socket_error&)
{
  write_array_header(TYPE_DOUBLE_ARRAY, len * SIZEOF_DOUBLE);

  for (int i = 0; i < len; i++)
    raw_write_long(*((llong*)&data[i]));
}

void
DataSerializer::write_ints (std::vector<int> &v)
{
  write_ints(&v.front(), v.size());
}

void
DataSerializer::write_bytes (std::vector<byte> &v)
{
  write_bytes(&v.front(), v.size());
}

void
DataSerializer::write_floats (std::vector<float> &v)
{
  write_floats(&v.front(), v.size());
}

void
DataSerializer::write_doubles (std::vector<double> &v)
{
  write_doubles(&v.front(), v.size());
}

//
// Reading methods
//

int
DataSerializer::raw_read_int () throw(socket_error&)
{
  read(buf, SIZEOF_INT);
  return (buf[0] << 24) |
         (buf[1] << 16) |
         (buf[2] <<  8) |
         (buf[3]      );
}

llong
DataSerializer::raw_read_long () throw(socket_error&)
{
  read(buf, SIZEOF_LLONG);
  return (((llong)buf[0]) << 56) |
         (((llong)buf[1]) << 48) |
         (((llong)buf[2]) << 40) |
         (((llong)buf[3]) << 32) |
         (((llong)buf[4]) << 24) |
         (((llong)buf[5]) << 16) |
         (((llong)buf[6]) <<  8) |
         (((llong)buf[7])      );
}

void
DataSerializer::read_array_header (byte type, int length) throw(socket_error&)
{
  byte read_type;
  read(&read_type, SIZEOF_BYTE);

  if (read_type != type) {
    close();
    throw SOCKET_ERROR(ERROR_DATATYPE);
  }

  int read_len = raw_read_int();

  if (read_len != length) {
    close();
    fprintf(stderr, "Expected length %i, got length %i\n", length, read_len);
    throw SOCKET_ERROR(ERROR_DATASIZE);
  }
}

int
DataSerializer::read_int () throw(socket_error&)
{
  read(&buf[0], SIZEOF_BYTE + SIZEOF_INT);

  if (buf[0] != TYPE_INT) {
    close();
    throw SOCKET_ERROR(ERROR_DATATYPE);
  }

  return raw_read_int();
}

byte
DataSerializer::read_byte () throw(socket_error&)
{
  read(&buf[0], 2 * SIZEOF_BYTE);

  if (buf[0] != TYPE_BYTE) {
    close();
    throw SOCKET_ERROR(ERROR_DATATYPE);
  }
  
  return buf[SIZEOF_BYTE];
}

double
DataSerializer::read_double () throw(socket_error&)
{
  read(&buf[0], SIZEOF_BYTE + SIZEOF_DOUBLE);

  if (buf[0] != TYPE_DOUBLE) {
    close();
    throw SOCKET_ERROR(ERROR_DATATYPE);
  }

  return *((double*)&buf[SIZEOF_BYTE]);
}

void
DataSerializer::read_ints (int *data, int len) throw(socket_error&)
{
  read_array_header(TYPE_INT_ARRAY, len * SIZEOF_INT);

  for (int i = 0; i < len; i++)
    data[i] = raw_read_int();
}

void
DataSerializer::read_bytes (byte *data, int len) throw(socket_error&)
{
  read_array_header(TYPE_BYTE_ARRAY, len * SIZEOF_BYTE);

  read(data, len * SIZEOF_BYTE);
}

void
DataSerializer::read_floats (float *data, int len) throw(socket_error&)
{
  read_array_header(TYPE_FLOAT_ARRAY, len * SIZEOF_FLOAT);

  for (int i = 0; i < len; i++)
	  data[i] = static_cast<float>(raw_read_int());
}

void
DataSerializer::read_doubles (double *data, int len) throw(socket_error&)
{
  read_array_header(TYPE_DOUBLE_ARRAY, len * SIZEOF_DOUBLE);

  for (int i = 0; i < len; i++)
	  data[i] = static_cast<double>( raw_read_long() );
}


