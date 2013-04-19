#include "fakerIterators.h"
#include "fakerIO.h"
using namespace std;
using namespace boost;
using namespace NBMath;

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
    // printOutObsLine(obsFile, Z_t, noMove, &currentPose,
    //                 &currentBall, *visBall, BALL_ID);

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

            // printOutObsLine(obsFile, Z_t, letsGo->myMoves[i].move,
            //                 &currentPose, &currentBall, *visBall, BALL_ID);
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
void iterateFakerPath(fstream * mclFile, fstream * ekfFile,
					  fstream * ekfDiffFile, NavPath * letsGo,
                      float noiseLevel)
{
    // Method variables
    vector<Observation> Z_t;
    // shared_ptr<MCL> mclLoc = shared_ptr<MCL>(new MCL(100));
    // shared_ptr<BallEKF> MCLballEKF = shared_ptr<BallEKF>(new BallEKF());
#ifdef USE_MM_LOC_EKF
    shared_ptr<LocSystem> ekfLoc = shared_ptr<LocSystem>(new MMLocEKF());
#else
    shared_ptr<LocSystem> ekfLoc = shared_ptr<LocSystem>(new LocEKF());
#endif
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
    // printOutMCLLogLine(mclFile, mclLoc, Z_t, noMove, &currentPose,
    //                  &currentBall, MCLballEKF);
    printOutLogLine(ekfFile, ekfLoc, Z_t, noMove, currentPose,
                    currentBall, EKFballEKF,
                    *visBall, TEAM_COLOR, PLAYER_NUMBER, BALL_ID);
	printOutPoseDiffs(ekfDiffFile, ekfLoc, currentPose);

    unsigned frameCounter = 0;
    // Iterate through the moves
    for(unsigned int i = 0; i < letsGo->myMoves.size(); ++i) {

        // Continue the move for as long as specified
        for (int j = 0; j < letsGo->myMoves[i].time; ++j, ++ frameCounter) {

            // Determine the current frame info
            currentPose += letsGo->myMoves[i].move;
            currentBall += letsGo->myMoves[i].ballVel;

            Z_t = determineObservedLandmarks(currentPose, 0.0, noiseLevel);

            // Figure out the current ball distance and bearing
            visBall->setDistanceEst(determineBallEstimate(&currentPose,
                                                          &currentBall,
                                                          0.0));
            RangeBearingMeasurement m(visBall);

            // Update the MCL sytem
            // mclLoc->updateLocalization(letsGo->myMoves[i].move, Z_t);
            // // Update the MCL ball

            // if (usePerfectLocForBall) {
            //     MCLballEKF->updateModel(m, currentPose;
            // } else {
            //     MCLballEKF->updateModel(m, mclLoc->getCurrentEstimate());
            // }

            // Update the EKF sytem
            ekfLoc->updateLocalization(letsGo->myMoves[i].move, Z_t);
            // Update the EKF ball
            if (usePerfectLocForBall) {
                EKFballEKF->updateModel(m, currentPose);
            } else {
                EKFballEKF->updateModel(m, ekfLoc->getCurrentEstimate());
            }

            // Print the current MCL frame to file
            // printCoreLogLine(mclFile, mclLoc, Z_t, letsGo->myMoves[i].move,
            //                  &currentPose, &currentBall, MCLballEKF);
            // Print the current EKF frame to file
            printOutLogLine(ekfFile, ekfLoc, Z_t, letsGo->myMoves[i].move,
                            currentPose, currentBall, EKFballEKF,
                            *visBall, TEAM_COLOR, PLAYER_NUMBER, BALL_ID);
			printOutPoseDiffs(ekfDiffFile, ekfLoc, currentPose);
        }
    }

    delete visBall;
}

void iterateObsPath(fstream * locFile, fstream * coreFile,
                    shared_ptr<LocSystem> loc,
                    vector<PoseEst> * realPoses,
                    vector<BallPose> * ballPoses,
                    vector<MotionModel> * odos,
                    vector<vector<Observation> > * sightings,
                    vector<float> * ballDists, vector<float> * ballBearings,
                    int ball_id)

