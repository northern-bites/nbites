#include "fakerIO.h"
using namespace std;
using namespace boost;
#define BGLP_ID 30
#define BGRP_ID 31
#define YGLP_ID 32
#define YGRP_ID 33
#define BALL_ID 40
#define NO_DATA_VALUE -111.111f

/**
 * Method to read in a robot path from a formatted file
 *
 * Reads in path defined by robot velocities.
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
    char line[1024];

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
        inputFile->getline(line, 1024);
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
                const ConcreteCorner* corn;
                switch(ids[k]) {
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

                VisualCorner vc(20, 20, dists[k], bearings[k],
                                shared_ptr<VisualLine>(new VisualLine()),
								shared_ptr<VisualLine>(new VisualLine()),
								10.0f, 10.0f);
                vc.setPossibleCorners(toUse);

                // Set ID
				const vector<const ConcreteCorner*> lCorners =
					ConcreteCorner::lCorners();
				for (int i = 0; i < lCorners.size(); ++i){
					const ConcreteCorner* c = lCorners[i];

					if (c == corn){
						vc.setID(L_INNER_CORNER);
					}
				}

				if (vc.getID() != L_INNER_CORNER){
					const vector<const ConcreteCorner*> tCorners =
						ConcreteCorner::tCorners();
					for (int i = 0; i < tCorners.size(); ++i){
						const ConcreteCorner* c = tCorners[i];

						if (c == corn)
							vc.setID(T_CORNER);
					}
				}
				if (vc.getID() != L_INNER_CORNER ||
					vc.getID() != T_CORNER){
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
                << NBMath::subPIAngle(currentPose->h) << " ";

    // print actual ball position
	*outputFile << currentBall->x << " "
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
                        const vector<Observation>& sightings,
						const MotionModel& lastOdo,
                        const PoseEst& currentPose, const BallPose& currentBall,
                        shared_ptr<BallEKF> ballEKF, const VisualBall& _b,
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
                     const vector<Observation>& sightings,
					 const MotionModel& lastOdo,
                     const PoseEst &currentPose, const BallPose& currentBall,
                     shared_ptr<BallEKF> ballEKF, const VisualBall& _b,
                     int team_color, int player_number, int ball_id)
{
    // Output standard infos
    *outputFile << setprecision(6) << team_color<< " " << player_number << "|"
                << myLoc->getXEst() << " " << myLoc->getYEst() << " "
                << myLoc->getHEst() << " "
                << myLoc->getXUncert() << " " << myLoc->getYUncert() << " "
                << myLoc->getHUncert() << " ";

#ifdef USE_MM_LOC_EKF
	shared_ptr<MMLocEKF> mmloc = boost::dynamic_pointer_cast<MMLocEKF>(myLoc);
	const list<LocEKF*> models = mmloc->getModels();
	list<LocEKF*>::const_iterator model;

	*outputFile << ";";

	for(model = models.begin(); model != models.end() ; ++model){
		if (!(*model)->isActive())
			continue;
		*outputFile << (*model)->getXEst() << " " <<
			(*model)->getYEst() << " " <<
			(*model)->getHEst() << " " <<
			(*model)->getXUncert() << " " <<
			(*model)->getYUncert() << " " <<
			(*model)->getHUncert()
					<< ";";	// Split models with ;
	}
	// Split models section from ball and obs section
#endif
	*outputFile << "|";

    // X Estimate
	 *outputFile << (ballEKF->getXEst()) << " "
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
    *outputFile << currentPose.x << " "
                << currentPose.y << " "
                << currentPose.h << " "
		// print actual ball position
                << currentBall.x << " "
                << currentBall.y << " "
                << currentBall.velX << " "
                << currentBall.velY << " ";

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

void printOutPoseDiffHeader(std::fstream* outputFile)
{
	*outputFile << "x y h 2d 3d" << endl;
}

/**
 * Print out the differences between real pose and the
 * loc estimated pose.
 *
 */
