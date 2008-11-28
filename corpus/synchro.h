
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

#ifndef Synchro_h_DEFINED
#define Synchro_h_DEFINED

#include <map>
#include <string>
#include <pthread.h>
#include <boost/shared_ptr.hpp>


class Event
{
  friend class Synchro;

  private:
    Event(std::string name);
  public:
    ~Event();

  public:
    void await();
    bool poll();
    void signal();

  public:
    const std::string name;

  private:
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    bool signalled;
};


class Synchro
{
  public:
    Synchro();
    ~Synchro();

  public:
    // Obtain a map of event names to Events
    const std::map<std::string, boost::shared_ptr<Event> >& available();
    // Create a new event with the given name
    boost::shared_ptr<Event> create(std::string event_name);

    // Wait for an event to be signalled, and clear the signal
    void await(Event* ev);
    // Check if an event has been signalled, and clear the signal
    bool poll(Event* ev);
    // Signal an event has occurred
    void signal(Event* ev);

  private:
    std::map<std::string, boost::shared_ptr<Event> > events;
};

static const std::string THREAD_START_EVENT_SUFFIX("_start");
static const std::string THREAD_STOP_EVENT_SUFFIX("_stop");
class Thread
{

  public:
    Thread(boost::shared_ptr<Synchro> _synchro, std::string _name)
        : name(_name), running(false),
          start_event(_synchro->create(_name + THREAD_START_EVENT_SUFFIX)),
          stop_event(_synchro->create(_name + THREAD_STOP_EVENT_SUFFIX))
    { }
    ~Thread() { }

  public:
    int start();
    void stop();

    virtual void run();

    // do not lock anything
    const std::string getName() { return name; }
    const bool isRunning() { return running; }

    // these are only fired once!  be careful, or deadlock could ensue
    const boost::shared_ptr<Event> getStart() { return start_event; }
    const boost::shared_ptr<Event> getStop() { return stop_event; }

  private:
    static void* runThread(void* _thread);

  private:
    std::string name;
    pthread_t thread;
    bool running;

    boost::shared_ptr<Event> start_event;
    boost::shared_ptr<Event> stop_event;
};

#endif // Synchro_h_DEFINED
