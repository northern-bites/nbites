/* fakerIterators.h */

#ifndef fakerIterators_h_DEFINED
#define fakerIterators_h_DEFINED

#include <time.h> // for srand(time(NULL))
#include <cstdlib> // for MAX_RAND
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "VisualBall.h"
#include "VisionDef.h" // For NAO_FOV_X_DEG
#include "EKFStructs.h"
#include "Observation.h"
#include "NavStructs.h"
#include "fakerIO.h"

// Observation parameter
// Ranges at which objects are viewable
// Field objects
#define FO_MAX_VIEW_RANGE 575.0f
#define CORNER_MAX_VIEW_RANGE 200.0f
#define LINE_MAX_VIEW_RANGE 250.0f
#define BALL_ID 40
#define TEAM_COLOR 0
#define PLAYER_NUMBER 3

static const bool usePerfectLocForBall = false;
static const bool use_perfect_dists = false;

// Get half of the nao FOV converted to radians
static float FOV_OFFSET = NAO_FOV_X_DEG * M_PI / 360.0f + M_PI / 4.0f;

void iterateNavPath(std::fstream * obsFile, NavPath * letsGo);
void iterateObsPath(std::fstream * locFile, std::fstream * coreFile,
                    boost::shared_ptr<LocSystem> loc,
                    std::vector<PoseEst> * realPoses,
                    std::vector<BallPose> * ballPoses,
                    std::vector<MotionModel> * odos,
                    std::vector<std::vector<Observation> > * sightings,
                    std::vector<float> * ballDists,
                    std::vector<float> * ballBearings,
                    int ball_id);
void iterateMCLObsPath(std::fstream * locFile, std::fstream * coreFile,
                       boost::shared_ptr<MCL> loc,
                       std::vector<PoseEst> * realPoses,
                       std::vector<BallPose> * ballPoses,
                       std::vector<MotionModel> * odos,
                       std::vector<std::vector<Observation> > * sightings,
                       std::vector<float> * ballDists,
                       std::vector<float> * ballBearings,
                       int ball_id);

void iterateFakerPath(std::fstream * mclFile, std::fstream * ekfFile,
					  std::fstream * ekfDiffFile,
                      NavPath * letsGo, float noiseLevel = 0.05);
void checkObjects(std::vector<Observation> &Z_t, PoseEst myPos,
				  float noiseLevel);
void checkCorners(std::vector<Observation> &Z_t, PoseEst myPos,
				  float noiseLevel);
void checkCrosses(std::vector<Observation> &Z_t, PoseEst myPos,
				  float noiseLevel);
void checkLines(std::vector<Observation> &Z_t, PoseEst myPos);
std::vector<Observation> determineObservedLandmarks(PoseEst myPos,
                                                    float neckYaw,
                                                    float noiseLevel = 0.05);
estimate determineBallEstimate(PoseEst * currentPose, BallPose * currentBall,
                               float neckYaw);
float sampleNormalDistribution(float sd);
float sampleTriangularDistribution(float sd);

#endif // obsToLoc_h_DEFINED

std::pair<float,float>
findClosestLinePointCartesian(LineLandmark l, float x_r,
							  float y_r, float h_r);
