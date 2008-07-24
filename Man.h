
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

/**
 * The Naoqi module to run our main Nao robot system.
 *
 * @author Jeremy R. Fishman
 * @author Bowdoin College Northern Bites
 */
class Man
  : public AL::ALModule
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

    // Man goes, and goes, and goes.  In a background thread.
    void go();
    // Man runs, and runs, and runs.  In the current thread.
    void run();
    // Signal that a new image is available for processing
    void notifyVision();
    // Join and wait on the vision thread.  Only one thread is allowed to do
    // this
    void joinVision();
    // Return a boolean indicating whether the threads for the Man
    // processes are currently running
    bool isRunning();
    // Wait on the condition variable until signaled that Vision has finished
    // processing the current frame.  Again, only one thread is allowed to do
    // this at once, as only one thread can lock the mutex at a given time
    void joinVisionLoop();

    // stops the vision thread
    void stop();

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

    // misc
    bool running;
    pthread_t       vision_thread;
    pthread_mutex_t vision_mutex;
    pthread_cond_t  vision_cond;

    int frame_counter;
    int saved_frames;
    int hack_frames;
    int balls_seen;
};


#endif
