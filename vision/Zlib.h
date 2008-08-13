
#ifndef ZLIB_H_NORTHERN_BITES
#define ZLIB_H_NORTHERN_BITES

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "debug.h"
#include "zlib/zlib.h"

class Zlib {
 private:
  static void zerr(int ret);
 public:
  static unsigned char* readCompressedFile(FILE *in, int& outLength);
  static bool writeCompressedFile(FILE *out, const unsigned char *in, int inLength);
};

#endif // ZLIB_H_NORTHERN_BITES
