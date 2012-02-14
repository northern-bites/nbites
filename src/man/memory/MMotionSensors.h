/**
 * @class MMotionSensors : this is the memory object representation of motion sensors
 *
 * @author Octavian Neamtu
 *
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "protos/Sensors.pb.h"
#include "MObject.h"
#include "Sensors.h"
#include "ClassHelper.h"

namespace man {
namespace memory {
class MMotionSensors: public Message {

    ADD_SHARED_PTR(MMotionSensors);

public:
    static const MObject_ID id = MMOTION_SENSORS_ID;

public:
    MMotionSensors(boost::shared_ptr<Sensors> sensors);
    virtual ~MMotionSensors();
    /**
     * Updates all the fields of the underlying proto::PSensors with values
     * from the Sensors object
     */
    void updateData();

private:
    boost::shared_ptr<Sensors> sensors;
    boost::shared_ptr<proto::PMotionSensors> data;

};
}
}
