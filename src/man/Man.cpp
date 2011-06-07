
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

using namespace std;
using boost::shared_ptr;


/////////////////////////////////////////
//                                     //
//  Module class function definitions  //
//                                     //
/////////////////////////////////////////

Man::Man (shared_ptr<Sensors> _sensors,
          shared_ptr<Transcriber> _transcriber,
          shared_ptr<ImageTranscriber> _imageTranscriber,
          shared_ptr<MotionEnactor> _enactor,
          shared_ptr<Synchro> synchro,
          shared_ptr<Lights> _lights,
          shared_ptr<Speech> _speech)

  : sensors(_sensors),
    transcriber(_transcriber),
    imageTranscriber(_imageTranscriber),
    enactor(_enactor),
    lights(_lights),
    speech(_speech)
{
  // initialize system helper modules
  profiler = shared_ptr<Profiler>(new Profiler(&micro_time));
#ifdef USE_TIME_PROFILING
  profiler->profiling = true;
  profiler->profileFrames(700);
#endif
  // give python a pointer to the sensors structure. Method defined in
  // Sensors.h
  set_sensors_pointer(sensors);

  imageTranscriber->setSubscriber(this);

  pose = shared_ptr<NaoPose>(new NaoPose(sensors));

  guardian = shared_ptr<RoboGuardian>(new RoboGuardian(synchro, sensors));

  // initialize core processing modules
#ifdef USE_MOTION
  motion = shared_ptr<Motion>(
                              new Motion(synchro, enactor, sensors,profiler));
  guardian->setMotionInterface(motion->getInterface());
#endif
  // initialize python roboguardian module.
  // give python a pointer to the guardian. Method defined in PyRoboguardian.h
  set_guardian_pointer(guardian);

  set_lights_pointer(_lights);
  set_speech_pointer(_speech);

  vision = shared_ptr<Vision>(new Vision(pose, profiler));
  comm = shared_ptr<Comm>(new Comm(synchro, sensors, vision));
#ifdef USE_NOGGIN
  noggin = shared_ptr<Noggin>(new Noggin(profiler,vision,comm,guardian,
                                         sensors, motion->getInterface()));
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

    vision->notifyImage(sensors->getImage());

    sensors->releaseImage();
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
  // Synchronize noggin's information about joint angles with the motion
  // thread's information

  sensors->updateVisionAngles();

  transcriber->postVisionSensors();

  // Process current frame
  processFrame();

  //Release the camera image
  //if(camera_active)
  imageTranscriber->releaseImage();

  // Make sure messages are printed
  fflush(stdout);
}
