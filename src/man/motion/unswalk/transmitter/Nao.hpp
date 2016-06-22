#pragma once

#include <boost/asio/buffer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/udp.hpp>
#include <string>

class NaoTransmitter {
   protected:
      /**
       * Constructor.  Opens a socket for sending.
       *
       * @param port the port to send to
       * @param address the broadcast address
       */
      NaoTransmitter(int port, std::string address);

      /**
       * Destructor. Closes the socket.
       */
      virtual ~NaoTransmitter();

      /**
       * One cycle of this thread
       *
       * @param b the boost buffer to send
       */
      void tick(const boost::asio::mutable_buffers_1 &b);

   private:
      boost::asio::io_service service;
      boost::asio::ip::udp::socket socket;
      boost::asio::ip::udp::endpoint broadcast_endpoint;
};
