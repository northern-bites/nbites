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
 * The switchboard and enactor will both exist in their own thread in order to
 * ensure that commands get sent to the low level on time.
 */
#ifndef _Motion_h_DEFINED
#define _Motion_h_DEFINED

#include <boost/shared_ptr.hpp>

#include "synchro.h"
#include "MotionSwitchboard.h"
#include "SimulatorEnactor.h"
#include "MotionInterface.h"
#include "Sensors.h"

class Motion
//  : public MotionCore
{
  public:
#ifdef NAOQI1
    Motion(ALPtr<ALMotionProxy> _proxy,boost::shared_ptr<Synchro> synchro, Sensors *s);
#else
    Motion(ALMotionProxy * _proxy,boost::shared_ptr<Synchro> synchro, Sensors *s);
#endif

    virtual ~Motion() {}
    void start();
    void stop();

private:
    MotionSwitchboard switchboard;
    
    SimulatorEnactor *enactor;
    MotionInterface interface;

};

#endif
