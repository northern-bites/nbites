
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

#include <map>
#include <string>
#include <pthread.h>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include "synchro.h"
#include "corpusconfig.h"
using namespace std;
using namespace boost;

#include "NullStream.h"

#ifdef DEBUG_THREAD
#define debug_thread_out cout
#else
#define debug_thread_out (*NullStream::NullInstance())
#endif

Event::Event (string _name)
  : name(_name), signalled(false)
{
    mutex = shared_ptr<pthread_mutex_t>(new pthread_mutex_t(), MutexDeleter());

    pthread_mutex_init(mutex.get(), NULL);
    pthread_cond_init(&cond, NULL);
}

Event::Event (string _name, shared_ptr<pthread_mutex_t> _mutex)
  : name(_name), mutex(_mutex), signalled(false)
{
    pthread_cond_init(&cond, NULL);
}

Event::~Event ()
{
    pthread_cond_destroy(&cond);
}

void Event::await ()
{
    pthread_mutex_lock(mutex.get());

    if (!signalled)
        pthread_cond_wait(&cond, mutex.get());
    signalled = false;

    pthread_mutex_unlock(mutex.get());
}

void Event::signal ()
{
    pthread_mutex_lock(mutex.get());

    signalled = true;
    pthread_cond_signal(&cond);

    pthread_mutex_unlock(mutex.get());
}

Synchro::Synchro ()
    : events()
{
}

Synchro::~Synchro ()
{
}

shared_ptr<Event> Synchro::create (string name)
{
    map<string, shared_ptr<Event> >::iterator itr = events.find(name);
    if (itr == events.end()) {
        const shared_ptr<Event> e(new Event(name));
        events[name] = e;
    }
    return events[name];
}

shared_ptr<Event>
Synchro::create (string name, shared_ptr<pthread_mutex_t> mutex)
{
    map<string, shared_ptr<Event> >::iterator itr = events.find(name);
    if (itr == events.end()) {
        const shared_ptr<Event> e(new Event(name, mutex));
        events[name] = e;
    }
    return events[name];
}

const map<string, shared_ptr<Event> >& Synchro::available ()
{
    return events;
}

void Synchro::await (Event* ev)
{
    ev->await();
}

void Synchro::signal (Event* ev)
{
    ev->signal();
}

static const std::string SIGNAL_SUFFIX = "_signal";


Thread::Thread (string _name)
  : name(_name), running(false),
    trigger(new Trigger(_name, false)),
    signal(name + SIGNAL_SUFFIX)
{
    debug_thread_out << name << "::created" << endl;
}

Thread::~Thread ()
{
}

int Thread::start ()
{
    if (running)
        return -1;

    debug_thread_out << name << "::starting" << endl;

    // Set thread attributes
    pthread_attr_t attr;
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);

    // Create thread
    const int result = pthread_create(&thread, &attr, runThread, (void*)this);

    // Wait for thread to start running
    trigger->await_on();

    // Free attribute data
    pthread_attr_destroy(&attr);
    return result;
}

void Thread::stop ()
{
    running = false;
    //thread might be waiting for a signal, so if we're just waiting
    //for it to stop, then we might wait forever
    this->signalToResume();
    debug_thread_out << this->name << "::stopping" << endl;
}

void* Thread::runThread (void* _this)
{
    Thread* this_instance = reinterpret_cast<Thread*>(_this);

    debug_thread_out << this_instance->name << "::run" << endl;

    this_instance->running = true;
    this_instance->trigger->on();

    this_instance->run();

    this_instance->running = false;
    this_instance->trigger->off();

    debug_thread_out << this_instance->name << "::exiting" << endl;
    pthread_exit(NULL);
    return _this;
}

void Thread::signalToResume() {
    this->signal.signal();
}

void Thread::waitForSignal() {
    this->signal.await();
}

void Thread::waitForThreadToFinish() {
    this->trigger->await_off();
}

void Thread::yield() {
    pthread_yield();
}

Trigger::Trigger (string name, bool _v)
  : mutex(shared_ptr<pthread_mutex_t>(new pthread_mutex_t(), MutexDeleter())),
    on_event(name + TRIGGER_ON_SUFFIX, mutex),
    off_event(name + TRIGGER_OFF_SUFFIX, mutex),
    flip_event(name + TRIGGER_FLIP_SUFFIX, mutex),
    value(_v)
{
    // Set mutex to recursive (reentrant)
    pthread_mutexattr_t attrs;
    pthread_mutexattr_init(&attrs);
    pthread_mutexattr_settype(&attrs, PTHREAD_MUTEX_RECURSIVE);
    // Initialize mutex
    pthread_mutex_init(mutex.get(), &attrs);
}

void Trigger::flip ()
{
    lock();

    value = !value;
    if (value)
        on_event.signal();
    else
        off_event.signal();
    flip_event.signal();

    release();
}

void Trigger::on ()
{
    lock();

    if (!value) {
        // flip and signal
        value = true;
        on_event.signal();
        flip_event.signal();
    }

    release();
}

void Trigger::off ()
{
    lock();

    if (value) {
        // flip and signal
        value = false;
        off_event.signal();
        flip_event.signal();
    }

    release();
}

bool Trigger::poll ()
{
    lock();
    const bool _v = value;
    release();

    return _v;
}

void Trigger::await_on ()
{
    if (!poll())
        on_event.await();
}

void Trigger::await_off ()
{
    if (poll())
        off_event.await();
}

void Trigger::await_flip ()
{
    flip_event.await();
}
