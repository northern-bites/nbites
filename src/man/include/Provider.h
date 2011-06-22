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

class Provider;

#include "Subscriber.h"
#include "ProviderEvent.h"

class Provider {

private:
    std::list <Subscriber*> subscribers;

public:

    Provider(){};

    void addSubscriber(Subscriber* s) { subscribers.push_back(s); }
    void notify(const ProviderEvent e) const {
        for (std::list<Subscriber*>::const_iterator i = subscribers.begin();
                i != subscribers.end(); i++) {
            (*i)->update(e);
        }
    }

};
