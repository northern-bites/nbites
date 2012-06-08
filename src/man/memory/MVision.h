/**
 * MVision.hpp
 *
 * @class MVision : this is the memory object representation of vision
 *
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include "protos/Vision.pb.h"
#include "MemoryCommon.h"
#include "ClassHelper.h"

namespace man {
namespace memory {

class MVision: public TemplatedProtobufMessage<proto::PVision> {

    ADD_SHARED_PTR(MVision);

public:
    MVision(std::string name);
    virtual ~MVision();

    /**
     * Updates all the fields of the underlying proto::PVision with values
     * from the Vision object
     */
    void updateData();

};
}
}