void printOutPoseDiffs(std::fstream* outputFile,
					   boost::shared_ptr<LocSystem> myLoc,
					   const PoseEst& currentPose)
{

	*outputFile << setprecision(6)
		// Print in following order:
				<< currentPose.x - myLoc->getXEst() << " "
				<< currentPose.y - myLoc->getYEst() << " "
				<< currentPose.h - myLoc->getHEst() << " "
				<< sqrt(pow(currentPose.x - myLoc->getXEst(),2) +
						pow(currentPose.y - myLoc->getYEst(),2) +
						pow(currentPose.h - myLoc->getHEst(),2)) << " "
				<< sqrt(pow(currentPose.x - myLoc->getXEst(),2) +
						pow(currentPose.y - myLoc->getYEst(),2))
				<< endl;
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

/**
 * Take a robot log and convert it to something we can use in the EKF log
 *
 * @param inputFile The robot log
 * @param outputFile The log to be read in by TOOL
 */
void readRobotLogFile(fstream* inputFile, fstream* outputFile)
{
    vector<Observation> sightings;
    MotionModel lastOdo(0.0f, 0.0f, 0.0f);
    // Known robot and ball data, set to unknown
    PoseEst currentPose(NO_DATA_VALUE, NO_DATA_VALUE, NO_DATA_VALUE);
    BallPose currentBall(NO_DATA_VALUE, NO_DATA_VALUE,
                         NO_DATA_VALUE, NO_DATA_VALUE);

    VisualBall * _b = new VisualBall();

    int teamColor, playerNumber;
    float initX, initY, initH,
        initUncertX, initUncertY, initUncertH,
        initBallX, initBallY,
        initBallXUncert, initBallYUncert,
        initBallVelX, initBallVelY,
        initBallVelXUncert, initBallVelYUncert;

    if(!inputFile->eof()) {
        stringstream headerLine(stringstream::in | stringstream::out);
        char line[1024];
        // Read in the header line
        inputFile->getline(line, 1024);
        headerLine << line;
        headerLine >> teamColor >> playerNumber;

        stringstream startLine(stringstream::in | stringstream::out);
        // Read the EKF start configuration
        inputFile->getline(line, 1024);
        startLine << line;
        startLine >> initX >> initY >> initH
                  >> initUncertX >> initUncertY >> initUncertH
                  >> initBallX >> initBallY
                  >> initBallXUncert >> initBallYUncert
                  >> initBallVelX >> initBallVelY
                  >> initBallVelXUncert >> initBallVelYUncert;
    }
    // Initialize localization systems
    shared_ptr<LocSystem> locEKF  = shared_ptr<MMLocEKF>(
        new MMLocEKF());
    shared_ptr<BallEKF> ballEKF =  shared_ptr<BallEKF>(
        new BallEKF(initBallX, initBallY, initBallVelX, initBallVelY,
                    initBallXUncert, initBallYUncert,
                    initBallVelXUncert, initBallVelYUncert));

    printOutLogLine(outputFile,locEKF, sightings, lastOdo,
                    currentPose, currentBall, ballEKF, *_b,
                    teamColor, playerNumber, BALL_ID);

    float ballDist, ballBearing;
    // Collect the frame by frame data

    while(!inputFile->eof()) {
        stringstream inputLine(stringstream::in | stringstream::out);
        char line[1024];
        inputFile->getline(line, 1024);
        inputLine << line;
        // Read in the base data
        inputLine >> lastOdo.deltaF >> lastOdo.deltaL >> lastOdo.deltaR
                  >> ballDist >> ballBearing;

        // Update Ball
        if (ballDist > 0) {
            _b->setDistanceWithSD(ballDist);
            _b->setBearingWithSD(ballBearing);
        } else {
            _b->setDistanceWithSD(0.0f);
            _b->setBearingWithSD(0.0f);
        }
        RangeBearingMeasurement m(_b);

        // Read in observations
        sightings.clear();

        // Observations are separated by colons
        while(inputLine.peek() == ':') {
            int id;
            char c;
            float dist, bearing, distSD, bearingSD;
            inputLine >> c >> id >> dist >> bearing >> distSD >> bearingSD;

            Observation obs(id, dist, bearing, distSD, bearingSD,
                             Observation::isLineID(id));
            while(inputLine.peek() != ':' &&
                  inputLine.peek() != EOF) {
                PointLandmark p;
                inputLine >> p.x >> p.y;
                obs.addPointPossibility(p);
            }
            sightings.push_back(obs);
        }

        // Update localization
        locEKF->updateLocalization(lastOdo, sightings);
        ballEKF->updateModel(m, locEKF->getCurrentEstimate());

        // Write out the next observation line
        printOutLogLine(outputFile, locEKF, sightings, lastOdo,
                        currentPose, currentBall, ballEKF, *_b,
                        teamColor, playerNumber, BALL_ID);
    }
}
