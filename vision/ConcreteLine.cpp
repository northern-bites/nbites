#include "ConcreteLine.h"
using namespace std;

ConcreteLine::ConcreteLine(const float _fieldX1, const float _fieldY1,
                           const float _fieldX2, const float _fieldY2,
                           const lineID _id) :
    fieldX1(_fieldX1), fieldY1(_fieldY1), fieldX2(_fieldX2), fieldY2(_fieldY2)
{
    id = _id;
}

ConcreteLine::ConcreteLine(const ConcreteLine& other) :
    fieldX1(other.fieldX1), fieldY1(other.fieldY1), fieldX2(other.fieldX2),
    fieldY2(other.fieldY2)
{
    id = other.getID();
}

ConcreteLine::~ConcreteLine() {} // Nothing to delete

///////////////////////////////////////////////////////////////////////////////
// Constants for absolute coordinates on the field of the specific corners.
// (0,0) is the lower left corner of the field when BLUE goal is at the
// LEFT SIDE of the field!
///////////////////////////////////////////////////////////////////////////////
// Create static instances of the class for every possible line

/**
 * ENDLINES:
 */
// Blue goal
const ConcreteLine ConcreteLine::
blue_goal_top_endline(FIELD_WHITE_LEFT_SIDELINE_X,
					  FIELD_WHITE_TOP_SIDELINE_Y,
					  FIELD_WHITE_LEFT_SIDELINE_X,
					  LANDMARK_BLUE_GOAL_TOP_POST_Y,
					  BLUE_GOAL_TOP_ENDLINE);

const ConcreteLine ConcreteLine::
blue_goal_mid_endline(FIELD_WHITE_LEFT_SIDELINE_X,
					  LANDMARK_BLUE_GOAL_TOP_POST_Y,
					  FIELD_WHITE_LEFT_SIDELINE_X,
					  LANDMARK_BLUE_GOAL_BOTTOM_POST_Y,
					  BLUE_GOAL_MID_ENDLINE);

const ConcreteLine ConcreteLine::
blue_goal_bottom_endline(FIELD_WHITE_LEFT_SIDELINE_X,
						 LANDMARK_BLUE_GOAL_BOTTOM_POST_Y,
						 FIELD_WHITE_LEFT_SIDELINE_X,
						 FIELD_WHITE_BOTTOM_SIDELINE_Y,
						 BLUE_GOAL_BOTTOM_ENDLINE);
const ConcreteLine ConcreteLine::
yellow_goal_top_endline(FIELD_WHITE_LEFT_SIDELINE_X,
						FIELD_WHITE_TOP_SIDELINE_Y,
						FIELD_WHITE_LEFT_SIDELINE_X,
						LANDMARK_YELLOW_GOAL_TOP_POST_Y,
						YELLOW_GOAL_TOP_ENDLINE);

const ConcreteLine ConcreteLine::
yellow_goal_mid_endline(FIELD_WHITE_LEFT_SIDELINE_X,
						LANDMARK_YELLOW_GOAL_TOP_POST_Y,
						FIELD_WHITE_LEFT_SIDELINE_X,
						LANDMARK_YELLOW_GOAL_BOTTOM_POST_Y,
						YELLOW_GOAL_MID_ENDLINE);

const ConcreteLine ConcreteLine::
yellow_goal_bottom_endline(FIELD_WHITE_LEFT_SIDELINE_X,
						   LANDMARK_YELLOW_GOAL_BOTTOM_POST_Y,
						   FIELD_WHITE_LEFT_SIDELINE_X,
						   FIELD_WHITE_BOTTOM_SIDELINE_Y,
						   YELLOW_GOAL_BOTTOM_ENDLINE);

const ConcreteLine ConcreteLine::
blue_goal_top_sideline(FIELD_WHITE_LEFT_SIDELINE_X,
					   FIELD_WHITE_TOP_SIDELINE_Y,
					   CENTER_FIELD_X,
					   FIELD_WHITE_TOP_SIDELINE_Y,
					   BLUE_GOAL_TOP_SIDELINE);

const ConcreteLine ConcreteLine::
yellow_goal_top_sideline(CENTER_FIELD_X,
						 FIELD_WHITE_TOP_SIDELINE_Y,
						 FIELD_WHITE_RIGHT_SIDELINE_X,
						 FIELD_WHITE_TOP_SIDELINE_Y,
						 YELLOW_GOAL_TOP_SIDELINE);

