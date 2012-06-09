#include <iostream>
#include <boost/shared_ptr.hpp>

#include "Man.h"
#include "manconfig.h"
#include "synchro/synchro.h"
#include "VisionDef.h"
#include "Common.h"
#include "Camera.h"
#include "PyRoboGuardian.h"
#include "PySensors.h"
#include "PyLights.h"
#include "PySpeech.h"
#include "memory/log/OutputProviderFactory.h"

//#include <valgrind/callgrind.h>

using namespace std;
using boost::shared_ptr;
using namespace man::corpus;
using namespace man::memory;
using namespace man::memory::log;

/////////////////////////////////////////
//                                     //
//  Module class function definitions  //
//                                     //
/////////////////////////////////////////

Man::Man (RobotMemory::ptr memory,
          boost::shared_ptr<Sensors> _sensors,
          boost::shared_ptr<Transcriber> _transcriber,
          boost::shared_ptr<ImageTranscriber> _imageTranscriber,
          boost::shared_ptr<MotionEnactor> _enactor,
          boost::shared_ptr<Lights> _lights,
          boost::shared_ptr<Speech> _speech)
    :     memory(memory),
          sensors(_sensors),
          transcriber(_transcriber),
          imageTranscriber(_imageTranscriber),
          enactor(_enactor),
          lights(_lights),
          speech(_speech)
{
#ifdef USE_TIME_PROFILING
    Profiler::getInstance()->profileFrames(1400);
#endif

    // give python a pointer to the sensors structure. Method defined in
    // Sensors.h
    set_sensors_pointer(sensors);

    imageTranscriber->setSubscriber(this);

    guardian = boost::shared_ptr<RoboGuardian>(new RoboGuardian(sensors));

    pose = boost::shared_ptr<NaoPose> (new NaoPose(sensors));

    // initialize core processing modules
#ifdef USE_MOTION
    motion = boost::shared_ptr<Motion> (new Motion(enactor, sensors, pose));
    guardian->setMotionInterface(motion->getInterface());
#endif
    // initialize python roboguardian module.
    // give python a pointer to the guardian. Method defined in PyRoboguardian.h
    set_guardian_pointer(guardian);
    set_lights_pointer(_lights);
    set_speech_pointer(_speech);

    try {
        vision = boost::shared_ptr<Vision> (new Vision(pose, memory->get<MVision>()));

    set_vision_pointer(vision);

    comm = boost::shared_ptr<Comm> (new Comm(sensors, vision));

    loggingBoard = boost::shared_ptr<LoggingBoard> (new LoggingBoard(memory));
    set_logging_board_pointer(loggingBoard);

#ifdef USE_NOGGIN
    noggin = boost::shared_ptr<Noggin> (new Noggin(vision, comm, guardian, sensors,
                                            loggingBoard,
                                            motion->getInterface(), memory));

    } catch(std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

#endif// USE_NOGGIN

    loggingBoard->setMemory(memory);


#if defined USE_MEMORY && !defined OFFLINE
    OutputProviderFactory::AllSocketOutput(memory.get(), loggingBoard.get());
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

    loggingBoard->reset();

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
    PROF_ENTER(P_VISION);
    vision->notifyImage(sensors->getImage(Camera::BOTTOM));
    PROF_EXIT(P_VISION);
    sensors->releaseImage();
//    cout<<vision->ball->getDistance() << endl;
#endif

#ifdef USE_NOGGIN
    noggin->runStep();
#endif

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
