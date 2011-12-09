/**
 * MVisionSensors.hpp
 *
 * @class MVisionSensors : this is the memory object representation of Sensors
 *
 */

#pragma once

#include "protos/Sensors.pb.h"
#include "MObject.h"
#include "Sensors.h"
#include "ClassHelper.h"

namespace man {
namespace memory {
class MVisionSensors: public proto::PVisionSensors, public MObject {

ADD_SHARED_PTR(MVisionSensors);

public:
    /**
     * @param v : the shared pointer to the instance of Sensors this MVisionSensors
     * links to
     */
    MVisionSensors(MObject_ID id, boost::shared_ptr<Sensors> s,
                   boost::shared_ptr<proto::PVisionSensors> vision_s_data);
    virtual ~MVisionSensors();
    /**
     * Updates all the fields of the underlying proto::PSensors with values
     * from the Sensors object
     */
    void update();
    void copyTo(boost::shared_ptr<Sensors> sensors) const;

private:
    boost::shared_ptr<Sensors> sensors;
    boost::shared_ptr<proto::PVisionSensors> data;
};
}
}
