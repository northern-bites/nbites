#include "Team.hpp"
#include "blackboard/Blackboard.hpp"
#include "types/SPLStandardMessage.hpp"
#include "utils/incapacitated.hpp"
//#include "types/

#include <iostream>

using namespace boost::asio;
using namespace std;

TeamTransmitter::TeamTransmitter(Blackboard *bb) :
   Adapter(bb),
   NaoTransmitter((blackboard->config)["transmitter.port"].as<int>(),
                  (blackboard->config)["transmitter.address"].as<string>()),
   service(),
   socket(service, ip::udp::v4()),
   broadcast_endpoint(ip::address::from_string("255.255.255.255"),
      GAMECONTROLLER_PORT),
      delay(0)  {
   
   socket_base::broadcast option(true);
   socket.set_option(option);
   boost::system::error_code ec;
   socket.connect(broadcast_endpoint, ec);   
      
}

void TeamTransmitter::tick() {
   BroadcastData bd((blackboard->config)["player.number"].as<int>(),
                    (blackboard->config)["player.team"].as<int>(),
                    readFrom(localisation, robotPos),
                    readFrom(localisation, ballPos),
                    readFrom(localisation, ballPosRR),
                    readFrom(localisation, ballLostCount),
                    readFrom(localisation, sharedLocalisationBundle),
                    readFrom(behaviour, behaviourSharedData),
                    readFrom(motion, active).body.actionType,
                    readFrom(motion, uptime));
   
   // calculate incapacitated
   int playerNum = (blackboard->config)["player.number"].as<int>();
   bool incapacitated = false;
   if (readFrom(gameController, our_team).players[playerNum - 1].penalty
       != PENALTY_NONE) {
      incapacitated = true;
   }

   const ActionCommand::Body::ActionType &acB =
            readFrom(motion, active).body.actionType;
   incapacitated |= isIncapacitated(acB);

   const AbsCoord &robotPos = readFrom(localisation, robotPos);

   SPLStandardMessage m (playerNum,
                         readFrom(gameController, team_red), // colour
                         incapacitated, // fallen
                         robotPos,
                         robotPos, // TODO: walking
                         robotPos, // TODO: shooting
                         readFrom(localisation, ballLostCount),
                         readFrom(localisation, ballPos),
                         readFrom(localisation, ballVel),
                         SPLStandardIntention::NOTHING, // TODO: intention
                         bd);

   writeTo(localisation, havePendingOutgoingSharedBundle, false);
   NaoTransmitter::tick(boost::asio::buffer(&m, sizeof(SPLStandardMessage)));

   // hax to send the gc packet once every two team ticks
   ++delay;
   if (delay > 1) {
      sendToGameController();
      delay = 0;
   }
}

TeamTransmitter::~TeamTransmitter() {
   socket.close();
}

void TeamTransmitter::sendToGameController() {
   boost::system::error_code ec = boost::system::error_code();
   RoboCupGameControlReturnData d = RoboCupGameControlReturnData();
   d.team = (blackboard->config)["player.team"].as<int>();
   d.player = (blackboard->config)["player.number"].as<int>();
   d.message = GAMECONTROLLER_RETURN_MSG_ALIVE;

   socket.send(boost::asio::buffer(&d, sizeof(RoboCupGameControlReturnData)), 0, ec);
}
