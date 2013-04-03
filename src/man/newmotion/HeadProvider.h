#pragma once

#include <vector>
#include <queue>

#include "MotionProvider.h"
#include "HeadJointCommand.h"
#include "SetHeadCommand.h"
//#include "CoordHeadCommand.h"
#include "ChopShop.h"
#include "Kinematics.h"

namespace man
{
namespace motion
{

class HeadProvider : public MotionProvider
{
public:
    HeadProvider();
    virtual ~HeadProvider();

    void requestStopFirstInstance();
    void calculateNextJointsAndStiffnesses(
        std::vector<float>&      sensorAngles,
        messages::InertialState& sensorInertials,
        messages::FSR&           sensorFSRs
        );

    void hardReset();

    void enqueueSequence(std::vector<HeadJointCommand::ptr> &seq);
    void setCommand(const HeadJointCommand::ptr command);
    void setCommand(const SetHeadCommand::ptr command);
//    void setCommand(const CoordHeadCommand::ptr command);

private:
    enum HeadMode
    {
        SCRIPTED,
        SET
    };

    void transitionTo(HeadMode newMode);
    void setMode();
    void scriptedMode(std::vector<float>& sensorAngles);
    void stopScripted();
    void stopSet();

    void setActive();
    bool isDone();

    ChopShop chopper;
    std::vector< std::vector<float> > nextJoints;

    std::vector<float> latestJointAngles;

    ChoppedCommand::ptr currChoppedCommand;
    MotionCommand::ptr currHeadCommand;
    // Queue of all future commands
    std::queue<HeadJointCommand::ptr> headCommandQueue;

    HeadMode curMode;
    float yawDest,pitchDest,lastYawDest,lastPitchDest;
    float pitchMaxSpeed, yawMaxSpeed;
    float headSetStiffness;

    std::vector<float> getCurrentHeads(std::vector<float>&);
    void setNextHeadCommand(std::vector<float>&);

    static const float SPEED_CLIP_FACTOR = 0.1f;
};

} // namespace motion
} // namespace man
