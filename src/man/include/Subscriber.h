/**
 *
 * Subscriber.h
 *
 *
 * @class Subscriber: implements a simple subscriber for a subscriber/notifier
 * type connection
 *
 * @class EventSubscriber: the subscriber will also receive an additional
 * event object
 *
 * @author oneamtu
 *
 */

#pragma once

template <class T>
class EventSubscriber {

    typedef T event_type;

public:
    EventSubscriber(){}

    virtual ~EventSubscriber(){}

    virtual void update(event_type event) = 0;
};

class Subscriber {

public:
    Subscriber(){}

    virtual ~Subscriber(){}

    virtual void update() = 0;

};
