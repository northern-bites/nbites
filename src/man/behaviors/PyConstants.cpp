// Wrapper for PyNogginConstants.h. Import the noggin_constants module from
// python.

#include "PyConstants.h"

#include <boost/python.hpp>

#include <iostream>
#include <fstream>
#include <stdio.h>

#include "DebugConfig.h"

using nblog::SExpr;

using namespace boost::python;
using namespace man::behaviors;
using namespace man;

BOOST_PYTHON_MODULE(noggin_constants)
{
    // Lab Field or Full Field?
#ifdef USE_LAB_FIELD
    scope().attr("USE_LAB_FIELD") = true;
#else
    scope().attr("USE_LAB_FIELD") = false;
#endif

#ifdef V5_ROBOT
    scope().attr("V5_ROBOT") = true;
#else
    scope().attr("V5_ROBOT") = false;
#endif

    // Walk time
    scope().attr("TIME_STEP") = TIME_STEP;
    scope().attr("TIME_PER_STEP") = TIME_PER_STEP;

    // Team stuff
    scope().attr("NUM_PLAYERS_PER_TEAM") = NUM_PLAYERS_PER_TEAM;
    scope().attr("LENGTH_OF_HALF") = LENGTH_OF_HALF;

    scope().attr("NUM_GAME_TEAM_COLORS") = NUM_GAME_TEAM_COLORS;

    enum_<teamColor>("teamColor")
        .value("TEAM_BLUE", PY_TEAM_BLUE)
        .value("TEAM_RED", PY_TEAM_RED)
        ;

    // Vision connection
    scope().attr("CAMERA_FPS") = CAMERA_FPS;
    scope().attr("IMAGE_WIDTH") = IMAGE_WIDTH;
    scope().attr("IMAGE_HEIGHT") = IMAGE_HEIGHT;

    scope().attr("IMAGE_CENTER_X") = IMAGE_CENTER_X;
    scope().attr("FOV_X_DEG") = FOV_X_DEG;
    scope().attr("FOV_Y_DEG") = FOV_Y_DEG;
    scope().attr("IMAGE_ANGLE_X") = IMAGE_ANGLE_X;
    scope().attr("IMAGE_ANGLE_Y") = IMAGE_ANGLE_Y;

    // Ball
    scope().attr("NUM_TOTAL_BALL_VALUES") = NUM_TOTAL_BALL_VALUES;

    // Localization
    scope().attr("FIELD_WHITE_WIDTH") = FIELD_WHITE_WIDTH;
    scope().attr("FIELD_WHITE_HEIGHT") = FIELD_WHITE_HEIGHT;
    scope().attr("GREEN_PAD_X") = GREEN_PAD_X;
    scope().attr("GREEN_PAD_Y") = GREEN_PAD_Y;

    scope().attr("FIELD_GREEN_WIDTH") = FIELD_GREEN_WIDTH;
    scope().attr("FIELD_GREEN_HEIGHT") = FIELD_GREEN_HEIGHT;

    scope().attr("FIELD_WIDTH") = FIELD_WIDTH;
    scope().attr("FIELD_HEIGHT") = FIELD_HEIGHT;

    scope().attr("CENTER_FIELD_X") = CENTER_FIELD_X;
    scope().attr("CENTER_FIELD_Y") = CENTER_FIELD_Y;

    scope().attr("FIELD_GREEN_LEFT_SIDELINE_X") =
        FIELD_GREEN_LEFT_SIDELINE_X;
    scope().attr("FIELD_GREEN_RIGHT_SIDELINE_X") =
        FIELD_GREEN_RIGHT_SIDELINE_X;
    scope().attr("FIELD_GREEN_BOTTOM_SIDELINE_Y") =
        FIELD_GREEN_BOTTOM_SIDELINE_Y;
    scope().attr("FIELD_GREEN_TOP_SIDELINE_Y") =
        FIELD_GREEN_TOP_SIDELINE_Y;

    scope().attr("FIELD_WHITE_LEFT_SIDELINE_X") =
        FIELD_WHITE_LEFT_SIDELINE_X;
    scope().attr("FIELD_WHITE_RIGHT_SIDELINE_X") =
        FIELD_WHITE_RIGHT_SIDELINE_X;
    scope().attr("FIELD_WHITE_BOTTOM_SIDELINE_Y") =
        FIELD_WHITE_BOTTOM_SIDELINE_Y;
    scope().attr("FIELD_WHITE_TOP_SIDELINE_Y") =
        FIELD_WHITE_TOP_SIDELINE_Y;

    scope().attr("OPP_GOAL_HEADING") = OPP_GOAL_HEADING;
    scope().attr("MY_GOAL_HEADING") = MY_GOAL_HEADING;

    scope().attr("GOAL_POST_CM_HEIGHT") = GOAL_POST_CM_HEIGHT;
    scope().attr("GOAL_POST_CM_WIDTH") = GOAL_POST_CM_WIDTH;
    scope().attr("CROSSBAR_CM_WIDTH") = CROSSBAR_CM_WIDTH;
    scope().attr("CROSSBAR_CM_HEIGHT") = CROSSBAR_CM_HEIGHT;
    scope().attr("GOAL_POST_RADIUS") = GOAL_POST_RADIUS;

    scope().attr("LANDMARK_MY_GOAL_LEFT_POST_X") =
        LANDMARK_MY_GOAL_LEFT_POST_X;
    scope().attr("LANDMARK_MY_GOAL_RIGHT_POST_X") =
        LANDMARK_MY_GOAL_RIGHT_POST_X;
    scope().attr("LANDMARK_OPP_GOAL_LEFT_POST_X") =
        LANDMARK_OPP_GOAL_LEFT_POST_X;
    scope().attr("LANDMARK_OPP_GOAL_RIGHT_POST_X") =
        LANDMARK_OPP_GOAL_RIGHT_POST_X;

    scope().attr("LANDMARK_MY_GOAL_LEFT_POST_Y") =
        LANDMARK_MY_GOAL_LEFT_POST_Y;
    scope().attr("LANDMARK_MY_GOAL_RIGHT_POST_Y") =
        LANDMARK_MY_GOAL_RIGHT_POST_Y;
    scope().attr("LANDMARK_OPP_GOAL_LEFT_POST_Y") =
        LANDMARK_OPP_GOAL_LEFT_POST_Y;
    scope().attr("LANDMARK_OPP_GOAL_RIGHT_POST_Y") =
        LANDMARK_OPP_GOAL_RIGHT_POST_Y;

    scope().attr("GOAL_WIDTH") = GOAL_WIDTH;

    scope().attr("CENTER_CIRCLE_RADIUS") = CENTER_CIRCLE_RADIUS;

    scope().attr("GOALBOX_DEPTH") = GOALBOX_DEPTH;
    scope().attr("GOALBOX_WIDTH") = GOALBOX_WIDTH;

    scope().attr("MIDFIELD_X") = MIDFIELD_X;
    scope().attr("MIDFIELD_Y") = MIDFIELD_Y;

    scope().attr("OUTSIDE_GOALBOX_Y") = OUTSIDE_GOALBOX_Y;

    scope().attr("MY_GOALBOX_LEFT_X") = MY_GOALBOX_LEFT_X;
    scope().attr("MY_GOALBOX_RIGHT_X") = MY_GOALBOX_RIGHT_X;
    scope().attr("MY_GOALBOX_BOTTOM_Y") = MY_GOALBOX_BOTTOM_Y;
    scope().attr("MY_GOALBOX_TOP_Y") = MY_GOALBOX_TOP_Y;
    scope().attr("MY_GOALBOX_MIDDLE_Y") = MY_GOALBOX_MIDDLE_Y;

    scope().attr("OPP_GOALBOX_LEFT_X") = OPP_GOALBOX_LEFT_X;
    scope().attr("OPP_GOALBOX_RIGHT_X") = OPP_GOALBOX_RIGHT_X;
    scope().attr("OPP_GOALBOX_BOTTOM_Y") = OPP_GOALBOX_BOTTOM_Y;
    scope().attr("OPP_GOALBOX_TOP_Y") = OPP_GOALBOX_TOP_Y;
    scope().attr("OPP_GOALBOX_MIDDLE_Y") = OPP_GOALBOX_MIDDLE_Y;
    scope().attr("OPP_GOAL_MIDPOINT") = OPP_GOAL_MIDPOINT;

    scope().attr("LINE_CROSS_OFFSET") = LINE_CROSS_OFFSET;

    scope().attr("NUM_LANDMARKS") = NUM_LANDMARKS;

    enum_<landmarkID>("landmarkID")
        .value("LANDMARK_MY_GOAL_LEFT_POST_ID",
               LANDMARK_MY_GOAL_LEFT_POST_ID)
        .value("LANDMARK_MY_GOAL_RIGHT_POST_ID",
               LANDMARK_MY_GOAL_RIGHT_POST_ID)
        .value("LANDMARK_OPP_GOAL_LEFT_POST_ID",
               LANDMARK_OPP_GOAL_LEFT_POST_ID)
        .value("LANDMARK_OPP_GOAL_RIGHT_POST_ID",
               LANDMARK_OPP_GOAL_RIGHT_POST_ID)
        .value("LANDMARK_BALL_ID",
               LANDMARK_BALL_ID)
        .value("LANDMARK_MY_CORNER_LEFT_L_ID",
               LANDMARK_MY_CORNER_LEFT_L_ID)
        .value("LANDMARK_MY_CORNER_RIGHT_L_ID",
               LANDMARK_MY_CORNER_RIGHT_L_ID)
        .value("LANDMARK_MY_GOAL_LEFT_T_ID",
               LANDMARK_MY_GOAL_LEFT_T_ID)
        .value("LANDMARK_MY_GOAL_RIGHT_T_ID",
               LANDMARK_MY_GOAL_RIGHT_T_ID)
        .value("LANDMARK_MY_GOAL_LEFT_L_ID",
               LANDMARK_MY_GOAL_LEFT_L_ID)
        .value("LANDMARK_MY_GOAL_RIGHT_L_ID",
               LANDMARK_MY_GOAL_RIGHT_L_ID)
        .value("LANDMARK_CENTER_LEFT_T_ID",
               LANDMARK_CENTER_LEFT_T_ID)
        .value("LANDMARK_CENTER_RIGHT_T_ID",
               LANDMARK_CENTER_RIGHT_T_ID)
        .value("LANDMARK_OPP_CORNER_LEFT_L_ID",
               LANDMARK_OPP_CORNER_LEFT_L_ID)
        .value("LANDMARK_OPP_CORNER_RIGHT_L_ID",
               LANDMARK_OPP_CORNER_RIGHT_L_ID)
        .value("LANDMARK_OPP_GOAL_LEFT_T_ID",
               LANDMARK_OPP_GOAL_LEFT_T_ID)
        .value("LANDMARK_OPP_GOAL_RIGHT_T_ID",
               LANDMARK_OPP_GOAL_RIGHT_T_ID)
        .value("LANDMARK_OPP_GOAL_LEFT_L_ID",
               LANDMARK_OPP_GOAL_LEFT_L_ID)
        .value("LANDMARK_OPP_GOAL_RIGHT_L_ID",
               LANDMARK_OPP_GOAL_RIGHT_L_ID)
        ;

    enum_<man::vision::LineID>("LineID")
        .value("Line", man::vision::LineID::Line)
        .value("EndlineOrSideline", man::vision::LineID::EndlineOrSideline)
        .value("TopGoalboxOrSideGoalbox", man::vision::LineID::TopGoalboxOrSideGoalbox)
        .value("SideGoalboxOrMidline", man::vision::LineID::SideGoalboxOrMidline)
        .value("Sideline", man::vision::LineID::Sideline)
        .value("SideGoalbox", man::vision::LineID::SideGoalbox)
        .value("Endline", man::vision::LineID::Endline)
        .value("TopGoalbox", man::vision::LineID::TopGoalbox)
        .value("Midline", man::vision::LineID::Midline);

    scope().attr("LANDMARK_MY_GOAL_LEFT_POST") = make_tuple(
        LANDMARK_MY_GOAL_LEFT_POST[0],
        LANDMARK_MY_GOAL_LEFT_POST[1],
        LANDMARK_MY_GOAL_LEFT_POST[2]);

    scope().attr("LANDMARK_MY_GOAL_RIGHT_POST") = make_tuple(
        LANDMARK_MY_GOAL_RIGHT_POST[0],
        LANDMARK_MY_GOAL_RIGHT_POST[1],
        LANDMARK_MY_GOAL_RIGHT_POST[2]);

    scope().attr("LANDMARK_OPP_GOAL_LEFT_POST") = make_tuple(
        LANDMARK_OPP_GOAL_LEFT_POST[0],
        LANDMARK_OPP_GOAL_LEFT_POST[1],
        LANDMARK_OPP_GOAL_LEFT_POST[2]);

    scope().attr("LANDMARK_OPP_GOAL_RIGHT_POST") = make_tuple(
        LANDMARK_OPP_GOAL_RIGHT_POST[0],
        LANDMARK_OPP_GOAL_RIGHT_POST[1],
        LANDMARK_OPP_GOAL_RIGHT_POST[2]);

    scope().attr("NUM_VIS_LANDMARKS") = NUM_VIS_LANDMARKS;

    enum_<vis_landmark>("vis_landmark")
        .value("VISION_YGLP", VISION_YGLP)
        .value("VISION_YGRP", VISION_YGRP)
        .value("VISION_BGLP", VISION_BGLP)
        .value("VISION_BGRP", VISION_BGRP)
        .value("VISION_BG_CROSSBAR", VISION_BG_CROSSBAR)
        .value("VISION_YG_CROSSBAR", VISION_YG_CROSSBAR)
        ;

    scope().attr("LANDMARK_OPP_FIELD_CROSS") = make_tuple(
        LANDMARK_OPP_FIELD_CROSS[0],
        LANDMARK_OPP_FIELD_CROSS[1]);

    scope().attr("LANDMARK_MY_FIELD_CROSS") = make_tuple(
        LANDMARK_MY_FIELD_CROSS[0],
        LANDMARK_MY_FIELD_CROSS[1]);

    scope().attr("BLUE_CC_NEAREST_POINT_X") = BLUE_CC_NEAREST_POINT_X;
    scope().attr("BLUE_CROSS_CIRCLE_MIDPOINT_X") = BLUE_CROSS_CIRCLE_MIDPOINT_X;
    scope().attr("BLUE_GOALBOX_MIDPOINT_X") = BLUE_GOALBOX_MIDPOINT_X;
    scope().attr("BLUE_GOALBOX_CROSS_MIDPOINT_X") = BLUE_GOALBOX_CROSS_MIDPOINT_X;

    scope().attr("NUM_LOC_SCORES") = NUM_LOC_SCORES;

    enum_<locScore>("locScore")
        .value("BAD_LOC", BAD_LOC)
        .value("OK_LOC", OK_LOC)
        .value("GOOD_LOC", GOOD_LOC)
        ;

    scope().attr("GOOD_LOC_XY_UNCERT_THRESH") = GOOD_LOC_XY_UNCERT_THRESH;
    scope().attr("GOOD_LOC_THETA_UNCERT_THRESH") =
        GOOD_LOC_THETA_UNCERT_THRESH;

    scope().attr("OK_LOC_XY_UNCERT_THRESH") = OK_LOC_XY_UNCERT_THRESH;
    scope().attr("OK_LOC_THETA_UNCERT_THRESH") = OK_LOC_THETA_UNCERT_THRESH;

    scope().attr("BAD_LOC_XY_UNCERT_THRESH") = BAD_LOC_XY_UNCERT_THRESH;
    scope().attr("BAD_LOC_THETA_UNCERT_THRESH") = BAD_LOC_THETA_UNCERT_THRESH;

    scope().attr("BLUE_GOALBOX_RIGHT_X") = BLUE_GOALBOX_RIGHT_X;
	scope().attr("YELLOW_GOALBOX_LEFT_X") = YELLOW_GOALBOX_LEFT_X;
    scope().attr("BLUE_GOALBOX_TOP_Y") = BLUE_GOALBOX_TOP_Y;
    scope().attr("BLUE_GOALBOX_BOTTOM_Y") = BLUE_GOALBOX_BOTTOM_Y;
    scope().attr("LANDMARK_BLUE_GOAL_CROSS_X") = LANDMARK_BLUE_GOAL_CROSS_X;
	scope().attr("LANDMARK_YELLOW_GOAL_CROSS_X") = LANDMARK_YELLOW_GOAL_CROSS_X;
    scope().attr("LANDMARK_BLUE_GOAL_TOP_POST_Y") = LANDMARK_BLUE_GOAL_TOP_POST_Y;
    scope().attr("LANDMARK_BLUE_GOAL_BOTTOM_POST_Y") = LANDMARK_BLUE_GOAL_BOTTOM_POST_Y;
    scope().attr("HEADING_UP") = HEADING_UP;
    scope().attr("HEADING_DOWN") = HEADING_DOWN;
    scope().attr("HEADING_RIGHT") = HEADING_RIGHT;
    scope().attr("HEADING_LEFT") = HEADING_LEFT;

	// Messages
	scope().attr("GAME_STATE_IN") = GAME_STATE_IN;
	scope().attr("FILTERED_BALL_IN") = FILTERED_BALL_IN;
	scope().attr("WORLD_MODEL_IN") = WORLD_MODEL_IN;

    // Robots
    scope().attr("NUM_POSSIBLE_ROBOTS") = NUM_POSSIBLE_ROBOTS;

    enum_<robotID>("robotID")
        .value("RED_1", RED_1)
        .value("RED_2", RED_2)
        .value("RED_3", RED_3)
        .value("BLUE_1", BLUE_1)
        .value("BLUE_2", BLUE_2)
        .value("BLUE_3", BLUE_3)
        ;

    get_config_params();
}

