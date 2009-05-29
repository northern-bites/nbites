#ifndef NullProvider_h
#define NullProvider_h

#include <boost/shared_ptr.hpp>

#include "MotionProvider.h"
#include "OnFreezeCommand.h"
#include "OffFreezeCommand.h"

#include "Sensors.h"

class NullProvider : public MotionProvider {
public:
    NullProvider(boost::shared_ptr<Sensors> s, std::vector<bool> chain_mask);
    virtual ~NullProvider();

    void calculateNextJointsAndStiffnesses();
    void hardReset(){} //Not implemented

    void setCommand(const boost::shared_ptr<OnFreezeCommand> command);
    void setCommand(const boost::shared_ptr<OffFreezeCommand> command);

protected:
    void setActive();
    void requestStopFirstInstance(){} //Not implemented
private:
    void readNewStiffness();
private:
    boost::shared_ptr<Sensors> sensors;
    std::vector<float> currentStiffness,lastStiffness;
    std::vector<bool> chainMask;
    mutable pthread_mutex_t null_provider_mutex;
    bool frozen, freezingOn, freezingOff, newCommand;
    boost::shared_ptr<FreezeCommand> nextCommand;
};

#endif
