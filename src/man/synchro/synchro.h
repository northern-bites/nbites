#pragma once

#include <map>
#include <string>
#include <pthread.h>
#include <boost/shared_ptr.hpp>
#include <vector>

#undef MUTEX_TYPE
#ifdef NDEBUG
#  define STATIC_MUTEX_TYPE PTHREAD_MUTEX_INITIALIZER
#  define MUTEX_INIT fastmutex
#else
#  define STATIC_MUTEX_TYPE PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP
#  define MUTEX_INIT errchkmutex
#endif

namespace man {
namespace synchro {

struct MutexDeleter
{
    void operator() (pthread_mutex_t *mutex) { pthread_mutex_destroy(mutex); }
};

class Lock
{
  public:
    Lock() { pthread_mutex_init(&mutex, NULL); }
    ~Lock() { pthread_mutex_destroy(&mutex); }

  public:
    inline int dolock()    { return pthread_mutex_lock(&mutex); }
    inline int trylock() { return pthread_mutex_trylock(&mutex); }
    inline int release() { return pthread_mutex_unlock(&mutex); }

  private:
    pthread_mutex_t mutex;
};

class Event
{

public:
    Event(std::string name);
    Event(std::string name, boost::shared_ptr<pthread_mutex_t> mutex);
    virtual ~Event();

    // Makes thread await for signal() to be called on this event
    // Or if signalled is true, then the thread will continue without
    // waiting for a signal
    // Note : that holds true only for the first thread calling await,
    // since await sets signalled to false afterwards
    void await();
    // Signals threads waiting on this event to resume
    // It also sets signalled to true, so that if a thread starts waiting
    // on the event after it's been signalled, it doesn't get stuck on it
    // until the next signal (which may never come)
    // Note : that holds true only for the first thread calling await,
    // since await sets signalled to false afterwards
    void signal();

public:
    const std::string name;

private:
    boost::shared_ptr<pthread_mutex_t> mutex;
    pthread_cond_t cond;
    bool signalled;
};


class Synchro
{
  public:
    Synchro();
    virtual ~Synchro();

  public:
    // Obtain a map of event names to Events
    const std::map<std::string, boost::shared_ptr<Event> >& available();
    // Create a new event with the given name
    boost::shared_ptr<Event> create(std::string event_name);
    boost::shared_ptr<Event> create(std::string event_name,
                                    boost::shared_ptr<pthread_mutex_t> mutex);

    // Wait for an event to be signalled, and clear the signal
    void await(Event* ev);
    // Check if an event has been signalled, and clear the signal
    bool poll(Event* ev);
    // Signal an event has occurred
    void signal(Event* ev);

  private:
    std::map<std::string, boost::shared_ptr<Event> > events;
};

class TriggeredEvent
{
  public:
    TriggeredEvent() { }
    virtual ~TriggeredEvent() { }

    virtual bool poll() = 0;
    virtual void await_on() = 0;
    virtual void await_off() = 0;
    virtual void await_flip() = 0;
};

static const std::string TRIGGER_ON_SUFFIX("_on");
static const std::string TRIGGER_OFF_SUFFIX("_off");
static const std::string TRIGGER_FLIP_SUFFIX("_flip");

class Trigger
  : public TriggeredEvent
{
  public:
    Trigger(std::string name, bool _value=false);
    virtual ~Trigger() { }

    void flip();
    void on();
    void off();

    bool poll();
    void await_on();
    void await_off();
    void await_flip();

  private:
    inline int lock() { return pthread_mutex_lock(mutex.get()); }
    inline int release() { return pthread_mutex_unlock(mutex.get()); }

  private:
    boost::shared_ptr<pthread_mutex_t> mutex;
    Event on_event;
    Event off_event;
    Event flip_event;
    bool value;
};

class Thread {

public:
    Thread(std::string _name);
    virtual ~Thread();

public:
    // To start and stop the thread.  Thread may be run repeatedly, but make
    // sure the thread has stopped before starting it once more.
    virtual int start();
    virtual void stop();

    void signalToResume();
    void waitForThreadToFinish();

    // Overload this method to run your thread code
    virtual void run() = 0;

protected:
    void waitForSignal();

private:
    static void* runThread(void* _thread);

public:
    const std::string name;

private:
    pthread_t thread;

protected:
    bool running;
    boost::shared_ptr<Trigger> trigger;
    Event signal;
};

}
}
