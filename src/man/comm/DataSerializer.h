
#ifndef DataSerializer_H
#define DataSerializer_H

#include <exception>
#include <string.h>
#include <vector>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>       // sprintf()

#include "Common.h"
#include "CommDef.h"

//
// Constant definitions
//

#define SOCKETNS

//
// socket_error exception class definition
//

#define SOCKET_ERROR(arg) (socket_error(__FILE__, __LINE__, arg))

class socket_error: public std::exception
{
  public:
    socket_error(const socket_error& other) throw()
      : std::exception()
    {
      fname = other.fname;
      lineno = other.lineno;
      err = other.err;

      msg = (char *) malloc(strlen(other.msg));
      strcpy(msg, other.msg);
    }

    socket_error(const char *f, int l, int e) throw()
      : std::exception()
    {
      fname = f;
      lineno = l;
      err = e;
      
      build_message(strerror(e));
    }

    socket_error(const char *f, int l, const char *m) throw()
      : std::exception()
    {
      fname = f;
      lineno = l;
      err = -1;

      build_message(m);
    }
    ~socket_error() throw() {
      free(msg);
    }

    void build_message(const char *m) {
      //format:
      // [file ..., line ...] ... 
      // 123456   7890123   45     = 15 + 10 characters for line number
      int len = strlen(fname) + strlen(m) + 25;

      msg = (char*) malloc(len);
      sprintf(msg, "[file %s, line %d] %s", fname, lineno, m);
    }

    virtual const char* what() const throw() { 
      return msg;
    }

    const char* file() {
      return fname;
    }

    int line() {
      return lineno;
    }

    int error() {
      return err;
    }

  private:
    const char *fname;
    char *msg;
    int lineno;
    int err;
};


//
// DataSerializer class definition
//

class DataSerializer {
  public:
    DataSerializer() throw(socket_error&);
    ~DataSerializer();

    void accept  () throw(socket_error&);
    void bind    () throw(socket_error&);
    void close   () throw();
    void closeAll() throw();
    void setblocking(bool toBlock) throw(socket_error&);

    bool bound() const;
    bool connected() const;

    void write_int   (int value)    throw(socket_error&);
    void write_byte  (byte value)   throw(socket_error&);
    void write_float (float value)  throw(socket_error&);
    void write_double(double value) throw(socket_error&);

    void write_ints   (const int *data, int len)    throw(socket_error&);
    void write_bytes  (const byte *data, int len)   throw(socket_error&);
    void write_floats (const float *data, int len)  throw(socket_error&);
    void write_doubles(const double *data, int len) throw(socket_error&);

    void write_ints   (const int **data, int len1, int len2)
        throw(socket_error&);
    void write_bytes  (const byte **data, int len1, int len2)
        throw(socket_error&);
    void write_floats (const float **data, int len1, int len2)
        throw(socket_error&);
    void write_doubles(const double **data, int len1, int len2)
        throw(socket_error&);

    void write_ints   (std::vector<int> &v);
    void write_bytes  (std::vector<byte> &v);
    void write_floats (std::vector<float> &v);
    void write_doubles(std::vector<double> &v);

    int    read_int   () throw(socket_error&);
    byte   read_byte  () throw(socket_error&);
    float  read_float () throw(socket_error&);
    double read_double() throw(socket_error&);

    void read_ints   (int *data, int length)    throw(socket_error&);
    void read_bytes  (byte *data, int length)   throw(socket_error&);
    void read_floats (float *data, int length)  throw(socket_error&);
    void read_doubles(double *data, int length) throw(socket_error&);

    int read_ints   (int *data, int len, bool var_len)    throw(socket_error&);
    int read_bytes  (byte *data, int len, bool var_len)   throw(socket_error&);
    int read_floats (float *data, int len, bool var_len)  throw(socket_error&);
    int read_doubles(double *data, int len, bool var_len) throw(socket_error&);

    // not implemented
    void read_ints   (int **data, int len1, int len2)    throw(socket_error&);
    void read_bytes  (byte **data, int len1, int len2)   throw(socket_error&);
    void read_floats (float **data, int len1, int len2)  throw(socket_error&);
    void read_doubles(double **data, int len1, int len2) throw(socket_error&);

  private:
    void write_array_header(byte type, int length) throw(socket_error&);
    void read_array_header(byte type, int length)  throw(socket_error&);
    void read_array_header(byte type, int *length, bool varLength)
        throw(socket_error&);

    // blocking reads and writes
    void write(const void *data, int len) throw(socket_error&);
    void read (void *data, int len) throw(socket_error&);
    void  raw_write_int (int val)   throw(socket_error&);
    int   raw_read_int  ()          throw(socket_error&);
    void  raw_write_long(llong val) throw(socket_error&);
    llong raw_read_long ()          throw(socket_error&);

    int bind_sockn;
    int sockn;
    bool blocking;
    byte buf[9];
};


#endif /* DataSerializer_H */
