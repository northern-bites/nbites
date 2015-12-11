/**
 * GameController.hpp
 * Modified: 2009-11-13
 * Description: A thread to recieve game state information from the Game
 * Controller sever and implements the Button Interface. Adapted from 2009 code.
 */

#include "gamecontroller/GameController.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <netdb.h>
#include "blackboard/Blackboard.hpp"
#include "utils/Logger.hpp"
#include "utils/speech.hpp"

#define POLL_TIME 1000

using namespace std;

GameController::GameController(Blackboard *bb)
   : Adapter(bb), team_red(false), connected(false) {
   lastState = STATE_INVALID;
   myLastPenalty = PENALTY_NONE;
   if (readFrom(gameController, connect)) {
      initialiseConnection();
   }
}

GameController::~GameController() {
   close(sock);
}

void GameController::tick() {
   data = readFrom(gameController, data);
   teamNumber = readFrom(gameController, our_team).teamNumber;
   playerNumber = readFrom(gameController, player_number);
   if (!connected && readFrom(gameController, connect)) initialiseConnection();
   if (connected) wirelessUpdate();
   buttons = readFrom(motion, buttons);
   buttonUpdate();
   writeTo(motion, buttons, buttons);
   // make our_team point to the my actual team, based on teamNumber
   TeamInfo *our_team = NULL;
   if (data.teams[TEAM_BLUE].teamNumber == teamNumber) {
      our_team = &(data.teams[TEAM_BLUE]);
      team_red = false;
   } else if (data.teams[TEAM_RED].teamNumber == teamNumber) {
      our_team = &(data.teams[TEAM_RED]);
      team_red = true;
   }
   writeTo(gameController, data, data);
   writeTo(gameController, our_team, *our_team);
   writeTo(gameController, team_red, team_red);
}

