/*
 * Memory.cpp
 *
 * @author Octavian Neamtu
 */

#include <iostream>

#include "Common.h"
#include "Memory.h"

namespace man {
namespace memory {

long long int birth_time; //the time we initialized memory
//everything else is time stamped relative to this

using boost::shared_ptr;
using namespace proto;
using namespace std;

Memory::Memory(shared_ptr<Vision> vision_ptr,
               shared_ptr<Sensors> sensors_ptr,
               shared_ptr<LocSystem> loc_ptr) :
        mVision(new MVision(MVISION_ID,
                vision_ptr, shared_ptr<PVision>(new PVision))),
        mVisionSensors(new MVisionSensors(MVISION_SENSORS_ID,
                sensors_ptr, shared_ptr<PVisionSensors>(new PVisionSensors))),
        mMotionSensors(new MMotionSensors(MMOTION_SENSORS_ID,
                sensors_ptr, shared_ptr<PMotionSensors>(new PMotionSensors))),
        mImage(new MImage(MIMAGE_ID,
                          sensors_ptr, shared_ptr<PImage>(new PImage))),
        mLocalization(new MLocalization(MLOCALIZATION_ID,
                                        loc_ptr, shared_ptr<PLoc>(new PLoc)))
{
    birth_time = process_micro_time();

    if(sensors_ptr.get()) {
        sensors_ptr->addSubscriber(this);
    }

    mobject_IDMap.insert(MObject_IDPair(MVISION_ID, mVision));
    mobject_IDMap.insert(MObject_IDPair(MVISION_SENSORS_ID, mVisionSensors));
    mobject_IDMap.insert(MObject_IDPair(MMOTION_SENSORS_ID, mMotionSensors));
    mobject_IDMap.insert(MObject_IDPair(MIMAGE_ID, mImage));
    mobject_IDMap.insert(MObject_IDPair(MLOCALIZATION_ID, mLocalization));
}

Memory::~Memory() {
    cout << "Memory destructor" << endl;
}

void Memory::update(boost::shared_ptr<MObject> obj) {
    obj->update();
}

void Memory::updateVision() {
    update(mVision);
    notifySubscribers(MVISION_ID);
//    loggingBoard->log(mVision);
}

void Memory::update(SensorsEvent event) {
#if defined USE_MEMORY || defined OFFLINE
    if (event == NEW_MOTION_SENSORS) {
        PROF_ENTER(P_MEMORY_MOTION_SENSORS);
        mMotionSensors->update();
        notifySubscribers(MMOTION_SENSORS_ID);
        PROF_EXIT(P_MEMORY_MOTION_SENSORS);
    }

    if (event == NEW_VISION_SENSORS) {
        PROF_ENTER(P_MEMORY_VISION_SENSORS);
        mVisionSensors->update();
        notifySubscribers(MVISION_SENSORS_ID);
        PROF_EXIT(P_MEMORY_VISION_SENSORS);
    }

    if (event == NEW_IMAGE) {
        PROF_ENTER(P_MEMORY_IMAGE);
        mImage->update();
        notifySubscribers(MIMAGE_ID);
        PROF_EXIT(P_MEMORY_IMAGE);
    }
#endif
}

MObject::const_ptr Memory::getMObject(MObject_ID id) const {
    MObject_IDMap::const_iterator it = mobject_IDMap.find(id);

    if (it != mobject_IDMap.end()) {
        return it->second;
    } else {
        return MObject::const_ptr();
    }
}

MObject::ptr Memory::getMutableMObject(MObject_ID id) {
    MObject_IDMap::iterator it = mobject_IDMap.find(id);

    if (it != mobject_IDMap.end()) {
        return it->second;
    } else {
        return MObject::ptr();
    }
}

}
}
