
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
        bottomMImage(new MBottomImage(sensors_ptr)),
        topMImage(new MTopImage(sensors_ptr)),
        mLocalization(new MLocalization(loc_ptr))
{

#if defined USE_MEMORY || defined OFFLINE
    if(sensors_ptr.get()) {
        sensors_ptr->addSubscriber(mVisionSensors.get(), NEW_VISION_SENSORS);
        sensors_ptr->addSubscriber(mMotionSensors.get(), NEW_MOTION_SENSORS);
        sensors_ptr->addSubscriber(bottomMImage.get(), NEW_IMAGE);
        sensors_ptr->addSubscriber(topMImage.get(), NEW_IMAGE);
    }
#endif

    mobject_IDMap.insert(MObject_IDPair(class_name<MVision>(), mVision));
    mobject_IDMap.insert(MObject_IDPair(class_name<MVisionSensors>(), mVisionSensors));
    mobject_IDMap.insert(MObject_IDPair(class_name<MMotionSensors>(), mMotionSensors));
    mobject_IDMap.insert(MObject_IDPair(class_name<MTopImage>(), topMImage));
    mobject_IDMap.insert(MObject_IDPair(class_name<MBottomImage>(), bottomMImage));
    mobject_IDMap.insert(MObject_IDPair(class_name<MLocalization>(), mLocalization));
}

Memory::~Memory() {
    cout << "Memory destructor" << endl;
}

void Memory::update(boost::shared_ptr<Object> obj) {
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

}
}
