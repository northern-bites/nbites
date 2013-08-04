#include "ConcreteLine.h"
using namespace std;

namespace man {
namespace vision {

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


const vector<const ConcreteLine*>& ConcreteLine::concreteLines()
{
    static const ConcreteLine* lines_array[NUM_LINES] =
    { &blue_goal_endline(),
      &yellow_goal_endline(),
      &top_sideline(),
      &bottom_sideline(),
      &midline(),
      &blue_goalbox_top_line(),
      &blue_goalbox_left_line(),
      &blue_goalbox_right_line(),
      &yellow_goalbox_top_line(),
      &yellow_goalbox_left_line(),
      &yellow_goalbox_right_line()};
    static const vector<const ConcreteLine*>
    lines(lines_array,
          lines_array + sizeof(lines_array) / sizeof(lines_array[0]));
    return lines;
}

const vector<const ConcreteLine*>& ConcreteLine::goalboxLines()
{
    static const ConcreteLine* lines_array[NUM_GOALBOX_LINES] =
    {
     &blue_goalbox_top_line(),
     &blue_goalbox_left_line(),
     &blue_goalbox_right_line(),
     &yellow_goalbox_top_line(),
     &yellow_goalbox_left_line(),
     &yellow_goalbox_right_line() };
    static const vector<const ConcreteLine*>
    lines(lines_array,
          lines_array + sizeof(lines_array) / sizeof(lines_array[0]));
    return lines;
}

const vector<const ConcreteLine*>& ConcreteLine::sidelines()
{
    static const ConcreteLine* lines_array[NUM_SIDELINES] =
    { &top_sideline(),
      &bottom_sideline() };
    static const vector<const ConcreteLine*>
    lines(lines_array,
          lines_array + sizeof(lines_array) / sizeof(lines_array[0]));
    return lines;
}

const vector<const ConcreteLine*>& ConcreteLine::tBarLines()
{
    static const ConcreteLine* lines_array[NUM_T_BAR_LINES] =
    {
     &top_sideline(),
     &bottom_sideline(),
     &blue_goal_endline(),
     &yellow_goal_endline() };
    static const vector<const ConcreteLine*>
    lines(lines_array,
          lines_array + sizeof(lines_array) / sizeof(lines_array[0]));
    return lines;
}

const vector<const ConcreteLine*>& ConcreteLine::tStemLines()
{
    static const ConcreteLine* lines_array[NUM_T_STEM_LINES] =
    {
     &midline(),
     &blue_goalbox_right_line(),
     &blue_goalbox_left_line(),
     &yellow_goalbox_right_line(),
     &yellow_goalbox_left_line() };
    static const vector<const ConcreteLine*>
    lines(lines_array,
          lines_array + sizeof(lines_array) / sizeof(lines_array[0]));
    return lines;
}

const vector<const ConcreteLine*>& ConcreteLine::lCornerLines()
{
    static const ConcreteLine* lines_array[NUM_L_LINES] =
    { &blue_goal_endline(),
      &yellow_goal_endline(),
      &top_sideline(),
      &bottom_sideline(),
      &blue_goalbox_top_line(),
      &blue_goalbox_left_line(),
      &blue_goalbox_right_line(),
      &yellow_goalbox_top_line(),
      &yellow_goalbox_left_line(),
      &yellow_goalbox_right_line()};
    static const vector<const ConcreteLine*>
    lines(lines_array,
          lines_array + sizeof(lines_array) / sizeof(lines_array[0]));
    return lines;
}

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
    case BLUE_GOAL_ENDLINE:
        return "blue goal endline";
    case YELLOW_GOAL_ENDLINE:
        return "yellow goal endline";
    case TOP_SIDELINE:
        return "top sideline";
    case BOTTOM_SIDELINE:
        return "bottom sideline";
    case MIDLINE:
        return "midline";
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

// Functions to guarantee initialization of concretelines
const ConcreteLine& ConcreteLine::blue_goal_endline()
{
    static const ConcreteLine
    blue_goal_endline(FIELD_WHITE_LEFT_SIDELINE_X,
                      FIELD_WHITE_TOP_SIDELINE_Y,
                      FIELD_WHITE_LEFT_SIDELINE_X,
                      FIELD_WHITE_BOTTOM_SIDELINE_Y,
                      BLUE_GOAL_ENDLINE);
    return blue_goal_endline;
}
const ConcreteLine& ConcreteLine::yellow_goal_endline()
{
    static const ConcreteLine
    yellow_goal_endline(FIELD_WHITE_RIGHT_SIDELINE_X,
                        FIELD_WHITE_TOP_SIDELINE_Y,
                        FIELD_WHITE_RIGHT_SIDELINE_X,
                        FIELD_WHITE_BOTTOM_SIDELINE_Y,
                        YELLOW_GOAL_ENDLINE);

    return yellow_goal_endline;
}
const ConcreteLine& ConcreteLine::top_sideline()
{
    static const ConcreteLine
    top_sideline(FIELD_WHITE_LEFT_SIDELINE_X,
                 FIELD_WHITE_TOP_SIDELINE_Y,
                 FIELD_WHITE_RIGHT_SIDELINE_X,
                 FIELD_WHITE_TOP_SIDELINE_Y,
                 TOP_SIDELINE);
    return top_sideline;
}
const ConcreteLine& ConcreteLine::bottom_sideline()
{
    static const ConcreteLine
    bottom_sideline(FIELD_WHITE_LEFT_SIDELINE_X,
                    FIELD_WHITE_BOTTOM_SIDELINE_Y,
                    FIELD_WHITE_RIGHT_SIDELINE_X,
                    FIELD_WHITE_BOTTOM_SIDELINE_Y,
                    BOTTOM_SIDELINE);
    return bottom_sideline;
}
const ConcreteLine& ConcreteLine::blue_goalbox_top_line()
{
    static const ConcreteLine
    blue_goalbox_top_line(BLUE_GOALBOX_RIGHT_X,
                          BLUE_GOALBOX_TOP_Y,
                          BLUE_GOALBOX_RIGHT_X,
                          BLUE_GOALBOX_BOTTOM_Y,
                          BLUE_GOALBOX_TOP_LINE);

    return blue_goalbox_top_line;
}
const ConcreteLine& ConcreteLine::blue_goalbox_left_line()
{
    static const ConcreteLine
    blue_goalbox_left_line(BLUE_GOALBOX_LEFT_X,
                           BLUE_GOALBOX_TOP_Y,
                           BLUE_GOALBOX_RIGHT_X,
                           BLUE_GOALBOX_TOP_Y,
                           BLUE_GOALBOX_LEFT_LINE);

    return blue_goalbox_left_line;
}
const ConcreteLine& ConcreteLine::blue_goalbox_right_line()
{
    static const ConcreteLine
    blue_goalbox_right_line(BLUE_GOALBOX_LEFT_X,
                            BLUE_GOALBOX_BOTTOM_Y,
                            BLUE_GOALBOX_RIGHT_X,
                            BLUE_GOALBOX_BOTTOM_Y,
                            BLUE_GOALBOX_RIGHT_LINE);

    return blue_goalbox_right_line;
}
const ConcreteLine& ConcreteLine::yellow_goalbox_top_line()
{

    static const ConcreteLine
    yellow_goalbox_top_line(YELLOW_GOALBOX_LEFT_X,
                            YELLOW_GOALBOX_TOP_Y,
                            YELLOW_GOALBOX_LEFT_X,
                            YELLOW_GOALBOX_BOTTOM_Y,
                            YELLOW_GOALBOX_TOP_LINE);
    return yellow_goalbox_top_line;
}
const ConcreteLine& ConcreteLine::yellow_goalbox_left_line()
{

    static const ConcreteLine
    yellow_goalbox_left_line(YELLOW_GOALBOX_LEFT_X,
                             YELLOW_GOALBOX_BOTTOM_Y,
                             YELLOW_GOALBOX_RIGHT_X,
                             YELLOW_GOALBOX_BOTTOM_Y,
                             YELLOW_GOALBOX_LEFT_LINE);
    return yellow_goalbox_left_line;
}
const ConcreteLine& ConcreteLine::yellow_goalbox_right_line()
{
    static const ConcreteLine
    yellow_goalbox_right_line(YELLOW_GOALBOX_LEFT_X,
                              YELLOW_GOALBOX_TOP_Y,
                              YELLOW_GOALBOX_RIGHT_X,
                              YELLOW_GOALBOX_TOP_Y,
                              YELLOW_GOALBOX_RIGHT_LINE);
    return yellow_goalbox_right_line;
}

const ConcreteLine& ConcreteLine::midline()
{
    static const ConcreteLine
    midline_line(CENTER_FIELD_X,
                 FIELD_WHITE_TOP_SIDELINE_Y,
                 CENTER_FIELD_X,
                 FIELD_WHITE_BOTTOM_SIDELINE_Y,
                 MIDLINE);

    return midline_line;
}

}
}
