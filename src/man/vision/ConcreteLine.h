#ifndef ConcreteLine_h_defined
#define ConcreteLine_h_defined

// Forward reference
#include <ostream>
#include <vector>
#include <string>

// Includes
#include "FieldConstants.h"
#include "VisionField.pb.h"

namespace man {
namespace vision {

// Line ID values

    typedef ::messages::VisualLine::line_id lineID;
	static const lineID UNKNOWN_LINE = ::messages::VisualLine_line_id_UNKNOWN_LINE;
	static const lineID SIDE_OR_ENDLINE = ::messages::VisualLine_line_id_SIDE_OR_ENDLINE;
	static const lineID SIDELINE_LINE = ::messages::VisualLine_line_id_SIDELINE_LINE;
	static const lineID ENDLINE_LINE = ::messages::VisualLine_line_id_ENDLINE_LINE;
	static const lineID GOALBOX_LINE = ::messages::VisualLine_line_id_GOALBOX_LINE;
	static const lineID GOALBOX_SIDE_LINE = ::messages::VisualLine_line_id_GOALBOX_SIDE_LINE;
	static const lineID GOALBOX_TOP_LINE = ::messages::VisualLine_line_id_GOALBOX_TOP_LINE;
	static const lineID BLUE_GOAL_ENDLINE = ::messages::VisualLine_line_id_BLUE_GOAL_ENDLINE;
	static const lineID YELLOW_GOAL_ENDLINE = ::messages::VisualLine_line_id_YELLOW_GOAL_ENDLINE;
	static const lineID TOP_SIDELINE = ::messages::VisualLine_line_id_TOP_SIDELINE;
	static const lineID BOTTOM_SIDELINE = ::messages::VisualLine_line_id_BOTTOM_SIDELINE;
	static const lineID MIDLINE = ::messages::VisualLine_line_id_MIDLINE;
	static const lineID BLUE_GOALBOX_TOP_LINE = ::messages::VisualLine_line_id_BLUE_GOALBOX_TOP_LINE;
	static const lineID BLUE_GOALBOX_LEFT_LINE = 
		::messages::VisualLine_line_id_BLUE_GOALBOX_LEFT_LINE;
	static const lineID BLUE_GOALBOX_RIGHT_LINE = 
		::messages::VisualLine_line_id_BLUE_GOALBOX_RIGHT_LINE;
	static const lineID YELLOW_GOALBOX_TOP_LINE = 
		::messages::VisualLine_line_id_YELLOW_GOALBOX_TOP_LINE;
	static const lineID YELLOW_GOALBOX_LEFT_LINE = 
		::messages::VisualLine_line_id_YELLOW_GOALBOX_LEFT_LINE;
	static const lineID YELLOW_GOALBOX_RIGHT_LINE = 
		::messages::VisualLine_line_id_YELLOW_GOALBOX_RIGHT_LINE;

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




