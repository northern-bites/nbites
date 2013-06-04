#include "DiagramThread.h"
#include "Common.h"
#include "Profiler.h"
#include "DebugConfig.h"
#include <iostream>

using std::cout;
using std::endl;
using namespace portals;

namespace man{

// Constructor for the private subclass
DiagramThread::RobotDiagram::RobotDiagram(std::string name_, long long frame)
    : RoboGram(),
      name(name_),
      frameLengthMicro(frame)
{
#ifdef DEBUG_THREADS
    std::cout << name << " thread has frame length " << frameLengthMicro <<
        " uS " << std::endl;
#endif
}

// Overrides the RoboGram::run() method to do timing as well
void DiagramThread::RobotDiagram::run()
{
    // Start timer
    const long long startTime = realtime_micro_time();

    if (name == "cognition")
    {
        PROF_ENTER(P_COGNITION_THREAD);
    }
    else if (name == "sensors")
    {
        PROF_ENTER(P_MOTION_THREAD);
    }
    else if (name == "comm")
    {
        PROF_ENTER(P_COMM_THREAD);
    }
    else if (name == "guardian")
    {
        PROF_ENTER(P_GUARDIAN_THREAD);
    }

    RoboGram::run();

    if (name == "cognition")
    {
        PROF_EXIT(P_COGNITION_THREAD);
        // Count cognition frames
        PROF_NFRAME();
    }
    else if (name == "sensors")
    {
        PROF_EXIT(P_MOTION_THREAD);
    }
    else if (name == "comm")
    {
        PROF_EXIT(P_COMM_THREAD);
    }
    else if (name == "guardian")
    {
        PROF_EXIT(P_GUARDIAN_THREAD);
    }

    // Stop timer
    const long long processTime = realtime_micro_time() - startTime;

    // If we're under the frame length, this is good.
    // We can sleep for the rest of the frame and let others process.
    if (processTime < frameLengthMicro)
    {
        // Compute the time we should sleep from the amount of time
        // we processed this frame and the amount of time allotted to a frame
        const long int microSleepTime =
            static_cast<long int>(frameLengthMicro - processTime);
        const long int nanoSleepTime =
            static_cast<long int>((microSleepTime %(1000 * 1000)) * 1000);

        const long int secSleepTime =
            static_cast<long int>(microSleepTime / (1000*1000));

        interval.tv_sec = static_cast<time_t>(secSleepTime);
        interval.tv_nsec = nanoSleepTime;

        // Sleep!
        nanosleep(&interval, &remainder);
    }

#ifdef DEBUG_THREADS
    else if (processTime > frameLengthMicro*2) {
        std::cout<< "Warning: time spent in " << name << " thread longer"
                 << " than frame length: "<< processTime << " uS" <<
            std::endl;
    }
#endif

}

DiagramThread::DiagramThread(std::string name_, long long frame) :
    diagram(name_, frame),
    running(false)
{}

DiagramThread::~DiagramThread()
{
    stop();
}

// Just add the desired module to the underlying diagram
void DiagramThread::addModule(Module& mod)
{
    diagram.addModule(mod);
}

int DiagramThread::start()
{
    // Don't let it recreate the same thread!
    if (running) return -1;

    cout << "Thread " << diagram.name << " starting." << endl;

    // Since we don't need to join threads, creating them explicitly
    // detached may save resources
    pthread_attr_t attr;
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);

    // Create thread
    const int result = pthread_create(&thread, &attr, runDiagram, (void*)this);
    // Free space from attr
    pthread_attr_destroy(&attr);
    return result;
}

// If called from outside the thread, this should break the loop in
// runDiagram and allow the thread to exit.
void DiagramThread::stop()
{
    running = false;
    cout << "Thread " << diagram.name << " stopping." << endl;
}

/*
 * @brief pthreads needs a method with this signature to run in the thread.
 *        Since it has to be static, we need to pass in a pointer to the
 *        current instance in order to actually run the diagram.
 */
void* DiagramThread::runDiagram(void* _this)
{
    DiagramThread* this_instance = reinterpret_cast<DiagramThread*>(_this);

    cout << "Thread " << this_instance->diagram.name << " running." << endl;

    this_instance->running = true;

    // Run the diagram over and over again!
    // NOTE: we can't do timing here because a sleep call here is made
    //       from our main naoqi thread.
    while(this_instance->running) this_instance->diagram.run();

    cout << "Thread " << this_instance->diagram.name << " exiting." << endl;
    pthread_exit(NULL);
}

}
