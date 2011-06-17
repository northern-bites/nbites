/**
 *
 * Subscriber.h
 *
 *
 * @class Subscriber: implements a simple subscriber for a subscriber/provider
 * type connection
 *
 * @author oneamtu
 *
 */

#pragma once

template <class T>
class Subscriber {

    typedef T event_type;

public:
    Subscriber(){};

    virtual void update(event_type event) = 0;
};