{

    shared_ptr<BallEKF> ballEKF = shared_ptr<BallEKF>(new BallEKF());
    MotionModel noMove(0.0, 0.0, 0.0);

    VisualBall * visBall = new VisualBall();
    visBall->setDistanceWithSD(0.0f);
    visBall->setBearingWithSD(0.0f);
    vector<Observation> sx;

    // printOutLogLine(locFile, loc, sx, noMove,
    //                 &(*realPoses)[0], &(*ballPoses)[0],
    //                 ballEKF, *visBall,
    //                 TEAM_COLOR, PLAYER_NUMBER, BALL_ID);

    printCoreLogLine(coreFile, loc, sx, noMove,
                     &(*realPoses)[0], &(*ballPoses)[0],
                     ballEKF);

    for(unsigned int i = 0; i < realPoses->size(); ++i) {
        // Update the localization sytem
        loc->updateLocalization((*odos)[i], (*sightings)[i]);
        visBall->setDistanceWithSD((*ballDists)[i]);
        visBall->setBearingWithSD((*ballBearings)[i]);

        // Update the EKF ball
        RangeBearingMeasurement m(visBall);
        ballEKF->updateModel(m, loc->getCurrentEstimate());

        // printOutLogLine(locFile, loc, (*sightings)[i], (*odos)[i],
        //                 &(*realPoses)[i], &(*ballPoses)[i],
        //                 ballEKF, *visBall,
        //                 TEAM_COLOR, PLAYER_NUMBER, BALL_ID);
        printCoreLogLine(coreFile, loc, (*sightings)[i], (*odos)[i],
                         &(*realPoses)[i], &(*ballPoses)[i],
                         ballEKF);
    }
    delete visBall;
}

void iterateMCLObsPath(fstream * locFile, fstream * coreFile,
                       shared_ptr<MCL> loc,
                       vector<PoseEst> * realPoses,
                       vector<BallPose> * ballPoses,
                       vector<MotionModel> * odos,
                       vector<vector<Observation> > * sightings,
                       vector<float> * ballDists, vector<float> * ballBearings,
                       int ball_id)

