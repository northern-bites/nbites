#ifndef NullProvider_h
#define NullProvider_h

#include <boost/shared_ptr.hpp>

#include "MotionProvider.h"
#include "OnFreezeCommand.h"
#include "OffFreezeCommand.h"


class NullProvider : public MotionProvider {
public:
    NullProvider();
    ~NullProvider();

    virtual void calculateNextJointsAndStiffnesses();
    virtual void hardReset();

    virtual void setCommand(const boost::shared_ptr<OnFreezeCommand> command);
    virtual void setCommand(const boost::shared_ptr<OffFreezeCommand> command);

protected:
    virtual void setActive();
    virtual void requestStopFirstInstance();
};

#endif
