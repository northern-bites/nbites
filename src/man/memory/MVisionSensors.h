/**
 * MVisionSensors.hpp
 *
 * @class MVisionSensors : this is the memory object representation of Sensors
 *
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "protos/Sensors.pb.h"
#include "MObject.h"
#include "Sensors.h"

namespace man {
namespace memory {
class MVisionSensors: public proto::PVisionSensors, public MObject {

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

private:
    boost::shared_ptr<Sensors> sensors;
    boost::shared_ptr<proto::PVisionSensors> data;
};
}
}
