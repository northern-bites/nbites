#ifndef LOCLOGFAKER_h_DEFINED
#define LOCLOGFAKER_h_DEFINED

#include <time.h> // for srand(time(NULL))
#include <cstdlib> // for MAX_RAND
#include <fstream>
#include <iostream>
#include <string>
#include "MCL.h"
#include "VisionDef.h" // For NAO_FOV_X_DEG
using namespace std;

// Observation parameters
// Ranges at which objects are viewable
// Field objects
#define FO_MAX_VIEW_RANGE 575.0f
#define CORNER_MAX_VIEW_RANGE 200.0f
#define LINE_MAX_VIEW_RANGE 250.0f

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
    vector<NavMove> myMoves; // direction of movement per itteration
};

vector<Observation> determineObservedLandmarks(PoseEst myPos, float neckYaw);

// IO Functions
void printOutLogLine(fstream* outputFile, MCL* myLoc, vector<Observation>
                     sightings, MotionModel lastOdo);

string team_color, player_number;
// Helper functions
float subPIAngle(float theta);
float getDistSD(float dist);
float getBearingSD(float bearing);

#endif // LOCLOGFAKER_h_DEFINED

