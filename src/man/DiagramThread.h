/*
 * @class DiagramThread
 *
 * A class that holds a diagram and runs it in its own thread. Users can
 * add modules, which are added to the diagram it contains. Handles
 * all of the managing of pthreads; since portals are thread-safe, there
 * should be no further need of things like mutexes. Provides a way to log
 * any type of message from an appropriate OutPortal in this thread also.
 *
 * Update: while we don't need mutexes, we DO need to limit the frame rate
 * of each of our threads. Each thread needs to sleep for the rest of its
 * "time slice" after it has processed. This way we don't process the same
 * image twice, for example, or query naoqi over and over again for the
 * same joint angles. Also, all of our systems get a turn at the CPU.
 * all of this is handled in the private subclass of Diagram, RobotDiagram.
 *
 * @author Lizzie Mamantov
 * @date February 2013
 *
 */

#pragma once

#include <string>
#include <pthread.h>
#include <time.h>
#include "RoboGrams.h"
#include "DebugConfig.h"
#include "log/LogModule.h"

namespace man{

class DiagramThread
{
public:
    // Requires a name; helps in with tracking the different threads' running
    DiagramThread(std::string name_, long long frame);
    virtual ~DiagramThread();

    // How modules are passed to the diagram
    void addModule(portals::Module& mod);

    std::string getName() { return diagram.name; }

    // Thread control
    int start();
    void stop();

    /*
     * @brief Adds a LogModule that will log the output of
     *        the specified OutPortal.
     * @param out The OutPortal that will be providing data
     * @param name The name of the file that will be ceated by the logger
     */
    template<class T>
    void log(portals::OutPortal<T>* out, std::string name)
    {
// Only let us do this if we really intend to log
#ifdef USE_LOGGING
        logs.push_back(new log::LogModule<T>(out, name));
        diagram.addModule(*logs.back());
#endif
    }

private:
    /*
     * @class RobotDiagram
     *
     * This is private to DiagramThread because nothing else should use
     * it. It extends a typical diagram by giving a diagram a desired frame
     * length. If the diagram's processing takes less time than the frame
     * length, which is what we want, it sleeps for the rest of the frame.
     * Turning on DEBUG_THREADS should help us figure out if a thread is
     * always overrunning its frames.
     */
    class RobotDiagram : public portals::RoboGram
    {
        // Just so that we can get things without getters
        friend class DiagramThread;
    public:
        // Holds the name of the thread
        RobotDiagram(std::string name_, long long frame);
        // Overrides RoboGram::run() but also calls it
        void run();

    private:
        // The name given to this thread
        std::string name;
        // The length of a processing frame for this thread
        long long frameLengthMicro;
        // Used for making nanosleep calls
        struct timespec interval, remainder;
    };

    // We need a method with this signature to pass to pthread_create
    static void* runDiagram(void* _this);

    // Hold all of the LogModules
    std::vector<log::LogBase*> logs;

    // The diagram that will be run in this thread
    RobotDiagram diagram;

    pthread_t thread;

    // Keeps track of whether this thread's main method should be going
    bool running;
};

}
