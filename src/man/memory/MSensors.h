/**
 * MSensors.hpp
 *
 * @class MSensors : this is the memory object representation of Sensors
 * It wraps the two distinct sides of sensors : motion and vision sensors
 *
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "protos/Sensors.pb.h"
#include "MObject.h"
#include "MVisionSensors.h"
#include "MMotionSensors.h"
#include "MImage.h"

#include "Sensors.h"
#include "Profiler.h"

#include "log/CodedFileLogger.h"
#include "Subscriber.h"

namespace memory {
class MSensors: public MMotionSensors, public MVisionSensors,
    public MImage, public Subscriber {

public:
    /**
     * @param v : the shared pointer to the instance of Sensors this MSensors
     * links to
     * @return
     */
    MSensors(boost::shared_ptr<Profiler> p,
            boost::shared_ptr<Sensors> s);
    ~MSensors();
    /**
     * Updates all the fields of the underlying proto::PSensors with values
     * from the Sensors object
     */
    void update(const ProviderEvent e);

private:
    boost::shared_ptr<Profiler> _profiler;

};
}