const ConcreteLine ConcreteLine::
blue_goal_bottom_sideline(FIELD_WHITE_LEFT_SIDELINE_X,
						  FIELD_WHITE_BOTTOM_SIDELINE_Y,
						  CENTER_FIELD_X,
						  FIELD_WHITE_BOTTOM_SIDELINE_Y,
						  BLUE_GOAL_BOTTOM_SIDELINE);

const ConcreteLine ConcreteLine::
yellow_goal_bottom_sideline(CENTER_FIELD_X,
							FIELD_WHITE_BOTTOM_SIDELINE_Y,
							FIELD_WHITE_RIGHT_SIDELINE_X,
							FIELD_WHITE_BOTTOM_SIDELINE_Y,
							YELLOW_GOAL_BOTTOM_SIDELINE);


const ConcreteLine ConcreteLine::
top_midline(CENTER_FIELD_X,
			FIELD_WHITE_TOP_SIDELINE_Y,
			CENTER_FIELD_X,
			TOP_CC_Y,
			TOP_MIDLINE);

const ConcreteLine ConcreteLine::
center_midline(CENTER_FIELD_X,
			   TOP_CC_Y,
			   CENTER_FIELD_X,
			   BOTTOM_CC_Y,
			   CENTER_MIDLINE);

const ConcreteLine ConcreteLine::
bottom_midline(CENTER_FIELD_X,
			   BOTTOM_CC_Y,
			   CENTER_FIELD_X,
			   FIELD_WHITE_BOTTOM_SIDELINE_Y,
			   BOTTOM_MIDLINE);


/**
 * Goalbox lines are labeled as if looking out from within the goal.
 * Top line is the "top" of the goalbox as the goalie sees it.
 */
const ConcreteLine ConcreteLine::
blue_goalbox_top_line(BLUE_GOALBOX_RIGHT_X,
                      BLUE_GOALBOX_TOP_Y,
                      BLUE_GOALBOX_RIGHT_X,
                      BLUE_GOALBOX_BOTTOM_Y,
                      BLUE_GOALBOX_TOP_LINE);

const ConcreteLine ConcreteLine::
blue_goalbox_left_line(BLUE_GOALBOX_LEFT_X,
					   BLUE_GOALBOX_TOP_Y,
                       BLUE_GOALBOX_RIGHT_X,
                       BLUE_GOALBOX_TOP_Y,
                       BLUE_GOALBOX_LEFT_LINE);

const ConcreteLine ConcreteLine::
blue_goalbox_right_line(BLUE_GOALBOX_LEFT_X,
                        BLUE_GOALBOX_BOTTOM_Y,
                        BLUE_GOALBOX_RIGHT_X,
                        BLUE_GOALBOX_BOTTOM_Y,
                        BLUE_GOALBOX_RIGHT_LINE);

const ConcreteLine ConcreteLine::
yellow_goalbox_top_line(YELLOW_GOALBOX_LEFT_X,
                        YELLOW_GOALBOX_TOP_Y,
                        YELLOW_GOALBOX_LEFT_X,
                        YELLOW_GOALBOX_BOTTOM_Y,
                        YELLOW_GOALBOX_TOP_LINE);

const ConcreteLine ConcreteLine::
yellow_goalbox_left_line(YELLOW_GOALBOX_LEFT_X,
                         YELLOW_GOALBOX_BOTTOM_Y,
                         YELLOW_GOALBOX_RIGHT_X,
                         YELLOW_GOALBOX_BOTTOM_Y,
                         YELLOW_GOALBOX_LEFT_LINE);

const ConcreteLine ConcreteLine::
yellow_goalbox_right_line(YELLOW_GOALBOX_LEFT_X,
                          YELLOW_GOALBOX_TOP_Y,
                          YELLOW_GOALBOX_RIGHT_X,
                          YELLOW_GOALBOX_TOP_Y,
                          YELLOW_GOALBOX_RIGHT_LINE);

