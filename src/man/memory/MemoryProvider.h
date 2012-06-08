/*
 * MemoryProvider.h
 *
 * Helper class that updates a memory object from a robot object
 * based on a method passed to it
 *
 * @author Octavian Neamtu
 */
#pragma once

#include <boost/shared_ptr.hpp>
#include <iostream>

namespace man {
namespace memory {

template <class MemoryObject, class RobotObject>
class MemoryProvider {

public:
    typedef boost::shared_ptr<MemoryObject> MemoryObjectPtr;
    typedef void (*updateMethod)(const RobotObject*, MemoryObjectPtr);

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

        if (memoryObject.get()) {
            memoryObject->lock();
            memoryObject->get()->set_timestamp(memoryObject->time_stamp());
            (*update)(robotObject, memoryObject);
            memoryObject->release();
            memoryObject->notifySubscribers();
        }
    }

protected:
    updateMethod update;
    MemoryObjectPtr memoryObject;
    const RobotObject* robotObject;

};

}
}
