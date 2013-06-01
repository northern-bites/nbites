#include "VisionModule.h"

#include "Profiler.h"
#include <iostream>

using namespace portals;

namespace man{
namespace vision{

VisionModule::VisionModule() : Module(),
                               topThrImage(),
                               topYImage(),
                               topUImage(),
                               topVImage(),
                               botThrImage(),
                               botYImage(),
                               botUImage(),
                               botVImage(),
                               joint_angles(),
                               inertial_state(),
                               vision_field(base()),
                               vision_ball(base()),
                               vision_robot(base()),
                               vision_obstacle(base()),
							   topOutPic(base()),
							   botOutPic(base()),
                               vision(boost::shared_ptr<Vision>(new Vision()))
{
}

VisionModule::~VisionModule()
{
}


void VisionModule::run_()
{
    topThrImage.latch();
    topYImage.latch();
    topUImage.latch();
    topVImage.latch();
    botThrImage.latch();
    botYImage.latch();
    botUImage.latch();
    botVImage.latch();
    joint_angles.latch();
    inertial_state.latch();

    PROF_ENTER(P_VISION);

    vision->notifyImage(topThrImage.message(), topYImage.message(),
                        topUImage.message(), topVImage.message(),
                        botThrImage.message(), botYImage.message(),
                        botUImage.message(), botVImage.message(),
                        joint_angles.message(), inertial_state.message());

    PROF_EXIT(P_VISION);

    updateVisionBall();
    updateVisionRobot();
    updateVisionField();
    updateVisionObstacle();

	portals::Message<messages::ThresholdImage> top, bot;
	top = new messages::ThresholdImage(vision->thresh->thresholded, 320, 240, 320);
	bot = new messages::ThresholdImage(vision->thresh->thresholdedBottom, 320, 240, 320);


	topOutPic.setMessage(top);
	botOutPic.setMessage(bot);
		

}

void VisionModule::updateVisionObstacle() {
    portals::Message<messages::VisionObstacle> obstacle_data(0);

    obstacle_data.get()->set_on_left(vision->obstacles->onLeft());
    obstacle_data.get()->set_on_right(vision->obstacles->onRight());
    obstacle_data.get()->set_off_field(vision->obstacles->offField());

    vision_obstacle.setMessage(obstacle_data);

}

void VisionModule::updateVisionBall() {

    portals::Message<messages::VisionBall> ball_data(0);

    ball_data.get()->set_intopcam(vision->ball->isTopCam());
    ball_data.get()->set_distance(vision->ball->getDistance());
    ball_data.get()->set_angle_x_deg(vision->ball->getAngleXDeg());
    ball_data.get()->set_angle_y_deg(vision->ball->getAngleYDeg());
    ball_data.get()->set_bearing(vision->ball->getBearing());
    ball_data.get()->set_bearing_deg(vision->ball->getBearingDeg());
    ball_data.get()->set_elevation_deg(vision->ball->getElevationDeg());
    ball_data.get()->set_distance_sd(vision->ball->getDistanceSD());
    ball_data.get()->set_bearing_sd(vision->ball->getBearingSD());
    ball_data.get()->set_radius(vision->ball->getRadius());
    ball_data.get()->set_frames_on(vision->ball->getFramesOn());
    ball_data.get()->set_frames_off(vision->ball->getFramesOff());
    ball_data.get()->set_heat(vision->ball->getHeat());
    ball_data.get()->set_on(vision->ball->isOn());
    ball_data.get()->set_confidence(vision->ball->getConfidence());
	ball_data.get()->set_x(vision->ball->getX());
	ball_data.get()->set_y(vision->ball->getY());

    vision_ball.setMessage(ball_data);
}

void VisionModule::updateVisionRobot() {

    portals::Message<messages::VisionRobot> robot_data(0);

    messages::Robot* red1 = robot_data.get()->mutable_red1();
    updateRobot(red1, vision->red1);
    messages::Robot* red2 = robot_data.get()->mutable_red2();
    updateRobot(red2, vision->red2);
    messages::Robot* red3 = robot_data.get()->mutable_red3();
    updateRobot(red3, vision->red3);

    messages::Robot* navy1 = robot_data.get()->mutable_navy1();
    updateRobot(navy1, vision->navy1);
    messages::Robot* navy2 = robot_data.get()->mutable_navy2();
    updateRobot(navy2, vision->navy2);
    messages::Robot* navy3 = robot_data.get()->mutable_navy3();

    updateRobot(navy3, vision->navy3);

    vision_robot.setMessage(robot_data);

}

void updateRobot(messages::Robot* bot_, VisualRobot* visualRobot) {

    bot_->set_distance(visualRobot->getDistance());
    bot_->set_bearing(visualRobot->getBearing());
    bot_->set_bearing_deg(visualRobot->getBearingDeg());
    bot_->set_angle_x_deg(visualRobot->getAngleXDeg());
    bot_->set_angle_y_deg(visualRobot->getAngleYDeg());
    bot_->set_x(visualRobot->getX());
    bot_->set_y(visualRobot->getY());
    bot_->set_elevation_deg(visualRobot->getElevationDeg());
    bot_->set_on(visualRobot->isOn());

}


void VisionModule::updateVisionField() {

    portals::Message<messages::VisionField> field_data(0);

    // setting lines info
    const std::vector<boost::shared_ptr<VisualLine> >* visualLines = vision->fieldLines->getLines();
    for(std::vector<boost::shared_ptr<VisualLine> >::const_iterator i = visualLines->begin();
        i != visualLines->end(); i++)
    {
        messages::VisualLine *visLine = field_data.get()->add_visual_line();
        visLine->mutable_visual_detection()->set_distance(i->get()->getDistance());
        visLine->mutable_visual_detection()->set_bearing(i->get()->getBearing());
        visLine->mutable_visual_detection()->set_distance_sd(i->get()->getDistanceSD());
        visLine->mutable_visual_detection()->set_bearing_sd(i->get()->getBearingSD());
        //we wont set concrete coords for the lines, since they are lines
        visLine->set_start_x(i->get()->getStartpoint().x);
        visLine->set_start_y(i->get()->getStartpoint().y);
        visLine->set_end_x(i->get()->getEndpoint().x);
        visLine->set_end_y(i->get()->getEndpoint().y);
        visLine->set_angle(i->get()->getAngle());
        visLine->set_avg_width(i->get()->getAvgWidth());
        visLine->set_length(i->get()->getLength());
        visLine->set_slope(i->get()->getSlope());
        visLine->set_y_int(i->get()->getYIntercept());
        const std::vector<lineID> id_for_line = i->get()->getIDs();
        for (unsigned int k = 0; k < id_for_line.size(); k++) {
            visLine->add_possibilities(id_for_line[k]);
        }

    }
    //end lines info

    //setting the corner info
    std::list<VisualCorner>* visualCorners = vision->fieldLines->getCorners();
    for(std::list<VisualCorner>::iterator i = visualCorners->begin();
        i != visualCorners->end(); i++)
    {
        messages::VisualCorner *visCorner = field_data.get()->add_visual_corner();
        visCorner->set_orientation(i->getOrientation());
        visCorner->set_corner_type(i->getShape());
        visCorner->set_physical_orientation(i->getPhysicalOrientation());
        visCorner->mutable_visual_detection()->set_distance(i->getDistance());
        visCorner->mutable_visual_detection()->set_bearing(i->getBearing());
        visCorner->mutable_visual_detection()->set_distance_sd(i->getDistanceSD());
        visCorner->mutable_visual_detection()->set_bearing_sd(i->getBearingSD());

        const std::list<const ConcreteCorner *>* possible = i->getPossibilities();
        for(std::list<const ConcreteCorner*>::const_iterator j = possible->begin();
            j != possible->end(); j++)
        {
            messages::Point *field_point =
                visCorner->mutable_visual_detection()->add_concrete_coords();

            field_point->set_x((**j).getFieldX());
            field_point->set_y((**j).getFieldY());
        }

        const std::vector<cornerID> p_id = i->getIDs();
        for (unsigned int k = 0; k < p_id.size(); k++)
        {
            visCorner->add_poss_id(p_id[k]);
        }


    }
    //end corner info

    //setting goalpostleft info
    field_data.get()->mutable_goal_post_l()->set_height(vision->yglp->getHeight());
    field_data.get()->mutable_goal_post_l()->set_width(vision->yglp->getWidth());
    field_data.get()->mutable_goal_post_l()->mutable_visual_detection()->
        set_distance(vision->yglp->getDistance());
    field_data.get()->mutable_goal_post_l()->mutable_visual_detection()->
        set_bearing(vision->yglp->getBearing());
    field_data.get()->mutable_goal_post_l()->mutable_visual_detection()->
        set_bearing_deg(vision->yglp->getBearingDeg());
    field_data.get()->mutable_goal_post_l()->mutable_visual_detection()->
        set_distance_sd(vision->yglp->getDistanceSD());
    field_data.get()->mutable_goal_post_l()->mutable_visual_detection()->
        set_bearing_sd(vision->yglp->getBearingSD());
    field_data.get()->mutable_goal_post_l()->mutable_visual_detection()->
        set_on(vision->yglp->isOn());
    field_data.get()->mutable_goal_post_l()->mutable_visual_detection()->
        set_frames_on(vision->yglp->getFramesOn());
    field_data.get()->mutable_goal_post_l()->mutable_visual_detection()->
        set_frames_off(vision->yglp->getFramesOff());
    field_data.get()->mutable_goal_post_l()->mutable_visual_detection()->
        set_certainty(vision->yglp->getIDCertainty());
    field_data.get()->mutable_goal_post_l()->mutable_visual_detection()->
        set_angle_x_deg(vision->yglp->getAngleXDeg());
    field_data.get()->mutable_goal_post_l()->mutable_visual_detection()->
        set_angle_y_deg(vision->yglp->getAngleYDeg());
    field_data.get()->mutable_goal_post_l()->mutable_visual_detection()->
        set_red_goalie(vision->yglp->getRedGoalieCertain());
    field_data.get()->mutable_goal_post_l()->mutable_visual_detection()->
        set_navy_goalie(vision->yglp->getNavyGoalieCertain());

    const std::list<const ConcreteFieldObject *>* possible_l = vision->yglp->getPossibilities();
    for(std::list<const ConcreteFieldObject*>::const_iterator i = possible_l->begin();
        i != possible_l->end(); i++)
    {
        messages::Point *field_point =
            field_data.get()->mutable_goal_post_l()->mutable_visual_detection()->
            add_concrete_coords();

        field_point->set_x((**i).getFieldX());
        field_point->set_y((**i).getFieldY());
    }
    //end goalpostleft info

    //setting goalpostright info
    field_data.get()->mutable_goal_post_r()->set_height(vision->ygrp->getHeight());
    field_data.get()->mutable_goal_post_r()->set_width(vision->ygrp->getWidth());
    field_data.get()->mutable_goal_post_r()->mutable_visual_detection()->
        set_distance(vision->ygrp->getDistance());
    field_data.get()->mutable_goal_post_r()->mutable_visual_detection()->
        set_bearing(vision->ygrp->getBearing());
    field_data.get()->mutable_goal_post_r()->mutable_visual_detection()->
        set_bearing_deg(vision->ygrp->getBearingDeg());
    field_data.get()->mutable_goal_post_r()->mutable_visual_detection()->
        set_distance_sd(vision->ygrp->getDistanceSD());
    field_data.get()->mutable_goal_post_r()->mutable_visual_detection()->
        set_bearing_sd(vision->ygrp->getBearingSD());
    field_data.get()->mutable_goal_post_r()->mutable_visual_detection()->
        set_on(vision->ygrp->isOn());
    field_data.get()->mutable_goal_post_r()->mutable_visual_detection()->
        set_frames_on(vision->ygrp->getFramesOn());
    field_data.get()->mutable_goal_post_r()->mutable_visual_detection()->
        set_frames_off(vision->ygrp->getFramesOff());
    field_data.get()->mutable_goal_post_r()->mutable_visual_detection()->
        set_certainty(vision->ygrp->getIDCertainty());
    field_data.get()->mutable_goal_post_r()->mutable_visual_detection()->
        set_red_goalie(vision->ygrp->getRedGoalieCertain());
    field_data.get()->mutable_goal_post_r()->mutable_visual_detection()->
        set_navy_goalie(vision->ygrp->getNavyGoalieCertain());

    const std::list<const ConcreteFieldObject *>* possible_r = vision->ygrp->getPossibilities();
    for(std::list<const ConcreteFieldObject*>::const_iterator i = possible_r->begin();
        i != possible_r->end(); i++)
    {
        messages::Point *field_point =
            field_data.get()->mutable_goal_post_r()->mutable_visual_detection()->
            add_concrete_coords();

        field_point->set_x((**i).getFieldX());
        field_point->set_y((**i).getFieldY());
    }
    //end goalpostright info

    //setting fieldedge data
    field_data.get()->mutable_visual_field_edge()->set_distance_l(vision->fieldEdge->
                                                                  getDistanceLeft());
    field_data.get()->mutable_visual_field_edge()->set_distance_m(vision->fieldEdge->
                                                                  getDistanceCenter());
    field_data.get()->mutable_visual_field_edge()->set_distance_r(vision->fieldEdge->
                                                                  getDistanceRight());
    //end fieldedge data


    //setting cross info
    field_data.get()->mutable_visual_cross()->set_distance(vision->cross->getDistance());
    field_data.get()->mutable_visual_cross()->set_bearing(vision->cross->getBearing());
    field_data.get()->mutable_visual_cross()->set_distance_sd(vision->cross->getDistanceSD());
    field_data.get()->mutable_visual_cross()->set_bearing_sd(vision->cross->getBearingSD());

    const std::list<const ConcreteCross *>* possible_cross = vision->cross->getPossibilities();
    for (std::list<const ConcreteCross*>::const_iterator i = possible_cross->begin();
         i != possible_cross->end(); i++)
    {
        messages::Point *field_point =
            field_data.get()->mutable_visual_cross()->add_concrete_coords();

        field_point->set_x((**i).getFieldX());
        field_point->set_y((**i).getFieldY());
    }

    vision_field.setMessage(field_data);
}

}
}
