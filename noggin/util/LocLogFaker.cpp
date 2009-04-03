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
using namespace NBMath;
static const bool usePerfectLocForBall = true;

int main(int argc, char** argv)
{
    // Information needed for the main method
    // Make navPath
    NavPath letsGo;
    // IO Variables
    fstream inputFile;
    fstream mclFile;
    fstream ekfFile;

    /* Test for the correct number of CLI arguments */
    if(argc != 2) {
        cerr << "usage: " << argv[0] << " input-file" << endl;
        return 1;
    }
    try {
        inputFile.open(argv[1], ios::in);

    } catch (const exception& e) {
        cout << "Failed to open input file" << argv[1] << endl;
        return 1;
    }

    // Get the info from the input file
    readInputFile(&inputFile, &letsGo);

    // Clost the input file
    inputFile.close();

    // Open output files
    string mclFileName(argv[1]);
    string ekfFileName(argv[1]);
    mclFileName.replace(mclFileName.end()-3, mclFileName.end(), "mcl");
    ekfFileName.replace(ekfFileName.end()-3, ekfFileName.end(), "ekf");

    mclFile.open(mclFileName.c_str(), ios::out);
    ekfFile.open(ekfFileName.c_str(), ios::out);

    // Iterate through the path
    iteratePath(&mclFile, &ekfFile, &letsGo);

    // Close the output files
    mclFile.close();
    ekfFile.close();

    return 0;
}

/**
 * Method to iterate through a robot path and write the localization info.
 *
 * @param outputFile The file to have everything printed to
 * @param letsGo The robot path from which to localize
 */
void iteratePath(fstream * mclFile, fstream * ekfFile, NavPath * letsGo)
{
    // Method variables
    vector<Observation> Z_t;
    shared_ptr<MCL> mclLoc = shared_ptr<MCL>(new MCL());
    shared_ptr<BallEKF> MCLballEKF = shared_ptr<BallEKF>(new BallEKF());
    shared_ptr<LocEKF> ekfLoc = shared_ptr<LocEKF>(new LocEKF());
    shared_ptr<BallEKF> EKFballEKF = shared_ptr<BallEKF>(new BallEKF());
    PoseEst currentPose;
    BallPose currentBall;
    MotionModel noMove(0.0, 0.0, 0.0);
    VisualBall * visBall = new VisualBall();

    currentPose.x = letsGo->startPos.x;
    currentPose.y = letsGo->startPos.y;
    currentPose.h = letsGo->startPos.h;
    currentBall = letsGo->ballStart;

    // Print out starting configuration
    printOutMCLLogLine(mclFile, mclLoc, Z_t, noMove, &currentPose,
                       &currentBall, MCLballEKF, *visBall);
    printOutLogLine(ekfFile, ekfLoc, Z_t, noMove, &currentPose,
                    &currentBall, EKFballEKF, *visBall);

    unsigned frameCounter = 0;
    // Iterate through the moves
    for(unsigned int i = 0; i < letsGo->myMoves.size(); ++i) {

        // Continue the move for as long as specified
        for (int j = 0; j < letsGo->myMoves[i].time; ++j, ++ frameCounter) {

            // Determine the current frame info
            currentPose += letsGo->myMoves[i].move;
            currentBall += letsGo->myMoves[i].ballVel;

            Z_t = determineObservedLandmarks(currentPose, 0.0);

            // Figure out the current ball distance and bearing
            visBall->setDistanceEst(determineBallEstimate(&currentPose,
                                                          &currentBall,
                                                          0.0));

            // Update the MCL sytem
            mclLoc->updateLocalization(letsGo->myMoves[i].move, Z_t);
            // Update the MCL ball
            if (usePerfectLocForBall) {
                MCLballEKF->updateModel(visBall,currentPose, true);
            } else {
                MCLballEKF->updateModel(visBall,mclLoc->getCurrentEstimate(),
                                        true);
            }

            // Update the EKF sytem
            ekfLoc->updateLocalization(letsGo->myMoves[i].move, Z_t);
            // Update the EKF ball
            if (usePerfectLocForBall) {
                EKFballEKF->updateModel(visBall,currentPose, true);
            } else {
                EKFballEKF->updateModel(visBall,ekfLoc->getCurrentEstimate(),
                                        true);
            }

            // Print the current MCL frame to file
            printOutMCLLogLine(mclFile, mclLoc, Z_t, letsGo->myMoves[i].move,
                               &currentPose, &currentBall, MCLballEKF,
                               *visBall);
            // Print the current EKF frame to file
            printOutLogLine(ekfFile, ekfLoc, Z_t, letsGo->myMoves[i].move,
                               &currentPose, &currentBall, EKFballEKF,
                               *visBall);
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
        visBearing = subPIAngle(atan2(deltaY, deltaX) - myPos.h);

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
            if (false && (rand() / (float(RAND_MAX)+1)) < 0.12) {
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
        visBearing = subPIAngle(atan2(deltaY, deltaX) - myPos.h);

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
            if (false && (rand() / (float(RAND_MAX)+1)) < 0.32) {
                shape s = ConcreteCorner::inferCornerType(id);
                toUse = ConcreteCorner::getPossibleCorners(s);
            } else {
                const ConcreteCorner * corn;
                switch(id) {
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
                           currentPose->h);

    // Calculate distance if object is within view
    if ( e.bearing > -FOV_OFFSET && e.bearing < FOV_OFFSET &&
         (rand() / (float(RAND_MAX)+1)) < 0.85) {
        e.dist = hypot(currentPose->x - currentBall->x,
                       currentPose->y - currentBall->y);
        e.dist += e.dist*UNIFORM_1_NEG_1*0.13;
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
void printOutMCLLogLine(fstream* outputFile, shared_ptr<MCL> myLoc,
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

    printOutLogLine(outputFile, myLoc, sightings, lastOdo, currentPose,
                    currentBall, ballEKF, _b);
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
                     shared_ptr<BallEKF> ballEKF, VisualBall _b)
{
    // Output standard infos
    *outputFile << team_color<< " " << player_number << " "
                << myLoc->getXEst() << " " << myLoc->getYEst() << " "
                << myLoc->getHEst() << " "
                << myLoc->getXUncert() << " " << myLoc->getYUncert() << " "
                << myLoc->getHUncertDeg() << " "
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
