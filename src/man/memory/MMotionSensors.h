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
     * @param s : the shared pointer to the instance of Sensors this MMotionSensors
     * links to
     */
    MMotionSensors(MObject_ID id, boost::shared_ptr<Sensors> s,
                   boost::shared_ptr<proto::PMotionSensors> motion_s_data);
    virtual ~MMotionSensors();
    /**
     * Updates all the fields of the underlying proto::PSensors with values
     * from the Sensors object
     */
    void update();

private:
    boost::shared_ptr<Sensors> sensors;
    boost::shared_ptr<proto::PMotionSensors> data;

};
}
}
