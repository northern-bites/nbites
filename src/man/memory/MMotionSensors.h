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
class MMotionSensors: public MObject<proto::PMotionSensors> {

    ADD_SHARED_PTR(MMotionSensors);

public:
    static const MObject_ID id = MMOTION_SENSORS_ID;

public:
    MMotionSensors(boost::shared_ptr<Sensors> sensors,
                   data_ptr data = data_ptr(new proto::PMotionSensors));
    virtual ~MMotionSensors();

    /**
     * Updates all the fields of the underlying proto::PSensors with values
     * from the Sensors object
     */
    void updateData();

private:
    boost::shared_ptr<Sensors> sensors;

};
}
}
