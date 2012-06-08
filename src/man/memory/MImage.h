/**
 * MImage.hpp
 *
 * @class MImage: this is the memory object representation of an Image
 *
 */

#pragma once

#include <protos/Sensors.pb.h>
#include "MemoryCommon.h"
#include "Sensors.h"
#include "ClassHelper.h"
#include "Camera.h"

namespace man {
namespace memory {

class MImage: public TemplatedProtobufMessage<proto::PImage> {

ADD_SHARED_PTR(MImage);

public:
    typedef boost::shared_ptr<proto::PImage> PImage_ptr;

public:
    MImage(std::string name,
           boost::shared_ptr<Sensors> sensors,
           corpus::Camera::Type type);
    virtual ~MImage();
    /**
     * Updates all the fields of the underlying proto::PSensors with values
     * from the Sensors object
     */
    void updateData();

    boost::shared_ptr<const proto::PImage> getThresholded() const {
        return thresholded_data;
    }

private:
    boost::shared_ptr<Sensors> sensors;
    PImage_ptr thresholded_data;
    corpus::Camera::Type cameraType;
};

class MTopImage: public MImage {
public:
    MTopImage(std::string name, boost::shared_ptr<Sensors> sensors);
};

class MBottomImage: public MImage {
public:
    MBottomImage(std::string name, boost::shared_ptr<Sensors> sensors);
};

}
}
