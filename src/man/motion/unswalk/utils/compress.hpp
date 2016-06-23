// from http://old.nabble.com/boost::iostreams-zlib-compression-td15815415.html
#include <string>

enum CompressionScheme {
   NO_COMPRESSION,
   ZLIB_COMPRESSION,
   BZIP2_COMPRESSION
};

void compress(const std::string& data, std::string& buffer,
              CompressionScheme scheme);

void decompress(const std::string& buffer, std::string& data,
                CompressionScheme scheme);
