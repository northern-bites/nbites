#include <iostream>
#include <boost/shared_ptr.hpp>

#include "Man.h"
#include "manconfig.h"
#include "synchro/synchro.h"
#include "VisionDef.h"
#include "Common.h"
#include "PyRoboGuardian.h"
#include "PySensors.h"
#include "PyLights.h"
#include "PySpeech.h"
#include "memory/log/IOProviderFactory.h"

//#include <valgrind/callgrind.h>

using namespace std;
using boost::shared_ptr;
using namespace man::memory;
using namespace man::memory::log;

/////////////////////////////////////////
//                                     //
//  Module class function definitions  //
//                                     //
/////////////////////////////////////////

Man::Man (shared_ptr<Sensors> _sensors,
          shared_ptr<Transcriber> _transcriber,
          shared_ptr<ImageTranscriber> _imageTranscriber,
          shared_ptr<MotionEnactor> _enactor,
          shared_ptr<Lights> _lights,
          shared_ptr<Speech> _speech)
    :     sensors(_sensors),
          transcriber(_transcriber),
          imageTranscriber(_imageTranscriber),
          enactor(_enactor),
          lights(_lights),
          speech(_speech)
{
#ifdef USE_TIME_PROFILING
    Profiler::getInstance()->profileFrames(700);
#endif

    // give python a pointer to the sensors structure. Method defined in
    // Sensors.h
    set_sensors_pointer(sensors);

    imageTranscriber->setSubscriber(this);

    guardian = shared_ptr<RoboGuardian>(new RoboGuardian(sensors));

    pose = shared_ptr<NaoPose> (new NaoPose(sensors));

    // initialize core processing modules
#ifdef USE_MOTION
    motion = shared_ptr<Motion> (new Motion(enactor, sensors, pose));
    guardian->setMotionInterface(motion->getInterface());
#endif
    // initialize python roboguardian module.
    // give python a pointer to the guardian. Method defined in PyRoboguardian.h
    set_guardian_pointer(guardian);
    set_lights_pointer(_lights);
    set_speech_pointer(_speech);

    vision = shared_ptr<Vision> (new Vision(pose));

    set_vision_pointer(vision);

    comm = shared_ptr<Comm> (new Comm(sensors, vision));

#ifdef USE_NOGGIN
    noggin = shared_ptr<Noggin> (new Noggin(vision, comm, guardian, sensors,
                                            loggingBoard,
                                            motion->getInterface()));
#endif// USE_NOGGIN

    memory = shared_ptr<Memory> (new Memory(vision, sensors,noggin->loc));

    loggingBoard = shared_ptr<LoggingBoard> (new LoggingBoard(memory));
    set_logging_board_pointer(loggingBoard);
    memory->addSubscriber(loggingBoard.get());

#ifdef USE_MEMORY
    loggingBoard->newIOProvider(IOProviderFactory::newAllObjectsProvider());
#endif
}

Man::~Man ()
{
  cout << "Man destructor" << endl;
}

void Man::startSubThreads() {

#ifdef DEBUG_MAN_THREADING
    cout << "Man starting" << endl;
#endif

    if (guardian->start() != 0)
        cout << "Guardian failer to start" << endl;

    if (comm->start() != 0)
        cout << "Comm failed to start" << endl;

#ifdef USE_MOTION
    // Start Motion thread (it handles its own threading
    if (motion->start() != 0)
        cout << "Motion failed to start" << endl;
#endif

    //  CALLGRIND_START_INSTRUMENTATION;
    //  CALLGRIND_TOGGLE_COLLECT;
}

void Man::stopSubThreads() {

#ifdef DEBUG_MAN_THREADING
    cout << "Man stopping: " << endl;
#endif

    guardian->stop();
    guardian->waitForThreadToFinish();

#ifdef USE_MOTION
    motion->stop();
    motion->waitForThreadToFinish();
#endif

    //TODO: fix this from hanging
    comm->stop();
    comm->waitForThreadToFinish();
    // @jfishman - tool will not exit, due to socket blocking
    //comm->getTOOLTrigger()->await_off();
}

void
Man::processFrame ()
{

#ifdef USE_VISION
    // Need to lock image and vision angles for duration of
    // vision processing to ensure consistency.
    sensors->lockImage();
#ifdef USE_MEMORY
    // TODO: this is temporarily here
//    loggingBoard->log(MIMAGE_ID);
#endif
    PROF_ENTER(P_VISION);
    vision->notifyImage(sensors->getImage());
    PROF_EXIT(P_VISION);
    sensors->releaseImage();
#endif
#if defined USE_MEMORY || defined OFFLINE
    memory->updateVision();
    loggingBoard->log(MVISION_ID);
#endif
#ifdef USE_NOGGIN
    noggin->runStep();
#endif

    memory->getMutableMObject(MLOCALIZATION_ID)->update();
    loggingBoard->log(MLOCALIZATION_ID);
    PROF_ENTER(P_LIGHTS);
    lights->sendLights();
    PROF_EXIT(P_LIGHTS);
}


void Man::notifyNextVisionImage() {

  transcriber->postVisionSensors();

  // Process current frame
  processFrame();

  // Make sure messages are printed
  fflush(stdout);
}
