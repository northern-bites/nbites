#ifndef _Man_h_DEFINED
#define _Man_h_DEFINED

#include <pthread.h>
#include <signal.h>

#include <boost/shared_ptr.hpp>

#include "manconfig.h"


#include "MotionEnactor.h"
#include "ImageSubscriber.h"
#include "ImageTranscriber.h"
#include "Transcriber.h"
#include "Lights.h"
#include "Speech.h"
#include "Common.h"
#include "Profiler.h"
#include "Sensors.h"
#include "Vision.h"
#include "Noggin.h"
#include "Comm.h"
#include "Motion.h"
#include "NaoPose.h"
#include "memory/Memory.h"
#include "memory/log/LoggingBoard.h"
#include "synchro/synchro.h"
#include "RoboGuardian.h"

/**
 * The Naoqi module to run our main Nao robot system.
 *
 * @author Jeremy R. Fishman
 * @author Bowdoin College Northern Bites
 */
class Man : public ImageSubscriber
{
public:

    // contructors
    Man(boost::shared_ptr<Sensors> _sensors,
        boost::shared_ptr<Transcriber> _transcriber,
        boost::shared_ptr<ImageTranscriber> _imageTranscriber,
        boost::shared_ptr<MotionEnactor> _enactor,
        boost::shared_ptr<Lights> _lights,
        boost::shared_ptr<Speech> _speech);
    // destructor
    virtual ~Man();

    //
    // Our methods
    //

    // start/stop called by manmodule
    virtual void startSubThreads();
    virtual void stopSubThreads();

private:
    // run Vision and call Noggin's main loop function
    void processFrame(void);

    void notifyNextVisionImage();

  //
  // Variables
  //
public:
    boost::shared_ptr<Sensors> sensors;
    boost::shared_ptr<RoboGuardian> guardian;
    boost::shared_ptr<Transcriber> transcriber;
    boost::shared_ptr<ImageTranscriber> imageTranscriber;
    boost::shared_ptr<MotionEnactor> enactor;

    // Sub-module instances
    // ** ORDER MATTERS HERE **
    //   if the modules are not instantiated in this order, some dependedcies
    //   (i.e. the Python modules exported) will not be available by the time
    //   other modules are imported
    boost::shared_ptr<NaoPose> pose;
#ifdef USE_MOTION
    boost::shared_ptr<Motion> motion;
#endif
    boost::shared_ptr<Vision> vision;
    boost::shared_ptr<Comm> comm;
    boost::shared_ptr<man::memory::Memory> memory;
    boost::shared_ptr<man::memory::log::LoggingBoard> loggingBoard;

#ifdef USE_NOGGIN
    boost::shared_ptr<Noggin> noggin;
#endif// USE_NOGGIN
    boost::shared_ptr<Lights> lights;
    boost::shared_ptr<Speech> speech;

};


#endif