const ConcreteLine* ConcreteLine::concreteLineList[NUM_LINES] =
{ &blue_goal_top_endline,
  &blue_goal_mid_endline,
  &blue_goal_bottom_endline,
  &yellow_goal_top_endline,
  &yellow_goal_mid_endline,
  &yellow_goal_bottom_endline,
  &blue_goal_top_sideline,
  &yellow_goal_top_sideline,
  &blue_goal_bottom_sideline,
  &yellow_goal_bottom_sideline,
  &top_midline,
  &center_midline,
  &bottom_midline,
  &blue_goalbox_top_line,
  &blue_goalbox_left_line,
  &blue_goalbox_right_line,
  &yellow_goalbox_top_line,
  &yellow_goalbox_left_line,
  &yellow_goalbox_right_line};

const ConcreteLine* ConcreteLine::goalboxLineList[NUM_GOALBOX_LINES] =
{ &blue_goalbox_top_line,
  &blue_goalbox_left_line,
  &blue_goalbox_right_line,
  &yellow_goalbox_top_line,
  &yellow_goalbox_left_line,
  &yellow_goalbox_right_line};

const ConcreteLine* ConcreteLine::sidelineList[NUM_SIDELINES] =
{ &blue_goal_top_sideline,
  &yellow_goal_top_sideline,
  &blue_goal_bottom_sideline,
  &yellow_goal_bottom_sideline};

const list <const ConcreteLine*> ConcreteLine::concreteLines =
    list <const ConcreteLine*>( ConcreteLine::concreteLineList,
                                &ConcreteLine::concreteLineList[NUM_LINES] );

const list <const ConcreteLine*> ConcreteLine::goalboxLines =
	list <const ConcreteLine*>(ConcreteLine::goalboxLineList,
							   &ConcreteLine::goalboxLineList[NUM_GOALBOX_LINES] );

const list <const ConcreteLine*> ConcreteLine::sidelines =
	list <const ConcreteLine*>(ConcreteLine::sidelineList,
							   &ConcreteLine::sidelineList[NUM_SIDELINES] );

const string ConcreteLine::toString() const
{

    switch (id) {
	case UNKNOWN_LINE:
		return "unknown line";
    case SIDE_OR_ENDLINE:
		return "side or endline";
    case SIDELINE_LINE:
		return "sideline line";
    case ENDLINE_LINE:
		return "endline line";
    case GOALBOX_LINE:
		return "goalbox line";
    case GOALBOX_SIDE_LINE:
		return "goalbox side line";
    case GOALBOX_TOP_LINE:
		return "goalbox top line";
    case BLUE_GOAL_TOP_ENDLINE:
		return "blue goal top endline";
    case BLUE_GOAL_MID_ENDLINE:
		return "blue goal mid endline";
    case BLUE_GOAL_BOTTOM_ENDLINE:
		return "blue goal bottom endline";
    case YELLOW_GOAL_TOP_ENDLINE:
		return "yellow goal top endline";
    case YELLOW_GOAL_MID_ENDLINE:
		return "yellow goal mid endline";
    case YELLOW_GOAL_BOTTOM_ENDLINE:
		return "yellow goal bottom endline";
    case BLUE_GOAL_TOP_SIDELINE:
		return "blue goal top sideline";
    case YELLOW_GOAL_TOP_SIDELINE:
		return "yellow goal top sideline";
    case BLUE_GOAL_BOTTOM_SIDELINE:
		return "blue goal bottom sideline";
    case YELLOW_GOAL_BOTTOM_SIDELINE:
		return "yellow goal bottom sideline";
    case TOP_MIDLINE:
		return "top midline";
    case CENTER_MIDLINE:
		return "center midline";
    case BOTTOM_MIDLINE:
		return "bottom midline";
    case BLUE_GOALBOX_TOP_LINE:
		return "blue goalbox top line";
    case BLUE_GOALBOX_LEFT_LINE:
		return "blue goalbox left line";
    case BLUE_GOALBOX_RIGHT_LINE:
		return "blue goalbox right line";
    case YELLOW_GOALBOX_TOP_LINE:
		return "yellow goalbox top line";
    case YELLOW_GOALBOX_LEFT_LINE:
		return "yellow goalbox left line";
    case YELLOW_GOALBOX_RIGHT_LINE:
		return "yellow goalbox right line";
	default:
        return "Invalid Line ID";
	};
}
