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
#include <string>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
using namespace std;
using boost::lexical_cast;
using boost::bad_lexical_cast;

// Local
#include "MCL.h"

#define MCL_LOG_PREFIX "LOG"
#define MCL_LOG_SUFFIX ".mcl"
#define DEBUG_INPUT false
#define DEBUG_INPUT_CORNERS false
// Define a tokenizer type
typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

// IO Functions
void printOutLogLine(fstream* outputFile, MCL* myLoc, vector<Observation>
                     sightings, MotionModel lastOdo);

// Core Functions
void processLogLine(string current, MCL* myLoc, fstream* outputFile);

string team_color, player_number;

#endif
