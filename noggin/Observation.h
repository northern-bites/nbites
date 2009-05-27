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
#include "NogginStructs.h"

/**
 * Class to hold the informations associated with the observation of a landmark
 */
class Observation
{
public:
    // Fields
    std::vector< std::pair<float, float> > posibilities;

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
    const float getVisBearingDeg() const { return TO_DEG * visBearing; }
    /**
     * @return The standard deviation of the distance of the observation.
     */
    const float getDistanceSD() const { return sigma_d; }
    /*
     * @return The standard deviation of the bearing of the observation.
     */
    const float getBearingSD() const { return sigma_b; }

    const RangeBearingMeasurement getRangeBearingMeasurement() const {
        return RangeBearingMeasurement(visDist, visBearing, sigma_d,
                                             sigma_b);
    }

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

    friend std::ostream& operator<< (std::ostream &o, const Observation &c) {
        return o << "Obs " << c.id << ": (" << c.visDist << ", " << c.visBearing
                 << ", " << c.sigma_d << ", " << c.sigma_b << ")";
    }


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
