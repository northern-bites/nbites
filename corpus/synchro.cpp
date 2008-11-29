
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <map>
#include <string>
#include <pthread.h>
#include <boost/shared_ptr.hpp>
#include "synchro.h"

using namespace std;
using namespace boost;

Event::Event (string _name)
    : name(_name), signalled(false)
{
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
}

Event::~Event ()
{
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}

void Event::await ()
{
    pthread_mutex_lock(&mutex);

    while (!signalled)
        pthread_cond_wait(&cond, &mutex);
    signalled = false;

    pthread_mutex_unlock(&mutex);
}

bool Event::poll ()
{
    pthread_mutex_lock(&mutex);

    const bool result = signalled;
    signalled = false;

    pthread_mutex_unlock(&mutex);
    return result;
}

void Event::signal ()
{
    pthread_mutex_lock(&mutex);

    signalled = true;
    pthread_cond_signal(&cond);

    pthread_mutex_unlock(&mutex);
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

const map<string, shared_ptr<Event> >& Synchro::available ()
{
    return events;
}

void Synchro::await (Event* ev)
{
    ev->await();
}

bool Synchro::poll (Event* ev)
{
    return ev->poll();
}

void Synchro::signal (Event* ev)
{
    ev->signal();
}


Thread::Thread (shared_ptr<Synchro> _synchro, string _name)
  : name(_name), synchro(_synchro), running(false),
    start_event(_synchro->create(_name + THREAD_START_EVENT_SUFFIX)),
    stop_event(_synchro->create(_name + THREAD_STOP_EVENT_SUFFIX))
{
}

Thread::~Thread ()
{
}

int
Thread::start ()
{
    // Set thread attributes
    pthread_attr_t attr;
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);

    // Create thread
    const int result = pthread_create(&thread, &attr, runThread, (void*)this);

    // Free attribute data
    pthread_attr_destroy(&attr);

    return result;
}

void
Thread::stop ()
{
    running = false;
}

void*
Thread::runThread (void* _this)
{
    reinterpret_cast<Thread*>(_this)->run();

    pthread_exit(NULL);
}

void
Thread::run ()
{
    // Signal when all startup operations are complete
    start_event->signal();

    // Signal when stop condition is met and shutdown operations are complete
    stop_event->signal();
}
