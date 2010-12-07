#ifndef ConcreteCorner_h_defined
#define ConcreteCorner_h_defined

class ConcreteCorner;

#include <ostream>
#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <algorithm>

#include "FieldConstants.h"
#include "ConcreteLine.h"
#include "ConcreteLandmark.h"

enum cornerID {
    L_INNER_CORNER = 0,
    L_OUTER_CORNER,
    T_CORNER,
    CENTER_CIRCLE,

    // FUZZY/CLEAR CORNER IDS start at = 4
    BLUE_GOAL_T,
    YELLOW_GOAL_T,
    BLUE_GOAL_RIGHT_L_OR_YELLOW_GOAL_LEFT_L,
    BLUE_GOAL_LEFT_L_OR_YELLOW_GOAL_RIGHT_L,
    BLUE_CORNER_TOP_L_OR_YELLOW_CORNER_BOTTOM_L,
    BLUE_CORNER_BOTTOM_L_OR_YELLOW_CORNER_TOP_L,
    CORNER_INNER_L,
    GOAL_BOX_INNER_L,

    // FUZZY/CLEAR CORNER IDS start at = 12
    BLUE_GOAL_OUTER_L,
    YELLOW_GOAL_OUTER_L,
    CENTER_T,

    // SPECIFIC CORNER IDS start at = 15
    BLUE_CORNER_TOP_L,
    BLUE_CORNER_BOTTOM_L,
    BLUE_GOAL_LEFT_T,
    BLUE_GOAL_RIGHT_T,
    BLUE_GOAL_LEFT_L,
    BLUE_GOAL_RIGHT_L, // 20
    CENTER_BOTTOM_T,
    CENTER_TOP_T,
    YELLOW_CORNER_BOTTOM_L,
    YELLOW_CORNER_TOP_L,
    YELLOW_GOAL_LEFT_T, // 25
    YELLOW_GOAL_RIGHT_T,
    YELLOW_GOAL_LEFT_L,
    YELLOW_GOAL_RIGHT_L,
    CORNER_NO_IDEA_ID,
    TOP_CC, // 30
    BOTTOM_CC,

    // secondary shapes
    LEFT_GOAL_L,
    RIGHT_GOAL_L,
    GOAL_T,
    LEFT_GOAL_T,
    RIGHT_GOAL_T
};

enum shape {
    INNER_L,
    OUTER_L,
    T,
    CIRCLE,
    UNKNOWN
};


class ConcreteCorner : public ConcreteLandmark {
    /* Constructors are private because no one should ever have to create more
     * concrete corners. All of them are defined as constant static members of
     * this class.
     */
private:
    ConcreteCorner(const float fieldX, const float fieldY, const cornerID _id);
    ConcreteCorner(const float _fieldX, const float _fieldY,
                   const ConcreteLine& _l1, const ConcreteLine& _l2,
                   const cornerID _id);

    // copy constructor
    ConcreteCorner(const ConcreteCorner&);

public: // Constants
    static const unsigned int NUM_L_CORNERS = 8;
    static const unsigned int NUM_T_CORNERS = 6;
    static const unsigned int NUM_CC_CORNERS = 2;
    static const unsigned int NUM_CORNERS = (NUM_L_CORNERS +
											 NUM_T_CORNERS +
											 NUM_CC_CORNERS);
    static const unsigned int NUM_YELLOW_GOAL_CORNERS = 4;
    static const unsigned int NUM_BLUE_GOAL_CORNERS = 4;
    static const unsigned int NUM_GOAL_CORNERS = (NUM_YELLOW_GOAL_CORNERS +
												  NUM_BLUE_GOAL_CORNERS);
    static const unsigned int NUM_BLUE_GOAL_T_CORNERS = 2;
    static const unsigned int NUM_YELLOW_GOAL_T_CORNERS = 2;

public:
    // destructor
    virtual ~ConcreteCorner();

