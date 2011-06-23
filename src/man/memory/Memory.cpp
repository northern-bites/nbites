/*
 * Memory.cpp
 *
 *      Author: oneamtu
 */

#include "Common.h"

#include "Memory.h"

namespace man {
namespace memory {

long long int birth_time; //the time we initialized memory
//everything else is time stamped relative to this

using log::LoggingBoard;

Memory::Memory(shared_ptr<Profiler> profiler_ptr,
        shared_ptr<Vision> vision_ptr,
        shared_ptr<Sensors> sensors_ptr) :
        _profiler(profiler_ptr),
        _sensors(sensors_ptr),
        mVision(new MVision(vision_ptr)),
        mVisionSensors(new MVisionSensors(sensors_ptr)),
        mMotionSensors(new MMotionSensors(sensors_ptr)),
        mImage(new MImage(sensors_ptr)),
        loggingBoard(new LoggingBoard(this)){
    birth_time = process_micro_time();
    sensors_ptr->addSubscriber(this);
}

Memory::~Memory() {
    delete mVision;
    delete mVisionSensors;
    delete mMotionSensors;
    delete mImage;

    delete loggingBoard;
}

void Memory::update(MObject* obj) {
    obj->update();
}

void Memory::updateVision() {
    update(mVision);
    loggingBoard->log(mVision);
}

void Memory::update(const ProviderEvent e) {

    if (e.getType() == NEW_MOTION_SENSORS) {
        PROF_ENTER(_profiler.get(), P_MEMORY_MOTION_SENSORS);
        mMotionSensors->update();
        loggingBoard->log(mMotionSensors);
        PROF_EXIT(_profiler.get(), P_MEMORY_MOTION_SENSORS);
    }

    if (e.getType() == NEW_VISION_SENSORS) {
        PROF_ENTER(_profiler.get(), P_MEMORY_VISION_SENSORS);
        mVisionSensors->update();
        loggingBoard->log(mVisionSensors);
        PROF_EXIT(_profiler.get(), P_MEMORY_VISION_SENSORS);
    }

    if (e.getType() == NEW_IMAGE) {
        PROF_ENTER(_profiler.get(), P_MEMORY_IMAGE);
        mImage->update();
        loggingBoard->log(mImage);
        //TODO: move this somewhere else
        _sensors->setNaoImage(loggingBoard->getImageLogger(mImage)->
                getCurrentImage());
        PROF_EXIT(_profiler.get(), P_MEMORY_IMAGE);
    }
}

}
}
