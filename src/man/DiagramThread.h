/*
 * @class DiagramThread
 *
 * A class that holds a diagram and runs it in its own thread. Users can
 * add modules, which are added to the diagram it contains. Handles
 * all of the managing of pthreads; since portals are thread-safe, there
 * should be no further need of things like mutexes. Provides a way to log
 * any type of message from an appropriate OutPortal in this thread also.
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
        logs.push_back(new log::LogModule<T>(out, name));
        diagram.addModule(*logs.back());
    }

private:
    class RobotDiagram : public portals::RoboGram
    {
        friend class DiagramThread;
    public:
        RobotDiagram(std::string name_, long long frame);
        void run();

    private:
        std::string name;
        long long frameLengthMicro;
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