    friend std::ostream& operator<< (std::ostream &o, const ConcreteCorner &c)
        {
            return o << c.toString();
        }

	bool operator== (const ConcreteCorner& secondCorner) const
		{
			return id == secondCorner.getID();
		}

    ////////////////////////////////////////////////////////////
    // GETTERS
    ////////////////////////////////////////////////////////////
    const cornerID getID() const { return id; }

    virtual const std::string toString() const;

    static const std::vector <const ConcreteCorner*>&
	getPossibleCorners(shape corner_type);

	const std::vector<const ConcreteLine*> getLines() const {
		return lines;
	}

//     // Private methods
// private:
public:
    static const shape inferCornerType(const cornerID id);
	void assignTCornerLines();
	bool isLineInCorner(const ConcreteLine* line) const;

public:
    static const ConcreteCorner& blue_corner_top_l();
	static const ConcreteCorner& blue_corner_bottom_l();
	static const ConcreteCorner& blue_goal_left_t();
	static const ConcreteCorner& blue_goal_right_t();
	static const ConcreteCorner& blue_goal_left_l();
	static const ConcreteCorner& blue_goal_right_l();
	static const ConcreteCorner& center_top_t();
	static const ConcreteCorner& center_bottom_t();
	static const ConcreteCorner& yellow_corner_bottom_l();
	static const ConcreteCorner& yellow_corner_top_l();
	static const ConcreteCorner& yellow_goal_left_t();
	static const ConcreteCorner& yellow_goal_right_t();
	static const ConcreteCorner& yellow_goal_left_l();
	static const ConcreteCorner& yellow_goal_right_l();
	static const ConcreteCorner& top_cc();
	static const ConcreteCorner& bottom_cc();
	static const ConcreteCorner& fake_cc();

    static const std::string getShapeString(shape s);
    static const std::string cornerIDToString(const cornerID _id);
    static const ConcreteCorner* concreteCornerList[NUM_CORNERS];

	const ConcreteLine* getTBar() const { return tBar; }
	const ConcreteLine* getTStem() const { return tStem; }

public:
	static const std::vector <const ConcreteCorner*>& concreteCorners();
    static const std::vector <const ConcreteCorner*>& lCorners();
    static const std::vector <const ConcreteCorner*>& tCorners();
    static const std::vector <const ConcreteCorner*>& ccCorners();

    static const std::vector <const ConcreteCorner*>& yellowGoalCorners();
    static const std::vector <const ConcreteCorner*>& blueGoalCorners();

    static const std::vector <const ConcreteCorner*>& yellowGoalTCorners();
    static const std::vector <const ConcreteCorner*>& blueGoalTCorners();

private: // Instance variables recording location on field and identifier
    cornerID id;
    shape cornerType;

    const ConcreteLine * line1;
    const ConcreteLine * line2;
	std::vector<const ConcreteLine*> lines;

	const ConcreteLine * tBar;
	const ConcreteLine * tStem;

};

class CornerOfField : public std::unary_function<const ConcreteCorner*, bool> {
public:

    explicit CornerOfField() {}
    bool operator() (const ConcreteCorner *c) const {
        return
            (*c == ConcreteCorner::blue_corner_top_l() ||
             *c == ConcreteCorner::blue_corner_bottom_l() ||
             *c == ConcreteCorner::yellow_corner_bottom_l() ||
             *c == ConcreteCorner::yellow_corner_top_l());

    }
};

class InList : public std::unary_function<const ConcreteCorner*, bool> {
    const std::list<const ConcreteCorner *> possibilities;

public:
    explicit InList(const std::list<const ConcreteCorner *> &_possibilities) :
        possibilities(_possibilities) { }

    bool operator() (const ConcreteCorner *c) const {

        // find will return possibilities.end() if c is not in the list.
        return find(possibilities.begin(), possibilities.end(), c) !=
            possibilities.end();
    }
};

#endif
