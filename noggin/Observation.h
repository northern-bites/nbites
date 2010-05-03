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
#include "VisualCross.h"
#include "NBMath.h"
#include "NogginStructs.h"

/**
 * @brief Class to hold the informations associated with the observation of a landmark
 */
class Observation
{
public:
    // Construcotrs & Destructors
    Observation(VisualFieldObject &_object);
    Observation(VisualCross &_cross);
    Observation(const VisualCorner &_corner);
    Observation(const VisualLine &_line);
    Observation(int _ID = -1, float _visDist = 0.0, float _visBearing = 0.0,
                float _distSD = 0.0, float _bearingSD = 0.0,
                bool _line_truth = false);
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
     * @return true if the observed object is a line
     */
    const bool isLine() const { return line_truth; }

    /*
     * @return The number of possibilities for the landmark
     */
    const unsigned int getNumPossibilities() const { return numPossibilities; }

	const bool isAmbiguous() const { return numPossibilities != 1; }

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
    void addPointPossibility(PointLandmark p);
    void addLinePossibility(LineLandmark l);
	void setPointPossibility(PointLandmark p){
		pointPossibilities.clear();
		pointPossibilities.push_back(p);
	}
	void setLinePossibility(LineLandmark l){
		linePossibilities.clear();
		linePossibilities.push_back(l);
	}

    // Helper functions
    friend std::ostream& operator<< (std::ostream &o, const Observation &c) {
        return o << "Obs " << c.id << ": (" << c.visDist << ", " << c.visBearing
                 << ", " << c.sigma_d << ", " << c.sigma_b << ")";
    }
    static const bool isLineID(int toTest) {
        return (toTest >= 50 && toTest <= 68);
    }

private:
    // Vision information
    float visDist;
    float visBearing;
    float sigma_d;
    float sigma_b;

    // Identity information
    int id;
    bool line_truth;
    std::vector<LineLandmark> linePossibilities;
    std::vector<PointLandmark> pointPossibilities;
    unsigned int numPossibilities;
};
#endif // _Observation_h_DEFINED
