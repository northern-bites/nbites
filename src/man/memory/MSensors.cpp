/*
 * MSensors.cpp
 *
 *      Author: oneamtu
 */

#include "MemoryMacros.hpp"
#include "MSensors.hpp"
#include <vector>

namespace memory {

using boost::shared_ptr;
using namespace std;

MSensors::MSensors(shared_ptr<Profiler> p, shared_ptr<Sensors> s) :
        MMotionSensors(s), MVisionSensors(s), MImage(s), _profiler(p) {
}

MSensors::~MSensors() {
}

void MSensors::update(const ProviderEvent e) {

    if (e.getType() == NEW_MOTION_SENSORS) {
        PROF_ENTER(_profiler.get(), P_MEMORY_MOTION_SENSORS);
        this->MMotionSensors::update();
        this->MMotionSensors::log();
        PROF_EXIT(_profiler.get(), P_MEMORY_MOTION_SENSORS);
    }

    if (e.getType() == NEW_VISION_SENSORS) {
        PROF_ENTER(_profiler.get(), P_MEMORY_VISION_SENSORS);
        this->MVisionSensors::update();
        this->MVisionSensors::log();
        PROF_EXIT(_profiler.get(), P_MEMORY_VISION_SENSORS);
    }

    if (e.getType() == NEW_IMAGE) {
        PROF_ENTER(_profiler.get(), P_MEMORY_IMAGE);
        this->MImage::update();
        this->MImage::log();
        PROF_EXIT(_profiler.get(), P_MEMORY_IMAGE);
    }
}
}
