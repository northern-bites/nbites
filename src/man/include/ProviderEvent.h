/*
 * Event.h
 *
 *  Created on: Mar 5, 2011
 *      @author: oneamtu
 *
 *  @class Event : used in messaging to identify the purpose of the notification
 */

#pragma once

#include <string>

class ProviderEvent;

#include "Provider.h"

class ProviderEvent {

private:
    const Provider* owner;
    int type;

public:
    ProviderEvent(const Provider* _owner, int _type) : owner(_owner), type(_type) {}
    const Provider* getOwner() const {return owner;}
    int getType() const {return type;}


};
