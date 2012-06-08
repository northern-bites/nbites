/**
 * MVision.hpp
 *
 * @class MVision : this is the memory object representation of vision
 *
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "protos/Vision.pb.h"
#include "vision/Vision.h"
#include "MemoryCommon.h"
#include "ClassHelper.h"

namespace man {
namespace memory {

class MVision: public TemplatedProtobufMessage<proto::PVision> {

    ADD_SHARED_PTR(MVision);

public:
    MVision(std::string name, boost::shared_ptr<Vision> vision);
    virtual ~MVision();

    /**
     * Updates all the fields of the underlying proto::PVision with values
     * from the Vision object
     */
    void updateData();

private:
    //update helper methods
    void update(proto::PVision::PVisualDetection* visual_detection,
            VisualDetection* visualDetection);
    void update(proto::PVision::PVisualLandmark* visual_landmark,
	    VisualLandmark* visualLandmark);
    void update(proto::PVision::PVisualFieldObject* visual_field_object,
	    VisualFieldObject* visualFieldObject);
    void update(proto::PVision::PVisualRobot* visual_robot,
	    VisualRobot* visualRobot);
    void update(proto::PVision::PVisualLine* visual_line,
		boost::shared_ptr<VisualLine> visualLine);
    void update(proto::PVision::PVisualCross* visual_cross,
	    VisualCross* visualCross);
private:
    boost::shared_ptr<Vision> vision;

};
}
}
