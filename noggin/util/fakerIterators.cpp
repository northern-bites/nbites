#include "fakerIterators.h"
using namespace std;
using namespace boost;
using namespace NBMath;
#define UNIFORM_1_NEG_1 (2*(rand() / (float(RAND_MAX)+1)) - 1)

/**
 * Method to iterate through a robot path and write the localization info.
 *
 * @param outputFile The file to have everything printed to
 * @param letsGo The robot path from which to localize
 */
void iterateNavPath(fstream * obsFile, NavPath * letsGo)
{
    // Method variables
    vector<Observation> Z_t;
    PoseEst currentPose;
    BallPose currentBall;
    MotionModel noMove(0.0, 0.0, 0.0);
    VisualBall * visBall = new VisualBall();

    currentPose.x = letsGo->startPos.x;
    currentPose.y = letsGo->startPos.y;
    currentPose.h = letsGo->startPos.h;
    currentBall = letsGo->ballStart;

    // Print out starting configuration
    printOutObsLine(obsFile, Z_t, noMove, &currentPose,
                    &currentBall, *visBall, BALL_ID);

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

            printOutObsLine(obsFile, Z_t, letsGo->myMoves[i].move,
                            &currentPose, &currentBall, *visBall, BALL_ID);
        }
    }

    delete visBall;
}

/**
 * Method to iterate through a robot path and write the localization info.
 *
 * @param outputFile The file to have everything printed to
 * @param letsGo The robot path from which to localize
 */
void iterateFakerPath(fstream * mclFile, fstream * ekfFile, NavPath * letsGo)
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
                       &currentBall, MCLballEKF, *visBall, TEAM_COLOR,
                       PLAYER_NUMBER, BALL_ID);
    printOutLogLine(ekfFile, ekfLoc, Z_t, noMove, &currentPose,
                    &currentBall, EKFballEKF, *visBall, TEAM_COLOR,
                    PLAYER_NUMBER, BALL_ID);

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
                               *visBall, TEAM_COLOR, PLAYER_NUMBER, BALL_ID);
            // Print the current EKF frame to file
            printOutLogLine(ekfFile, ekfLoc, Z_t, letsGo->myMoves[i].move,
                               &currentPose, &currentBall, EKFballEKF,
                               *visBall, TEAM_COLOR, PLAYER_NUMBER, BALL_ID);
        }
    }

    delete visBall;
}

void iterateObsPath(fstream * obsFile, fstream * locFile)
{

    // shared_ptr<LocEKF> ekfLoc = shared_ptr<LocEKF>(new LocEKF());
    // shared_ptr<BallEKF> EKFballEKF = shared_ptr<BallEKF>(new BallEKF());


    // VisualBall * visBall = new VisualBall();
    // visBall->setDistanceWithSD(0.0f);
    // visBall->setBearingWithSD(0.0f);

    // printOutLogLine(locFile, ekfLoc, sightings[0], noMove, &realPoses[0],
    //                 &ballPoses[0], EKFballEKF, *visBall, TEAM_COLOR,
    //                 PLAYER_NUMBER, BALL_ID);

    // for(unsigned int i = 0; i < realPoses.size(); ++i) {

    //     // Update the EKF sytem
    //     ekfLoc->updateLocalization(odos[i], sightings[i]);

    //     visBall->setDistanceWithSD(ballDists[i]);
    //     visBall->setBearingWithSD(ballBearings[i]);

    //     // Update the EKF ball
    //     EKFballEKF->updateModel(visBall,ekfLoc->getCurrentEstimate(),
    //                             true);

    //     printOutLogLine(locFile, ekfLoc, sightings[i], odos[i], &realPoses[i],
    //                     &ballPoses[i], EKFballEKF, *visBall, TEAM_COLOR,
    //                     PLAYER_NUMBER, BALL_ID);
    // }
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
    float visDist, visBearing;

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
            visDist += (visDist * 0.175 + 2)*UNIFORM_1_NEG_1;

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
            visDist += UNIFORM_1_NEG_1*0.05*visDist;

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

    } else {
        e.dist = 0.0f;
        e.bearing = 0.0f;
    }
    return e;
}
