
#include <iostream>

#include "Common.h"
#include "RobotMemory.h"

namespace man {
namespace memory {

using boost::shared_ptr;
using namespace proto;
using namespace std;

RobotMemory::RobotMemory(shared_ptr<Sensors> sensors_ptr) :
        mVision(new MVision()),
        mVisionSensors(new MVisionSensors(class_name<MVisionSensors>(), sensors_ptr)),
        mMotionSensors(new MMotionSensors(class_name<MMotionSensors>(),sensors_ptr)),
        mRawImages(new MRawImages()),
        mLocalization(new MLocalization())
{

#if defined USE_MEMORY || defined OFFLINE
    if(sensors_ptr.get()) {
        sensors_ptr->addSubscriber(mVisionSensors.get(), NEW_VISION_SENSORS);
        sensors_ptr->addSubscriber(mMotionSensors.get(), NEW_MOTION_SENSORS);
    }
#endif

    mobject_IDMap.insert(MObject_IDPair(mVision->getName(), mVision));
    mobject_IDMap.insert(MObject_IDPair(mVisionSensors->getName(), mVisionSensors));
    mobject_IDMap.insert(MObject_IDPair(mMotionSensors->getName(), mMotionSensors));
    mobject_IDMap.insert(MObject_IDPair(mRawImages->getName(), mRawImages));
    mobject_IDMap.insert(MObject_IDPair(mLocalization->getName(), mLocalization));
}

RobotMemory::~RobotMemory() {
    cout << "Robot Memory destructor" << endl;
}

}
}
