#ifndef ConcreteFieldObject_h_defined
#define ConcreteFieldObject_h_defined

// Forward Reference
class ConcreteFieldObject;

// STL libraries
#include <ostream>
#include <string>
#include <list>
#include <algorithm>

// Local headers
#include "Structs.h"
#include "FieldConstants.h"
#include "ConcreteLandmark.h"

#define CONCRETE_FIELD_OBJECT_START_ID 30
enum fieldObjectID {
    BLUE_GOAL_LEFT_POST = CONCRETE_FIELD_OBJECT_START_ID,
    BLUE_GOAL_RIGHT_POST,
    YELLOW_GOAL_LEFT_POST,
    YELLOW_GOAL_RIGHT_POST,
    BLUE_GOAL_POST,
    YELLOW_GOAL_POST,
    UNKNOWN_FIELD_OBJECT
};

class ConcreteFieldObject : public ConcreteLandmark {
    /* Constructors are private because no one should ever have to create more
     * concrete field objects. All of them are defined as constant static
     * members of this class.
     */
private:
    ConcreteFieldObject(const float fieldX, const float fieldY,
                        const fieldObjectID _id);
    // copy constructor
    ConcreteFieldObject(const ConcreteFieldObject&);

public: // Constants
    static const int NUM_BLUE_GOAL_POSTS = 2;
    static const int NUM_YELLOW_GOAL_POSTS = 2;
    static const int NUM_FIELD_OBJECTS = NUM_BLUE_GOAL_POSTS +
        NUM_YELLOW_GOAL_POSTS;

public:
    virtual ~ConcreteFieldObject();

    friend std::ostream& operator<< (std::ostream &o,
                                     const ConcreteFieldObject &fo)
    {
        return o << fo.toString();
    }

    ////////////////////////////////////////////////////////////
    // GETTERS
    ////////////////////////////////////////////////////////////
    const fieldObjectID getID() const { return id; }

    virtual const std::string toString() const;
    static const std::string getStringFromID(const fieldObjectID testID);
    static const float getHeightFromGround(const fieldObjectID id);

    static const bool isAbstract(const fieldObjectID testID);
    static const bool isGoal(const fieldObjectID testID);

// Static member variables
public:
    static const ConcreteFieldObject blue_goal_left_post,
                    blue_goal_right_post,
                    yellow_goal_left_post,
                    yellow_goal_right_post,
                    blue_goal_post,
                    yellow_goal_post;

    static const ConcreteFieldObject* concreteFieldObjectList[
        NUM_FIELD_OBJECTS];

private:
    static const ConcreteFieldObject* BLUE_GOAL_POSTS[NUM_BLUE_GOAL_POSTS];
    static const ConcreteFieldObject* YELLOW_GOAL_POSTS[NUM_YELLOW_GOAL_POSTS];

public:
    static const std::list <const ConcreteFieldObject*> blueGoalPosts;
    static const std::list <const ConcreteFieldObject*> yellowGoalPosts;
    static const std::list <const ConcreteFieldObject*> blueGoalLeftPostList;
    static const std::list <const ConcreteFieldObject*> blueGoalRightPostList;
    static const std::list <const ConcreteFieldObject*> yellowGoalLeftPostList;
    static const std::list <const ConcreteFieldObject*> yellowGoalRightPostList;

private: // Instance variables recording location on field and identifier
    fieldObjectID id;

}; // class ConcreteFieldObject
////////////////////////////////////////////////////////////
// Constants for absolute coordinates on the field of the six
// most meaningful field objects (in terms of stationary things
// we extract distance information from).  Since we do not use
// backstops for any distance/bearing information, I am not including
// it here.
// (0,0) is the lower left corner of the field when BLUE goal is at the
// bottom
// NICK
//
////////////////////////////////////////////////////////////

static const point <float> BLUE_GOAL_BOTTOM_POST_LOC =
    point<float>(LANDMARK_BLUE_GOAL_BOTTOM_POST_X,
                  LANDMARK_BLUE_GOAL_BOTTOM_POST_Y);

static const point <float> BLUE_GOAL_TOP_POST_LOC =
    point<float>(LANDMARK_BLUE_GOAL_TOP_POST_X,
                  LANDMARK_BLUE_GOAL_TOP_POST_Y);

static const point <float> YELLOW_GOAL_TOP_POST_LOC =
    point<float>(LANDMARK_YELLOW_GOAL_TOP_POST_X,
                  LANDMARK_YELLOW_GOAL_TOP_POST_Y);

static const point <float> YELLOW_GOAL_BOTTOM_POST_LOC =
    point<float>(LANDMARK_YELLOW_GOAL_BOTTOM_POST_X,
                  LANDMARK_YELLOW_GOAL_BOTTOM_POST_Y);

// Arc locations are defined to be the center of the quarter circle spanned
// by the arc
static const point <float> BLUE_GOAL_YELLOW_ARC_CENTER_LOC =
    point<float>(FIELD_WHITE_LEFT_SIDELINE_X, FIELD_WHITE_BOTTOM_SIDELINE_Y);

static const point <float> BLUE_GOAL_BLUE_ARC_CENTER_LOC =
    point<float>(FIELD_WHITE_RIGHT_SIDELINE_X, FIELD_WHITE_BOTTOM_SIDELINE_Y);

static const point <float> YELLOW_GOAL_YELLOW_ARC_CENTER_LOC =
    point<float>(FIELD_WHITE_RIGHT_SIDELINE_X, FIELD_WHITE_TOP_SIDELINE_Y);

static const point <float> YELLOW_GOAL_BLUE_ARC_CENTER_LOC =
    point<float>(FIELD_WHITE_LEFT_SIDELINE_X, FIELD_WHITE_TOP_SIDELINE_Y);

static const point <float> UNKNOWN_LOC =
    point<float>(-1, -1);

#endif // ConcreteFieldObject_h_defined
