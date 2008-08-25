#ifndef ConcreteLine_h_defined
#define ConcreteLine_h_defined

// Forward reference
class ConcreteLine;
using namespace std;

// Includes
#include "FieldConstants.h"

// Line ID values
enum lineID {
    // Ambiguous lines
    UNKNOWN_LINE = 100,
    SIDE_OR_ENDLINE,
    SIDELINE,
    ENDLINE,
    GOALBOX_LINE,
    GOALBOX_SIDE_LINE,
    GOALBOX_TOP_LINE,

    // Distinct lines
    BLUE_GOAL_ENDLINE,
    YELLOW_GOAL_ENDLINE,
    // looking from center field out, left end is at the blue goal
    BLUE_YELLOW_SIDELINE,
    YELLOW_BLUE_SIDELINE,
    CENTER_FIELD_LINE,
    BLUE_GOALBOX_TOP_LINE,
    BLUE_GOALBOX_LEFT_LINE,
    BLUE_GOALBOX_RIGHT_LINE,
    YELLOW_GOALBOX_TOP_LINE,
    YELLOW_GOALBOX_LEFT_LINE,
    YELLOW_GOALBOX_RIGHT_LINE
};

#endif // File
