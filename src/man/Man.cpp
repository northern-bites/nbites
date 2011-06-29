
// This file is part of Man, a robotic perception, locomotion, and
// team strategy application created by the Northern Bites RoboCup
// team of Bowdoin College in Brunswick, Maine, for the Aldebaran
// Nao robot.
//
// Man is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Man is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.

#include <iostream>
#include <sstream>
#include <time.h>
#include <sys/time.h>
#include <Python.h>
#include <boost/shared_ptr.hpp>
#include <boost/assign/std/vector.hpp>
using namespace boost::assign;

#include "Man.h"
#include "manconfig.h"
#include "corpus/synchro.h"
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

Man::Man (shared_ptr<Profiler> _profiler,
          shared_ptr<Sensors> _sensors,
          shared_ptr<Transcriber> _transcriber,
          shared_ptr<ImageTranscriber> _imageTranscriber,
          shared_ptr<MotionEnactor> _enactor,
          shared_ptr<Synchro> synchro,
          shared_ptr<Lights> _lights,
          shared_ptr<Speech> _speech)
    :     sensors(_sensors),
          transcriber(_transcriber),
          imageTranscriber(_imageTranscriber),
          enactor(_enactor),
          profiler(_profiler),
          lights(_lights),
          speech(_speech)
{

  // initialize system helper modules

#ifdef USE_TIME_PROFILING
  profiler->profiling = true;
  profiler->profileFrames(1400);
#endif
  // give python a pointer to the sensors structure. Method defined in
  // Sensors.h
  set_sensors_pointer(sensors);

  imageTranscriber->setSubscriber(this);

  pose = shared_ptr<NaoPose>(new NaoPose(sensors));

  guardian = shared_ptr<RoboGuardian>(new RoboGuardian(synchro, sensors));

  // initialize core processing modules
#ifdef USE_MOTION
  motion = shared_ptr<Motion>(new Motion(synchro, enactor, sensors,profiler,pose));
  guardian->setMotionInterface(motion->getInterface());
#endif
  // initialize python roboguardian module.
  // give python a pointer to the guardian. Method defined in PyRoboguardian.h
  set_guardian_pointer(guardian);

  set_lights_pointer(_lights);
  set_speech_pointer(_speech);

  vision = shared_ptr<Vision>(new Vision(pose, profiler));

  CHECK_SUCCESS(-1);

  set_vision_pointer(vision);

  comm = shared_ptr<Comm>(new Comm(synchro, sensors, vision));

  memory = shared_ptr<Memory>(new Memory(profiler, vision, sensors));

  loggingBoard = shared_ptr<LoggingBoard>(new LoggingBoard(memory));
  set_logging_board_pointer(loggingBoard);
  memory->addSubscriber(loggingBoard.get());

#ifdef USE_MEMORY
  loggingBoard->newIOProvider(IOProviderFactory::newAllObjectsProvider());
#endif

#ifdef USE_NOGGIN
  noggin = shared_ptr<Noggin>(new Noggin(profiler,vision,comm,guardian,
                                         sensors, loggingBoard,
                                         motion->getInterface()));
#endif// USE_NOGGIN

  PROF_ENTER(profiler.get(), P_GETIMAGE);
}

Man::~Man ()
{
  cout << "Man destructor" << endl;
}

void Man::startSubThreads() {

#ifdef DEBUG_MAN_THREADING
  cout << "Man::start" << endl;
#endif


  // Start Comm thread (it handles its own threading
  if (comm->start() != 0)
    cerr << "Comm failed to start" << endl;
  else
    comm->getTrigger()->await_on();

#ifdef USE_MOTION
  // Start Motion thread (it handles its own threading
  if (motion->start() != 0)
    cerr << "Motion failed to start" << endl;
  else
    motion->getTrigger()->await_on();
#endif


  if(guardian->start() != 0)
    cout << "RoboGuardian failed to start" << endl;
  else
    guardian->getTrigger()->await_on();


#ifdef DEBUG_MAN_THREADING
  cout << "  run :: Signalling start" << endl;
#endif

//  printf("Start time: %lli \n", process_micro_time());
//  CALLGRIND_START_INSTRUMENTATION;
//  CALLGRIND_TOGGLE_COLLECT;
}

void Man::stopSubThreads() {

  guardian->stop();
  guardian->getTrigger()->await_off();
#ifdef DEBUG_MAN_THREADING
  cout << "  Guardian thread is stopped" << endl;
#endif

#ifdef DEBUG_MAN_THREADING
  cout << "  Man stoping:" << endl;
#endif

#ifdef USE_MOTION
  // Finished with run loop, stop sub-threads and exit
  motion->stop();
  motion->getTrigger()->await_off();
#ifdef DEBUG_MAN_THREADING
  cout << "  Motion thread is stopped" << endl;
#endif

#endif
  comm->stop();
  comm->getTrigger()->await_off();
  // @jfishman - tool will not exit, due to socket blocking
  //comm->getTOOLTrigger()->await_off();
#ifdef DEBUG_MAN_THREADING
  cout << "  Comm thread is stopped" << endl;
#endif
}

void
Man::processFrame ()
{
    PROF_ENTER(profiler.get(), P_FINAL);
    PROF_EXIT(profiler.get(), P_GETIMAGE);

#ifdef USE_VISION
    // Need to lock image and vision angles for duration of
    // vision processing to ensure consistency.
    sensors->lockImage();
#ifdef USE_MEMORY
    // TODO: this is temporarily here
    //loggingBoard->log(MIMAGE_ID);
#endif

    vision->notifyImage(sensors->getImage());

    sensors->releaseImage();
#endif
#ifdef USE_MEMORY
    memory->updateVision();
    loggingBoard->log(MVISION_ID);
#endif
#ifdef USE_NOGGIN
    noggin->runStep();
#endif

    PROF_ENTER(profiler.get(), P_LIGHTS);
    lights->sendLights();
    PROF_EXIT(profiler.get(), P_LIGHTS);

    PROF_ENTER(profiler.get(), P_GETIMAGE);
    PROF_EXIT(profiler.get(), P_FINAL);
    PROF_NFRAME(profiler.get());
}


void Man::notifyNextVisionImage() {

  transcriber->postVisionSensors();

  // Process current frame
  processFrame();

  // Make sure messages are printed
  fflush(stdout);
}
