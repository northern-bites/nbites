/**
 *
 * @class RobotMemoryManager
 *
 * Combines a Memory object and a MemorySignalingInterface to
 * connect QObjects slots to Memory Object notifications
 *
 * @author Octavian Neamtu
 *
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <QObject>

#include "man/memory/Memory.h"
#include "MemorySignalingInterface.h"

namespace qtool {
namespace data {

class RobotMemoryManager: public QObject {

    Q_OBJECT
    ADD_SHARED_PTR(RobotMemoryManager);

public:
    RobotMemoryManager(man::memory::Memory::ptr memory) :
        memory(memory), memorySignalingInterface(memory) {
    }

    virtual ~RobotMemoryManager() {}

    man::memory::Memory::const_ptr getMemory() const {
        return memory;
    }

    void connectSlotToMObject(const QObject* subscriber,
                const char* slot, std::string name) const {
        memorySignalingInterface.subscribeSlotToMObject(subscriber, slot, name);
    }


protected:
    man::memory::Memory::ptr memory;
    MemorySignalingInterface memorySignalingInterface;

};

}
}
