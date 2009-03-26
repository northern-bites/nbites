

#ifndef __ChoppedCommand_h
#define __ChoppedCommand_h

#include <vector>
#include "Kinematics.h"
#include "MotionConstants.h"

using std::vector;

// At the moment, this only works for Linear Interpolation.
// Will later extended to apply to Smooth Interpolation
class ChoppedCommand
{
public:

	// Empty constructor. Will initialize a finished
	// body joint command with no values. Don't use!
	// ***SHOULD NOT BE USED***
	ChoppedCommand() : finished(true) { }
	
	ChoppedCommand (vector<float> *first,
					vector<float> *diffs,
					int chops,int motionType);
	vector<float> getNextJoints(int id);
	bool isDone();

private:
	// Current Joint Chains
	vector<float> currentHead;
	vector<float> currentLArm;
	vector<float> currentLLeg;
	vector<float> currentRLeg;
	vector<float> currentRArm;
	// Diff chains
	vector<float> diffHead;
	vector<float> diffLArm;
	vector<float> diffLLeg;
	vector<float> diffRLeg;
	vector<float> diffRArm;

	int numChops;
	vector<int> numChopped;
	int motionType;
	bool finished;

	vector<float>* getCurrentChain(int id);
	vector<float>* getDiffChain(int id);
	void incrCurrChain(int id);
	void checkDone();
};

#endif
