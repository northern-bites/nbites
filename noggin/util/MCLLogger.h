/**
 * MCLLogger.h
 *
 * @author Tucker Hermans
 * @date 17.11.08
 * @version %I%
 */

#ifndef MCLLogger_h_DEFINED
#define MCLLogger_h_DEFINED

// Includes
// STL
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

// Local
#include "MCL.h"

#define MCL_LOG_PREFIX "LOG"
#define MCL_LOG_SUFFIX ".mcl"

// IO Functions
void readRawFile(string fileName);
void printMCLLog(int logNum);

// Core Functions
void processLogLine(string current, MCL* myLoc);

#endif
