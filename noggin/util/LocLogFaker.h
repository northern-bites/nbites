/* LocLogFaker.h */

#ifndef LOCLOGFAKER_h_DEFINED
#define LOCLOGFAKER_h_DEFINED

#include <time.h> // for srand(time(NULL))
#include <cstdlib> // for MAX_RAND
#include <fstream>
#include <iostream>
#include <string>
#include <boost/shared_ptr.hpp>
#include "Ball.h"
#include "MCL.h"
#include "BallEKF.h"
#include "VisionDef.h" // For NAO_FOV_X_DEG

// Observation parameter
// Ranges at which objects are viewable
// Field objects
#define FO_MAX_VIEW_RANGE 575.0f
#define CORNER_MAX_VIEW_RANGE 200.0f
#define LINE_MAX_VIEW_RANGE 250.0f
// Number of frames to wait between resampling
#define RESAMPLE_RATE 5
#define BALL_ID 40

// Constants
string team_color = "0";
string player_number = "3";
string DEFAULT_OUTFILE_NAME = "FAKELOG.mcl";
// Get half of the nao FOV converted to radians
float FOV_OFFSET = NAO_FOV_X_DEG * M_PI / 360.0f;

/**
 * Class to hold the ball positin and velocity in the x and y directions
 */
class BallPose
{
public:
    float x;
    float y;
    float velX;
    float velY;

    BallPose operator+ (const BallPose o){
        return BallPose(x + o.velX,
                        y + o.velY,
                        o.velX,
                        o.velY);
    }

    void operator+= (const BallPose o){
        x += o.velX;
        y += o.velY;
        velX = o.velX;
        velY = o.velY;
    }

    BallPose(float _x, float _y, float _vx, float _vy) : x(_x), y(_y), velX(_vx),
                                                         velY(_vy) {}
    BallPose() {}

};

/**
 * Class to hold a constant robot path vector over a given number of frames
 */
class NavMove
{
public:
    MotionModel move;
    BallPose ballVel;
    int time; // Number of frames over which to continue on the move vector
    NavMove(MotionModel _m, BallPose _b, int _t);
};

class NavPath
{
public:
    PoseEst startPos; // Real start position of the robot
    BallPose ballStart; // Real start position of the ball
    std::vector<NavMove> myMoves; // direction of movement per itteration
};

std::vector<Observation> determineObservedLandmarks(PoseEst myPos,
                                                    float neckYaw);
estimate determineBallEstimate(PoseEst * currentPose, BallPose * currentBall,
                               float neckYaw);
void iteratePath(fstream * outputFile, NavPath * letsGo);
// IO Functions
void readInputFile(fstream* name, NavPath * letsGo);
void printOutLogLine(fstream* outputFile, boost::shared_ptr<MCL> myLoc,
                     std::vector<Observation>
                     sightings, MotionModel lastOdo, PoseEst * currentPose,
                     BallPose * currentBall, boost::shared_ptr<BallEKF> ballEKF,
                     Ball _b);

// Helper functions
float subPIAngle(float theta);
float getDistSD(float dist);
float getBearingSD(float bearing);

#endif // LOCLOGFAKER_h_DEFINED