void get_config_params() {
    std::string filepath = "/home/nao/nbites/Config/behaviorParams.txt";

    if(FILE *file = fopen(filepath.c_str(),"r")) {
        fclose(file);
        std::ifstream inputFile(filepath);
        std::string readInFile((std::istreambuf_iterator<char>(inputFile)),
                                std::istreambuf_iterator<char>());
        int i=0;
        SExpr params = *SExpr::read(readInFile,i);

        // should defenders have a fixed home or dynamic
        scope().attr("FIXED_D_HOME") = get_defensive_strategy(params);
        // do we want defenders to play up or back
        scope().attr("LEFT_FORWARD_DEFENSE") = get_left_defender_home(params);
        scope().attr("RIGHT_FORWARD_DEFENSE") = get_right_defender_home(params);
        // max speed when chasing the ball
        scope().attr("MAX_SPEED") = get_max_speed(params);
        // min speed when chasing the ball
        scope().attr("MIN_SPEED") = get_min_speed(params);


    } else {
        std::cout<<"[ERR] BehaviorParams File Does Not Exist"<<std::endl;

        // default values
        scope().attr("FIXED_D_HOME") = true;
        // do we want defenders to play up or back
        scope().attr("LEFT_FORWARD_DEFENSE") = false;
        scope().attr("RIGHT_FORWARD_DEFENSE") = false;
        // max speed when chasing the ball
        scope().attr("MAX_SPEED") = 0.8f;
        // min speed when chasing the ball
        scope().attr("MIN_SPEED") = 0.5f;
    }
}

