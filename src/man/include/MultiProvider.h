/*
 * MultiProvider.h
 *
 *  @author: Octavian Neamtu <oneamtu89@gmail.com>
 *
 *  @class MultiProvider : an extension to Provider that has subscribers
 *  that subscribe to specific events
 */

#pragma once

#include <map>

#include "MultiProvider.h"
#include "Subscriber.h"

template <class T>
class MultiProvider : public Provider<T> {

    typedef T event_type;

    typedef std::pair <event_type, Subscriber<event_type>* > EventSubscriberPair;
    typedef std::multimap <event_type, Subscriber<event_type>* > EventSubscriberMap;

    typedef
            typename EventSubscriberMap::iterator map_iterator;

public:

    MultiProvider(){
    }

    virtual void inline addSubscriber(Subscriber<event_type>* s) {
        Provider<event_type>::addSubscriber(s);
    }

    virtual void inline addSubscriber(Subscriber<event_type>* s,
            event_type event_to_subscribe_to) {
        eventSubscriberMap.insert(
                EventSubscriberPair(event_to_subscribe_to, s));
    }

protected:
    //this checks to see if a subscriber is NULL and removes it as well
    virtual void notifySubscribers(event_type event) {
        //first update all of the general subscribers
        Provider<event_type>::notifySubscribers(event);
        //and now all of the specific subscribers that need this event
        std::pair<map_iterator, map_iterator> range =
                eventSubscriberMap.equal_range(event);
        for (map_iterator iter = range.first; iter != range.second; iter++) {
            if (iter->second == NULL) {
                eventSubscriberMap.erase(iter);
            } else {
                iter->second->update(iter->first);
            }
        }
    }

private:
     EventSubscriberMap eventSubscriberMap;

};
