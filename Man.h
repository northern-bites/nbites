
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

#include "config.h"

#include "alxplatform.h"
#include "altools.h"
#include "albroker.h"
#include "almodule.h"
#include "alloggerproxy.h"

#include "Common.h"
#include "Profiler.h"
#include "Sensors.h"
#include "Comm.h"
#include "Vision.h"
#include "Noggin.h"
#include "Motion.h"
#include "NaoPose.h"
#include "corpus/synchro.h"

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
class Man
  : public AL::ALModule,
    public Thread
{
  public:

    // contructors
    Man();
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

    std::string version() { return TRUNK_REVISION; };

    //
    // Our methods
    //

    // Man runs, and runs, and runs.  In the current thread.  Use start() and
    // stop() (provided by the Thread class) to run in separate thread.
    void run();

    // Profiling methods
    void startProfiling(int nframes) {
       profiler.reset();
       profiler.profileFrames(nframes);
    }
    void stopProfiling() {
       profiler.profiling = false;
    }

    // Store the current frame to file
    void saveFrame();
    // Run the vision hacks for U/V switching detections over the next frames
    void visionHack();
    // Hack the current image
    void hackFrame();
  private:
    // static method to pass into a thread
    static void* runThread(void*);
    // run Vision and call Noggin's main loop function
    void processFrame(void);
    // wait for and retrieve the latest image
    void waitForImage(void);

    void initModule (void);
    void closeModule(void);

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
    Profiler profiler;
    Sensors sensors;
    Motion motion;
    Vision vision;
    Comm comm;
    Noggin noggin;   
   

  private:
    // Interfaces/Proxies to robot
    AL::ALLoggerProxy *log;
    AL::ALProxy *camera;
    AL::ALProxy *lem;
    std::string lem_name;

    int frame_counter;
    int saved_frames;
    int hack_frames;
    int balls_seen;
};


#endif
