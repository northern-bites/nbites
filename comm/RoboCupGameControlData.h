/*
    Copyright (C) 2005  University Of New South Wales
    Copyright (C) 2006  National ICT Australia

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software source code file, to deal in the file without restriction, 
    including without limitation the rights to use, copy, modify, merge, 
    publish, distribute, sublicense, and/or sell copies of the file, and to 
    permit persons to whom the file is furnished to do so, subject to the 
    following conditions:

    The above copyright notice and this permission notice shall be included in 
    all copies or substantial portions of the software source file.

    THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
*/

/*******************************************************************************
*
* RoboCupGameControlData.h
*
* Broadcasted data structure and constants
*
* willu@cse.unsw.edu.au
* shnl327@cse.unsw.edu.au
*
*******************************************************************************/

#ifndef _RoboCupGameControlData_h_DEFINED
#define _RoboCupGameControlData_h_DEFINED

// port for GameController network traffic
#define GAMECONTROLLER_PORT             3838

// data structure version number
#define GAMECONTROLLER_STRUCT_VERSION   6

// data structure header
//#define GAMECONTROLLER_STRUCT_HEADER    "RGme"
static const char* GAMECONTROLLER_STRUCT_HEADER = "RGme";

// the maximum number of players per team
#define MAX_NUM_PLAYERS             11
#define DEF_NUM_PLAYERS		    5

// team numbers
#define TEAM_BLUE                   0
#define TEAM_RED                    1

// game states
enum GCGameState {
  STATE_INITIAL = 0,
  STATE_READY,
  STATE_SET,
  STATE_PLAYING,
  STATE_FINISHED
};

// secondary game states
#define STATE2_NORMAL               0
#define STATE2_PENALTYSHOOT         1

// penalties
enum GCPenalty {
 PENALTY_NONE = 0,
 PENALTY_BALL_HOLDING,
 PENALTY_GOALIE_PUSHING,
 PENALTY_PLAYER_PUSHING,
 PENALTY_ILLEGAL_DEFENDER,
 PENALTY_ILLEGAL_DEFENSE,
 PENALTY_OBSTRUCTION,
 PENALTY_REQ_FOR_PICKUP,
 PENALTY_LEAVING,
 PENALTY_DAMAGE,
 PENALTY_MANUAL
};


#ifndef uint8
typedef unsigned char uint8;
#endif
#ifndef uint16
typedef unsigned short uint16;
#endif
#ifndef uint32
typedef unsigned long uint32;
#endif

// information that describes a player
struct RobotInfo {
    uint16 penalty;             // penalty state of the player
    uint16 secsTillUnpenalised; // estimate of time till unpenalised
};


// information that describes a team
struct TeamInfo {
    uint8 teamNumber;          // unique team number
    uint8 teamColor;           // color of the team
    uint16 score;              // team's score
    RobotInfo players[MAX_NUM_PLAYERS];       // the team's players
};


struct RoboCupGameControlData {
    char   header[4];           // header to identify the structure
    uint32 version;             // version of the data structure
    uint8 playersPerTeam;       // The number of players on a team
    uint8 state;                // state of the game (STATE_READY, STATE_PLAYING, etc)
    uint8 firstHalf;            // 1 = game in first half, 0 otherwise
    uint8 kickOffTeam;          // the next team to kick off
    uint8 secondaryState;       // Extra state information - (STATE2_NORMAL, STATE2_PENALTYSHOOT, etc)
    uint8 dropInTeam;           // team that caused last drop in
    uint16 dropInTime;          // number of seconds passed since the last drop in.  -1 before first dropin
    uint32 secsRemaining;       // estimate of number of seconds remaining in the half
    TeamInfo teams[2];
};

// data structure header
#define GAMECONTROLLER_RETURN_STRUCT_HEADER    "RGrt"

#define GAMECONTROLLER_RETURN_STRUCT_VERSION   1

#define GAMECONTROLLER_RETURN_MSG_MAN_PENALISE 0
#define GAMECONTROLLER_RETURN_MSG_MAN_UNPENALISE 1

struct RoboCupGameControlReturnData {
  char    header[4];
  uint32  version;
  uint16  team;
  uint16  player;       // player number is 1-based
  uint32  message;
};

#endif

