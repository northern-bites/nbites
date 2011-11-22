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
        shared_ptr<Sensors> sensors_ptr) :
        mVision(new MVision(MVISION_ID,
                vision_ptr, shared_ptr<PVision>(new PVision))),
        mVisionSensors(new MVisionSensors(MVISION_SENSORS_ID,
                sensors_ptr, shared_ptr<PVisionSensors>(new PVisionSensors))),
        mMotionSensors(new MMotionSensors(MMOTION_SENSORS_ID,
                sensors_ptr, shared_ptr<PMotionSensors>(new PMotionSensors))),
        mImage(new MImage(MIMAGE_ID,
                sensors_ptr, shared_ptr<PImage>(new PImage))) {
    birth_time = process_micro_time();

    if(sensors_ptr.get()) {
        sensors_ptr->addSubscriber(mVisionSensors.get(), NEW_VISION_SENSORS);
        sensors_ptr->addSubscriber(mMotionSensors.get(), NEW_MOTION_SENSORS);
        sensors_ptr->addSubscriber(mImage.get(), NEW_IMAGE);
    }

    mobject_IDMap.insert(MObject_IDPair(MVISION_ID, mVision));
    mobject_IDMap.insert(MObject_IDPair(MVISION_SENSORS_ID, mVisionSensors));
    mobject_IDMap.insert(MObject_IDPair(MMOTION_SENSORS_ID, mMotionSensors));
    mobject_IDMap.insert(MObject_IDPair(MIMAGE_ID, mImage));
}

Memory::~Memory() {
    cout << "Memory destructor" << endl;
}

void Memory::update(boost::shared_ptr<MObject> obj) {
    obj->update();
}

void Memory::updateVision() {
    update(mVision);
//    loggingBoard->log(mVision);
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

void Memory::addSubscriber(Subscriber* subscriber,
                           MObject_ID objectToSubscribeTo) const {
    getMObject(objectToSubscribeTo)->addSubscriber(subscriber);
}

}
}
