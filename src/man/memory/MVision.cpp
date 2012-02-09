/*
 * MVision.cpp
 *
 *      Author: oneamtu
 */

#include "MemoryMacros.h"
#include "MVision.h"

namespace man {
namespace memory {

using boost::shared_ptr;
using std::list;
using proto::PVision;

MVision::MVision(MObject_ID id, shared_ptr<Vision> v, shared_ptr<PVision> vision_data) :
        MObject(id, vision_data),
        vision(v),
        data(vision_data) {
}

MVision::~MVision() {
}

void MVision::update() {

    ADD_PROTO_TIMESTAMP;

    using namespace proto;
    //VisualBall
    PVision::PVisualBall* visual_ball;
    visual_ball = this->data->mutable_visual_ball();

    //VisualBall::VisualDetection
    PVision::PVisualDetection* visual_detection;
    visual_detection = visual_ball->mutable_visual_detection();
    update(visual_detection, vision->ball);

    //VisualBall::stuff
    visual_ball->set_radius(vision->ball->getRadius());
    visual_ball->set_confidence(vision->ball->getConfidence());
    
    //VisualFieldObject
    PVision::PVisualFieldObject* bglp;
    bglp= this->data->mutable_bglp();
    update(bglp, vision->bglp);
    
    PVision::PVisualFieldObject* bgrp;
    bgrp= this->data->mutable_bgrp();
    update(bgrp, vision->bgrp);

    PVision::PVisualFieldObject* yglp;
    yglp= this->data->mutable_yglp();
    update(yglp, vision->yglp);

    PVision::PVisualFieldObject* ygrp;
    ygrp= this->data->mutable_ygrp();
    update(ygrp, vision->ygrp);
    
    
    //VisualCorners
    this->data->clear_visual_corner();
    list<VisualCorner>* visualCorners = vision->fieldLines->getCorners();
    for (list<VisualCorner>::iterator i = visualCorners->begin(); i
            != visualCorners->end(); i++) {
        //VisualCorner
        PVision::PVisualCorner* visual_corner =
                this->data->add_visual_corner();

        //VisualCorner::VisualDetection
        visual_detection = visual_corner->mutable_visual_detection();
        update(visual_detection, &(*i));

        //VisualCorner::VisualLandmark
        PVision::PVisualLandmark* visual_landmark =
                visual_corner->mutable_visual_landmark();
	update(visual_landmark, &(*i));
	

        //VisualCorner::stuff
        visual_corner->set_corner_type(i->getShape());
        visual_corner->set_secondary_shape(i->getSecondaryShape());
        visual_corner->set_angle_between_lines(i->getAngleBetweenLines());
        visual_corner->set_orientation(i->getOrientation());
    }

}

void MVision::update(PVision::PVisualDetection* visual_detection,
        VisualDetection* visualDetection) {
    visual_detection->set_distance(visualDetection->getDistance());
    visual_detection->set_center_x(visualDetection->getCenterX());
    visual_detection->set_center_y(visualDetection->getCenterY());
    visual_detection->set_x(visualDetection->getX());
    visual_detection->set_y(visualDetection->getY());
    visual_detection->set_angle_x(visualDetection->getAngleX());
    visual_detection->set_angle_y(visualDetection->getAngleY());
    visual_detection->set_bearing(visualDetection->getBearing());
    visual_detection->set_elevation(visualDetection->getElevation());
    visual_detection->set_distance_sd(visualDetection->getDistanceSD());
    visual_detection->set_bearing_sd(visualDetection->getBearingSD());
}

  void MVision::update(PVision::PVisualLandmark* visual_landmark, 
		       VisualLandmark* visualLandmark) {
    visual_landmark->set_id(visualLandmark->getID());
    visual_landmark->set_id_certainty(visualLandmark->getIDCertainty());
    visual_landmark->set_distance_certainty(visualLandmark->getDistanceCertainty());

}
  void MVision::update(PVision::PVisualFieldObject* visual_field_object,
		       VisualFieldObject* visualFieldObject) {
    PVision::PVisualDetection* visual_detection = visual_field_object->mutable_visual_detection();
    PVision::PVisualLandmark* visual_landmark = visual_field_object->mutable_visual_landmark();
    update(visual_detection, visualFieldObject);
    update(visual_landmark, visualFieldObject);
    visual_field_object->set_left_top_x(visualFieldObject->getLeftTopX());
    visual_field_object->set_left_top_y(visualFieldObject->getLeftTopY());
    visual_field_object->set_left_bottom_x(visualFieldObject->getLeftBottomX());
    visual_field_object->set_left_bottom_y(visualFieldObject->getLeftBottomY());
    visual_field_object->set_right_top_x(visualFieldObject->getRightTopX());
    visual_field_object->set_right_top_y(visualFieldObject->getRightTopY());    
    visual_field_object->set_right_bottom_x(visualFieldObject->getRightBottomX());
    visual_field_object->set_right_bottom_y(visualFieldObject->getRightBottomY());
}
}
}
