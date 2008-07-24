//
// Copyright 2002,2003 Sony Corporation 
//
// Permission to use, copy, modify, and redistribute this software for
// non-commercial use is hereby granted.
//
// This software is provided "as is" without warranty of any kind,
// either expressed or implied, including but not limited to the
// implied warranties of fitness for a particular purpose.
//
#ifndef _GameControllerConfig_h_DEFINED
#define _GameControllerConfig_h_DEFINED

#define GAMECONTROLLER_CONNECTION_MAX   1

// low battery warning; when the battery is lower than
// this %, the Game Controller will turn on red LEDs
// on the face during INITIAL, READY, and SET
#define GAMECONTROLLER_BATTERY_WARNING  40

// make buffer as big as the data structure
// in RoboCupGameControlData.h
#define GAMECONTROLLER_BUFFER_SIZE      sizeof(RoboCupGameControlData)

#define GAMECONTROLLER_RETURN_BUFFER_SIZE      sizeof(RoboCupGameControlReturnData)

// the file with the team number to load
#define GAMECONTROLLER_TEAM_CFG         "/MS/team.cfg"

#endif
