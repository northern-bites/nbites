#ifndef ConcreteCross_h_defined
#define ConcreteCross_h_defined

// STL libraries
#include <ostream>
#include <string>
#include <list>
#include <algorithm>

// Local headers
#include "Structs.h"
#include "FieldConstants.h"
#include "ConcreteLandmark.h"

#define CONCRETE_CROSS_START_ID 80
enum crossID {
    BLUE_GOAL_CROSS = CONCRETE_CROSS_START_ID,
    YELLOW_GOAL_CROSS,
    ABSTRACT_CROSS
};

class ConcreteCross : public ConcreteLandmark {
    /* Constructors are private because no one should ever have to create more
     * concrete field objects. All of them are defined as constant static
     * members of this class.
     */
private:
    ConcreteCross(const float fieldX, const float fieldY,
                  const crossID _id);
    // copy constructor
    ConcreteCross(const ConcreteCross&);

public: // Constants
    static const int NUM_FIELD_CROSSES = 2;

public:
    virtual ~ConcreteCross();

    friend std::ostream& operator<< (std::ostream &o,
                                     const ConcreteCross &fo)
    {
        return o << fo.toString();
    }

    ////////////////////////////////////////////////////////////
    // GETTERS
    ////////////////////////////////////////////////////////////
    const crossID getID() const { return id; }

    virtual const std::string toString() const;
    static const std::string getStringFromID(const int testID);
    static const float getHeightFromGround(const int id);

// Static member variables
public:
    static const ConcreteCross blue_goal_cross, yellow_goal_cross;

    static const ConcreteCross* concreteCrossList[NUM_FIELD_CROSSES];

private:
    static const ConcreteCross* FIELD_CROSSES[NUM_FIELD_CROSSES];
    static const ConcreteCross* BLUE_GOAL_CROSS_LIST[1];
    static const ConcreteCross* YELLOW_GOAL_CROSS_LIST[1];
public:
    static const std::list <const ConcreteCross*> blueGoalCrossList;
    static const std::list <const ConcreteCross*> yellowGoalCrossList;
    static const std::list <const ConcreteCross*> abstractCrossList;
    static const std::list <const ConcreteCross*> fieldCrosses;

private: // Instance variables recording location on field and identifier
    crossID id;

}; // class ConcreteCross

static const point <float> BLUE_GOAL_CROSS_LOC =
    point<float>(LANDMARK_BLUE_GOAL_CROSS_X,
                  LANDMARK_BLUE_GOAL_CROSS_Y);

static const point <float> YELLOW_GOAL_CROSS_LOC =
    point<float>(LANDMARK_YELLOW_GOAL_CROSS_X,
                  LANDMARK_YELLOW_GOAL_CROSS_Y);

#endif // ConcreteCross_h_defined
