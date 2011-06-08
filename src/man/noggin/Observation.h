/**
 * Observation.h
 *
 * @author Tucker Hermans
 * @version %I%
 */

#ifndef Observation_h_DEFINED
#define Observation_h_DEFINED
#include <vector>
#include <list>

#include "NBMath.h"
#include "NogginStructs.h"

class ConcreteCorner;
class ConcreteLandmark;
class VisualCorner;

/**
 * @brief Class to hold the informations associated with the observation of a landmark
 */
template<class VisualT, class ConcreteT, class LandmarkT>
class Observation
{
public:
    Observation(const VisualT &_object);
    // Observation(int _ID = -1, float _visDist = 0.0, float _visBearing = 0.0,
    //             float _distSD = 0.0, float _bearingSD = 0.0);
    virtual ~Observation();

    // Core Functions

    // Visual information Getters
    float getVisDistance() const    { return visDist;              }
    float getVisBearing() const     { return visBearing;           }
    float getVisBearingDeg() const  { return TO_DEG * visBearing;  }
    float getDistanceSD() const     { return sigma_d;              }
    float getBearingSD() const      { return sigma_b;              }
    int getID() const               { return id;                   }
    int getNumPossibilities() const { return possibilities.size(); }
    bool isAmbiguous() const        { return  getNumPossibilities() > 1; }
    std::vector<LandmarkT> getPossibilities() const {
        return possibilities;
    }
    RangeBearingMeasurement getRangeBearingMeasurement() const {
        return RangeBearingMeasurement(visDist, visBearing, sigma_d, sigma_b);
    }

    // Setters
    void setVisDistance(float _d)  { visDist = _d;    }
    void setVisBearing(float _b)   { visBearing = _b; }
    void setDistanceSD(float _sdD) { sigma_d = _sdD;  }
    void setBearingSD(float _sdB)  { sigma_b = _sdB;  }
    void setID(int _id)            { id = _id;        }
    void addPossibility(PointLandmark p){
        possibilities.push_back(p);
    }
    void setPossibility(PointLandmark p){
        possibilities.clear();
        possibilities.push_back(p);
    }

    // Helper functions
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
    std::vector<LandmarkT> possibilities;
};

typedef Observation<VisualDetection,
                    ConcreteLandmark,
                    PointLandmark> PointObservation;

typedef Observation<VisualCorner,
                    ConcreteCorner,
                    CornerLandmark> CornerObservation;

#endif // _Observation_h_DEFINED
