#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <zlib.h>
#include <iostream>
#include "utils/Logger.hpp"

template <typename T, typename Handler>
void Connection::async_write(const T& t, Handler handler) {
   // Serialize the data first so we know how large it is.
   std::ostringstream archive_stream;
   boost::archive::binary_oarchive archive(archive_stream);
   archive << BOOST_SERIALIZATION_NVP(t);
   outbound_data_ = archive_stream.str();
   llog(DEBUG1) << outbound_data_.size();

   // Compress it
   uLongf size = 1024 * 1024;
   static Bytef zlibOutput[1024 * 1024];
   if (compress2(zlibOutput, &size, outbound_data_.data(),
                 outbound_data_.size(), Z_BEST_SPEED) != Z_OK ||
       size >= outbound_data_.size()) {
      size = 0;
   }

   // Format the header.
   std::ostringstream header_stream;
   header_stream << std::setw(kHeaderLength) << std::hex << size <<
   std::setw(kHeaderLength) << outbound_data_.size();
   if (!header_stream || header_stream.str().size() != kHeaderLength * 2) {
      // Something went wrong, inform the caller.
      boost::system::error_code error(boost::asio::error::invalid_argument);
      socket_.get_io_service().post(boost::bind(handler, error));
      return;
   }
   outbound_header_ = header_stream.str();

   // Write the serialized data to the socket. We use "gather-write" to send
   // both the header and the data in a single write operation.
   std::vector<boost::asio::const_buffer> buffers;
   buffers.push_back(boost::asio::buffer(outbound_header_));
   if (size)
      buffers.push_back(boost::asio::buffer(zlibOutput, size));
   else
      buffers.push_back(boost::asio::buffer(outbound_data_));
   llog(DEBUG1) << outbound_header_ << std::endl;
   boost::asio::async_write(socket_, buffers, handler);
}

template <typename T, typename Handler>
void Connection::async_read(T& t, Handler handler) {
   // Issue a read operation to read exactly the number of bytes in a header.
   void (Connection::*f)(
      const boost::system::error_code &,
      T &, boost::tuple<Handler>)
      = &Connection::handle_read_header<T, Handler>;
   boost::asio::async_read(socket_, boost::asio::buffer(inbound_header_),
                           boost::bind(f,
                                       this, boost::asio::placeholders::error, boost::ref(t),
                                       boost::make_tuple(handler)));
}

template <typename T>
boost::system::error_code Connection::sync_write(const T& t) {
   // Serialize the data first so we know how large it is.
   std::ostringstream archive_stream;
   boost::archive::binary_oarchive archive(archive_stream);
   archive << BOOST_SERIALIZATION_NVP(t);
   outbound_data_ = archive_stream.str();
   llog(DEBUG1) << outbound_data_.size();

   // Compress it
   uLongf size = 1024 * 1024;
   static Bytef zlibOutput[1024 * 1024];
   if (compress2(zlibOutput, &size, (const Bytef *)outbound_data_.data(),
                 outbound_data_.size(), Z_BEST_SPEED) != Z_OK ||
       size >= outbound_data_.size()) {
      size = 0;
   }

   // Format the header.
   std::ostringstream header_stream;
   header_stream << std::setw(kHeaderLength) << std::hex << size <<
   std::setw(kHeaderLength) << outbound_data_.size();
   if (!header_stream || header_stream.str().size() != kHeaderLength * 2) {
      // Something went wrong, inform the caller.
      llog(ERROR) << "Header only contains \"" << header_stream << "\"" <<
      std::endl;
      boost::system::error_code error(boost::asio::error::invalid_argument);
      return error;
   }
   outbound_header_ = header_stream.str();

   // Write the serialized data to the socket. We use "gather-write" to send
   // both the header and the data in a single write operation.
   std::vector<boost::asio::const_buffer> buffers;
   buffers.push_back(boost::asio::buffer(outbound_header_));
   if (size)
      buffers.push_back(boost::asio::buffer(zlibOutput, size));
   else
      buffers.push_back(boost::asio::buffer(outbound_data_));
   llog(DEBUG1) << outbound_header_ << std::endl;
   // TODO(jayen): check for errors
   boost::asio::write(socket_, buffers);
   return boost::system::errc::make_error_code(boost::system::errc::success);
}

