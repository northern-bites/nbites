/**
 * MVision.hpp
 *
 * @class MVision : this is the memory object representation of vision
 *
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "protos/Vision.pb.h"
#include "MObject.h"
#include "vision/Vision.h"
#include "ClassHelper.h"

namespace man {
namespace memory {
class MVision: public proto::PVision, public MObject {

    ADD_SHARED_PTR(MVision);

public:
    /**
     * @param v : the shared pointer to the instance of vision this MVision
     * links to
     * @return
     */
    MVision(MObject_ID id, boost::shared_ptr<Vision> v,
            boost::shared_ptr<proto::PVision> vision_data);
    virtual ~MVision();
    /**
     * Updates all the fields of the underlying proto::PVision with values
     * from the Vision object
     */
    void update();

    boost::shared_ptr<const proto::PVision> get() const {return data; }

private:
    //update helper methods
    void update(PVision::PVisualDetection* visual_detection,
            VisualDetection* visualDetection);

private:
    boost::shared_ptr<Vision> vision;
    boost::shared_ptr<proto::PVision> data;

};
}
}
