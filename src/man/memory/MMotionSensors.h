/**
 * MMotionSensors.hpp
 *
 * @class MMotionSensors : this is the memory object representation of motion sensors
 *
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "protos/Sensors.pb.h"
#include "MObject.h"
#include "Sensors.h"

namespace man {
namespace memory {
class MMotionSensors: public proto::PMotionSensors, public MObject {

public:
    /**
     * @param v : the shared pointer to the instance of Sensors this MMotionSensors
     * links to
     * @return
     */
    MMotionSensors(boost::shared_ptr<Sensors> s);
    ~MMotionSensors();
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
