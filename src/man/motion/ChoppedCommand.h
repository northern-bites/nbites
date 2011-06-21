

#ifndef __ChoppedCommand_h
#define __ChoppedCommand_h

#include <vector>
#include <boost/shared_ptr.hpp>

#include "JointCommand.h"
#include "Kinematics.h"

class ChoppedCommand
{
 public:
    typedef boost::shared_ptr<ChoppedCommand> ptr;

    // HACK: Empty constructor. Will initialize a finished
    // body joint command with no values. Don't use!
    // ***SHOULD NOT BE USED***
    ChoppedCommand() : finished(true) { }

    ChoppedCommand ( const JointCommand *command, int chops );
    virtual ~ChoppedCommand(void) { }

    virtual std::vector<float> getNextJoints(int id) {
        return std::vector<float>(0);
    }
    virtual const std::vector<float> getStiffness( Kinematics::ChainID chaindID) const;
    virtual bool isDone() const { return finished; }

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
    std::vector<float> head_stiff, larm_stiff, rarm_stiff;
    std::vector<float> lleg_stiff, rleg_stiff;

};

#endif
