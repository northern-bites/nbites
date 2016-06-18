/**
 * GameController.hpp
 * Modified: 2009-11-13
 * Description: A thread to recieve game state information from the Game
 * Controller sever and implements the Button Interface. Adapted from 2009 code.
 */

#pragma once

#include <string>
#include "gamecontroller/RoboCupGameControlData.hpp"
#include "types/ButtonPresses.hpp"
#include "blackboard/Adapter.hpp"

class GameController : Adapter {
   public:
      // Constructor
      GameController(Blackboard *bb);
      // Destructor
      ~GameController();
      // Called on each cycle
      void tick();
   private:
      RoboCupGameControlData data;
      TeamInfo our_team;
      bool team_red;
      bool connected;
      int sock;

      /**
       * Connect to the GameController
       */
      void initialiseConnection();

      /**
       * Update the state using the Button Interface
       */
      void buttonUpdate();

      /**
       * Update the state using the GameController Interface
       */
      void wirelessUpdate();

      /**
       * Parse data from the GameController
       * @param update Pointer to newly recieved GC data
       */
      void parseData(RoboCupGameControlData *update);

      // parseData helper functions

      bool isValidData(RoboCupGameControlData *gameData);

      bool checkHeader(char* header);

      bool isThisGame(RoboCupGameControlData *gameData);

      bool gameDataEqual(void* gameData, void* previous);

      void rawSwapTeams(RoboCupGameControlData *gameData);

      /**
       * Internal state for speech updates
       */
      uint8 lastState;
      uint16 myLastPenalty;

      /* Player & team number re-checked from config each cycle */
      int playerNumber;
      int teamNumber;

      /* Structure containing mask of buttons that have been pushed */
      ButtonPresses buttons;
};
