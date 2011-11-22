/*
 *  Notifier.h
 *
 *  @author: Octavian Neamtu <oneamtu89@gmail.com>
 *
 *  @class Notifier: a class that accepts subscribers that want to listen in
 *  @class EventNotifier: a class that accepts subscribers that want to listen
 *  in to a particular event or general subscribers that listen to all events
 */

#pragma once

#include <list>
#include <map>

#include "Notifier.h"
#include "Subscriber.h"

class Notifier {

    // typename here "convinces" the compiler that Subscriber<T> is actually
    // a type and then we typedef that to a new type
    typedef typename std::list<Subscriber* >::const_iterator subscriber_iter;

public:

    Notifier(){ }

    virtual ~Notifier() {}

    virtual void inline addSubscriber(Subscriber* s) const {
        subscribers.push_back(s);
    }

    virtual void inline notifySubscribers() {
        for (subscriber_iter i = subscribers.begin();
                i != subscribers.end(); i++) {
            (*i)->update();
        }
    }

private:
    mutable std::list<Subscriber*> subscribers;
};

template <class T>
class EventNotifier {

    typedef T event_type;

    typedef std::pair <event_type, Subscriber* > SubscriberPair;
    typedef std::multimap <event_type, Subscriber* > SubscriberMap;

    typedef
            typename SubscriberMap::iterator map_iterator;

    // typename here "convinces" the compiler that Subscriber<T> is actually
    // a type and then we typedef that to a new type
    typedef typename std::list<EventSubscriber<event_type> * >::const_iterator
                                                               subscriber_iter;

public:

    EventNotifier(){ }

    virtual ~EventNotifier() {}

    virtual void inline addSubscriber(Subscriber* s,
                                      event_type event_to_subscribe_to) const {
        subscriberMap.insert(
                SubscriberPair(event_to_subscribe_to, s));
    }

    virtual void inline addSubscriber(EventSubscriber<event_type>* s) const {
        generalSubscribers.push_back(s);
    }

    virtual void inline notifyGeneralSubscribers(event_type event) {
        for (subscriber_iter i = generalSubscribers.begin();
                i != generalSubscribers.end(); i++) {
            (*i)->update(event);
        }
    }

    //this checks to see if a subscriber is NULL and removes it as well
    virtual void notifySubscribers(event_type event) {
        //first update all of the general subscribers
        this->notifyGeneralSubscribers(event);
        //and now all of the specific subscribers to this event
        std::pair<map_iterator, map_iterator> range =
                subscriberMap.equal_range(event);
        for (map_iterator iter = range.first; iter != range.second; iter++) {
            if (iter->second == NULL) {
                subscriberMap.erase(iter);
            } else {
                iter->second->update();
            }
        }
    }

private:
     mutable SubscriberMap subscriberMap;
     mutable std::list <EventSubscriber<event_type>*> generalSubscribers;

};
