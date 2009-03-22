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
    static const ConcreteLine blue_goal_endline,
                    yellow_goal_endline,
                    blue_yellow_sideline,
                    yellow_blue_sideline,
                    center_field_line,
                    blue_goalbox_top_line,
                    blue_goalbox_left_line,
                    blue_goalbox_right_line,
                    yellow_goalbox_top_line,
                    yellow_goalbox_left_line,
                    yellow_goalbox_right_line;

    static const ConcreteLine* concreteLineList[NUM_LINES];

public:
    static const std::list <const ConcreteLine*> concreteLines;
// We'll probably want to define some lists of sidelines vs goalbox lines, etc.
// This will be implemented in accordance with analysis within fieldlines...

private:
    // Instance variables
    const float fieldX1, fieldY1, fieldX2, fieldY2;
    lineID id;

}; // class ConcreteLine

#endif // ConcreteLine_h_defined
