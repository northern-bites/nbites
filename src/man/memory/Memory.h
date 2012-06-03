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

namespace man {
namespace memory {
class Memory; //forward declaration
}
}

#include "MObject.h"
#include "MVision.h"
#include "Vision.h"
#include "MVisionSensors.h"
#include "MMotionSensors.h"
#include "MImage.h"
#include "Camera.h"
#include "MLocalization.h"
#include "Sensors.h"
#include "Profiler.h"
#include "ClassHelper.h"

namespace man {
namespace memory {

class Memory {

public:
    ADD_NULL_INSTANCE(Memory)
    ADD_SHARED_PTR(Memory)
    typedef std::pair<MObject_ID,
            boost::shared_ptr<MObject> > MObject_IDPair;
    typedef std::map<MObject_ID,
            boost::shared_ptr<MObject> > MObject_IDMap;
    typedef MObject_IDMap::const_iterator const_iterator;

public:
    Memory(boost::shared_ptr<Vision> vision_ptr = boost::shared_ptr<Vision>(),
           boost::shared_ptr<Sensors> sensors_ptr = boost::shared_ptr<Sensors>(),
           boost::shared_ptr<LocSystem> loc_ptr = boost::shared_ptr<LocSystem>());
    virtual ~Memory();
    /**
     * calls the update function on @obj
     * this will usually make the MObject pull data
     * from its corresponding man object
     */
    void update(boost::shared_ptr<MObject> obj);
    void updateVision();

public:
    MVision::const_ptr getMVision() const {return mVision;}
    MVisionSensors::const_ptr getMVisionSensors() const {return mVisionSensors;}
    MMotionSensors::const_ptr getMMotionSensors() const {return mMotionSensors;}
    MImage::const_ptr getMImage(corpus::Camera::Type which) const;

    MObject::const_ptr getMObject(MObject_ID id) const;
    MObject::ptr getMutableMObject(MObject_ID id);

    const_iterator begin() const { return mobject_IDMap.begin(); }
    const_iterator end() const { return mobject_IDMap.end(); }

    void subscribe(Subscriber* subscriber,
                       MObject_ID objectToSubscribeTo) const;
    void unsubscribe(Subscriber* subscriber,
                     MObject_ID objectToUnsuscribeFrom) const;

private:
    MObject_IDMap mobject_IDMap;
    boost::shared_ptr<MVision> mVision;
    boost::shared_ptr<MVisionSensors> mVisionSensors;
    boost::shared_ptr<MMotionSensors> mMotionSensors;
        // FAKE
    boost::shared_ptr<MImage> mImage;
    boost::shared_ptr<MBottomImage> bottomMImage;
    boost::shared_ptr<MTopImage> topMImage;
    boost::shared_ptr<MLocalization> mLocalization;
};
}
}
