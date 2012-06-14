/**
 * Class definitions for our beloved memory objects
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include "protos/Vision.pb.h"
#include "protos/Loc.pb.h"
#include "protos/Sensors.pb.h"
#include "protos/Motion.pb.h"
#include "MemoryCommon.h"
#include "ClassHelper.h"

//for camera/image type
#include "corpus/Camera.h"

namespace man {
namespace memory {

namespace protobuf = google::protobuf;

typedef protobuf::RepeatedField<float> RepeatedFloats;

//being lazy ...

#define ADD_MEMORY_OBJECT(classType, protoType) \
class classType: public TemplatedProtobufMessage<proto::protoType> { \
    ADD_SHARED_PTR(classType); \
    \
public: \
    classType(std::string name = class_name<classType>()) \
        : TemplatedProtobufMessage<proto::protoType>(name) {} \
};

ADD_MEMORY_OBJECT(MVision, PVision)
ADD_MEMORY_OBJECT(MLocalization, PLoc)
ADD_MEMORY_OBJECT(MRawNaoImages, PRawNaoImages)
ADD_MEMORY_OBJECT(MVisionSensors, PVisionSensors)
ADD_MEMORY_OBJECT(MMotionSensors, PMotionSensors)
ADD_MEMORY_OBJECT(MMotion, Motion)

//adds quick camera methods to MRawNaoImages
class MRawImages : public MRawNaoImages {

    ADD_SHARED_PTR(MRawImages)

public:
    MRawImages() : MRawNaoImages(class_name<MRawImages>()) {}

    const proto::PRawImage* getPImage(corpus::Camera::Type which) const {
        if (which == corpus::Camera::TOP) return &(this->get()->topimage());
        return &(this->get()->bottomimage());
    }
};

}
}
