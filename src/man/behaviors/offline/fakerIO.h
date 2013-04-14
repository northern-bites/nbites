/**
 * fakerIO.h - all the I/O stuff needed for the fake data makers and readers
 */

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

#ifndef fakerIO_h_DEFINED
#define fakerIO_h_DEFINED
#include <fstream>
#include <iostream>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "NavStructs.h"
#include "Observation.h"
#include "VisualBall.h"
#include "MCL.h"
#include "BallEKF.h"
#include "LocEKF.h"
#include "MMLocEKF.h"

#define USE_MM_LOC_EKF
//#undef USE_MM_LOC_EKF

void readNavInputFile(std::fstream* name, NavPath * letsGo);
void readObsInputFile(std::fstream * inputFile,
                      std::vector<PoseEst> * realPoses,
                      std::vector<BallPose> * ballPoses,
                      std::vector<MotionModel> * odos,
                      std::vector<std::vector<Observation> > * sightings,
                      std::vector<float> * ballDists,
                      std::vector<float> * ballBearings,
                      int ball_id);
void readObsInputLine(std::stringstream * inputLine, PoseEst *currentPose,
                      BallPose * currentBall, MotionModel * currentOdo,
                      std::vector<int> * ids, std::vector<float> * dist,
                      std::vector<float> * bearing);
void printOutObsLine(std::fstream* outputFile,
                     std::vector<Observation> sightings, MotionModel lastOdo,
                     PoseEst *currentPose, BallPose * currentBall,
                     VisualBall _b, int ball_id);
void printOutMCLLogLine(std::fstream* outputFile, boost::shared_ptr<MCL> myLoc,
                        const std::vector<Observation>& sightings,
						const MotionModel& lastOdo,
                        const PoseEst& currentPose,
						const BallPose& currentBall,
                        boost::shared_ptr<BallEKF> ballEKF,
						const VisualBall& _b,
                        int team_color, int player_number, int ball_id);
void printOutLogLine(std::fstream* outputFile,
                     boost::shared_ptr<LocSystem> myLoc,
                     const std::vector<Observation>& sightings,
					 const MotionModel& lastOdo,
                     const PoseEst& currentPose,
					 const  BallPose& currentBall,
                     boost::shared_ptr<BallEKF> ballEKF,
					 const VisualBall& _b,
                     int team_color, int player_number, int ball_id);

void printOutPoseDiffHeader(std::fstream* outputFile);

void printOutPoseDiffs(std::fstream* outputFile,
					   boost::shared_ptr<LocSystem> myLoc,
					   const PoseEst& currentPose);

void printCoreLogLine(std::fstream* outputFile,
                      boost::shared_ptr<LocSystem> myLoc,
                      std::vector<Observation> sightings, MotionModel lastOdo,
                      PoseEst *currentPose, BallPose * currentBall,
                      boost::shared_ptr<BallEKF> ballEKF);

void readRobotLogFile(std::fstream* inputFile, std::fstream* outputFile);

#endif // fakerIO_h_DEFINED
