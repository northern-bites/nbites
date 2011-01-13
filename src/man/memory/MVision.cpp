/*
 * MVision.cpp
 *
 *      Author: oneamtu
 */

#include "MVision.hpp"

using boost::shared_ptr;

MVision::MVision(shared_ptr<Vision> v) : vision(v) {
}

void MVision::update() {

    Proto::PVision::VisualBall *visual_ball;
    visual_ball = this->mutable_visual_ball();
    visual_ball->set_distance(vision->ball->getDistance());
    visual_ball->set_center_x(vision->ball->getCenterX());
    visual_ball->set_center_y(vision->ball->getCenterY());
    visual_ball->set_x(vision->ball->getX());
    visual_ball->set_y(vision->ball->getY());
    visual_ball->set_radius(vision->ball->getRadius());
    cout << this << endl;
}
