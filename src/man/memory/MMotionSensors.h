/**
 * @class MMotionSensors : this is the memory object representation of motion sensors
 *
 * @author Octavian Neamtu
 *
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "protos/Sensors.pb.h"
#include "MemoryCommon.h"
#include "Sensors.h"
#include "ClassHelper.h"

namespace man {
namespace memory {
class MMotionSensors: public TemplatedProtobufMessage<proto::PMotionSensors> {

    ADD_SHARED_PTR(MMotionSensors);

public:
    MMotionSensors(std::string name, boost::shared_ptr<Sensors> sensors);
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
