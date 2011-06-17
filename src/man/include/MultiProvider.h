/*
 * MultiProvider.h
 *
 *  @author: Octavian Neamtu <oneamtu89@gmail.com>
 *
 *  @class MultiMultiProvider : an extension to MultiProvider that has subscribers
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
            typename EventSubscriberMap::const_iterator map_iterator;

public:

    MultiProvider(){
    }

    void addSubscriber(Subscriber<event_type>* s,
            event_type event_to_subscribe_to) {
        eventSubscriberMap.insert(
                EventSubscriberPair(event_to_subscribe_to, s));
    }

protected:

    virtual void notifySubscribers(event_type event) const {
        //first update all of the general subscribers
        Provider<event_type>::notifySubscribers(event);
        //and now all of the specific subscribers that need this event
        std::pair<map_iterator, map_iterator> range =
                eventSubscriberMap.equal_range(event);
        for (map_iterator iter = range.first; iter != range.second; iter++) {
            (*iter).second->update((*iter).first);
        }
    }

private:
     EventSubscriberMap eventSubscriberMap;

};
