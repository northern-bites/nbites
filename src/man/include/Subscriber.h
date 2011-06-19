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

#include "ProviderEvent.h"

class Subscriber {

public:
    Subscriber(){};

    virtual void update(const ProviderEvent ProviderEvent) = 0;
};
