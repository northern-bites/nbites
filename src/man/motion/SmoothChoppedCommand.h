
#ifndef __SmoothChoppedCommand_h
#define __SmoothChoppedCommand_h

#include <vector>
#include "Kinematics.h"
#include "JointCommand.h"
#include "ChoppedCommand.h"


// At the moment, this only works for Smooth Interpolation.
// Will later extended to apply to Smooth Interpolation
class SmoothChoppedCommand : public ChoppedCommand
{
public:
    SmoothChoppedCommand( const JointCommand::ptr command,
			  std::vector<float> startJoints,
			  int chops );

    virtual ~SmoothChoppedCommand(void) {  };

    virtual std::vector<float> getNextJoints(int id);

private:
    std::vector<float> startHead;
    std::vector<float> startLArm;
    std::vector<float> startLLeg;
    std::vector<float> startRLeg;
    std::vector<float> startRArm;

    std::vector<float> totalDiffHead;
    std::vector<float> totalDiffLArm;
    std::vector<float> totalDiffLLeg;
    std::vector<float> totalDiffRLeg;
    std::vector<float> totalDiffRArm;

    // Helper methods
    std::vector<float>* getStartChain(int id);
    std::vector<float>* getDiffChain(int id);
    void buildStartChains( const std::vector<float> &startJoints );
    void buildDiffChains( const std::vector<float> &finalJoints );

    void setDiffChainsToFinalJoints(const std::vector<float> &finalJoints );
    void subtractBodyStartFromFinalAngles();
    void subtractChainStartFromFinalAngles(int chain);

    bool isChainFinished(int id);
    std::vector<float> getNextChainFromCycloid(int id);
    float getCycloidStep(int id);
    float getCycloidAngle(float d_theta, float t);

};

#endif
