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

#define CONCRETE_FIELD_OBJECT_START_ID 40
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
    const int getID() const { return id; }

    virtual const std::string toString() const;
    static const std::string getStringFromID(const int testID);
    static const float getHeightFromGround(const int id);

    static const bool isAbstract(const int testID);
    static const bool isGoal(const int testID);

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
    int id;

}; // class ConcreteFieldObject

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

static const point <float> UNKNOWN_LOC =
    point<float>(-1, -1);

#endif // ConcreteFieldObject_h_defined
