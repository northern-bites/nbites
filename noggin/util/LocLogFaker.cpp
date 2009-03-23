/* LocLogFaker.cpp */

/**
 * Format of the navigation input file (*.nav):
 *
 * START POSITION LINE
 * x-value y-value heading-value
 *
 * NAVIGATION LINES
 * deltaForward deltaLateral deltaRotation
 *
 * Format of the log output file (*.mcl):
 *
 * PARTICLE INFO
 * x y h weight (for M particles)
 *
 * Colon signifying end of section
 *
 * DEBUG INFO
 * team_color player_number
 * x-estimate y-estimate heading-estimate deg
 * x-uncertinty y-uncert heading-uncert
 * ball-x ball-y
 * ball-uncert-x ball-uncert-y
 * ball-vel-x ball-vel-y
 * ball-vel-uncert-x ball-vel-uncert-y
 * odometery-lateral odometery-forward odometery-rotational
 *
 * ROBOT REAL INFO
 * x y h (as given by the system)
 *
 * LANDMARK INFO
 * ID dist bearing (for all landmarks observed in the frame)
 *
 */
#include "LocLogFaker.h"
#include "NBMath.h"
#define UNIFORM_1_NEG_1 (2*(rand() / (float(RAND_MAX)+1)) - 1)
#define USE_PERFECT_LOC_FOR_BALL
using namespace std;
using namespace boost;

int main(int argc, char** argv)
{
    // Information needed for the main method
    // Make navPath
    NavPath letsGo;
    // IO Variables
    fstream inputFile;
    fstream outputFile;

    /* Test for the correct number of CLI arguments */
    if(argc < 2 || argc > 3) {
        cerr << "usage: " << argv[0] << " input-file [output-file]" << endl;
        return 1;
    }
    try {
        inputFile.open(argv[1], ios::in);

    } catch (const exception& e) {
        cout << "Failed to open input file" << argv[1] << endl;
        return 1;
    }

    // Get the info from the file
    readInputFile(&inputFile, &letsGo);

    // Clost the file
    inputFile.close();

    // Open output file
    if(argc > 2) { // If an output file is specified
        outputFile.open(argv[2], ios::out);
    } else { // Otherwise use the default
        outputFile.open(DEFAULT_OUTFILE_NAME.c_str(), ios::out);
    }

    // Iterate through the path
    iteratePath(&outputFile, &letsGo);

    // Close the output file
    outputFile.close();

    return 0;
}

/**
 * Method to iterate through a robot path and write the localization info.
 *
 * @param outputFile The file to have everything printed to
 * @param letsGo The robot path from which to localize
 */
void iteratePath(fstream * outputFile, NavPath * letsGo)
{
    // Method variables
    vector<Observation> Z_t;
    shared_ptr<MCL> myLoc = shared_ptr<MCL>(new MCL);
    shared_ptr<BallEKF> ballEKF = shared_ptr<BallEKF>(new BallEKF());
    PoseEst currentPose;
    BallPose currentBall;
    MotionModel noMove(0.0, 0.0, 0.0);
    VisualBall * visBall = new VisualBall();

    currentPose.x = letsGo->startPos.x;
    currentPose.y = letsGo->startPos.y;
    currentPose.h = letsGo->startPos.h;
    currentBall = letsGo->ballStart;

    // Print out starting configuration
    printOutLogLine(outputFile, myLoc, Z_t, noMove, &currentPose,
                    &currentBall, ballEKF, *visBall);

    unsigned frameCounter = 0;
    // Iterate through the moves
    for(unsigned int i = 0; i < letsGo->myMoves.size(); ++i) {

        // Continue the move for as long as specified
        for (int j = 0; j < letsGo->myMoves[i].time; ++j, ++ frameCounter) {

            currentPose += letsGo->myMoves[i].move;
            currentBall += letsGo->myMoves[i].ballVel;

            Z_t = determineObservedLandmarks(currentPose, 0.0);

            // Figure out the current ball distance and bearing
            visBall->setDistanceEst(determineBallEstimate(&currentPose,
                                                          &currentBall,
                                                          0.0));

            // Update the ball estimate model
            myLoc->updateLocalization(letsGo->myMoves[i].move, Z_t);
            ballEKF->updateModel(visBall,false);
            // Print the current frame to file
            printOutLogLine(outputFile, myLoc, Z_t, letsGo->myMoves[i].move,
                            &currentPose, &currentBall, ballEKF, *visBall);
        }
    }

    delete visBall;
}

