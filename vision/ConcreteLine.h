#ifndef ConcreteLine_h_defined
#define ConcreteLine_h_defined

// Forward reference
class ConcreteLine;
#include <ostream>
#include <list>
#include <string>

// Includes
#include "FieldConstants.h"

class ConcreteLine; // Forward reference

// Line ID values
enum lineID {
    // Ambiguous lines
    UNKNOWN_LINE = 50,
    SIDE_OR_ENDLINE,
    SIDELINE_LINE,
    ENDLINE_LINE,
    GOALBOX_LINE,
    GOALBOX_SIDE_LINE,			// 55
    GOALBOX_TOP_LINE,

    // Distinct lines:
	// Named by looking from center field out, left end is at the blue goal

	// Endlines
    BLUE_GOAL_TOP_ENDLINE,
    BLUE_GOAL_MID_ENDLINE,
    BLUE_GOAL_BOTTOM_ENDLINE,
    YELLOW_GOAL_TOP_ENDLINE,	// 60
    YELLOW_GOAL_MID_ENDLINE,
    YELLOW_GOAL_BOTTOM_ENDLINE,

	// Sidelines
    BLUE_GOAL_TOP_SIDELINE,
    YELLOW_GOAL_TOP_SIDELINE,
    BLUE_GOAL_BOTTOM_SIDELINE,	// 65
    YELLOW_GOAL_BOTTOM_SIDELINE,

	// Midlines
    TOP_MIDLINE,
    CENTER_MIDLINE,
    BOTTOM_MIDLINE,

	// GOALBOX LINES:
	// Named as if you were the goalie (so the "top of the box" is the TOP_LINE)
    BLUE_GOALBOX_TOP_LINE,		// 70
    BLUE_GOALBOX_LEFT_LINE,
    BLUE_GOALBOX_RIGHT_LINE,
    YELLOW_GOALBOX_TOP_LINE,
    YELLOW_GOALBOX_LEFT_LINE,
    YELLOW_GOALBOX_RIGHT_LINE	// 75
};

class ConcreteLine
{
    /* Constructors are private because no one should ever have to create more
     * concrete lines. All of them are defined as constant static members of
     * this class.
     */
private: //Constructors and Deconstructors
    ConcreteLine(const float _fieldX1, const float _fieldY1,
                 const float _fieldX2, const float _fieldY2,
                 const lineID _id);
    ConcreteLine(const ConcreteLine& other);

public: // Constants
    static const int NUM_LINES = 19;
	static const int NUM_SIDELINES = 4;
	static const int NUM_GOALBOX_LINES = 6;

public:
    // Destructor
    virtual ~ConcreteLine();

    friend std::ostream& operator<< (std::ostream &o, const ConcreteLine &c)
    {
        return o << c.toString();
    }


    ////////////////////////////////////////////////////////////
    // GETTERS
    ////////////////////////////////////////////////////////////
    const lineID getID() const { return id; }

    virtual const std::string toString() const;

    const float getFieldX1() const { return fieldX1; }
    const float getFieldY1() const { return fieldY1; }
    const float getFieldX2() const { return fieldX2; }
    const float getFieldY2() const { return fieldY2; }

public:
    static const ConcreteLine blue_goal_top_endline,
					blue_goal_mid_endline,
					blue_goal_bottom_endline,
					yellow_goal_top_endline,
					yellow_goal_mid_endline,
					yellow_goal_bottom_endline,
					blue_goal_top_sideline,
					yellow_goal_top_sideline,
					blue_goal_bottom_sideline,
					yellow_goal_bottom_sideline,
					top_midline,
					center_midline,
					bottom_midline,
					blue_goalbox_top_line,
					blue_goalbox_left_line,
					blue_goalbox_right_line,
					yellow_goalbox_top_line,
					yellow_goalbox_left_line,
					yellow_goalbox_right_line;

    static const ConcreteLine* concreteLineList[NUM_LINES];
    static const ConcreteLine* sidelineList[NUM_SIDELINES];
    static const ConcreteLine* goalboxLineList[NUM_GOALBOX_LINES];

public:
    static const std::list <const ConcreteLine*> concreteLines;
	static const std::list <const ConcreteLine*> goalboxLines;
	static const std::list <const ConcreteLine*> sidelines;
// We'll probably want to define some lists of sidelines vs goalbox lines, etc.
// This will be implemented in accordance with analysis within fieldlines...

private:
    // Instance variables
    const float fieldX1, fieldY1, fieldX2, fieldY2;
    lineID id;

}; // class ConcreteLine

#endif // ConcreteLine_h_defined
