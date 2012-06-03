/**
 * MImage.hpp
 *
 * @class MImage: this is the memory object representation of an Image
 *
 */

#pragma once

#include <protos/Sensors.pb.h>
#include "RoboImage.h"
#include "MObject.h"
#include "Sensors.h"
#include "ClassHelper.h"
#include "Camera.h"

namespace man {
namespace memory {

class MImage: public MObject {

ADD_SHARED_PTR(MImage);

public:
    static const MObject_ID id = MIMAGE_ID;

    typedef boost::shared_ptr<proto::PImage> PImage_ptr;

public:
    MImage(boost::shared_ptr<Sensors> sensors,
           corpus::Camera::Type type,
           MObject_ID objectID = id,
           PImage_ptr data = PImage_ptr(new proto::PImage));
    virtual ~MImage();
    /**
     * Updates all the fields of the underlying proto::PSensors with values
     * from the Sensors object
     */
    void updateData();

    boost::shared_ptr<const proto::PImage> get() const { return data; }
    boost::shared_ptr<const proto::PImage> getThresholded() const {
        return thresholded_data;
    }

private:
    boost::shared_ptr<Sensors> sensors;
    PImage_ptr data;
    PImage_ptr thresholded_data;
    corpus::Camera::Type cameraType;
};

class MTopImage: public MImage {
public:
    MTopImage(boost::shared_ptr<Sensors> sensors);
};

class MBottomImage: public MImage {
public:
    MBottomImage(boost::shared_ptr<Sensors> sensors);
};

}
}
