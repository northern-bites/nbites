/**
 * Class definitions for our beloved memory objects
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include "protos/Vision.pb.h"
#include "protos/Loc.pb.h"
#include "MemoryCommon.h"
#include "ClassHelper.h"

namespace man {
namespace memory {

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

}
}
