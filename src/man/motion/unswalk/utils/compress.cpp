// from http://old.nabble.com/boost::iostreams-zlib-compression-td15815415.html

#include "compress.hpp"
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <algorithm>

void add_compressor
   (boost::iostreams::filtering_streambuf<boost::iostreams::output>& out,
   CompressionScheme scheme) {
   switch (scheme) {
   case ZLIB_COMPRESSION:
      out.push(boost::iostreams::zlib_compressor());
      break;
   case BZIP2_COMPRESSION:
      out.push(boost::iostreams::bzip2_compressor());
      break;
   default:
      break;
   }
}

//----------------------------------------------------------------------------
void add_decompressor(
      boost::iostreams::filtering_streambuf<boost::iostreams::input>& in,
      CompressionScheme scheme)
{
   switch (scheme) {
   case ZLIB_COMPRESSION:
      in.push(boost::iostreams::zlib_decompressor());
      break;
   case BZIP2_COMPRESSION:
      in.push(boost::iostreams::bzip2_decompressor());
      break;
   default:
      break;
   }
}

void compress(const std::string& data, std::string& buffer,
              CompressionScheme scheme) {
   buffer.clear();
   boost::iostreams::filtering_streambuf<boost::iostreams::output> out;
   add_compressor(out, scheme);
   out.push(boost::iostreams::back_inserter(buffer));
   boost::iostreams::copy(boost::make_iterator_range(data), out);
}


void decompress(const std::string& buffer, std::string& data,
                CompressionScheme scheme) {
   boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
   add_decompressor(in, scheme);
   in.push(boost::make_iterator_range(buffer));
   data.clear();
   boost::iostreams::copy(in, boost::iostreams::back_inserter(data));
}
