#ifndef NavStructs_h_DEFINED
#define NavStructs_h_DEFINED

#include "EKFStructs.h"
#include "NogginStructs.h"
/**
 * Class to hold a constant robot path vector over a given number of frames
 */
class NavMove
{
public:
    MotionModel move;
    BallPose ballVel;
    int time; // Number of frames over which to continue on the move vector
    NavMove(MotionModel _m, BallPose _b, int _t)  : move(_m), ballVel(_b),
                                                    time(_t) {}
};

class NavPath
{
public:
    PoseEst startPos; // Real start position of the robot
    BallPose ballStart; // Real start position of the ball
    std::vector<NavMove> myMoves; // direction of movement per itteration
};

#endif // NavStructs_h_DEFINED
