
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

#include "alvisionimage.h"
#include "alvisiondefinitions.h"

#include "Man.h"
#include "manconfig.h"
#include "corpus/synchro.h"
#include "VisionDef.h"
#include "Common.h"
#include "_ledsmodule.h"

using namespace std;
using namespace AL;
using boost::shared_ptr;


/////////////////////////////////////////
//                                     //
//  Module class function definitions  //
//                                     //
/////////////////////////////////////////

Man::Man (shared_ptr<Sensors> _sensors,
          shared_ptr<Transcriber> _transcriber,
          shared_ptr<ALImageTranscriber> _imageTranscriber,
          shared_ptr<ALEnactor> _enactor,
          shared_ptr<RoboGuardian> _guardian,
          shared_ptr<Synchro> synchro
          ,ALPtr<ALBroker> broker)
    : sensors(_sensors),
      transcriber(_transcriber),
      imageTranscriber(_imageTranscriber),
      enactor(_enactor),
      guardian(_guardian)
{
    cout << "Entering Man constructor body"<<endl;
    //synchro = shared_ptr<Synchro>(new Synchro());

    // initialize system helper modules
    profiler = shared_ptr<Profiler>(new Profiler(&micro_time));
    //messaging = shared_ptr<Messenger>(new Messenger());
//     sensors = shared_ptr<Sensors>(new Sensors());
    // give python a pointer to the sensors structure. Method defined in
    // Sensors.h
    set_sensors_pointer(sensors);

    setLedsProxy(AL::ALPtr<AL::ALLedsProxy>(new AL::ALLedsProxy(broker)));

    //transcriber = shared_ptr<Transcriber>(new ALTranscriber(broker, sensors));
    //imageTranscriber =
    //    shared_ptr<ALImageTranscriber>(new ALImageTranscriber(synchro, sensors,
    //                                                          broker));
    imageTranscriber->setSubscriber(this);

    pose = shared_ptr<NaoPose>(new NaoPose(sensors));

    // initialize core processing modules
#ifdef USE_MOTION
// #ifdef USE_DCM
//     enactor = shared_ptr<EnactorT>(new NaoEnactor(sensors,
//                                                        transcriber,broker));
// #else//USE_DCM
//     enactor = shared_ptr<EnactorT>(new ALEnactor(sensors,synchro,
//                                                       transcriber,broker));
// #endif//USE_DCM

    motion = shared_ptr<Motion>(
        new Motion(synchro, enactor, sensors));

//     guardian = shared_ptr<RoboGuardian>(
//         new RoboGuardian(synchro,sensors, broker));

    guardian->setMotionInterface( motion->getInterface());

    // give python a pointer to the guardian. Method defined in PyRoboguardian.h
    set_guardian_pointer(guardian);
    // initialize python roboguardian module.
#endif
    vision = shared_ptr<Vision>(new Vision(pose, profiler));
    comm = shared_ptr<Comm>(new Comm(synchro, sensors, vision));
#ifdef USE_NOGGIN
    noggin = shared_ptr<Noggin>(new Noggin(profiler, vision,comm, guardian,
                                           motion->getInterface()));
#endif// USE_NOGGIN
}

Man::~Man ()
{
    cout << "Man destructor" << endl;
}

void Man::startSubThreads() {

#ifdef DEBUG_MAN_THREADING
    cout << "Man::start" << endl;
#endif

#ifndef USE_DCM
    if(enactor->start()!=0)
        cout << "Failed to start enactor" <<endl;
    else
        enactor->getTrigger()->await_on();
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

    if(guardian->start() != 0)
        cout << "RoboGuardian failed to start" << endl;
    else
        guardian->getTrigger()->await_on();
#endif

    // Start Image transcriber thread (it handles its own threading
    if (imageTranscriber->start() != 0) {
        cerr << "Image transcriber failed to start" << endl;
    }
    else
        imageTranscriber->getTrigger()->await_on();


#ifdef DEBUG_MAN_THREADING
    cout << "  run :: Signalling start" << endl;
#endif
    
}

void Man::stopSubThreads() {
#ifdef DEBUG_MAN_THREADING
    cout << "  Man stoping:" << endl;
#endif

    imageTranscriber->stop();
    imageTranscriber->getTrigger()->await_off();
#ifdef DEBUG_MAN_THREADING
    cout << "  Image Transcriber thread is stopped" << endl;
#endif
#ifdef USE_MOTION
    // Finished with run loop, stop sub-threads and exit
    motion->stop();
    motion->getTrigger()->await_off();
#ifdef DEBUG_MAN_THREADING
    cout << "  Motion thread is stopped" << endl;
#endif

    guardian->stop();
    guardian->getTrigger()->await_off();
#ifdef DEBUG_MAN_THREADING
    cout << "  Guardian thread is stopped" << endl;
#endif

#endif
    comm->stop();
    comm->getTrigger()->await_off();
    // @jfishman - tool will not exit, due to socket blocking
    //comm->getTOOLTrigger()->await_off();
#ifdef DEBUG_MAN_THREADING
    cout << "  Comm thread is stopped" << endl;
#endif

#ifndef USE_DCM
    enactor->stop();
    enactor->getTrigger()->await_off();
#ifdef DEBUG_MAN_THREADING
    cout << "  Enactor thread is stopped" << endl;
#endif
#endif
}

void
Man::processFrame ()
{
#ifdef USE_VISION
    //  This is called from Python right now
    //if(camera_active)
    //vision->copyImage(sensors->getImage());
#endif
    PROF_EXIT(profiler.get(), P_GETIMAGE);

    PROF_ENTER(profiler.get(), P_FINAL);
#ifdef USE_VISION
    //if(camera_active)
    vision->notifyImage(sensors->getImage());
    //vision->notifyImage();
#endif

    // run Python behaviors
#ifdef USE_NOGGIN
    noggin->runStep();
#endif

    PROF_EXIT(profiler.get(), P_FINAL);
    PROF_NFRAME(profiler.get());

    PROF_ENTER(profiler.get(), P_GETIMAGE);
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
