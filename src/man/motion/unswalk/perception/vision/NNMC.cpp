#include <stdexcept>
#include <stdio.h>
#include <bzlib.h>

#include "NNMC.hpp"
#include "utils/Logger.hpp"

static const char bzip_magic[] = {'B', 'Z', 'h'};

/* #include "utils/bzip_compress.hpp" */

NNMC::NNMC()
{
}

void NNMC::load(const char *filename)
{
   size_t nnmc_size = MAXY * MAXU * MAXV;
   nnmc = boost::shared_array<uint8_t>(new uint8_t[nnmc_size]);
   /* bunzip.str().copy((char *)nnmc.get(), nnmc_size); */
   
   FILE *f = fopen(filename, "rb");

   char magic[ sizeof (bzip_magic) ];
   if (fread(magic, 1, sizeof(magic), f) != sizeof(magic)) {
      throw std::runtime_error("error openning nnmc file");
   }
   fseek(f, 0, SEEK_SET);

   /* Note: as our calibration files only contains bytes in [0..9] it
    * is impossible to have a false magic number on an uncompressed file
    */
   if (memcmp(magic, bzip_magic, sizeof(magic)) == 0) {
      /* File is bzip'd */
      int bzerror;
      BZFILE *bf = BZ2_bzReadOpen(&bzerror, f, 0, 0, 0, 0);
      if (! bzerror == BZ_OK) {
         throw std::runtime_error("error openning nnmc file for decompression");
      }
      BZ2_bzRead (&bzerror, bf, nnmc.get(), nnmc_size);
      if (! bzerror == BZ_OK && ! bzerror == BZ_STREAM_END) {
         throw std::runtime_error("error decompressing nnmc file");
      }
      BZ2_bzReadClose(&bzerror, bf);
      if (! bzerror == BZ_OK) {
         throw std::runtime_error("error closing nnmc file");
      }
   } else {
      /* Read uncompressed file, faster loading */
      size_t ret;
      ret = fread(nnmc.get(), nnmc_size, 1, f);
   }
   fclose(f);
}

void NNMC::unload()
{
   nnmc.reset(NULL);
}

bool NNMC::isLoaded() const
{
   return nnmc.get() != NULL;
}


