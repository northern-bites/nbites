/**
* @file Tools/ProcessFramework/TeamHandler.h
* The file declares a class for the team communication between robots.
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
*/

#pragma once

#include "Platform/UdpComm.h"
#include "Tools/MessageQueue/MessageQueue.h"
#include <string>

#define TEAM_COMM \
  MessageQueue theTeamReceiver; \
  MessageQueue theTeamSender; \
  TeamHandler theTeamHandler;

#define INIT_TEAM_COMM \
  theTeamHandler(theTeamReceiver, theTeamSender)

#ifdef TARGET_SIM
#define START_TEAM_COMM \
  theTeamHandler.startLocal(Global::getSettings().teamPort, (unsigned short) Global::getSettings().playerNumber);
#else
#define START_TEAM_COMM \
  std::string bcastAddr = UdpComm::getWifiBroadcastAddress(); \
  theTeamHandler.start(Global::getSettings().teamPort, bcastAddr.c_str());
#endif

#define RECEIVE_TEAM_COMM \
  theTeamHandler.receive()

#define SEND_TEAM_COMM \
  theTeamHandler.send()

/**
* @class TeamHandler
* A class for team communication by broadcasting.
*/
class TeamHandler
{
public:
  /**
  * Constructor.
  * @param in Incoming debug data is stored here.
  * @param out Outgoing debug data is stored here.
  */
  TeamHandler(MessageQueue& in, MessageQueue& out);

  /**
  * The method starts the actual communication for local communication.
  * @param port The UDP port this handler is listening to.
  * @param localId An identifier for a local robot
  */
  void startLocal(int port, unsigned short localId);

  /**
  * The method starts the actual communication on the given port.
  * @param port The UDP port this handler is listening to.
  * @param subnet The subnet the handler is broadcasting to.
  */
  void start(int port, const char* subnet);

  /**
  * The method sends the outgoing message queue if possible.
  */
  void send();

  /**
  * The method receives packages if available.
  */
  void receive();

private:
  MessageQueue& in, /**< Incoming debug data is stored here. */
              & out; /**< Outgoing debug data is stored here. */
  int port; /**< The UDP port this handler is listening to. */
  UdpComm socket; /**< The socket used to communicate. */
  unsigned short localId; /**< The id of a local team communication participant or 0 for normal udp communication. */
};
