
#include <iostream>

#include "Common.h"
#include "RobotMemory.h"

namespace man {
namespace memory {

using boost::shared_ptr;
using namespace proto;
using namespace std;

RobotMemory::RobotMemory(shared_ptr<Sensors> sensors_ptr) :
        mVision(new MVision(class_name<MVision>())),
        mVisionSensors(new MVisionSensors(class_name<MVisionSensors>(), sensors_ptr)),
        mMotionSensors(new MMotionSensors(class_name<MMotionSensors>(),sensors_ptr)),
        mBottomImage(new MBottomImage(class_name<MBottomImage>(),sensors_ptr)),
        mTopImage(new MTopImage(class_name<MTopImage>(), sensors_ptr)),
        mLocalization(new MLocalization(class_name<MLocalization>()))
{

#if defined USE_MEMORY || defined OFFLINE
    if(sensors_ptr.get()) {
        sensors_ptr->addSubscriber(mVisionSensors.get(), NEW_VISION_SENSORS);
        sensors_ptr->addSubscriber(mMotionSensors.get(), NEW_MOTION_SENSORS);
        sensors_ptr->addSubscriber(mBottomImage.get(), NEW_IMAGE);
        sensors_ptr->addSubscriber(mTopImage.get(), NEW_IMAGE);
    }
#endif

    mobject_IDMap.insert(MObject_IDPair(mVision->getName(), mVision));
    mobject_IDMap.insert(MObject_IDPair(mVisionSensors->getName(), mVisionSensors));
    mobject_IDMap.insert(MObject_IDPair(mMotionSensors->getName(), mMotionSensors));
    mobject_IDMap.insert(MObject_IDPair(mTopImage->getName(), mTopImage));
    mobject_IDMap.insert(MObject_IDPair(mBottomImage->getName(), mBottomImage));
    mobject_IDMap.insert(MObject_IDPair(mLocalization->getName(), mLocalization));
}

RobotMemory::~RobotMemory() {
    cout << "Robot Memory destructor" << endl;
}

}
}
