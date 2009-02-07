/**
 * Motion is a module-level connection between all the core aspects of the
 * motion module.
 *
 * It has the following jobs:
 *  - Create a switchboard, which will select between walk engine, chop engine,
 *    etc.
 *  - Create the approriate enactor, which is the layer that communicates
 *    joint angles between the switchboard and the target platform (e.g. robot,
 *    simulator)
 *  - Instantiate a MotionInterface for PyMotion to use to communicate with
 *    Cpp motion module.
 *
 * The 'Motion' thread is really the switchboard thread (see MotionSwitchboard
 * for details). The enactor will also create its own thread, which is dependant
 * on the platform (i.e. simulator or real robot)
 */
#ifndef _Motion_h_DEFINED
#define _Motion_h_DEFINED

#include <boost/shared_ptr.hpp>

#include "synchro.h"
#include "MotionSwitchboard.h"
#include "SimulatorEnactor.h"
#include "ALEnactor.h"
#include "MotionInterface.h"
#include "Sensors.h"

class Motion : public Thread
{
public:
#ifdef NAOQI1
    Motion(boost::shared_ptr<Synchro> synchro,
           boost::shared_ptr<MotionEnactor> _enactor,
           boost::shared_ptr<Sensors> s);
#else
    Motion(ALMotionProxy * _proxy, boost::shared_ptr<Synchro> synchro,
            boost::shared_ptr<Sensors> s);
#endif
    ~Motion();

    int start();
    void stop();
    void run();
private:
    MotionSwitchboard switchboard;
#ifdef NAOQI1
    boost::shared_ptr<MotionEnactor> enactor;
#else
    boost::shared_ptr<SimulatorEnactor> enactor;
#endif
    MotionInterface interface;
};

#endif
