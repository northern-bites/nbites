#pragma once

#include <vector>
#include "Kinematics.h"
#include "JointCommand.h"
#include "ChoppedCommand.h"

namespace man
{
namespace motion
{

// At the moment, this only works for Linear Interpolation.
// Will later extended to apply to Smooth Interpolation
class LinearChoppedCommand : public ChoppedCommand
{
public:
    LinearChoppedCommand( const JointCommand::ptr command,
			  std::vector<float> currentJoints,
			  int chops );

    virtual ~LinearChoppedCommand(void) {  };

    virtual std::vector<float> getNextJoints(int id);

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

    void incrCurrChain(int id);

    std::vector<float> getDiffPerChop( std::vector<float> current,
				       std::vector<float> final,
				       int numChops );

    // Helper methods
    std::vector<float>* getCurrentChain(int id);
    std::vector<float>* getDiffChain(int id);
    void buildCurrentChains( std::vector<float> currentJoints );
    void buildDiffChains( std::vector<float> diffPerChop );

};

} // namespace motion
} // namespace man
