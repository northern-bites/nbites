/**
 * Observation.h
 *
 * @author Tucker Hermans
 * @version %I%
 */

#ifndef Observation_h_DEFINED
#define Observation_h_DEFINED
#include <vector>

#include "ConcreteCorner.h"
#include "ConcreteLine.h"
#include "ConcreteFieldObject.h"
#include "VisualLine.h"
#include "VisualCorner.h"
#include "VisualFieldObject.h"
#include "NBMath.h"
// Structs

// Odometery change
class MotionModel
{
public:
    MotionModel(float f, float l, float r);
    MotionModel(const MotionModel& other);
    MotionModel();
    float deltaF;
    float deltaL;
    float deltaR;
};

// Pose Estimate
class PoseEst
{
public:
    // Constructors
    PoseEst(float _x, float _y, float _h);
    PoseEst();
    PoseEst(const PoseEst& other);
    float x;
    float y;
    float h;

    PoseEst operator+ (const PoseEst o)
    {
        return PoseEst(o.x + x,
                       o.y + y,
                       o.h + h);
    }
    void operator+= (const PoseEst o)
    {
        x += o.x;
        y += o.y;
        h += o.h;
    }
    PoseEst operator+ (const MotionModel u_t)
    {
        // Translate the relative change into the global coordinate system
        // And add that to the current estimate
        float calcFromAngle = h - M_PI / 2.0f;
        return PoseEst(u_t.deltaF * -cos(calcFromAngle) +
                       u_t.deltaL * sin(calcFromAngle),
                       u_t.deltaF * -sin(calcFromAngle) -
                       u_t.deltaL * cos(calcFromAngle),
                       h += u_t.deltaR);
    }
    void operator+= (const MotionModel u_t)
    {
        // Translate the relative change into the global coordinate system
        // And add that to the current estimate
        float calcFromAngle = h - M_PI / 2.0f;
        x += u_t.deltaF * -cos(calcFromAngle) + u_t.deltaL * sin(calcFromAngle);
        y += u_t.deltaF * -sin(calcFromAngle) - u_t.deltaL * cos(calcFromAngle);
        h += u_t.deltaR;
    }

  friend std::ostream& operator<< (std::ostream &o, const PoseEst &c)
  {
      return o << "(" << c.x << ", " << c.y << ", " << c.h << ")";
  }


};

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
    std::vector< pair<float, float> > posibilities;

    // Construcotrs & Destructors
    Observation(VisualFieldObject &_object);
    Observation(const VisualCorner &_corner);
    Observation(const VisualLine &_line);
    virtual ~Observation();

    // Core Functions

    // Getters
    /**
     * @return The distance reported by the visual sighting
     */
    const float getVisDistance() const { return visDist; }
    /*
     * @return The bearing reported by the visual sighting
     */
    const float getVisBearing() const { return visBearing; }
    /*
     * @return The bearing reported by the visual sighting in degrees
     */
    const float getVisBearingDeg() const { return DEG_OVER_RAD * visBearing; }
    /**
     * @return The standard deviation of the distance of the observation.
     */
    const float getDistanceSD() const { return sigma_d; }
    /*
     * @return The standard deviation of the bearing of the observation.
     */
    const float getBearingSD() const { return sigma_b; }

    /*
     * @return The ID of the landmark, element of ObservationID enumeration.
     */
    const int getID() const { return id; }
    /*
     * @return The x value of the landmark on the playing field.
     */
    const float getX() const { return x; }
    /*
     * @return The y value of the landmark on the playing field.
     */
    const float getY() const { return y; }

    /*
     * @return true if the observed object is a line
     */
    const bool isLine() const { return line_truth; }

    /*
     * @return The number of possibilities for the landmark
     */
    const unsigned int getNumPossibilities() const { return numPossibilities; }

    /*
     * @return The list of possible line landmarks
     */
    const std::vector<LineLandmark> getLinePossibilities() const {
        return linePossibilities;
    }

    /*
     * @return The list of possible point landmarks
     */
    const std::vector<PointLandmark> getPointPossibilities() const {
        return pointPossibilities;
    }

    // Setters
    void setVisDistance(float _d) { visDist = _d; }
    void setVisBearing(float _b) { visBearing = _b; }
    void setDistanceSD(float _sdD) { sigma_d = _sdD; }
    void setBearingSD(float _sdB) { sigma_b = _sdB; }
    void setID(int _id) { id = _id; }

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
    std::vector<LineLandmark> linePossibilities;
    std::vector<PointLandmark> pointPossibilities;
    unsigned int numPossibilities;
};
#endif // _Observation_h_DEFINED
