#ifndef _ALMan_h
#define _ALMan_h

#include "albroker.h"

#include "Man.h"

class ALMan : public Man {
public:

    ALMan(AL::ALPtr<AL::ALBroker> broker);
    virtual ~ALMan();

    void startSubThreads();

    void stopSubThreads();

private:
    boost::shared_ptr<Sensors> sensors;

};

#endif
