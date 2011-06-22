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

MVision::MVision(shared_ptr<Vision> v) :
    vision(v) {
}

MVision::~MVision() {
}

void MVision::update() {

    ADD_PROTO_TIMESTAMP;
//    cout << "MVision_updata timestamp is " << process_micro_time() << endl;

    using namespace proto;
    //VisualBall
    PVision::PVisualBall* visual_ball;
    visual_ball = this->mutable_visual_ball();

    //VisualBall::VisualDetection
    PVision::PVisualDetection* visual_detection;
    visual_detection = visual_ball->mutable_visual_detection();
    update(visual_detection, vision->ball);

    //VisualBall::stuff
    visual_ball->set_radius(vision->ball->getRadius());
    visual_ball->set_confidence(vision->ball->getConfidence());

    //VisualCorners
    this->clear_visual_corner();
    list<VisualCorner>* visualCorners = vision->fieldLines->getCorners();
    for (list<VisualCorner>::iterator i = visualCorners->begin(); i
            != visualCorners->end(); i++) {
        //VisualCorner
        PVision::PVisualCorner* visual_corner = this->add_visual_corner();

        //VisualCorner::VisualDetection
        visual_detection = visual_corner->mutable_visual_detection();
        update(visual_detection, &(*i));

        //VisualCorner::VisualLandmark
        PVision::PVisualLandmark* visual_landmark =
                visual_corner->mutable_visual_landmark();
        visual_landmark->set_id(i->getID());
        visual_landmark->set_id_certainty(i->getIDCertainty());
        visual_landmark->set_distance_certainty(i->getDistanceCertainty());

        //VisualCorner::stuff
        visual_corner->set_corner_type(i->getShape());
        visual_corner->set_secondary_shape(i->getSecondaryShape());
        visual_corner->set_angle_between_lines(i->getAngleBetweenLines());
        visual_corner->set_orientation(i->getOrientation());
        visual_corner->set_up(i->doesItPointUp());
        visual_corner->set_right(i->doesItPointDown());

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
}
}