bool get_defensive_strategy(SExpr params) {
    SExpr *dStrat = params.find("fixed_defender_home")->get(1);
    bool fixedD = true;
    
    if (dStrat) { fixedD = dStrat->valueAsInt()==1 ? true : false; }
    else { std::cout << "[WARN] Invalid defensive strategy. Default to fixed home positions." << std::endl; }
    
    return fixedD;
}

bool get_left_defender_home(SExpr params) {
    SExpr *dPos = params.find("left_forward_defense")->get(1);
    bool forwardD = false;
    
    if (dPos) { forwardD = dPos->valueAsInt()==1 ? true : false; }
    else { std::cout << "[WARN] Invalid defense home. Default to back positions." << std::endl; }
    
    return forwardD;
}

bool get_right_defender_home(SExpr params) {
    SExpr *dPos = params.find("right_forward_defense")->get(1);
    bool forwardD = false;
    
    if (dPos) { forwardD = dPos->valueAsInt()==1 ? true : false; }
    else { std::cout << "[WARN] Invalid defense home. Default to back positions." << std::endl; }
    
    return forwardD;
}

float get_max_speed(SExpr params) {
    SExpr *maxSpeed = params.find("max_speed")->get(1);
    
    if (maxSpeed) { 
        std::string speedString = maxSpeed->value();
        if (speedString.size() >= 3) {
            if (speedString.substr(0,3) == "1.0") { return 1.0f; }
            if (speedString.substr(0,2) == "0." && isdigit(speedString[2])) {
                int iSpeed = speedString[2] - '0';
                return iSpeed / 10.f;
            }
        }      
    }
    
    std::cout << "[WARN] Invalid max speed. Default to 0.8" << std::endl;
    return 0.8f;
}

float get_min_speed(SExpr params) {
    SExpr *minSpeed = params.find("min_speed")->get(1);
    
    if (minSpeed) { 
        std::string speedString = minSpeed->value();
        if (speedString.size() >= 3) {
            if (speedString.substr(0,3) == "1.0") { return 1.0f; }
            if (speedString.substr(0,2) == "0." && isdigit(speedString[2])) {
                int iSpeed = speedString[2] - '0';
                return iSpeed / 10.f;
            }
        }      
    }
    
    std::cout << "[WARN] Invalid min speed. Default to 0.5" << std::endl;
    return 0.5f;
}

void c_init_noggin_constants() {
    if (!Py_IsInitialized())
        Py_Initialize();
    try{
        initnoggin_constants();
    } catch (error_already_set) {
        PyErr_Print();
    }
}
