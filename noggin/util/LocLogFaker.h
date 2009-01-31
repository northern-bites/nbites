/* LocLogFaker.h */

#ifndef LOCLOGFAKER_h_DEFINED
#define LOCLOGFAKER_h_DEFINED

#include <time.h> // for srand(time(NULL))
#include <cstdlib> // for MAX_RAND
#include <fstream>
#include <iostream>
#include <string>
#include "MCL.h"
#include "VisionDef.h" // For NAO_FOV_X_DEG

// Observation parameters
// Ranges at which objects are viewable
// Field objects
#define FO_MAX_VIEW_RANGE 575.0f
#define CORNER_MAX_VIEW_RANGE 200.0f
#define LINE_MAX_VIEW_RANGE 250.0f
// Number of frames to wait between resampling
#define RESAMPLE_RATE 5

// Constants
string team_color = "0";
string player_number = "3";
string DEFAULT_OUTFILE_NAME = "FAKELOG.mcl";

/**
   Class to hold a constant robot path vector over a given number of frames
 */
class NavMove
{
public:
    MotionModel move;
    int time; // Number of frames over which to continue on the move vector
    NavMove(MotionModel _m, int _t);
};

class NavPath
{
public:
    PoseEst startPos; // Current real position of the robot
    std::vector<NavMove> myMoves; // direction of movement per itteration
};

std::vector<Observation> determineObservedLandmarks(PoseEst myPos,
                                                    float neckYaw);
void iteratePath(fstream * outputFile, NavPath * letsGo);
// IO Functions
void readInputFile(fstream* name, NavPath * letsGo);
void printOutLogLine(fstream* outputFile, MCL* myLoc, std::vector<Observation>
                     sightings, MotionModel lastOdo);

// Helper functions
float subPIAngle(float theta);
float getDistSD(float dist);
float getBearingSD(float bearing);

#endif // LOCLOGFAKER_h_DEFINED

