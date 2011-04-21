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

//TODO: make everything .h
#include "MVision.hpp"
#include "Vision.h"
#include "MSensors.hpp"
#include "Sensors.h"
#include "Profiler.h"
//TODO: remove this after making the logging board
#include "NaoPaths.h"

namespace memory {

using boost::shared_ptr;

class Memory {

public:
    Memory( shared_ptr<Profiler> profiler_ptr,
            shared_ptr<Vision> vision_ptr,
            shared_ptr<Sensors> sensors_ptr);
    ~Memory();
    /**
     * calls the update function on @obj
     * this will usually make the MObject pull data
     * from its corresponding man object and maybe log it
     */
    void update(MObject* obj);
    /**
     * calls update(mvision)
     */
    void updateVision();
    /**
     * calls update((MMotionSensors) msensors)
     */
    void updateMotionSensors();
    /**
     * calls update((MVisionSensors) msensors)
     */
    void updateVisionSensors();

public:
    MVision* mvision;
    MSensors* msensors;
    //TODO: make this return a const poitner
    MSensors* getMSensors() const {return msensors;}

private:
    shared_ptr<Profiler> _profiler;
};
}
