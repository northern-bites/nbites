/**
 * Observation.h
 *
 * @author Tucker Hermans
 * @version %I%
 */

#ifndef Observation_h_DEFINED
#define Observation_h_DEFINED
using namespace std;

#include "FieldConstants.h"
#include "ConcreteCorner.h"
#include "ConcreteLine.h"
#include "ConcreteFieldObject.h"

// Temporary until field objects gets in line with the rest of the world...
#define SURE       2
#define MILDLYSURE 1
#define NOTSURE    0

// // All possible observation IDs
// enum ObservationID
// {
//     // Blue goal
//     BLUE_GOAL_LEFT_POST;
//     BLUE_GOAL_RIGHT_POST;
//     BLUE_GOAL_POST_UNCLER;

//     // Yellow goal
//     YELLOW_GOAL_LEFT_POST;
//     YELLOW_GOAL_RIGHT_POST;
//     YELLOW_GOAL_POST_UNCLER;

//     // Line intersections

//     // Lines
// };

// Structs

/**
 * Stores field location information about a concrete point landmark
 */
class PointLandmark
{
public:
    float x;
    float y;
};

/**
 * Stores field location information about a concrete line landmark
 */
class LineLandmark : public Landmark
{
public:
    float x1;
    float y1;
    float x2;
    float y2;
};


/**
 * Class to hold the informations associated with the observation of a landmark
 */
class Observation
{
public:
    // Fields
    vector< pair<float, float> > posibilities;

    // Construcotrs & Destructors
    Observation(FieldObject &_object);
    Observation(VisualCorner &_corner);
    Observation(VisualLine &_line);
    virtual ~Observation();

    // Core Functions

    // Getters
    /**
     * @return The distance reported by the visual sighting
     */
    float getVisDist() { return visDist; }
    /*
     * @return The bearing reported by the visual sighting
     */
    float getVisBearing() { return visBearing; }
    /**
     * @return The standard deviation of the distance of the observation.
     */
    float getDistSD() { return sigma_d; }
    /*
     * @return The standard deviation of the bearing of the observation.
     */
    float getBearingSD() { return sigma_b; }

    /*
     * @return The ID of the landmark, element of ObservationID enumeration.
     */
    int getID() { return id; }
    /*
     * @return true if the observed landmark was not IDed to a specifc (x,y).
     */
    bool isAmbgiuous() { return ambiguous; }
    /*
     * @return The x value of the landmark on the playing field.
     */
    float getX() { return x; }
    /*
     * @return The y value of the landmark on the playing field.
     */
    float getY() { return y; }

    /*
     * @return true if the observed object is a line
     */
    bool isLine() { return line_truth; }

    /*
     * @return The number of possibilities for the landmark
     */
    unsigned int getNumPossibilities() { return numPossibilities; }

    // Setters

private:
    // Vision information
    float visDist;
    float visBearing;
    float sigma_d;
    float sigma_b;

    // Identity information
    int id;
    bool ambiguous;
    float x;
    float y;
    float slope;
    bool line_truth;
    vector<LineLandmark> linePossibilities;
    vector<PointLandmark> pointPossibilities;
    unsigned int numPossibilities;
};
#endif // _Observation_h_DEFINED