/**
 * Method to determine which landmarks are viewable given a robot pose on the
 * field
 *
 * @param myPos The current pose of the robot
 * @param neckYaw The current head yaw of the robot
 *
 * @return A vector containing all of the observable landmarks at myPose
 */
vector<Observation> determineObservedLandmarks(PoseEst myPos, float neckYaw)
{
    vector<Observation> Z_t;
    // Measurements between robot position and seen object
    float deltaX, deltaY;
    // required measurements for the added observation
    float visDist, visBearing, sigmaD, sigmaB;

    // Check concrete field objects
    for(int i = 0; i < ConcreteFieldObject::NUM_FIELD_OBJECTS; ++i) {
        const ConcreteFieldObject* toView = ConcreteFieldObject::
            concreteFieldObjectList[i];
        deltaX = toView->getFieldX() - myPos.x;
        deltaY = toView->getFieldY() - myPos.y;
        visDist = hypot(deltaX, deltaY);
        visBearing = subPIAngle(atan2(deltaY, deltaX) - myPos.h
                                - (M_PI / 2.0f));

        // Check if the object is viewable
        if (visBearing > -FOV_OFFSET && visBearing < FOV_OFFSET) {

            // Get measurement variance and add noise to reading
            sigmaD = getDistSD(visDist);
            visDist += sigmaD*UNIFORM_1_NEG_1;
            sigmaB = getBearingSD(visBearing);
            visBearing += .04*UNIFORM_1_NEG_1;

            // Build the (visual) field object
            fieldObjectID foID = toView->getID();
            VisualFieldObject fo(foID);
            fo.setDistanceWithSD(visDist);
            fo.setBearingWithSD(visBearing);

            // set ambiguous data
            // Randomly set them to abstract
            if ((rand() / (float(RAND_MAX)+1)) < 0.12) {
                fo.setIDCertainty(NOT_SURE);
                if(foID == BLUE_GOAL_LEFT_POST ||
                   foID == BLUE_GOAL_RIGHT_POST) {
                    fo.setID(BLUE_GOAL_POST);
                } else {
                    fo.setID(YELLOW_GOAL_POST);
                }
            } else {
                fo.setIDCertainty(_SURE);
            }
            Observation seen(fo);
            Z_t.push_back(seen);
        }
    }

    // Check concrete corners
    for(int i = 0; i < ConcreteCorner::NUM_CORNERS; ++i) {
       const ConcreteCorner* toView = ConcreteCorner::concreteCornerList[i];
        float deltaX = toView->getFieldX() - myPos.x;
        float deltaY = toView->getFieldY() - myPos.y;
        visDist = hypot(deltaX, deltaY);
        visBearing = subPIAngle(atan2(deltaY, deltaX) - myPos.h
                                - (M_PI / 2.0f));

        // Check if the object is viewable
        if ((visBearing > -FOV_OFFSET && visBearing < FOV_OFFSET) &&
            visDist < CORNER_MAX_VIEW_RANGE) {

            // Get measurement variance and add noise to reading
            sigmaD = getDistSD(visDist);
            visDist += sigmaD*UNIFORM_1_NEG_1+.005*sigmaD;
            sigmaB = getBearingSD(visBearing);
            //visBearing += (M_PI / 2.0f);
            //visBearing += sigmaB*UNIFORM_1_NEG_1+.005*sigmaB;

            // Ignore the center circle for right now
            if (toView->getID() == CENTER_CIRCLE) {
                continue;
            }
            const cornerID id = toView->getID();
            list <const ConcreteCorner*> toUse;
            // Randomly set ambiguous data
            if ((rand() / (float(RAND_MAX)+1)) < 0.32) {
                shape s = ConcreteCorner::inferCornerType(id);
                toUse = ConcreteCorner::getPossibleCorners(s);
            } else {
                const ConcreteCorner * corn;
                switch(id) {
                case BLUE_CORNER_LEFT_L:
                    corn = &ConcreteCorner::blue_corner_left_l;
                    break;
                case BLUE_CORNER_RIGHT_L:
                    corn = &ConcreteCorner::blue_corner_right_l;
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
                case CENTER_BY_T:
                    corn = &ConcreteCorner::center_by_t;
                    break;
                case CENTER_YB_T:
                    corn = &ConcreteCorner::center_yb_t;
                    break;
                case YELLOW_CORNER_LEFT_L:
                    corn = &ConcreteCorner::yellow_corner_left_l;
                    break;
                case YELLOW_CORNER_RIGHT_L:
                    corn = &ConcreteCorner::yellow_corner_right_l;
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
            }

            // Build the visual corner
            VisualCorner vc(20, 20, visDist,visBearing,
                            VisualLine(), VisualLine(), 10.0f, 10.0f);
            vc.setPossibleCorners(toUse);

            // Set ID
            if (toUse == ConcreteCorner::lCorners) {
                vc.setID(L_INNER_CORNER);
            } else if (toUse == ConcreteCorner::tCorners) {
                vc.setID(T_CORNER);
            } else {
                vc.setID(id);
            }
            // Build the observation
            Observation seen(vc);
            Z_t.push_back(seen);
        }
    }

    // Check concrete lines

    return Z_t;
}

/**
 * Function to return the current estimate towards the known ball position
 *
 * @param currentPose the current known pose of the robot
 *
 * @return The current distance and bearing of the ball with noise
 */
estimate determineBallEstimate(PoseEst * currentPose, BallPose * currentBall,
                               float neckYaw)
{
    estimate e;
    e.bearing = subPIAngle(atan2(currentBall->y - currentPose->y,
                                 currentBall->x - currentPose->x) -
                           M_PI / 2.0f);

    // Calculate distance if object is within view
    if ( true ||
         e.bearing > -FOV_OFFSET && e.bearing < FOV_OFFSET &&
         (rand() / (float(RAND_MAX)+1)) < 0.85) {
        e.dist = hypot(currentPose->x - currentBall->x,
                       currentPose->y - currentBall->y);
        e.dist += e.dist*UNIFORM_1_NEG_1*0.03;
        e.bearing += subPIAngle(UNIFORM_1_NEG_1*0.05);

    } else {
        e.dist = 0.0f;
        e.bearing = 0.0f;
    }
    return e;
}


////////////////////////
// File I/O           //
////////////////////////
/**
 * Method to read in a robot path from a formatted file
 *
 * @param inputFile The opened file containing the path information
 * @param letsGo Where the robot path is to be stored
 */
void readInputFile(fstream* inputFile, NavPath * letsGo)
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

/**
 * Prints the input to a log file to be read by the TOOL
 *
 * @param outputFile File to write the log line to
 * @param myLoc Current localization module
 * @param sightings Vector of landmark observations
 * @param lastOdo Odometery since previous frame
 */
void printOutLogLine(fstream* outputFile, shared_ptr<MCL> myLoc,
                     vector<Observation> sightings, MotionModel lastOdo,
                     PoseEst *currentPose, BallPose * currentBall,
                     shared_ptr<BallEKF> ballEKF, VisualBall _b)
{
    // Output particle infos
    vector<Particle> particles = myLoc->getParticles();
    for(unsigned int j = 0; j < particles.size(); ++j) {
        Particle p = particles[j];
        *outputFile << p << " ";
    }

    // Divide the sections with a colon
    *outputFile << ":";

    // Output standard infos
    *outputFile << team_color<< " " << player_number << " "
                << myLoc->getXEst() << " " << myLoc->getYEst() << " "
                << myLoc->getHEstDeg() << " "
                << myLoc->getXUncert() << " " << myLoc->getYUncert() << " "
                << myLoc->getHUncertDeg() << " "
                // Ball estimates
                // << (ballEKF->getXEst()*cos(myLoc->getHEst()) +
                //     ballEKF->getYEst()*sin(myLoc->getHEst()) +
                //     myLoc->getXEst()) << " "
                // // Y Estimate
                // << (ballEKF->getXEst()*sin(myLoc->getHEst()) +
                //     ballEKF->getYEst()*cos(myLoc->getHEst()) +
                //     myLoc->getYEst()) << " "
                // X Estimate
                << (ballEKF->getXEst()*cos(currentPose->h) +
                    ballEKF->getYEst()*sin(currentPose->h) +
                    currentPose->x) << " "
                // Y Estimate
                << (ballEKF->getXEst()*sin(currentPose->h) +
                    ballEKF->getYEst()*cos(currentPose->h) +
                    currentPose->y) << " "
                // // X Estimate
                // << (ballEKF->getXEst() +
                //     currentPose->x) << " "
                // // Y Estimate
                // << (ballEKF->getYEst() +
                //     currentPose->y) << " "
                // X Uncert
                << (ballEKF->getXUncert()) << " "
                // Y Uncert
                << (ballEKF->getYUncert()) << " "
                // // X Uncert
                // << (fabs(ballEKF->getXUncert()*cos(myLoc->getHEst())) +
                //     fabs(ballEKF->getYUncert()*sin(myLoc->getHEst()))) << " "
                // // Y Uncert
                // << (fabs(ballEKF->getXUncert()*sin(myLoc->getHEst())) +
                //     fabs(ballEKF->getYUncert()*cos(myLoc->getHEst()))) << " "
                // X Velocity Estimate
                << (-ballEKF->getXVelocityEst()*cos(myLoc->getHEst()) +
                    ballEKF->getYVelocityEst()*sin(myLoc->getHEst())) << " "
                // Y Estimate
                << (ballEKF->getXVelocityEst()*sin(myLoc->getHEst()) +
                    ballEKF->getYVelocityEst()*cos(myLoc->getHEst())) << " "
                // X Velocity Uncert
                << (fabs(ballEKF->getXVelocityUncert()*cos(myLoc->getHEst())) +
                    fabs(ballEKF->getYVelocityUncert()*sin(myLoc->getHEst())))
                << " "
                // Y Velocity Uncert
                << (fabs(ballEKF->getXVelocityUncert()*sin(myLoc->getHEst())) +
                    fabs(ballEKF->getYVelocityUncert()*cos(myLoc->getHEst())))
                << " "
                // Odometery
                << lastOdo.deltaL << " " << lastOdo.deltaF << " "
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
        *outputFile << sightings[k].getID() << " "
                    << sightings[k].getVisDistance() << " "
                    << sightings[k].getVisBearingDeg() << " ";
    }
    // Output ball as landmark
    if (_b.getDistance() > 0.0) {
        *outputFile << BALL_ID << " "
                    << _b.getDistance() << " "
                    << _b.getBearing() << " ";
    }

    // Close the line
    *outputFile << endl;
}



// NavMove
// Constructors
NavMove::NavMove(MotionModel _p, BallPose _b, int _t) : move(_p), ballVel(_b),
                                                        time(_t)
{
};

/**
 * Get standard deviation for the associated distance reading
 */
float getDistSD(float distance)
{
    return distance * 0.175 + 2;
}

/**
 * Get standard deviation for the associated bearing reading
 */
float getBearingSD(float bearing)
{
    return bearing*0.20 + M_PI / 4.0;
}
