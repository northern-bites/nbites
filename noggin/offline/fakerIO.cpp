#include "fakerIO.h"
using namespace std;
using namespace boost;

/**
 * Method to read in a robot path from a formatted file
 *
 * @param inputFile The opened file containing the path information
 * @param letsGo Where the robot path is to be stored
 */
void readNavInputFile(fstream* inputFile, NavPath * letsGo)
{
    // Method variables
    MotionModel motion;
    BallPose ballMove;
    int time;

    // Read the start info from the first line of the file
    if (!inputFile->eof()) { // start position
        *inputFile >> letsGo->startPos.x >> letsGo->startPos.y
                   >> letsGo->startPos.h // heaading start
                   >> letsGo->ballStart.x >> letsGo->ballStart.y; // Ball info
    }
    letsGo->ballStart.velX = 0.0;
    letsGo->ballStart.velY = 0.0;

    // Convert input value to radians
    letsGo->startPos.h *= TO_RAD;

    // Build NavMoves from the remaining lines
    while (!inputFile->eof()) {
        *inputFile >> motion.deltaF >> motion.deltaL >> motion.deltaR
                   >> ballMove.velX >> ballMove.velY
                   >> time;

        motion.deltaR *= TO_RAD;
        letsGo->myMoves.push_back(NavMove(motion, ballMove, time));
    }
}

void readObsInputFile(fstream * inputFile,
                      vector<PoseEst> * realPoses,
                      vector<BallPose> * ballPoses,
                      vector<MotionModel> * odos,
                      vector<vector<Observation> > * sightings,
                      vector<float> * ballDists,
                      vector<float> * ballBearings, int ball_id)
{
    char line[256];

    while(!inputFile->eof()) {
        PoseEst pose;
        BallPose ball;
        MotionModel odo;
        vector<int> ids;
        vector<float> dists;
        vector<float> bearings;
        vector<Observation> z_t;
        stringstream inputLine(stringstream::in | stringstream::out);

        // Read in the next line
        inputFile->getline(line, 256);
        inputLine << line;

        readObsInputLine(&inputLine, &pose, &ball, &odo,
                         &ids, &dists, &bearings);

        realPoses->push_back(pose);
        ballPoses->push_back(ball);
        odos->push_back(odo);
        // Assume we don't see a ball
        ballDists->push_back(0.0);
        ballBearings->push_back(0.0);

        for(unsigned int k = 0; k < ids.size() ; ++k) {
            // Check if it's a ball observation
            if( ids[k] == ball_id ) {
                (*ballDists)[ballDists->size() - 1] = dists[k];
                (*ballBearings)[ballDists->size() - 1] = bearings[k];
            } else if (ids[k] >= CONCRETE_FIELD_OBJECT_START_ID) {
                // it's a field object
                VisualFieldObject fo((fieldObjectID)ids[k]);
                fo.setDistanceWithSD(dists[k]);
                fo.setBearingWithSD(bearings[k]);

                if (fo.getID() != BLUE_GOAL_POST ||
                    fo.getID() != YELLOW_GOAL_POST) {
                    fo.setIDCertainty(_SURE);
                }

                Observation seen(fo);
                z_t.push_back(seen);
            } else {
                // if it's a corner
                list <const ConcreteCorner*> toUse;
                const ConcreteCorner * corn;
                switch(ids[k]) {
                case BLUE_CORNER_TOP_L:
                    corn = &ConcreteCorner::blue_corner_top_l;
                    break;
                case BLUE_CORNER_BOTTOM_L:
                    corn = &ConcreteCorner::blue_corner_bottom_l;
                    break;
                case BLUE_GOAL_LEFT_T:
                    corn = &ConcreteCorner::blue_goal_left_t;
                    break;
                case BLUE_GOAL_RIGHT_T:
                    corn = &ConcreteCorner::blue_goal_right_t;
                    break;
                case BLUE_GOAL_LEFT_L:
                    corn = &ConcreteCorner::blue_goal_left_l;
                    break;
                case BLUE_GOAL_RIGHT_L:
                    corn = &ConcreteCorner::blue_goal_right_l;
                    break;
                case CENTER_TOP_T:
                    corn = &ConcreteCorner::center_top_t;
                    break;
                case CENTER_BOTTOM_T:
                    corn = &ConcreteCorner::center_bottom_t;
                    break;
                case YELLOW_CORNER_TOP_L:
                    corn = &ConcreteCorner::yellow_corner_top_l;
                    break;
                case YELLOW_CORNER_BOTTOM_L:
                    corn = &ConcreteCorner::yellow_corner_bottom_l;
                    break;
                case YELLOW_GOAL_LEFT_T:
                    corn = &ConcreteCorner::yellow_goal_left_t;
                    break;
                case YELLOW_GOAL_RIGHT_T:
                    corn = &ConcreteCorner::yellow_goal_right_t;
                    break;
                case YELLOW_GOAL_LEFT_L:
                    corn = &ConcreteCorner::yellow_goal_left_l;
                    break;
                case YELLOW_GOAL_RIGHT_L:
                    // Intentional fall through
                default:
                    corn = &ConcreteCorner::yellow_goal_right_l;
                    break;
                }
                // Append to the list
                toUse.assign(1,corn);

                VisualCorner vc(20, 20, dists[k], bearings[k],
                                VisualLine(), VisualLine(), 10.0f, 10.0f);
                vc.setPossibleCorners(toUse);

                // Set ID
                if (toUse == ConcreteCorner::lCorners) {
                    vc.setID(L_INNER_CORNER);
                } else if (toUse == ConcreteCorner::tCorners) {
                    vc.setID(T_CORNER);
                } else {
                    vc.setID((cornerID)ids[k]);
                }
                Observation seen(vc);
                z_t.push_back(seen);
            }
        }
        sightings->push_back(z_t);
    }
}

