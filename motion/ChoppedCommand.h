

#ifndef __ChoppedCommand_h
#define __ChoppedCommand_h

#include <vector>
#include "Kinematics.h"
#include "MotionConstants.h"

// At the moment, this only works for Linear Interpolation.
// Will later extended to apply to Smooth Interpolation
class ChoppedCommand
{
public:
	ChoppedCommand (std::vector<float> *first,
					std::vector<float> *diffs,
					int chops,int motionType);
	std::vector<float> getNextJoints(int id);
	bool isDone();

private:
	// Current Joint Chains
	std::vector<float> currentHead;
	std::vector<float> currentLArm;
	std::vector<float> currentLLeg;
	std::vector<float> currentRLeg;
	std::vector<float> currentRArm;
	// Diff chains
	std::vector<float> diffHead;
	std::vector<float> diffLArm;
	std::vector<float> diffLLeg;
	std::vector<float> diffRLeg;
	std::vector<float> diffRArm;

	int numChops;
	std::vector<int> numChopped;
	int motionType;

	std::vector<float>* getCurrentChain(int id);
	std::vector<float>* getDiffChain(int id);
};

#endif
