#include "DiagramThread.h"
#include "Common.h"
#include <iostream>

#define DEBUG_THREADS

using std::cout;
using std::endl;
using namespace portals;

namespace man{

DiagramThread::RobotDiagram::RobotDiagram(std::string name_, long long frame)
    : RoboGram(),
      name(name_),
      frameLengthMicro(frame)
{
}

void DiagramThread::RobotDiagram::run()
{
    // start timer
    const long long startTime = monotonic_micro_time();

    RoboGram::run();

    //stop timer
    const long long processTime = monotonic_micro_time() - startTime;

    //sleep until next frame
    lastProcessTimeAvg = lastProcessTimeAvg/2 + processTime/2;

    if (processTime < frameLengthMicro)
    {
        const long int microSleepTime =
            static_cast<long int>(frameLengthMicro - processTime);
        const long int nanoSleepTime =
            static_cast<long int>((microSleepTime %(1000 * 1000)) * 1000);

        const long int secSleepTime =
            static_cast<long int>(microSleepTime / (1000*1000));

        //std::cerr << "Sleeping for nano: " << nanoSleepTime
        //<< " and sec:" << secSleepTime << std::endl;

        interval.tv_sec = static_cast<time_t>(secSleepTime);
        interval.tv_nsec = nanoSleepTime;

        nanosleep(&interval, &remainder);
    }

#ifdef DEBUG_THREADS
    else if (processTime > frameLengthMicro*2) {
        std::cout<< "Warning: time spent in " << name << " thread longer"
                 << " than frame length: "<< processTime << "uS" <<
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
    while(this_instance->running) this_instance->diagram.run();

    cout << "Thread " << this_instance->diagram.name << " exiting." << endl;
    pthread_exit(NULL);
}

}
