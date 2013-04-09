#ifndef ConcreteLine_h_defined
#define ConcreteLine_h_defined

// Forward reference
#include <ostream>
#include <vector>
#include <string>

// Includes
#include "FieldConstants.h"

namespace man {
namespace vision {

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
    BLUE_GOAL_ENDLINE,
    YELLOW_GOAL_ENDLINE,

    TOP_SIDELINE,
    BOTTOM_SIDELINE,
    MIDLINE,

	// GOALBOX LINES:
	// Named as if you were the goalie (so the "top of the box" is the TOP_LINE)
    BLUE_GOALBOX_TOP_LINE,
    BLUE_GOALBOX_LEFT_LINE,
    BLUE_GOALBOX_RIGHT_LINE,
    YELLOW_GOALBOX_TOP_LINE,
    YELLOW_GOALBOX_LEFT_LINE,
    YELLOW_GOALBOX_RIGHT_LINE
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
    static const int NUM_LINES = 11;
	static const int NUM_SIDELINES = 4;
	static const int NUM_GOALBOX_LINES = 6;
	static const int NUM_T_BAR_LINES = 4;
	static const int NUM_T_STEM_LINES = 5;
	static const int NUM_L_LINES = 10;

public:
    // Destructor
    virtual ~ConcreteLine();

    friend std::ostream& operator<< (std::ostream &o, const ConcreteLine &c)
    {
        return o << c.toString();
    }

	// Define equality to be if the ID's are equal.
	bool operator== (const ConcreteLine &secondLine) const {
		return getID() == secondLine.getID();
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

	static const ConcreteLine& blue_goal_endline();
	static const ConcreteLine& yellow_goal_endline();
	static const ConcreteLine& top_sideline();
	static const ConcreteLine& bottom_sideline();
	static const ConcreteLine& blue_goalbox_top_line();
	static const ConcreteLine& blue_goalbox_left_line();
	static const ConcreteLine& blue_goalbox_right_line();
	static const ConcreteLine& yellow_goalbox_top_line();
	static const ConcreteLine& yellow_goalbox_left_line();
	static const ConcreteLine& yellow_goalbox_right_line();
	static const ConcreteLine& midline();

public:
    static const std::vector <const ConcreteLine*>& concreteLines();
	static const std::vector <const ConcreteLine*>& goalboxLines();
	static const std::vector <const ConcreteLine*>& sidelines();
	static const std::vector <const ConcreteLine*>& tBarLines();
	static const std::vector <const ConcreteLine*>& tStemLines();
	static const std::vector <const ConcreteLine*>& lCornerLines();

private:
    // Instance variables
    const float fieldX1, fieldY1, fieldX2, fieldY2;
    lineID id;

}; // class ConcreteLine

}
}

#endif // ConcreteLine_h_defined