void GameController::initialiseConnection() {
   llog(INFO) << "GameController: Connecting on port "
              << GAMECONTROLLER_PORT << endl;
   stringstream s;
   s << GAMECONTROLLER_PORT;

   struct addrinfo myInfo, *results;
   memset(&myInfo, 0, sizeof myInfo);
   myInfo.ai_family = AF_UNSPEC;
   myInfo.ai_socktype = SOCK_DGRAM;
   myInfo.ai_flags = AI_PASSIVE;  // use my IP

   if (getaddrinfo(NULL, s.str().c_str(), &myInfo, &results) == -1) {
      llog(ERROR) << "GameController: Invalid Address Information" << endl;
      return;
   }

   // loop through all the results and bind to the first we can
   struct addrinfo *p;
   for (p = results; p != NULL; p = p->ai_next) {
      if ((sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
         llog(INFO) << "GameController: Cannot use Socket, trying next"
                    << endl;
         continue;
      }

      if (bind(sock, p->ai_addr, p->ai_addrlen) == -1) {
         close(sock);
         llog(INFO) << "GameController: Cannot Bind, trying next" << endl;
         continue;
      }

      break;
   }
   if (p == NULL) {
      llog(ERROR) << "GameController: Failed to bind socket" << endl;
      return;
   }

   // We don't want memory leaks...
   freeaddrinfo(results);

   llog(INFO) << "GameController: Connected on port - " << s.str() << endl;
   connected = true;
   writeTo(gameController, connected, connected);
}

void GameController::buttonUpdate() {
   if (buttons.pop(1)) {
      llog(INFO) << "button pushed once, switching state" << endl;
      switch (data.state) {
      case STATE_INITIAL:
      case STATE_PLAYING:
         data.state = STATE_PENALISED;
         data.teams[team_red].players[playerNumber - 1].penalty =
            PENALTY_MANUAL;
         SAY("Penalised");
         break;
      default:
         data.state = STATE_PLAYING;
         data.teams[team_red].players[playerNumber - 1].penalty =
            PENALTY_NONE;
         SAY("Playing");
      }
   }
}

void GameController::wirelessUpdate() {
   // Setup receiving client
   int bytesRecieved;
   struct sockaddr_storage clientAddress;
   socklen_t addr_len = sizeof(clientAddress);

   // Setup buffer to write to
   int dataSize = sizeof(RoboCupGameControlData);
   unsigned char buffer[dataSize + 1];

   // Setup for polling
   struct pollfd ufds[1];
   ufds[0].fd = sock;
   ufds[0].events = POLLIN;            // For incoming packets
   int rv = poll(ufds, 1, POLL_TIME);  // Poll every POLL_TIME ms

   // Check to see if we've received a packet
   if (rv > 0) {
      bytesRecieved = recvfrom(sock, buffer, dataSize, 0,
                               (struct sockaddr *)&clientAddress, &addr_len);
      if (bytesRecieved > 0) {
         parseData((RoboCupGameControlData*)buffer);
      }
   }
}

void GameController::parseData(RoboCupGameControlData *update) {
   if (isValidData(update)) {
      /* Normalise the team structure order so that BLUE is always first */
      if (update->teams[TEAM_BLUE].teamColour != TEAM_BLUE) {
         rawSwapTeams(update);
      }

      // Update the data
      if (!gameDataEqual(update, &data)) {
         memcpy(&data, update, sizeof(RoboCupGameControlData));
      }

      llog(VERBOSE) << "GameController: Valid data" << endl;
      if (data.state != lastState) {
         SAY(gameControllerStateNames[data.state]);
         /*
         if (data.state == STATE_READY
             && data.firstHalf == 1
             && lastState == STATE_INITIAL
             && playerNumber == 2) {
            system("/usr/bin/aplay -q /home/nao/data/startup1.wav &");
         }
         */
         lastState = data.state;
      }

      uint8 myPenalty = data.teams[team_red].players[playerNumber - 1].penalty;

      if (myPenalty != PENALTY_NONE) {
         data.state = STATE_PENALISED;
      }

      if (myPenalty != myLastPenalty) {
         if (myPenalty == PENALTY_NONE) {
            SAY("Unpenalised");
         } else {
            SAY((string("Penalised for ") +
                 gameControllerPenaltyNames[myPenalty]).c_str());
         }
         myLastPenalty = myPenalty;
      }
   } else {
      llog(ERROR) << "GameController: Invalid data" << endl;
   }
}

bool GameController::isValidData(RoboCupGameControlData *gameData) {
   // check the right structure header has come in
   if (!(checkHeader(gameData->header))) {
      llog(ERROR) << "GameController: DATA HEADER MISMATCH! "
                  << "Expected: " << GAMECONTROLLER_STRUCT_HEADER
                  << " received: " << gameData->header << endl;
      return false;
   }

   // check for partial packets
   if (sizeof(*gameData) != sizeof(RoboCupGameControlData)) {
      llog(ERROR) << "GameController: RECEIVED PARTIAL PACKET! "
                  << "Expected: " << sizeof(RoboCupGameControlData)
                  << " received: " << sizeof(*gameData) << endl;
      return false;
   }

   // check the right version of the structure is being used
   if (gameData->version != GAMECONTROLLER_STRUCT_VERSION) {
      llog(ERROR) << "GameController: DATA VERSION MISMATCH! "
                  << "Expected: " << GAMECONTROLLER_STRUCT_VERSION
                  << " received: " << gameData->version << endl;
      return false;
   }

   // check whether this packet belongs to this game at all
   if (!isThisGame(gameData)) {
      llog(ERROR) << "GameController: DATA NOT FOR THIS GAME!" << endl;
      return false;
   }

   // Data is valid ^_^
   return true;
}

bool GameController::checkHeader(char* header) {
   for (int i = 0; i < 4; i++) {
      if (header[i] != GAMECONTROLLER_STRUCT_HEADER[i]) return false;
   }
   return true;
}

bool GameController::isThisGame(RoboCupGameControlData* gameData) {
   if (gameData->teams[TEAM_BLUE].teamNumber != teamNumber &&
       gameData->teams[TEAM_RED].teamNumber  != teamNumber) {
      return false;
   }
   return true;
}

bool GameController::gameDataEqual(void* gameData, void* previous) {
   if (!memcmp(previous, gameData, sizeof(RoboCupGameControlData))) {
      return true;
   }
   return false;
}

void GameController::rawSwapTeams(RoboCupGameControlData* gameData) {
   size_t teamSize = sizeof(TeamInfo);
   TeamInfo* blueTeam = &(gameData->teams[TEAM_BLUE]);
   TeamInfo* redTeam  = &(gameData->teams[TEAM_RED]);

   TeamInfo tempTeam;
   memcpy(&tempTeam, blueTeam, teamSize);

   /* swap the teams */
   memcpy(blueTeam, redTeam, teamSize);
   memcpy(redTeam, &tempTeam, teamSize);
}