/**
 * Method to read in a robot path from a formatted file
 *
 * @param inputFile The opened file containing the path information
 * @param letsGo Where the robot path is to be stored
 */
void readObsInputLine(stringstream * inputLine, PoseEst *currentPose,
                      BallPose * currentBall, MotionModel * currentOdo,
                      vector<int> * ids, vector<float> * dists,
                      vector<float> * bearings)
{

    // Print the actual robot position
    *inputLine >> currentPose->x
               >> currentPose->y
               >> currentPose->h
    // print actual ball position
               >> currentBall->x
               >> currentBall->y
               >> currentBall->velX
               >> currentBall->velY
               >> currentOdo->deltaF
               >> currentOdo->deltaL
               >> currentOdo->deltaR;

    // cout << "\tCurrent pose: " << *currentPose << endl;
    // cout << "\tCurrent ball: " << *currentBall << endl;
    // cout << "\tCurrent odo: " << *currentOdo << endl;

    // Read up all of the sightings...
    while( !inputLine->eof() ) {
        int newId;
        float newDist, newBearing;
        *inputLine >> newId >> newDist >> newBearing;
        ids->push_back(newId);
        dists->push_back(newDist);
        bearings->push_back(newBearing);
    }

    // For some range we always push an extra copy, so, let's get rid of them
    if (ids->size() > 0) {
        ids->pop_back();
        dists->pop_back();
        bearings->pop_back();
    }

    // for(unsigned int i = 0; i < ids->size(); ++i) {
    //     cout << "\tObservation: (" << (*ids)[i] << ", " << (*dists)[i] << ", "
    //          << (*bearings)[i] << ")" << endl;
    // }

}

/**
 * Prints the input to a log file to be read by the TOOL
 *
 * @param outputFile File to write the log line to
 * @param myLoc Current localization module
 * @param sightings Vector of landmark observations
 * @param lastOdo Odometery since previous frame
 */
void printOutObsLine(fstream* outputFile, vector<Observation> sightings,
                     MotionModel lastOdo, PoseEst *currentPose,
                     BallPose * currentBall, VisualBall _b, int ball_id)
{
    // Print the actual robot position
    *outputFile << setprecision(6) << currentPose->x << " "
                << currentPose->y << " "
                << NBMath::subPIAngle(currentPose->h) << " "
    // print actual ball position
                << currentBall->x << " "
                << currentBall->y << " "
                << currentBall->velX << " "
                << currentBall->velY << " ";
    // Odometery
    *outputFile << lastOdo.deltaF << " " << lastOdo.deltaL << " "
                << lastOdo.deltaR << " ";
    // Output landmark infos
    for(unsigned int k = 0; k < sightings.size(); ++k) {
        *outputFile << setprecision(12) << sightings[k].getID() << " "
                    << sightings[k].getVisDistance() << " "
                    << sightings[k].getVisBearing() << " ";
    }
    // Output ball as landmark
    if (_b.getDistance() > 0.0) {
        *outputFile << setprecision(12) << ball_id << " "
                    << _b.getDistance() << " "
                    << _b.getBearing() << " ";
    }

    // Close the line
    *outputFile << endl;
}

/**
 * Prints the input to a log file to be read by the TOOL
 *
 * @param outputFile File to write the log line to
 * @param myLoc Current localization module
 * @param sightings Vector of landmark observations
 * @param lastOdo Odometery since previous frame
 */
