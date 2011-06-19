

#ifndef __ChoppedCommand_h
#define __ChoppedCommand_h

#include <vector>
#include "JointCommand.h"
#include "Kinematics.h"
#include "dsp.h" // for preview filter

// At the moment, this only works for Linear Interpolation.
// Will later extended to apply to Smooth Interpolation
class ChoppedCommand
{
 public:
    // HACK: Empty constructor. Will initialize a finished
    // body joint command with no values. Don't use!
    // ***SHOULD NOT BE USED***
    ChoppedCommand() : finished(true) { }

    ChoppedCommand ( const JointCommand *command, int chops );
    virtual ~ChoppedCommand(void) { }

    virtual std::vector<float> getNextJoints(int id) {
        return std::vector<float>(0);
    }

    const std::vector<float> getStiffness( Kinematics::ChainID chaindID) const;
    bool isDone() const { return finished; }

 protected:
    void checkDone();

    std::vector<float> getFinalJoints(const JointCommand *command,
                                      std::vector<float> currentJoints);

private:
    void constructStiffness( const JointCommand *command);
    void constructChainStiffness(Kinematics::ChainID id,
                                 const JointCommand *command);
    std::vector<float>* getStiffnessRef( Kinematics::ChainID chainID);

 protected:
    int numChops;
    std::vector<int> numChopped;
    int motionType;
    int interpolationType;
    bool finished;

 private:
    // filter for upcoming CoM changes goes here
    std::vector<float> head_stiff, larm_stiff, rarm_stiff;
    std::vector<float> lleg_stiff, rleg_stiff;

};

#endif
