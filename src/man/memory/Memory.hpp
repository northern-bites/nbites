/**
 * Memory.hpp
 *
 * @class Memory
 *
 * This class keeps instances of all the different memory objects and provides
 * an interface through which they get updated (each memory object pulls data
 * from its corresponding object
 *
 * Future work: we will be able to keep multiple instances of selected objects
 *
 *      Author: Octavian Neamtu
 */

#pragma once

#include <boost/shared_ptr.hpp>

namespace memory {
class Memory; //forward declaration
}

#include "MVision.hpp"
#include "Vision.h"
#include "MSensors.hpp"
#include "Sensors.h"

namespace memory {

using boost::shared_ptr;

class Memory {

public:
    Memory(shared_ptr<Vision> vision_ptr,
           shared_ptr<Sensors> sensors_ptr);
    ~Memory();
    /**
     * calls the update function on @obj
     * this will usually make the MObject pull data
     * from its corresponding man object and maybe log it
     */
    void update(MObject* obj);
    /**
     * calls update(MVision)
     */
    void updateVision();
    /**
     * calls update(MSensors)
     */
    void updateSensors();

public:
    MVision* mvision;
    MSensors* msensors;
};
}