template <typename T>
boost::system::error_code Connection::sync_read(T& t) {
   // Issue a read operation to read exactly the number of bytes in a header.
   // TODO(jayen): check for errors
   boost::asio::read(socket_, boost::asio::buffer(inbound_header_));
   // Determine the length of the serialized data.
   std::istringstream is(std::string(inbound_header_, kHeaderLength * 2));
   std::size_t inbound_compressed_size = 0;
   std::size_t inbound_data_size = 0;
   if (!(is >> std::hex >> inbound_compressed_size >> inbound_data_size)) {
      // Header doesn't seem to be valid. Inform the caller.
      boost::system::error_code error(boost::asio::error::invalid_argument);
      return error;
   }
   llog(DEBUG1) << inbound_compressed_size << inbound_data_size << std::endl;

   // Start a synchronous call to receive the data.
   inbound_data_.resize(inbound_data_size);
   if (inbound_compressed_size == 0) {
      // TODO(jayen): check for errors
      boost::asio::read(socket_, boost::asio::buffer(inbound_data_));
   } else {
      inbound_compressed_data_.resize(inbound_compressed_size);
      // TODO(jayen): check for errors
      boost::asio::read(socket_, boost::asio::buffer(inbound_compressed_data_));

      // Decompress it
      Bytef *zlibOutput = (Bytef *)&inbound_compressed_data_[0];
      uLongf size = inbound_compressed_size;
      uLongf destLen = inbound_data_size;
      int zStatus = uncompress((Bytef *)&inbound_data_[0], &destLen, zlibOutput,
                               size);
      if (zStatus != Z_OK || destLen != inbound_data_size)
         llog(ERROR) << "failed to decompress to " << inbound_data_size <<
         " bytes: " << zError(zStatus) << std::endl;
   }

   // Extract the data structure from the data just received.
   try {
      std::string archive_data(&inbound_data_[0], inbound_data_.size());
      llog(DEBUG1) << std::string(inbound_header_, kHeaderLength * 2) <<
      std::endl;
      std::istringstream archive_stream(archive_data);
      boost::archive::binary_iarchive archive(archive_stream);
      archive >> BOOST_SERIALIZATION_NVP(t);
   } catch(std::exception & e) {
      // Unable to decode data.
      boost::system::error_code error(boost::asio::error::invalid_argument);
      return error;
   }
   return boost::system::errc::make_error_code(boost::system::errc::success);
}

template <typename T, typename Handler>
void Connection::handle_read_header(const boost::system::error_code& e,
                                    T& t, boost::tuple<Handler> handler) {
   if (e) {
      boost::get<0>(handler) (e);
   } else {
      // Determine the length of the serialized data.
      std::istringstream is(std::string(inbound_header_, kHeaderLength * 2));
      std::size_t inbound_compressed_size = 0;
      std::size_t inbound_data_size = 0;
      if (!(is >> std::hex >> inbound_compressed_size >> inbound_data_size)) {
         // Header doesn't seem to be valid. Inform the caller.
         boost::system::error_code error(boost::asio::error::invalid_argument);
         boost::get<0>(handler) (error);
         return;
      }
      llog(DEBUG1) << inbound_compressed_size << inbound_data_size << std::endl;

      // Start an asynchronous call to receive the data.
      inbound_compressed_data_.resize(inbound_compressed_size);
      inbound_data_.resize(inbound_data_size);
      void (Connection::*f)(const boost::system::error_code &, T &,
                            boost::tuple<Handler>) =
         &Connection::handle_read_data<T, Handler>;
      if (inbound_compressed_size)
         boost::asio::async_read(socket_,
                                 boost::asio::buffer(inbound_compressed_data_),
                                 boost::bind(f, this,
                                             boost::asio::placeholders::error,
                                             boost::ref(t), handler));
      else
         boost::asio::async_read(socket_, boost::asio::buffer(inbound_data_),
                                 boost::bind(f, this,
                                             boost::asio::placeholders::error,
                                             boost::ref(t), handler));
   }
}

template <typename T, typename Handler>
void Connection::handle_read_data(const boost::system::error_code& e,
                                  T& t, boost::tuple<Handler> handler) {
   if (e) {
      boost::get<0>(handler) (e);
   } else {
      if (inbound_compressed_data_.size()) {
         // Decompress it
         Bytef *zlibOutput = (Bytef *)&inbound_compressed_data_[0];
         uLongf size = inbound_compressed_data_.size();
         uLongf destLen = inbound_data_.size();
         int zStatus = uncompress((Bytef *)&inbound_data_[0], &destLen,
                                  zlibOutput, size);
         if (zStatus != Z_OK || destLen != inbound_data_.size())
            llog(ERROR) << "failed to decompress to " << inbound_data_.size() <<
            " bytes: " << zError(zStatus) << std::endl;
      }

      // Extract the data structure from the data just received.
      try {
         std::string archive_data(&inbound_data_[0], inbound_data_.size());
         llog(DEBUG1) << std::string(inbound_header_, kHeaderLength * 2) <<
         std::endl;
         std::istringstream archive_stream(archive_data);
         boost::archive::binary_iarchive archive(archive_stream);
         archive >> BOOST_SERIALIZATION_NVP(t);
      } catch(std::exception & e) {
         // Unable to decode data.
         boost::system::error_code error(boost::asio::error::invalid_argument);
         boost::get<0>(handler) (error);
         return;
      }

      // Inform caller that data has been received ok.
      boost::get<0>(handler) (e);
   }
}
