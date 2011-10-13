/**
 * MImage.hpp
 *
 * @class MImage: this is the memory object representation of an Image
 *
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <protos/Sensors.pb.h>
#include "RoboImage.h"
#include "MObject.h"
#include "Sensors.h"

namespace man {
namespace memory {
class MImage: public MObject {

public:
    typedef boost::shared_ptr<MImage> ptr;
    typedef boost::shared_ptr<const MImage> const_ptr;

public:
    /**
     * @param v : the shared pointer to the instance of Sensors this MImage
     * links to
     */
    MImage(MObject_ID id, boost::shared_ptr<Sensors> s,
           boost::shared_ptr<proto::PImage> image_data);
    virtual ~MImage();
    /**
     * Updates all the fields of the underlying proto::PSensors with values
     * from the Sensors object
     */
    void update();
    void log() const;

    boost::shared_ptr<const proto::PImage> get() const { return data; }

private:
    boost::shared_ptr<Sensors> sensors;
    boost::shared_ptr<proto::PImage> data;
};
}
}
