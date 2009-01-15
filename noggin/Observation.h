/**
 * Observation.h
 *
 * @author Tucker Hermans
 * @version %I%
 */

#ifndef Observation_h_DEFINED
#define Observation_h_DEFINED
#include <vector>
using namespace std;

#include "ConcreteCorner.h"
#include "ConcreteLine.h"
#include "ConcreteFieldObject.h"
#include "VisualLine.h"
#include "VisualCorner.h"
#include "VisualFieldObject.h"
#include "Structs.h"

// Structs

/**
 * Stores field location information about a concrete point landmark
 */
class PointLandmark
{
public:
    PointLandmark(float _x, float _y);
    PointLandmark();
    float x;
    float y;
};

/**
 * Stores field location information about a concrete line landmark
 */
class LineLandmark
{
public:
    LineLandmark(float _x1, float _y1, float _x2, float _y2);
    LineLandmark();
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
    Observation(VisualFieldObject &_object);
    Observation(VisualCorner &_corner);
    Observation(VisualLine &_line);
    Observation(const ConcreteCorner &_corner);
    virtual ~Observation();

    // Core Functions

    // Getters
    /**
     * @return The distance reported by the visual sighting
     */
    float getVisDistance() { return visDist; }
    /*
     * @return The bearing reported by the visual sighting
     */
    float getVisBearing() { return visBearing; }
    /**
     * @return The standard deviation of the distance of the observation.
     */
    float getDistanceSD() { return sigma_d; }
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
    //bool isAmbgiuous() { return ambiguous; }
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

    /*
     * @return The list of possible line landmarks
     */
    vector<LineLandmark> getLinePossibilities() { return linePossibilities; }

    /*
     * @return The list of possible point landmarks
     */
    vector<PointLandmark> getPointPossibilities() { return pointPossibilities; }

    // Setters
    void setVisDistance(float _d) { visDist = _d; }
    void setVisBearing(float _b) { visBearing = _b; }
    void setDistanceSD(float _sdD) { sigma_d = _sdD; }
    void setBearingSD(float _sdB) { sigma_b = _sdB; }


private:
    // Vision information
    float visDist;
    float visBearing;
    float sigma_d;
    float sigma_b;

    // Identity information
    int id;
    float x;
    float y;
    float slope;
    bool line_truth;
    vector<LineLandmark> linePossibilities;
    vector<PointLandmark> pointPossibilities;
    unsigned int numPossibilities;
};
#endif // _Observation_h_DEFINED
