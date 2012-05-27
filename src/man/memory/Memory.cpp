
#include <iostream>

#include "Common.h"
#include "Memory.h"

namespace man {
namespace memory {

using boost::shared_ptr;
using namespace proto;
using namespace std;

Memory::Memory(shared_ptr<Vision> vision_ptr,
               shared_ptr<Sensors> sensors_ptr,
               shared_ptr<LocSystem> loc_ptr) :
        mVision(new MVision(vision_ptr)),
        mVisionSensors(new MVisionSensors(sensors_ptr)),
        mMotionSensors(new MMotionSensors(sensors_ptr)),
        bottomMImage(new MImage(sensors_ptr, corpus::Camera::BOTTOM)),
        topMImage(new MImage(sensors_ptr, corpus::Camera::TOP)),
        mLocalization(new MLocalization(loc_ptr))
{

#if defined USE_MEMORY || defined OFFLINE
    if(sensors_ptr.get()) {
        sensors_ptr->addSubscriber(mVisionSensors.get(), NEW_VISION_SENSORS);
        sensors_ptr->addSubscriber(mMotionSensors.get(), NEW_MOTION_SENSORS);
        //sensors_ptr->addSubscriber(bottomMImage.get(), NEW_IMAGE);
        sensors_ptr->addSubscriber(topMImage.get(), NEW_IMAGE);
    }
#endif

    mobject_IDMap.insert(MObject_IDPair(mVision->getID(), mVision));
    mobject_IDMap.insert(MObject_IDPair(mVisionSensors->getID(), mVisionSensors));
    mobject_IDMap.insert(MObject_IDPair(mMotionSensors->getID(), mMotionSensors));
    //mobject_IDMap.insert(MObject_IDPair(bottomMImage->getID(), bottomMImage));
    mobject_IDMap.insert(MObject_IDPair(topMImage->getID(), topMImage));
    mobject_IDMap.insert(MObject_IDPair(mLocalization->getID(), mLocalization));
}

Memory::~Memory() {
    cout << "Memory destructor" << endl;
}

void Memory::update(boost::shared_ptr<MObject> obj) {
    obj->update();
}

void Memory::updateVision() {
    update(mVision);
}

MImage::const_ptr Memory::getMImage(corpus::Camera::Type which) const
{
    if(which == corpus::Camera::BOTTOM)
        return bottomMImage;
    else return topMImage;
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

void Memory::subscribe(Subscriber* subscriber,
                           MObject_ID objectToSubscribeTo) const {
    getMObject(objectToSubscribeTo)->addSubscriber(subscriber);
}

void Memory::unsubscribe(Subscriber* subscriber,
                         MObject_ID objectToUnsuscribeFrom) const {
    getMObject(objectToUnsuscribeFrom)->unsubscribe(subscriber);
}

}
}
