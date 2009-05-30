

#ifndef __ChoppedCommand_h
#define __ChoppedCommand_h

#include <vector>
#include "JointCommand.h"


// At the moment, this only works for Linear Interpolation.
// Will later extended to apply to Smooth Interpolation
class ChoppedCommand
{
public:

	// HACK: Empty constructor. Will initialize a finished
	// body joint command with no values. Don't use!
	// ***SHOULD NOT BE USED***
	ChoppedCommand() : finished(true) { }

	virtual ~ChoppedCommand(void) {  };

	ChoppedCommand ( const JointCommand *command, int chops );

	virtual std::vector<float> getNextJoints(int id) {
		return std::vector<float>(0);
			}
	bool isDone() { return finished; }

protected:
	void checkDone();

	std::vector<float> getFinalJoints(const JointCommand *command,
                                      std::vector<float> currentJoints);

	int numChops;
	std::vector<int> numChopped;
	int motionType;
	int interpolationType;
	bool finished;

private:



};

#endif
