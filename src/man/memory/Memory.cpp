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

using boost::shared_ptr;

Memory::Memory(shared_ptr<Vision> vision_ptr,
        shared_ptr<Sensors> sensors_ptr) :
        _sensors(sensors_ptr),
        mVision(new MVision(MVISION_ID,
                MObject::NameFromID(MVISION_ID),
                vision_ptr)),
        mVisionSensors(new MVisionSensors(MVISION_SENSORS_ID,
                MObject::NameFromID(MVISION_SENSORS_ID),
                sensors_ptr)),
        mMotionSensors(new MMotionSensors(MMOTION_SENSORS_ID,
                MObject::NameFromID(MMOTION_SENSORS_ID),
                sensors_ptr)),
        mImage(new MImage(MIMAGE_ID,
                MObject::NameFromID(MIMAGE_ID),
                sensors_ptr)) {
    birth_time = process_micro_time();
    if(_sensors.get()) {
        sensors_ptr->addSubscriber(this);
    }

    protoMessageMap.insert(ProtoMessagePair(MVISION_ID, mVision));
    protoMessageMap.insert(ProtoMessagePair(MVISION_SENSORS_ID, mVisionSensors));
    protoMessageMap.insert(ProtoMessagePair(MMOTION_SENSORS_ID, mMotionSensors));
}

Memory::~Memory() {
}

void Memory::update(boost::shared_ptr<MObject> obj) {
    obj->update();
}

void Memory::updateVision() {
    update(mVision);
//    loggingBoard->log(mVision);
}

void Memory::update(SensorsEvent event) {
#ifdef USE_MEMORY
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
        PROF_EXIT(P_MEMORY_IMAGE);
        notifySubscribers(MIMAGE_ID);
    }
#endif
}

boost::shared_ptr<const ProtoMessage> Memory::getProtoMessage(MObject_ID id) const {
    ProtoMessageMap::const_iterator it = protoMessageMap.find(id);

    if (it != protoMessageMap.end()) {
        return it->second;
    } else {
        return boost::shared_ptr<const ProtoMessage>();
    }
}

boost::shared_ptr<ProtoMessage> Memory::getMutableProtoMessage(MObject_ID id) {
    ProtoMessageMap::iterator it = protoMessageMap.find(id);

    if (it != protoMessageMap.end()) {
        return it->second;
    } else {
        return boost::shared_ptr<ProtoMessage>();
    }
}

}
}
