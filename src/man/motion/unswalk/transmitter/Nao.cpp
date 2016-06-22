#include "Nao.hpp"
#include "blackboard/Blackboard.hpp"
#include "utils/Logger.hpp"
#include "utils/speech.hpp"

using namespace boost::asio;
using namespace std;

NaoTransmitter::NaoTransmitter(int port, string address)
   : service(),
     socket(service, ip::udp::v4()),
     broadcast_endpoint(ip::address::from_string(address), port) {
   socket_base::broadcast option(true);
   socket.set_option(option);
   boost::system::error_code ec;
   socket.connect(broadcast_endpoint, ec);
   if (ec) {
      llog(ERROR) << "could not connect: " << ec.message();
      sleep(5);
      SAY("not on the network", true);
      throw ec;
   }
   llog(INFO) << "Nao Transmitter constructed" << endl;
}

NaoTransmitter::~NaoTransmitter() {
   socket.close();
}

void NaoTransmitter::tick(const boost::asio::mutable_buffers_1 &b) {
   boost::system::error_code ec = boost::system::error_code();
   socket.send(b, 0, ec);

   if (ec) {
      llog(ERROR) << "could not send: " << ec.message();
      static time_t lastsaid = 0;
      time_t now = time(NULL);
      if (now >= lastsaid + 5) {
         SAY("nao transmitter " + ec.message());
         lastsaid = now;
      }
   }
}
