/**
 *
 * @class RobotMemoryManager
 *
 * Combines a Memory object and a MemorySignalingInterface to
 * connect QObjects slots to Memory Object (or other objects, such as Ground Truth)
 * notifications
 *
 * @author Octavian Neamtu
 *
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <QObject>

#include "man/memory/RobotMemory.h"
#include "MemorySignalingInterface.h"

namespace qtool {
namespace data {

class RobotMemoryManager: public QObject {

    Q_OBJECT
    ADD_SHARED_PTR(RobotMemoryManager);

public:
    RobotMemoryManager(man::memory::RobotMemory::ptr memory) :
        memory(memory), memorySignalingInterface(memory) {
    }

    virtual ~RobotMemoryManager() {}

    man::memory::RobotMemory::const_ptr getMemory() const {
        return memory;
    }

    void connectSlot(const QObject* subscriber, const char* slot, std::string name) const {
        memorySignalingInterface.subscribeSlotToMObject(subscriber, slot, name);
    }


protected:
    man::memory::RobotMemory::ptr memory;
    MemorySignalingInterface memorySignalingInterface;

};

}
}