void printOutMCLLogLine(fstream* outputFile, shared_ptr<MCL> myLoc,
                        vector<Observation> sightings, MotionModel lastOdo,
                        PoseEst *currentPose, BallPose * currentBall,
                        shared_ptr<BallEKF> ballEKF, VisualBall _b,
                        int team_color, int player_number, int ball_id)
{
    // Output particle infos
    vector<Particle> particles = myLoc->getParticles();
    for(unsigned int j = 0; j < particles.size(); ++j) {
        Particle p = particles[j];
        *outputFile << p << " ";
    }
    // Divide the sections with a colon
    *outputFile << ":";

    printOutLogLine(outputFile, myLoc, sightings, lastOdo,
                    currentPose, currentBall,
                    ballEKF, _b,
                    team_color, player_number, ball_id);
}

/**
 * Prints the input to a log file to be read by the TOOL
 *
 * @param outputFile File to write the log line to
 * @param myLoc Current localization module
 * @param sightings Vector of landmark observations
 * @param lastOdo Odometery since previous frame
 */
void printOutLogLine(fstream* outputFile, shared_ptr<LocSystem> myLoc,
                     vector<Observation> sightings, MotionModel lastOdo,
                     PoseEst *currentPose, BallPose * currentBall,
                     shared_ptr<BallEKF> ballEKF, VisualBall _b,
                     int team_color, int player_number, int ball_id)
{
    // Output standard infos
    *outputFile << setprecision(6) << team_color<< " " << player_number << " "
                << myLoc->getXEst() << " " << myLoc->getYEst() << " "
                << myLoc->getHEst() << " "
                << myLoc->getXUncert() << " " << myLoc->getYUncert() << " "
                << myLoc->getHUncert() << " "
                // X Estimate
                << (ballEKF->getXEst()) << " "
                // Y Estimate
                << (ballEKF->getYEst()) << " "
                // X Uncert
                << (ballEKF->getXUncert()) << " "
                // Y Uncert
                << (ballEKF->getYUncert()) << " "
                // X Velocity Estimate
                << ballEKF->getXVelocityEst() << " "
                // Y Velocity Estimate
                << ballEKF->getYVelocityEst() << " "
                // X Velocity Uncert
                << ballEKF->getXVelocityUncert() << " "
                // Y Velocity Uncert
                << ballEKF->getYVelocityUncert() << " "
                // Odometery
                << lastOdo.deltaF << " " << lastOdo.deltaL << " "
                << lastOdo.deltaR;

    // Divide the sections with a colon
    *outputFile << ":";

    // Print the actual robot position
    *outputFile << currentPose->x << " "
                << currentPose->y << " "
                << currentPose->h << " "
    // print actual ball position
                << currentBall->x << " "
                << currentBall->y << " "
                << currentBall->velX << " "
                << currentBall->velY << " ";

    // Divide the sections with a colon
    *outputFile << ":";

    // Output landmark infos
    for(unsigned int k = 0; k < sightings.size(); ++k) {
        *outputFile << setprecision(12) << sightings[k].getID() << " "
                    << sightings[k].getVisDistance() << " "
                    << sightings[k].getVisBearing() << " ";
    }
    // Output ball as landmark
    if (_b.getDistance() > 0.0) {
        *outputFile << setprecision(12) << ball_id << " "
                    << _b.getDistance() << " "
                    << _b.getBearing() << " ";
    }

    // Close the line
    *outputFile << endl;
}

/**
 * Prints the input to a log file to be read by the TOOL
 *
 * @param outputFile File to write the log line to
 * @param myLoc Current localization module
 * @param sightings Vector of landmark observations
 * @param lastOdo Odometery since previous frame
 */
void printCoreLogLine(fstream* outputFile, shared_ptr<LocSystem> myLoc,
                     vector<Observation> sightings, MotionModel lastOdo,
                     PoseEst *currentPose, BallPose * currentBall,
                     shared_ptr<BallEKF> ballEKF)
{
    // Output standard infos
    *outputFile << setprecision(6)
        // Robot estimate
                << myLoc->getXEst() << " "
                << myLoc->getYEst() << " "
                << myLoc->getHEst() << " "
        // estimate uncertainty
                << myLoc->getXUncert() << " "
                << myLoc->getYUncert() << " "
                << myLoc->getHUncert() << " "
        // ball estimates
                << (ballEKF->getXEst()) << " "
                << (ballEKF->getYEst()) << " "
                << (ballEKF->getXUncert()) << " "
                << (ballEKF->getYUncert()) << " "
        // ball uncertainty
                << ballEKF->getXVelocityEst() << " "
                << ballEKF->getYVelocityEst() << " "
                << ballEKF->getXVelocityUncert() << " "
                << ballEKF->getYVelocityUncert() << " "
        // odometry data
                << lastOdo.deltaF << " "
                << lastOdo.deltaL << " "
                << lastOdo.deltaR << " "
        // Print the actual robot position
                << currentPose->x << " "
                << currentPose->y << " "
                << currentPose->h << " "
        // print actual ball position
                << currentBall->x << " "
                << currentBall->y << " "
                << currentBall->velX << " "
                << currentBall->velY << endl;
}
