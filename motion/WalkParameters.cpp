#include "WalkParameters.h"

// WalkParameters static member initialization
const float DEFAULT_JUNK[6 + 4 + 4] = {
  .03f, .015f, .04f, .3f, .015f, .025f, // walk config
  5.0f, -5.0f, 0.19f, 5.0f,             // walk extra config
  0, 0, 0, 0};

const vector<float> WalkParameters::
  DEFAULT_CONFIG(&(DEFAULT_JUNK[0]),
		 &DEFAULT_JUNK[6]);
const vector<float> WalkParameters::
  DEFAULT_EXTRA(&(DEFAULT_JUNK[6]),
		&DEFAULT_JUNK[6 + 4]);
const vector<float> WalkParameters::
  DEFAULT_ARMS(&(DEFAULT_JUNK[6 + 4]),
	       &DEFAULT_JUNK[6 + 4 + 4]);

const WalkParameters WalkParameters::DEFAULT_PARAMS =
  WalkParameters();

// #ifdef NAOQI1
// void WalkParameters::apply(ALPtr<ALMotionProxy> proxy) const {
// #else
// void WalkParameters::apply(ALMotionProxy * proxy) const {
// #endif
//   proxy->setWalkConfig(walkConfig[pMaxStepLength], walkConfig[pMaxStepHeight],
// 		       walkConfig[pMaxStepSide], walkConfig[pMaxStepTurn],
// 		       walkConfig[pZmpOffsetX], walkConfig[pZmpOffsetY]);

//   proxy->setWalkExtraConfig(walkExtraConfig[pLHipRollBacklashCompensator],
// 			    walkExtraConfig[pRHipRollBacklashCompensator],
// 			    walkExtraConfig[pHipHeight],
// 			    walkExtraConfig[pTorsoYOrientation]);

//   proxy->setWalkArmsConfig(walkArmsConfig[pShoulderMedian],
// 			   walkArmsConfig[pShoulderAmplitude],
// 			   walkArmsConfig[pElbowMedian],
// 			   walkArmsConfig[pElbowAmplitude]);
// }
