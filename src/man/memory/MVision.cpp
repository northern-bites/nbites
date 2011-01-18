/*
 * MVision.cpp
 *
 *      Author: oneamtu
 */

#include "Common.h" //for micro_time

#include "MVision.hpp"

extern long long int birth_time;

namespace memory {

using boost::shared_ptr;

MVision::MVision(shared_ptr<Vision> v) : vision(v) {
    fileLogger = new log::FileLogger("Vision.log", MVISION_ID, this);
}

MVision::~MVision() {
    delete fileLogger;
}

void MVision::update() {

    //TODO: should we make this do milisecs instead of
    //micro to save space?
    this->set_timestamp(micro_time() - birth_time);

    proto::PVision::VisualBall* visual_ball;
    visual_ball = this->mutable_visual_ball();
    visual_ball->set_distance(vision->ball->getDistance());
    visual_ball->set_center_x(vision->ball->getCenterX());
    visual_ball->set_center_y(vision->ball->getCenterY());
    visual_ball->set_x(vision->ball->getX());
    visual_ball->set_y(vision->ball->getY());
    visual_ball->set_radius(vision->ball->getRadius());
    cout << this->DebugString() << endl;
}

void MVision::log() const {
    fileLogger->write();
}
}
