#ifndef NullProvider_h
#define NullProvider_h

#include <boost/shared_ptr.hpp>

#include "MotionProvider.h"
#include "FreezeCommand.h"
#include "UnfreezeCommand.h"

#include "Sensors.h"
#include "Kinematics.h"

class NullProvider : public MotionProvider {
public:
    NullProvider(boost::shared_ptr<Sensors> s,
                 const bool chain_mask[Kinematics::NUM_CHAINS]);
    virtual ~NullProvider();

    void calculateNextJointsAndStiffnesses();
    void hardReset(){} //Not implemented

    void setCommand(const boost::shared_ptr<FreezeCommand> command);
    void setCommand(const boost::shared_ptr<UnfreezeCommand> command);

protected:
    void setActive();
    void requestStopFirstInstance(){} //Not implemented
private:
    void readNewStiffness();
private:
    boost::shared_ptr<Sensors> sensors;
    std::vector<float> nextStiffness,lastStiffness;
    bool chainMask[Kinematics::NUM_CHAINS];
    mutable pthread_mutex_t null_provider_mutex;
    bool frozen, freezingOn, freezingOff, newCommand;
    bool doOnce;
    boost::shared_ptr<BaseFreezeCommand> nextCommand;
};

#endif
