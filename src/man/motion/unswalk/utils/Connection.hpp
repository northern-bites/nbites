// Based on http://www.boost.org/doc/libs/1_40_0/doc/html/boost_asio/example/serialization/Connection.hpp
//
// Connection.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>

/// The size of a fixed length header.
#define kHeaderLength 8

/// The Connection class provides serialization primitives on top of a socket.
/**
 * Each message sent using this class consists of:
 * @li An 8-byte header containing the length of the serialized data in
 * hexadecimal.
 * @li The serialized data.
 */
class Connection {
   public:
      /// Constructor.
      explicit Connection(boost::asio::io_service* io_service);

      /// Get the underlying socket. Used for making a Connection or for accepting
      /// an incoming Connection.
      boost::asio::ip::tcp::socket& socket();

      /// Asynchronously write a data structure to the socket.
      template <typename T, typename Handler>
      void async_write(const T& t, Handler handler);

      /// Asynchronously read a data structure from the socket.
      template <typename T, typename Handler>
      void async_read(T& t, Handler handler);

      /// Synchronously write a data structure to the socket.
      template <typename T> boost::system::error_code sync_write(const T& t);

      /// Synchronously read a data structure from the socket.
      template <typename T> boost::system::error_code sync_read(T& t);

      /// Handle a completed read of a message header. The handler is passed using
      /// a tuple since boost::bind seems to have trouble binding a function object
      /// created using boost::bind as a parameter.
      template <typename T, typename Handler>
      void handle_read_header(const boost::system::error_code& e,
                              T& t, boost::tuple<Handler> handler);

      /// Handle a completed read of message data.
      template <typename T, typename Handler>
      void handle_read_data(const boost::system::error_code& e,
                            T& t, boost::tuple<Handler> handler);

   private:
      /// The underlying socket.
      boost::asio::ip::tcp::socket socket_;

      /// Holds an outbound header.
      std::string outbound_header_;

      /// Holds the outbound data.
      std::string outbound_data_;

      /// Holds an inbound header.
      char inbound_header_[kHeaderLength * 2];

      /// Holds the inbound compressed data.
      std::vector<char> inbound_compressed_data_;

      /// Holds the inbound data.
      std::vector<char> inbound_data_;
};

typedef boost::shared_ptr<Connection> Connection_ptr;

#include "Connection.tcc"
