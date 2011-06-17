/*
 * Provider.h
 *
 *  Created on: Mar 5, 2011
 *      @author: oneamtu
 *
 *  @class Provider : simple provider that notifies all its subscribers
 */

#pragma once

#include <list>
#include "Subscriber.h"

template <class T>
class Provider {

    typedef T event_type;
    // typename here "convinces" the compiler that Subscriber<T> is actually
    // a type and then we typedef that to a new type
    typedef
            typename std::list<Subscriber<event_type> * >::const_iterator
            subscriber_iter;

private:
    std::list <Subscriber<event_type>*> subscribers;

public:

    Provider(){
    }

    void addSubscriber(Subscriber<event_type>* s) {
        subscribers.push_back(s);
    }

protected:

    virtual void notifySubscribers(event_type event) const {
        for (subscriber_iter i = subscribers.begin();
                i != subscribers.end(); i++) {
            (*i)->update(event);
        }
    }

};
