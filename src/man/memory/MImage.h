/**
 * MImage.hpp
 *
 * @class MImage: this is the memory object representation of an Image
 *
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "protos/Sensors.pb.h"
#include "RoboImage.h"
#include "MObject.h"
#include "Sensors.h"

namespace man {
namespace memory {
class MImage: public RoboImage, public MObject {

public:
    /**
     * @param v : the shared pointer to the instance of Sensors this MImage
     * links to
     */
    MImage(boost::shared_ptr<Sensors> s);
    ~MImage();
    /**
     * Updates all the fields of the underlying proto::PSensors with values
     * from the Sensors object
     */
    void update();
    void log() const;

private:
    boost::shared_ptr<Sensors> sensors;

};
}
}
