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
     * @return
     */
    MVisionSensors(MObject_ID id, std::string name, boost::shared_ptr<Sensors> s);
    virtual ~MVisionSensors();
    /**
     * Updates all the fields of the underlying proto::PSensors with values
     * from the Sensors object
     */
    void update();

private:
    boost::shared_ptr<Sensors> sensors;

};
}
}
