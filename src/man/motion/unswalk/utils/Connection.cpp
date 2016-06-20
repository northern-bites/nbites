#include "utils/Connection.hpp"

Connection::Connection(boost::asio::io_service* io_service) : socket_(*io_service), inbound_compressed_data_(), inbound_data_() {}

boost::asio::ip::tcp::socket& Connection::socket() {
   return socket_;
}
