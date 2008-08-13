#include "Zlib.h"

void Zlib::zerr(int ret)
{
  print("zipe: ");
  switch (ret) {
  case Z_BUF_ERROR:
    print("not enough memory in buffer\n");
    break;
  case Z_STREAM_ERROR:
    print("invalid compression level\n");
    break;
  case Z_DATA_ERROR:
    print("invalid or incomplete deflate data\n");
    break;
  case Z_MEM_ERROR:
    print("out of memory");
    break;
  case Z_VERSION_ERROR:
    print("zlib version mismatch!\n");
  }
}

/* This method takes a file pointer that indicates where we're reading from.
   It needs to be initialized prior to calling this method!
   It also needs to be closed/dereferenced by the user, not by
   this method.
   The decompressed string is returned through the out parameter
   which does not need to be malloced prior to calling this method!
   outLength indicates the length of the decompressed string
   Returns true if successful
*/
unsigned char* Zlib::readCompressedFile(FILE *in, int& outLength) {
  int length = 0;
  while(!isspace(fgetc(in))) {
    length++;
  }
  fseek(in, 0, SEEK_SET);
  char *size = (char*)malloc((size_t)length+1);
  
  fread(size, length, 1, in);
  int decompressedSize = 0;
  sscanf(size, "%d", &decompressedSize);
  outLength = decompressedSize;
  free(size);

  fseek(in, 0, SEEK_END);
  // the size of the compressed string is the size of the file minus
  // the header which is a number and a space character
  unsigned long fileSize = ftell(in) - (length + 1);
  
  void *fileData = malloc(fileSize+1);
  fseek(in, length+1, SEEK_SET);
  fread(fileData, fileSize, 1, in);

  unsigned char *out = (unsigned char*)malloc((size_t)outLength);
  int ret = uncompress((Bytef*)out, (uLongf*)&outLength, 
		       (Bytef*)fileData, (uLongf)fileSize);
  if (ret != Z_OK) {
    zerr(ret);
    return false;
  }

  free(fileData);
  return out;
}

/* This method takes a file pointer that indicates where we're writing to
   It needs to be initialized prior to calling this method!
   It also needs to be closed/dereferenced by the user, not by this method
*/
bool Zlib::writeCompressedFile(FILE *out, const unsigned char *in, int inLength) {
  unsigned char *compressed = NULL;

  //unsigned long bufferSize = inLength + (unsigned long)(stringLength*0.1) + 12;
  unsigned long bufferSize = compressBound(inLength);

  compressed = (unsigned char*)malloc((size_t)bufferSize);
  int ret = compress((Bytef*)compressed, (uLongf*)&bufferSize,
		     (const Bytef*)in, (uLong)inLength);

  if (ret != Z_OK) {
    zerr(ret);
    return false;
  }
  char sizeString[32];
  sprintf(sizeString, "%d ", inLength);

  fwrite(sizeString, strlen(sizeString), 1, out);
  fwrite(compressed, bufferSize, 1, out);
  return true;
}
