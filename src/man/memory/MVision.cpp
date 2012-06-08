
#include "MVision.h"

namespace man {
namespace memory {

using boost::shared_ptr;
using std::list;
using std::vector;
using proto::PVision;

MVision::MVision(shared_ptr<Vision> vision, PVision_ptr data) :
        MObject(id, data),
        vision(vision),
        data(data) {
}

MVision::~MVision() {
}

void MVision::updateData() {

    this->data->set_timestamp(time_stamp());

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
    
    //VisualRobot
    PVision::PVisualRobot* red1;
    red1=this->data->mutable_red1();
    update(red1, vision->red1);

    PVision::PVisualRobot* red2;
    red2=this->data->mutable_red2();
    update(red2, vision->red2);
    
    PVision::PVisualRobot* red3;
    red3=this->data->mutable_red3();
    update(red3, vision->red3);
    
    PVision::PVisualRobot* navy1;
    navy1=this->data->mutable_navy1();
    update(navy1, vision->navy1);
    
    PVision::PVisualRobot* navy2;
    navy2=this->data->mutable_navy2();
    update(navy2, vision->navy2);
    
    PVision::PVisualRobot* navy3;
    navy3=this->data->mutable_navy3();
    update(navy3, vision->navy3);

    PVision::PVisualCross* visual_cross;
    visual_cross=this->data->mutable_visual_cross();
    update(visual_cross, vision->cross);

    //VisualLines
    this->data->clear_visual_line();
    const vector<boost::shared_ptr<VisualLine> >* visualLines = vision->fieldLines->getLines();
    for(vector<boost::shared_ptr<VisualLine> >::const_iterator i = visualLines->begin(); 
	    i != visualLines->end(); i++){

      PVision::PVisualLine* visual_line = this->data->add_visual_line();
      update(visual_line, (*i));
    }
      

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
  void MVision::update(PVision::PVisualRobot* visual_robot,
		       VisualRobot* visualRobot) {
    PVision::PVisualDetection* visual_detection = visual_robot->mutable_visual_detection();
    update(visual_detection, visualRobot);

    visual_robot->set_left_top_x(visualRobot->getLeftTopX());
    visual_robot->set_left_top_y(visualRobot->getLeftTopY());
    visual_robot->set_left_bottom_x(visualRobot->getLeftBottomX());
    visual_robot->set_left_bottom_y(visualRobot->getLeftBottomY());
    visual_robot->set_right_top_x(visualRobot->getRightTopX());
    visual_robot->set_right_top_y(visualRobot->getRightTopY());
    visual_robot->set_right_bottom_x(visualRobot->getRightBottomX());
    visual_robot->set_right_bottom_y(visualRobot->getRightBottomY());				    
  }

  void MVision::update(PVision::PVisualLine* visual_line, 
		       boost::shared_ptr<VisualLine> visualLine) {
    PVision::PVisualLandmark* visual_landmark = visual_line->mutable_visual_landmark();
    update(visual_landmark, visualLine.get());
    
    visual_line->set_start_x(visualLine->getStartpoint().x);
    visual_line->set_start_y(visualLine->getStartpoint().y);
    visual_line->set_end_x(visualLine->getEndpoint().x);
    visual_line->set_end_y(visualLine->getEndpoint().y);
  }
  
  void MVision::update(PVision::PVisualCross* visual_cross,
		       VisualCross* visualCross) {
    PVision::PVisualDetection* visual_detection= visual_cross->mutable_visual_detection();
    update(visual_detection, visualCross);
    
    PVision::PVisualLandmark* visual_landmark= visual_cross->mutable_visual_landmark();
    update(visual_landmark, visualCross);

    visual_cross->set_left_top_x(visualCross->getLeftTopX());
    visual_cross->set_left_top_y(visualCross->getLeftTopY());
    visual_cross->set_left_bottom_x(visualCross->getLeftBottomX());
    visual_cross->set_left_bottom_y(visualCross->getLeftBottomY());
    visual_cross->set_right_top_x(visualCross->getRightTopX());
    visual_cross->set_right_top_y(visualCross->getRightTopY());
    visual_cross->set_right_bottom_x(visualCross->getRightBottomX());
    visual_cross->set_right_bottom_y(visualCross->getRightBottomY());

  }
}
}
