
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

#ifndef _Man_h_DEFINED
#define _Man_h_DEFINED

#include <pthread.h>
#include <signal.h>

#include <boost/shared_ptr.hpp>

#include "manconfig.h"

#include "alxplatform.h"
#include "altools.h"
#include "albroker.h"
#include "almodule.h"
#include "alloggerproxy.h"
#include "almemoryproxy.h"
#include "almemoryfastaccess.h"
#include "alptr.h"

#ifdef NAOQI1
#include "dcmproxy.h"
#endif

#ifdef USE_DCM
#include "NaoEnactor.h"
#else
#include "ALEnactor.h"
#endif

#include "Common.h"
#include "Profiler.h"
#include "Sensors.h"
#include "Comm.h"
#include "Vision.h"
#include "Noggin.h"
#include "Motion.h"
#include "NaoPose.h"
#include "synchro.h"

/**
 * Preferences class to alter Python settings for our robot system.
 *
 * This is kind of a hack.
 */
class PythonPreferences
{
public:
    PythonPreferences();
    virtual ~PythonPreferences() { }

    void modifySysPath();
};

/**
 * The Naoqi module to run our main Nao robot system.
 *
 * @author Jeremy R. Fishman
 * @author Bowdoin College Northern Bites
 */
class Man : public AL::ALModule, public Thread
{
public:

    // contructors
#ifdef NAOQI1
    Man(AL::ALPtr<AL::ALBroker> pBroker, std::string pName);
#else
    Man();
#endif
    // destructor
    virtual ~Man();

    //
    // ALModule methods
    //

    /**
     * Called by stm when the subcription has been modified.
     *
     * @param pDataName Name of the suscribed data
     * @param pValue Value of the suscribed data
     * @param pMessage Message written by user during subscription
     */
    void dataChanged(const std::string& pDataName, const ALValue& pValue,
        const std::string& pMessage) {};

    std::string version() { return "1.0.0-r"; /*TRUNK_REVISION;*/ };

    //
    // Our methods
    //

    // Man runs, and runs, and runs.  In the current thread.  Use start() and
    // stop() (provided by the Thread class) to run in separate thread.
    void run();

    // Profiling methods
    void startProfiling(int nframes) {
       profiler->reset();
       profiler->profileFrames(nframes);
    }
    void stopProfiling() {
       profiler->profiling = false;
    }

    //HelperBoundMethods:
    void manStart() { Thread::start(); } //should return 'int' or ALValue
    void manStop() { Thread::stop(); }
    void manAwaitOn() { getTrigger()->await_on(); }
    void manAwaitOff() { getTrigger()->await_off(); }

    void helloWorld(){std::cout<<"HelloWorld, C++ Style"<<std::endl;};
private:
    // run Vision and call Noggin's main loop function
    void processFrame(void);
    // wait for and retrieve the latest image
    void waitForImage(void);

    void initMan (void);
    void closeMan(void);
#ifdef NAOQI1
    void registerCamera();
    void initCameraSettings(int whichCam);
    void initSyncWithALMemory();
    void syncWithALMemory();
#else
    void initCamera();
#endif
    void releaseImage(void);

  //
  // Variables
  //
public:
    // Sub-module instances
    // ** ORDER MATTERS HERE **
    //   if the modules are not instantiated in this order, some dependedcies
    //   (i.e. the Python modules exported) will not be available by the time
    //   other modules are imported
    PythonPreferences python_prefs;
    boost::shared_ptr<Profiler> profiler;
    boost::shared_ptr<Sensors> sensors;
    boost::shared_ptr<NaoPose> pose;
#ifdef USE_MOTION
    boost::shared_ptr<MotionEnactor> enactor;
    boost::shared_ptr<Motion> motion;
#endif
    boost::shared_ptr<Vision> vision;
    boost::shared_ptr<Comm> comm;
    boost::shared_ptr<Noggin> noggin;

private:
    // Interfaces/Proxies to robot
#ifdef NAOQI1
    AL::ALPtr<AL::ALLoggerProxy> log;
    AL::ALPtr<AL::ALProxy> camera;
    AL::ALPtr<AL::ALProxy> lem;
    AL::ALPtr<AL::ALMemoryProxy> almemory;
    AL::ALPtr<ALMemoryFastAccess> alfastaccess;
    AL::DCMProxy *dcm;
#else
    AL::ALLoggerProxy *log;
    AL::ALProxy *camera;
    AL::ALProxy *lem;
#endif
    std::string lem_name;

    bool camera_active;

// nBites Camera Constants
public:
    // Camera identification
    static const int TOP_CAMERA = 0;
    static const int BOTTOM_CAMERA = 1;

    // Camera setup information
    static const int CAMERA_SLEEP_TIME = 200;
    static const int CAM_PARAM_RETRIES = 3;

    // Default Camera Settings
    // Basic Settings
    static const int DEFAULT_CAMERA_RESOLUTION = 14;
    static const int DEFAULT_CAMERA_FRAMERATE = 15;
    static const int DEFAULT_CAMERA_BUFFERSIZE = 16;
    // Collor Settings
    static const int DEFAULT_CAMERA_AUTO_GAIN = 0; // AUTO GAIN OFF
    static const int DEFAULT_CAMERA_GAIN = 4;
    static const int DEFAULT_CAMERA_AUTO_WHITEBALANCE = 0; // AUTO WB OFF
    static const int DEFAULT_CAMERA_BLUECHROMA = 158; //131;
    static const int DEFAULT_CAMERA_REDCHROMA = 68; //72;
    static const int DEFAULT_CAMERA_BRIGHTNESS = 128;
    static const int DEFAULT_CAMERA_CONTRAST = 64;
    static const int DEFAULT_CAMERA_SATURATION = 128;
    static const int DEFAULT_CAMERA_HUE = 0;
    // Lens correction
    static const int DEFAULT_CAMERA_LENSX = 0;
    static const int DEFAULT_CAMERA_LENSY = 0;
    // Exposure length
    static const int DEFAULT_CAMERA_AUTO_EXPOSITION = 0; // AUTO EXPOSURE ON
    static const int DEFAULT_CAMERA_EXPOSURE = 5;
    // Image orientation
    static const int DEFAULT_CAMERA_HFLIP = 0;
    static const int DEFAULT_CAMERA_VFLIP = 0;
};


#endif
