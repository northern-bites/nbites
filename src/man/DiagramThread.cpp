#include "DiagramThread.h"
#include <iostream>

using std::cout;
using std::endl;
using namespace portals;

namespace man{

DiagramThread::DiagramThread(std::string name_) : name(name_),
                                                  running(false)
{}

DiagramThread::~DiagramThread()
{
    stop();
}

void DiagramThread::addModule(Module& mod)
{
    diagram.addModule(mod);
}

int DiagramThread::start()
{
    // Don't let it recreate the same thread!
    if (running) return -1;

    cout << "Thread " << name << " starting." << endl;

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

void DiagramThread::stop()
{
    running = false;
    cout << "Thread " << name << " stopping." << endl;
}

void* DiagramThread::runDiagram(void* _this)
{
    DiagramThread* this_instance = reinterpret_cast<DiagramThread*>(_this);

    cout << "Thread " << this_instance->name << " running." << endl;

    this_instance->running = true;

    while(this_instance->running) this_instance->diagram.run();

    cout << "Thread " << this_instance->name << " exiting." << endl;
    pthread_exit(NULL);
}

}
