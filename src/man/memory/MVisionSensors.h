/**
 * @class MVisionSensors : this is the memory object representation of vision Sensors
 *
 * @author Octavian Neamtu
 */

#pragma once

#include "protos/Sensors.pb.h"
#include "MObject.h"
#include "Sensors.h"
#include "ClassHelper.h"

namespace man {
namespace memory {

class MVisionSensors: public MObject {

ADD_SHARED_PTR(MVisionSensors);

typedef boost::shared_ptr<proto::PVisionSensors> PVisionSensors_ptr;

public:
    static const MObject_ID id = MVISION_SENSORS_ID;

public:
    MVisionSensors(boost::shared_ptr<Sensors> sensors,
            PVisionSensors_ptr data = PVisionSensors_ptr(new proto::PVisionSensors));
    virtual ~MVisionSensors();

    /**
     * Updates all the fields of the underlying proto::PSensors with values
     * from the Sensors object
     */
    void updateData();
    void copyTo(boost::shared_ptr<Sensors> sensors) const;

private:
    boost::shared_ptr<Sensors> sensors;
    PVisionSensors_ptr data;
};
}
}
