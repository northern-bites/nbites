/**
 * MSensors.hpp
 *
 * @class MSensors : this is the memory object representation of Sensors
 *
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "protos/Sensors.pb.h"
#include "MObject.hpp"
#include "MVisionSensors.hpp"
#include "MMotionSensors.hpp"
#include "Sensors.h"
#include "log/FileLogger.hpp"

namespace memory {
class MSensors: public MMotionSensors, public MVisionSensors {

public:
    /**
     * @param v : the shared pointer to the instance of Sensors this MSensors
     * links to
     * @return
     */
    MSensors(boost::shared_ptr<Sensors> s);
    ~MSensors();
    /**
     * Updates all the fields of the underlying proto::PSensors with values
     * from the Sensors object
     */
    //void update();
    //void log() const;

private:
    boost::shared_ptr<Sensors> sensors;
    //log::FileLogger* fileLogger;

};
}
