
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
    const std::map<std::string, Event*>& available();
    // Create a new event with the given name
    const Event* create(std::string event_name);

    // Wait for an event to be signalled, and clear the signal
    void await(Event* ev);
    // Check if an event has been signalled, and clear the signal
    bool poll(Event* ev);
    // Signal an event has occurred
    void signal(Event* ev);

  private:
    std::map<std::string, Event*> events;
};

#endif // Synchro_h_DEFINED