{

    shared_ptr<BallEKF> ballEKF = shared_ptr<BallEKF>(new BallEKF());
    MotionModel noMove(0.0, 0.0, 0.0);

    VisualBall * visBall = new VisualBall();
    visBall->setDistanceWithSD(0.0f);
    visBall->setBearingWithSD(0.0f);
    vector<Observation> sx;

    // printOutMCLLogLine(locFile, loc, sx, noMove,
    //                    &(*realPoses)[0], &(*ballPoses)[0],
    //                    ballEKF, *visBall,
    //                    TEAM_COLOR, PLAYER_NUMBER, BALL_ID);

    printCoreLogLine(coreFile, loc, sx, noMove,
                     &(*realPoses)[0], &(*ballPoses)[0],
                     ballEKF);

    for(unsigned int i = 0; i < realPoses->size(); ++i) {
        // Update the localization sytem
        loc->updateLocalization((*odos)[i], (*sightings)[i]);
        visBall->setDistanceWithSD((*ballDists)[i]);
        visBall->setBearingWithSD((*ballBearings)[i]);

        // Update the EKF ball
        RangeBearingMeasurement m(visBall);
        ballEKF->updateModel(m, loc->getCurrentEstimate());

        // printOutMCLLogLine(locFile, loc, (*sightings)[i], (*odos)[i],
        //                    &(*realPoses)[i], &(*ballPoses)[i],
        //                    ballEKF, *visBall,
        //                    TEAM_COLOR, PLAYER_NUMBER, BALL_ID);
        printCoreLogLine(coreFile, loc, (*sightings)[i], (*odos)[i],
                         &(*realPoses)[i], &(*ballPoses)[i],
                         ballEKF);
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
vector<Observation> determineObservedLandmarks(PoseEst myPos, float neckYaw,
                                               float noiseLevel)
{
    vector<Observation> Z_t;

	checkObjects(Z_t, myPos, noiseLevel);
	checkCrosses(Z_t, myPos, noiseLevel);
	checkCorners(Z_t, myPos, noiseLevel);
	//checkLines(Z_t, myPos);

    return Z_t;
}

void checkObjects(vector<Observation> &Z_t, PoseEst myPos, float noiseLevel)
{
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
            if (!use_perfect_dists) {
                visDist += sampleNormalDistribution(visDist * noiseLevel);
            }

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
}

void checkCrosses(vector<Observation> &Z_t, PoseEst myPos, float noiseLevel)
{
    for(int i = 0; i < ConcreteCross::NUM_FIELD_CROSSES; ++i) {
		const ConcreteCross* toView = ConcreteCross::concreteCrossList[i];
        float deltaX = toView->getFieldX() - myPos.x;
        float deltaY = toView->getFieldY() - myPos.y;
        float visDist = hypot(deltaX, deltaY);
        float visBearing = subPIAngle(atan2(deltaY, deltaX) - myPos.h);

        // Check if the object is viewable
        if ((visBearing > -FOV_OFFSET && visBearing < FOV_OFFSET) &&
            visDist < CORNER_MAX_VIEW_RANGE) {

            // Get measurement variance and add noise to reading
            if(!use_perfect_dists) {
                visDist += sampleNormalDistribution(visDist*noiseLevel);
            }

            const crossID id = toView->getID();
            // Build the visual corner
            VisualCross vc = VisualCross(id);
            vc.setDistanceWithSD(visDist);
			vc.setBearingWithSD(visBearing);

            // Build the observation
            Observation seen(vc);
            Z_t.push_back(seen);
        }
    }
}

void checkCorners(vector<Observation> &Z_t, PoseEst myPos, float noiseLevel)
{
    // required measurements for the added observation
    float visDist, visBearing;

    // Check concrete corners
    for(int i = 0; i < ConcreteCorner::NUM_CORNERS; ++i) {
		const ConcreteCorner* toView = ConcreteCorner::concreteCorners()[i];
        float deltaX = toView->getFieldX() - myPos.x;
        float deltaY = toView->getFieldY() - myPos.y;
        visDist = hypot(deltaX, deltaY);
        visBearing = subPIAngle(atan2(deltaY, deltaX) - myPos.h);

        // Check if the object is viewable
        if ((visBearing > -FOV_OFFSET && visBearing < FOV_OFFSET) &&
            visDist < CORNER_MAX_VIEW_RANGE) {

            // Get measurement variance and add noise to reading
            if(!use_perfect_dists) {
                visDist += sampleNormalDistribution(visDist*noiseLevel);
            }

            // Ignore the center circle for right now
            if (toView->getID() == CENTER_CIRCLE) {
                continue;
            }
            const cornerID id = toView->getID();
            vector <const ConcreteCorner*> toUse;
            // Randomly set ambiguous data
            if ((rand() / (float(RAND_MAX)+1)) < 0.50) {
                shape s = ConcreteCorner::inferCornerType(id);
                toUse = ConcreteCorner::getPossibleCorners(s);
            } else {
                const ConcreteCorner * corn;
                switch(id) {
                case BLUE_CORNER_TOP_L:
                    corn = &ConcreteCorner::blue_corner_top_l();
                    break;
                case BLUE_CORNER_BOTTOM_L:
                    corn = &ConcreteCorner::blue_corner_bottom_l();
                    break;
                case BLUE_GOAL_LEFT_T:
                    corn = &ConcreteCorner::blue_goal_left_t();
                    break;
                case BLUE_GOAL_RIGHT_T:
                    corn = &ConcreteCorner::blue_goal_right_t();
                    break;
                case BLUE_GOAL_LEFT_L:
                    corn = &ConcreteCorner::blue_goal_left_l();
                    break;
                case BLUE_GOAL_RIGHT_L:
                    corn = &ConcreteCorner::blue_goal_right_l();
                    break;
                case CENTER_TOP_T:
                    corn = &ConcreteCorner::center_top_t();
                    break;
                case CENTER_BOTTOM_T:
                    corn = &ConcreteCorner::center_bottom_t();
                    break;
                case YELLOW_CORNER_TOP_L:
                    corn = &ConcreteCorner::yellow_corner_top_l();
                    break;
                case YELLOW_CORNER_BOTTOM_L:
                    corn = &ConcreteCorner::yellow_corner_bottom_l();
                    break;
                case YELLOW_GOAL_LEFT_T:
                    corn = &ConcreteCorner::yellow_goal_left_t();
                    break;
                case YELLOW_GOAL_RIGHT_T:
                    corn = &ConcreteCorner::yellow_goal_right_t();
                    break;
                case YELLOW_GOAL_LEFT_L:
                    corn = &ConcreteCorner::yellow_goal_left_l();
                    break;
                case YELLOW_GOAL_RIGHT_L:
                    // Intentional fall through
                default:
                    corn = &ConcreteCorner::yellow_goal_right_l();
                    break;
                }
                // Append to the list
                toUse.assign(1,corn);
            }

            // Build the visual corner
            VisualCorner vc(20, 20, visDist,visBearing,
                            shared_ptr<VisualLine>(new VisualLine()),
							shared_ptr<VisualLine>(new VisualLine()),
							10.0f, 10.0f);
            vc.setPossibleCorners(toUse);

            // Set ID
            if (toUse == ConcreteCorner::lCorners()) {
                vc.setID(L_INNER_CORNER);
            } else if (toUse == ConcreteCorner::tCorners()) {
                vc.setID(T_CORNER);
            } else {
                vc.setID(id);
            }
            // Build the observation
            Observation seen(vc);
            Z_t.push_back(seen);
        }
    }
}

void checkLines(vector<Observation> &Z_t, PoseEst myPos)
{

    // Check concrete lines
	for (int i = 0; i < ConcreteLine::NUM_LINES; ++i) {
		const ConcreteLine *toView = ConcreteLine::concreteLines()[i];
		LineLandmark ll(toView->getFieldX1(),
						toView->getFieldY1(),
						toView->getFieldX2(),
						toView->getFieldY2());
		std::pair<float,float> lineDelta =
			findClosestLinePointCartesian(ll, myPos.x, myPos.y, myPos.h);

		const float distance = hypot(lineDelta.first, lineDelta.second);
		const float bearing = subPIAngle(safe_atan2(lineDelta.second, lineDelta.first) - myPos.h);

		const lineID id = toView->getID();
		list<const ConcreteLine*> toUse;

		const ConcreteLine * line;
		switch(id) {
		case BLUE_GOAL_ENDLINE:
			line = &ConcreteLine::blue_goal_endline();
			break;
		case YELLOW_GOAL_ENDLINE:
			line = &ConcreteLine::yellow_goal_endline();
			break;
		case TOP_SIDELINE:
			line = &ConcreteLine::top_sideline();
			break;
		case BOTTOM_SIDELINE:
			line = &ConcreteLine::bottom_sideline();
			break;
		case BLUE_GOALBOX_TOP_LINE:
			line = &ConcreteLine::blue_goalbox_top_line();
			break;
		case BLUE_GOALBOX_LEFT_LINE:
			line = &ConcreteLine::blue_goalbox_left_line();
			break;
		case BLUE_GOALBOX_RIGHT_LINE:
			line = &ConcreteLine::blue_goalbox_right_line();
			break;
		case YELLOW_GOALBOX_TOP_LINE:
			line = &ConcreteLine::yellow_goalbox_top_line();
			break;
		case YELLOW_GOALBOX_LEFT_LINE:
			line = &ConcreteLine::yellow_goalbox_left_line();
			break;
		case YELLOW_GOALBOX_RIGHT_LINE:
		case UNKNOWN_LINE:
		case SIDE_OR_ENDLINE:
		case SIDELINE_LINE:
		case ENDLINE_LINE:
		case GOALBOX_LINE:
		case GOALBOX_SIDE_LINE:
		case GOALBOX_TOP_LINE:
		default:
			line = &ConcreteLine::yellow_goalbox_right_line();
			break;
		}
		toUse.assign(1, line);
		VisualLine vl = VisualLine(distance, bearing);
		vl.setPossibleLines(toUse);
		vl.setID(id);
		Observation seen(vl);
		Z_t.push_back(seen);
	}
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
        if (!use_perfect_dists) {
            e.dist += sampleNormalDistribution(e.dist*0.05);
        }

    } else {
        e.dist = 0.0f;
        e.bearing = 0.0f;
    }
    return e;
}

float sampleNormalDistribution(float sd)
{
    float samp = 0;
    for(int i = 0; i < 12; i++) {
        samp += (2*(rand() / float(RAND_MAX)) * sd) - sd;
    }
    return 0.5*samp;
}

float sampleTriangularDistribution(float sd)
{
    return sqrt(6.0)*0.5 * ((2*sd*(rand() / float(RAND_MAX))) - sd +
                            (2*sd*(rand() / float(RAND_MAX))) - sd);
}

std::pair<float,float>
findClosestLinePointCartesian(LineLandmark l, float x_r,
									  float y_r, float h_r)
{
	const float x_l = l.dx;
	const float y_l = l.dy;

	const float x_b = l.x1;
	const float y_b = l.y1;

	// Find closest point on the line to the robot (global frame)
	const float x_p = ((x_r - x_b)*x_l + (y_r - y_b)*y_l)*x_l + x_b;
	const float y_p = ((x_r - x_b)*x_l + (y_r - y_b)*y_l)*y_l + y_b;

	// Relativize the closest point
	const float relX_p = x_p - x_r;
	const float relY_p = y_p - y_r;
	return std::pair<float,float>(relX_p, relY_p);
}
