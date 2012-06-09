/*
 * MemoryProvider.h
 *
 * Helper class that updates a memory object from a robot object
 * based on a method passed to it
 *
 * @author Octavian Neamtu
 */

//TODO: comment this and make sure people understand how it works

#pragma once

#include <boost/shared_ptr.hpp>
#include <iostream>

#include "Profiler.h"

namespace man {
namespace memory {

template <class MemoryObject, class RobotObject>
class MemoryProvider {

public:
    typedef boost::shared_ptr<MemoryObject> MemoryObjectPtr;
    typedef void (RobotObject::*updateMethod)(MemoryObjectPtr) const;

public:
    MemoryProvider(updateMethod update,
                   const RobotObject* robotObject,
                   MemoryObjectPtr memoryObject)
        : update(update), robotObject(robotObject), memoryObject(memoryObject) {}
    virtual ~MemoryProvider() {}

    /**
     * Insert this wherever you need to update the memory object
     */
    void updateMemory() {

    #if defined USE_MEMORY
        if (memoryObject.get()) {

            memoryObject->lock();
            memoryObject->get()->set_timestamp(memoryObject->time_stamp());
            (robotObject->*update)(memoryObject);
            memoryObject->release();
            memoryObject->notifySubscribers();
        }
    #endif
    }

protected:
    updateMethod update;
    const RobotObject* robotObject;
    MemoryObjectPtr memoryObject;

};

}
}
