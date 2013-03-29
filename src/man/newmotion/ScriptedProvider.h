#pragma once

#include <vector>
#include <queue>
#include <boost/shared_ptr.hpp>

#include "MotionProvider.h"
#include "BodyJointCommand.h"
#include "ChopShop.h"
#include "ChoppedCommand.h"
#include "Kinematics.h"

namespace man
{
namespace motion
{

class ScriptedProvider : public MotionProvider {
public:
    ScriptedProvider();
    virtual ~ScriptedProvider();

    void requestStopFirstInstance();
    void calculateNextJointsAndStiffnesses(
        std::vector<float>&      sensorAngles,
        messages::InertialState& sensorInertials,
        messages::FSR&           sensorFSRs
        );

    void hardReset();

    void enqueueSequence(std::vector<BodyJointCommand::ptr> &seq);
    void setCommand(const BodyJointCommand::ptr command);

private:
    ChopShop chopper;
    std::vector<std::vector<float> > nextJoints;

    // The current chopped command which is being enacted
    ChoppedCommand::ptr currCommand;

    // Queue to hold the next body commands
    std::queue<BodyJointCommand::ptr> bodyCommandQueue;

    boost::shared_ptr<std::vector <std::vector <float> > > getCurrentChains(std::vector<float>& sensorAngles);

    void setNextBodyCommand(std::vector<float>& sensorAngles);
    void setActive();
    bool isDone();
    bool currCommandEmpty();
    bool commandQueueEmpty();

};

} // namespace motion
} // namespace man
