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
// bottom
///////////////////////////////////////////////////////////////////////////////
// Cretae static instances of the class for every possible line
const ConcreteLine ConcreteLine::
blue_goal_endline(FIELD_WHITE_LEFT_SIDELINE_X,
                  FIELD_WHITE_TOP_SIDELINE_Y,
                  FIELD_WHITE_RIGHT_SIDELINE_X,
                  FIELD_WHITE_TOP_SIDELINE_Y,
                  BLUE_GOAL_ENDLINE);

const ConcreteLine ConcreteLine::
yellow_goal_endline(FIELD_WHITE_LEFT_SIDELINE_X,
                    FIELD_WHITE_BOTTOM_SIDELINE_Y,
                    FIELD_WHITE_RIGHT_SIDELINE_X,
                    FIELD_WHITE_BOTTOM_SIDELINE_Y,
                    YELLOW_GOAL_ENDLINE);

const ConcreteLine ConcreteLine::
blue_yellow_sideline(FIELD_WHITE_LEFT_SIDELINE_X,
                     FIELD_WHITE_BOTTOM_SIDELINE_Y,
                     FIELD_WHITE_LEFT_SIDELINE_X,
                     FIELD_WHITE_TOP_SIDELINE_Y,
                     BLUE_YELLOW_SIDELINE);

const ConcreteLine ConcreteLine::
yellow_blue_sideline(FIELD_WHITE_RIGHT_SIDELINE_X,
                     FIELD_WHITE_BOTTOM_SIDELINE_Y,
                     FIELD_WHITE_RIGHT_SIDELINE_X,
                     FIELD_WHITE_TOP_SIDELINE_Y,
                     YELLOW_BLUE_SIDELINE);

const ConcreteLine ConcreteLine::
center_field_line(FIELD_WHITE_LEFT_SIDELINE_X,
                  CENTER_FIELD_Y,
                  FIELD_WHITE_RIGHT_SIDELINE_X,
                  CENTER_FIELD_Y,
                  CENTER_FIELD_LINE);

const ConcreteLine ConcreteLine::
blue_goalbox_top_line(BLUE_GOALBOX_LEFT_X,
                      BLUE_GOALBOX_TOP_Y,
                      BLUE_GOALBOX_RIGHT_X,
                      BLUE_GOALBOX_TOP_Y,
                      BLUE_GOALBOX_TOP_LINE);

const ConcreteLine ConcreteLine::
blue_goalbox_left_line(BLUE_GOALBOX_LEFT_X,
                       FIELD_WHITE_BOTTOM_SIDELINE_Y,
                       BLUE_GOALBOX_LEFT_X,
                       BLUE_GOALBOX_TOP_Y,
                       BLUE_GOALBOX_LEFT_LINE);

const ConcreteLine ConcreteLine::
blue_goalbox_right_line(BLUE_GOALBOX_RIGHT_X,
                        FIELD_WHITE_BOTTOM_SIDELINE_Y,
                        BLUE_GOALBOX_RIGHT_X,
                        BLUE_GOALBOX_TOP_Y,
                        BLUE_GOALBOX_RIGHT_LINE);

const ConcreteLine ConcreteLine::
yellow_goalbox_top_line(YELLOW_GOALBOX_LEFT_X,
                        YELLOW_GOALBOX_BOTTOM_Y,
                        YELLOW_GOALBOX_RIGHT_X,
                        YELLOW_GOALBOX_BOTTOM_Y,
                        YELLOW_GOALBOX_TOP_LINE);

const ConcreteLine ConcreteLine::
yellow_goalbox_left_line(YELLOW_GOALBOX_LEFT_X,
                         YELLOW_GOALBOX_BOTTOM_Y,
                         YELLOW_GOALBOX_LEFT_X,
                         FIELD_WHITE_TOP_SIDELINE_Y,
                         YELLOW_GOALBOX_LEFT_LINE);

const ConcreteLine ConcreteLine::
yellow_goalbox_right_line(YELLOW_GOALBOX_RIGHT_X,
                          YELLOW_GOALBOX_BOTTOM_Y,
                          YELLOW_GOALBOX_RIGHT_X,
                          FIELD_WHITE_TOP_SIDELINE_Y,
                          YELLOW_GOALBOX_RIGHT_LINE);

const ConcreteLine* ConcreteLine::concreteLineList[NUM_LINES] =
{&blue_goal_endline,
 &yellow_goal_endline,
 &blue_yellow_sideline,
 &yellow_blue_sideline,
 &center_field_line,
 &blue_goalbox_top_line,
 &blue_goalbox_left_line,
 &blue_goalbox_right_line,
 &yellow_goalbox_top_line,
 &yellow_goalbox_left_line,
 &yellow_goalbox_right_line
};

const list <const ConcreteLine*> ConcreteLine::concreteLines =
    list <const ConcreteLine*>( ConcreteLine::concreteLineList,
                                &ConcreteLine::concreteLineList[NUM_LINES] );

const string ConcreteLine::toString() const
{
    switch (id) {
    case BLUE_GOAL_ENDLINE:
        return "Blue Goal Sideline";
    case YELLOW_GOAL_ENDLINE:
        return "Yellow Goal Sideline";
    case BLUE_YELLOW_SIDELINE:
        return "Blue-Yellow Sideline";
    case YELLOW_BLUE_SIDELINE:
        return "Yellow-Blue Sideline";
    case CENTER_FIELD_LINE:
        return "Center Field Line";
    case BLUE_GOALBOX_TOP_LINE:
        return "Blue Goalbox Top Line";
    case BLUE_GOALBOX_LEFT_LINE:
        return "Blue Goalbox Left Line";
    case BLUE_GOALBOX_RIGHT_LINE:
        return "Blue Goalbox Right Line";
    case YELLOW_GOALBOX_TOP_LINE:
        return "Yellow Goalbox Top Line";
    case YELLOW_GOALBOX_LEFT_LINE:
        return "Yellow Goalbox Left Line";
    case YELLOW_GOALBOX_RIGHT_LINE:
        return "Yellow Goalbox Right Line";
    default:
        return "Invalid Line ID";
    }
}
