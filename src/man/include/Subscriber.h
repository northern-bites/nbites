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

class Subscriber {

public:
    Subscriber(){};

    virtual void update(int eventID) = 0;
};
