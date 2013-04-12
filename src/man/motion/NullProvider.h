#ifndef NullProvider_h
#define NullProvider_h

#include <boost/shared_ptr.hpp>

#include "MotionProvider.h"
#include "FreezeCommand.h"
#include "UnfreezeCommand.h"

namespace man
{
namespace motion
{
class NullProvider : public MotionProvider
{
public:
    NullProvider(const bool chain_mask[Kinematics::NUM_CHAINS]);
    virtual ~NullProvider();

    void calculateNextJointsAndStiffnesses(
        std::vector<float>&            sensorAngles,
        const messages::InertialState& sensorInertials,
        const messages::FSR&           sensorFSRs
        );
    void hardReset(){} //Not implemented

    void setCommand(const boost::shared_ptr<FreezeCommand> command);
    void setCommand(const boost::shared_ptr<UnfreezeCommand> command);

protected:
    void setActive();
    void requestStopFirstInstance(){} //Not implemented
private:
    void readNewStiffness();
private:
    std::vector<float> nextStiffness,lastStiffness;
    bool chainMask[Kinematics::NUM_CHAINS];
    bool frozen, freezingOn, freezingOff, newCommand;
    bool doOnce;
    boost::shared_ptr<BaseFreezeCommand> nextCommand;
};
} // namespace motion
} // namespace man

#endif
