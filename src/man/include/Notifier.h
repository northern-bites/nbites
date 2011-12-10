/*
 *  Notifier.h
 *
 *  @author: Octavian Neamtu <oneamtu89@gmail.com>
 *
 *  Part of the light-weight man Notifier/Subscriber framework
 *
 *  @class Notifier: a class that accepts subscribers that want to listen in
 *  once the Notifier calls notifySubscribers, the update method is called on
 *  each of the subscribers
 *
 *  @class EventNotifier: a class that accepts subscribers that want to listen
 *  in to a particular event or general subscribers that listen to all events
 *  once the Notifier calls notifySubscribers, it passes an event-type parameter
 *  that is used to determine which of the notifiers waiting for that particular
 *  event will be updated; all general subscribers are also updated
 *
 *  @class SpecializedNotifier: a class that inherits from EventNotifier
 *  but instead of passing an event to notifySubscribers, it uses a set
 *  event it uses to notify the subscribers
 *  (which gets passed in the constructor)
 */

#pragma once

#include <list>
#include <map>

#include "Notifier.h"
#include "Subscriber.h"

//TODO: make the notifySubscribers protected

class Notifier {

    // typename here "convinces" the compiler that Subscriber<T> is actually
    // a type and then we typedef that to a new type
    typedef std::list<Subscriber* >::const_iterator subscriber_iter;

public:

    Notifier(){ }

    virtual ~Notifier() {}

    virtual void inline addSubscriber(Subscriber* s) const {
        subscribers.push_back(s);
    }

    virtual void inline unsubscribe(Subscriber* s) const {
        subscribers.remove(s);
    }

    virtual void inline notifySubscribers() const {
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

    virtual void inline unsubscribe(Subscriber* s) const {
        for (map_iterator it = subscriberMap.begin();
                it != subscriberMap.end(); it++) {
            if (it->second == s) {
                subscriberMap.erase(it);
            }
        }
    }

    virtual void inline unsubscribe(EventSubscriber<event_type>* s) const {
        generalSubscribers.remove(s);
    }

    virtual void inline notifyGeneralSubscribers(event_type event) const {
        for (subscriber_iter i = generalSubscribers.begin();
                i != generalSubscribers.end(); i++) {
            (*i)->update(event);
        }
    }

    //this checks to see if a subscriber is NULL and removes it as well
    virtual void notifySubscribers(event_type event) const {
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

template <class T>
class SpecializedNotifier : public EventNotifier<T> {

public:
    SpecializedNotifier(T event_to_notify) : event_to_notify(event_to_notify){}

    virtual ~SpecializedNotifier() {}

    virtual void inline addSubscriber(Subscriber* s) const {
        EventNotifier<T>::addSubscriber(s, event_to_notify);
    }

    virtual void notifySubscribers() const {
        EventNotifier<T>::notifySubscribers(event_to_notify);
    }

private:
    T event_to_notify;

};
