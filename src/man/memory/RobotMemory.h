/**
 * @class Memory
 *
 * This class keeps instances of all the different memory objects and provides
 * an interface through which they get updated (each memory object copies data
 * from its corresponding man object
 *
 * Each MObject is associated with a MObject_ID that identifies it
 *
 * Future work: we will be able to keep multiple instances of selected objects
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <map>
#include <exception>

#include "Memory.h"
#include "MVision.h"
#include "Vision.h"
#include "MVisionSensors.h"
#include "MMotionSensors.h"
#include "MImage.h"
#include "Camera.h"
#include "MLocalization.h"
#include "Sensors.h"
#include "Profiler.h"

namespace man {
namespace memory {


class RobotMemory : public Memory {

    ADD_SHARED_PTR(RobotMemory)

public:
    RobotMemory(boost::shared_ptr<Sensors> sensors_ptr = Sensors::ptr());
    virtual ~RobotMemory();

public:
    //TODO: these calls should be deprecated with get<X>() calls
    MVision::const_ptr getMVision() const {return mVision;}
    MVisionSensors::const_ptr getMVisionSensors() const {return mVisionSensors;}
    MMotionSensors::const_ptr getMMotionSensors() const {return mMotionSensors;}

protected:
    boost::shared_ptr<MVision> mVision;
    boost::shared_ptr<MVisionSensors> mVisionSensors;
    boost::shared_ptr<MMotionSensors> mMotionSensors;
    boost::shared_ptr<MRawImages> mRawImages;
    boost::shared_ptr<MLocalization> mLocalization;
};


}
}
