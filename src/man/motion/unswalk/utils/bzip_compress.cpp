/* zpipe.c: example of proper use of zlib's inflate() and deflate()
   Not copyrighted -- provided to the public domain
   Version 1.4  11 December 2005  Mark Adler */

/* Version history:
   1.0  30 Oct 2004  First version
   1.1   8 Nov 2004  Add void casting for unused return values
   Use switch statement for inflate() return values
   1.2   9 Nov 2004  Add assertions to document zlib guarantees
   1.3   6 Apr 2005  Remove incorrect assertion in inf()
   1.4  11 Dec 2005  Add hack to avoid MSDOS end-of-line conversions
   Avoid some compiler warnings for input and output buffers
*/

#include "bzip_compress.hpp"
#include <stdexcept>

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "zlib.h"

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

#define CHUNK 16384

int bzip_deflate(std::istream &source, std::ostream &dest, int level)
{
   int ret, flush;
   unsigned have;
   z_stream strm;
   unsigned char in[CHUNK];
   unsigned char out[CHUNK];

   /* allocate deflate state */
   strm.zalloc = Z_NULL;
   strm.zfree = Z_NULL;
   strm.opaque = Z_NULL;
   ret = deflateInit(&strm, level);
   if (ret != Z_OK)
      return ret;

   /* compress until end of file */
   do {
      strm.avail_in = source.read((char *)in, CHUNK).gcount();
      if (source.fail()) {
         (void)deflateEnd(&strm);
         return Z_ERRNO;
      }
      flush = source.eof() ? Z_FINISH : Z_NO_FLUSH;
      strm.next_in = in;

      /* run deflate() on input until output buffer not full, finish
         compression if all of source has been read in */
      do {
         strm.avail_out = CHUNK;
         strm.next_out = out;
         ret = deflate(&strm, flush);    /* no bad return value */
         assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
         have = CHUNK - strm.avail_out;
         if (dest.write((char *)out, have).fail()) {
            (void)deflateEnd(&strm);
            return Z_ERRNO;
         }
      } while (strm.avail_out == 0);
      assert(strm.avail_in == 0);     /* all input will be used */

      /* done when last data in file processed */
   } while (flush != Z_FINISH);
   assert(ret == Z_STREAM_END);        /* stream will be complete */

   /* clean up and return */
   (void)deflateEnd(&strm);
   return Z_OK;
}

/* Decompress from file source to file dest until stream ends or EOF.
   inf() returns Z_OK on success, Z_MEM_ERROR if memory could not be
   allocated for processing, Z_DATA_ERROR if the deflate data is
   invalid or incomplete, Z_VERSION_ERROR if the version of zlib.h and
   the version of the library linked do not match, or Z_ERRNO if there
   is an error reading or writing the files. */
int bzip_inflate(std::istream &source, std::ostream &dest)
{
   int ret;
   unsigned have;
   z_stream strm;
   unsigned char in[CHUNK];
   unsigned char out[CHUNK];

   /* allocate inflate state */
   strm.zalloc = Z_NULL;
   strm.zfree = Z_NULL;
   strm.opaque = Z_NULL;
   strm.avail_in = 0;
   strm.next_in = Z_NULL;
   ret = inflateInit(&strm);
   if (ret != Z_OK)
      return ret;

   /* decompress until deflate stream ends or end of file */
   do {
      strm.avail_in = source.read((char *)in, CHUNK).gcount();
      if (source.fail()) {
         (void)inflateEnd(&strm);
         return Z_ERRNO;
      }
      if (strm.avail_in == 0)
         break;
      strm.next_in = in;

      /* run inflate() on input until output buffer not full */
      do {
         strm.avail_out = CHUNK;
         strm.next_out = out;
         ret = inflate(&strm, Z_NO_FLUSH);
         assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
         switch (ret) {
            case Z_NEED_DICT:
               ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
               (void)inflateEnd(&strm);
               return ret;
         }
         have = CHUNK - strm.avail_out;
         if (dest.write((char *)out, have).fail()) {
            (void)inflateEnd(&strm);
            return Z_ERRNO;
         }
      } while (strm.avail_out == 0);

      /* done when inflate() says it's done */
   } while (ret != Z_STREAM_END);

   /* clean up and return */
   (void)inflateEnd(&strm);
   return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

/* report a zlib or i/o error */
#if 0
static void zerr(int ret)
{
   std::string err("bzip2: ");
   switch (ret) {
      case Z_ERRNO:
         err += "error reading from stream\n";
         break;
      case Z_STREAM_ERROR:
         err += "invalid compression level\n";
         break;
      case Z_DATA_ERROR:
         err += "invalid or incomplete deflate data\n";
         break;
      case Z_MEM_ERROR:
         err += "out of memory\n";
         break;
      case Z_VERSION_ERROR:
         err += "zlib version mismatch!\n";
   }

   throw(std::runtime_error(err));
}
#endif

