/**
 * Memory.hpp
 *
 * @class Memory
 *
 * This class keeps instances of all the different memory objects and provides
 * an interface through which they get updated (each memory object pulls data
 * from its corresponding object
 *
 * Future work: we will be able to keep multiple instances of selected objects
 *
 *      Author: Octavian Neamtu
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <map>

namespace man {
namespace memory {
class Memory; //forward declaration
}
}

#include "MVision.h"
#include "Vision.h"
#include "MVisionSensors.h"
#include "MMotionSensors.h"
#include "MImage.h"
#include "Sensors.h"
#include "Profiler.h"
#include "include/MultiProvider.h"

namespace man {
namespace memory {

typedef google::protobuf::Message ProtoMessage;

class Memory : public Subscriber<SensorsEvent>,
               public MultiProvider<MObject_ID> {

public:
    typedef boost::shared_ptr<Memory> ptr;
    typedef boost::shared_ptr<const Memory> const_ptr;
    typedef std::pair<MObject_ID,
            boost::shared_ptr<ProtoMessage> > ProtoMessagePair;
    typedef std::map<MObject_ID,
            boost::shared_ptr<ProtoMessage> > ProtoMessageMap;

public:
    Memory( boost::shared_ptr<Vision> vision_ptr = boost::shared_ptr<Vision>(),
            boost::shared_ptr<Sensors> sensors_ptr = boost::shared_ptr<Sensors>());
    virtual ~Memory();
    /**
     * calls the update function on @obj
     * this will usually make the MObject pull data
     * from its corresponding man object and maybe log it
     */
    void update(boost::shared_ptr<MObject> obj);
    void updateVision();

    /**
     * This function is called whenever one of the Providers we are subscribed
     * to has something new/updated
     */
    void update(SensorsEvent eventID);

public:
    const MVision* getMVision() const {return mVision.get();}
    const MVisionSensors* getMVisionSensors() const {return mVisionSensors.get();}
    const MMotionSensors* getMMotionSensors() const {return mMotionSensors.get();}
    const MImage* getMImage() const {return mImage.get();}

    boost::shared_ptr<const RoboImage> getRoboImage() const {return mImage;}
    boost::shared_ptr<const ProtoMessage> getProtoMessage(MObject_ID id) const;

    boost::shared_ptr<RoboImage> getMutableRoboImage() {return mImage;}
    boost::shared_ptr<ProtoMessage> getMutableProtoMessage(MObject_ID id);

private:
    ProtoMessageMap protoMessageMap;
    boost::shared_ptr<Sensors> _sensors;
    boost::shared_ptr<MVision> mVision;
    boost::shared_ptr<MVisionSensors> mVisionSensors;
    boost::shared_ptr<MMotionSensors> mMotionSensors;
    boost::shared_ptr<MImage> mImage;
};
}
}
