#include "SmoothChoppedCommand.h"
#include "MotionConstants.h"
#include <math.h>

using namespace Kinematics;

namespace man
{
namespace motion
{

SmoothChoppedCommand::SmoothChoppedCommand(const JointCommand::ptr command,
                       std::vector<float> startJoints,
                       int chops )
    : ChoppedCommand(command, chops)
{
    buildStartChains(startJoints);

    std::vector<float> finalJoints = ChoppedCommand::getFinalJoints(command,
                                   startJoints);
    buildDiffChains( finalJoints );
}

    void SmoothChoppedCommand::buildStartChains(const std::vector<float> &startJoints ) {
    std::vector<float>::const_iterator firstStartJoint = startJoints.begin();
    std::vector<float>::const_iterator chainStart, chainEnd;

    for (unsigned int chain = 0; chain < NUM_CHAINS ; ++chain) {
    std::vector<float> *startChain = getStartChain(chain);

    chainStart = firstStartJoint + chain_first_joint[chain];
    chainEnd = firstStartJoint + chain_last_joint[chain] + 1;
    startChain->assign( chainStart, chainEnd );
    }
}

    void SmoothChoppedCommand::buildDiffChains( const std::vector<float> &finalJoints ) {
    setDiffChainsToFinalJoints(finalJoints);
    subtractBodyStartFromFinalAngles();
}

void SmoothChoppedCommand::setDiffChainsToFinalJoints(
    const std::vector<float> &finalJoints ) {

    std::vector<float>::const_iterator firstFinalJoint = finalJoints.begin();
    std::vector<float>::const_iterator chainStart,chainEnd;

    for (unsigned int chain = 0; chain < NUM_CHAINS ; ++chain) {
    std::vector<float> *diffChain = getDiffChain(chain);

    chainStart = firstFinalJoint + chain_first_joint[chain];
    chainEnd = firstFinalJoint + chain_last_joint[chain] + 1;
    diffChain->assign( chainStart, chainEnd );
    }
}

void SmoothChoppedCommand::subtractBodyStartFromFinalAngles() {
    for (unsigned int chain = 0; chain < NUM_CHAINS ; chain++ )
    subtractChainStartFromFinalAngles(chain);
}

void SmoothChoppedCommand::subtractChainStartFromFinalAngles(int chain){
    std::vector<float>* startChain = getStartChain(chain);
    std::vector<float>* diffChain = getDiffChain(chain);

    std::vector<float>::iterator startAngle = startChain->begin();
    std::vector<float>::iterator diffAngle = diffChain->begin();

    while ( startAngle != startChain->end() ){
    *diffAngle -= *startAngle;
    diffAngle++;
    startAngle++;
    }
}

    std::vector<float>* SmoothChoppedCommand::getDiffChain(int id) {
    switch (id) {
    case HEAD_CHAIN:
    return &totalDiffHead;
    case LARM_CHAIN:
    return &totalDiffLArm;
    case LLEG_CHAIN:
    return &totalDiffLLeg;
    case RLEG_CHAIN:
    return &totalDiffRLeg;
    case RARM_CHAIN:
    return &totalDiffRArm;
    default:
    std::cout << "INVALID CHAINID" << std::endl;
    return new std::vector<float>(0);
    }
}

    std::vector<float> SmoothChoppedCommand::getNextJoints(int id) {
    if ( !isChainFinished(id) ) {
    numChopped.at(id)++;
    checkDone();
    }

    return getNextChainFromCycloid(id);
}

    std::vector<float> SmoothChoppedCommand::getNextChainFromCycloid(int id) {
    float t = getCycloidStep(id);
    std::vector<float> nextChain;
    std::vector<float>* diffChain = getDiffChain(id);
    std::vector<float>* startChain = getStartChain(id);
    std::vector<float>::iterator diffAngle = diffChain->begin();
    std::vector<float>::iterator startAngle = startChain->begin();

    while ( diffAngle != diffChain->end() ) {
    nextChain.push_back(*startAngle + getCycloidAngle(*diffAngle,t) );
    diffAngle++;
    startAngle++;
    }

    return nextChain;
}

float SmoothChoppedCommand::getCycloidAngle(float d_theta, float t) {
    return ( (d_theta/(2*M_PI_FLOAT)) * (t - sinf(t)) );
}

float SmoothChoppedCommand::getCycloidStep( int id ) {
    return ( ( static_cast<float>(numChopped.at(id)) /
           static_cast<float>(numChops) ) * M_PI_FLOAT*2.0f);
}

bool SmoothChoppedCommand::isChainFinished(int id) {
    return (numChopped.at(id) >= numChops);
}
    std::vector<float>* SmoothChoppedCommand::getStartChain(int id) {
    switch (id) {
    case HEAD_CHAIN:
    return &startHead;
    case LARM_CHAIN:
    return &startLArm;
    case LLEG_CHAIN:
    return &startLLeg;
    case RLEG_CHAIN:
    return &startRLeg;
    case RARM_CHAIN:
    return &startRArm;
    default:
    std::cout << "INVALID CHAINID" << std::endl;
    return new std::vector<float>(0);
    }
}

} // namespace motion
} // namespace man
