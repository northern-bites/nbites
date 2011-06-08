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
#include "VisualObject.h"

class ConcreteCorner;
class ConcreteLandmark;
class ConcreteFieldObject;
class VisualCorner;
class VisualFieldObject;


/**
 * @brief Class to hold the informations associated with the
 *        observation of a landmark
 */
template<class VisualT, class ConcreteT, class LandmarkT>
class Observation
{
public:
    Observation(const VisualT &_object);
    // Observation(int _ID = -1, float _visDist = 0.0, float _visBearing = 0.0,
    //             float _distSD = 0.0, float _bearingSD = 0.0);
    virtual ~Observation() { }

    // Core Functions

    // Visual information Getters
    float getVisDistance()    const { return visDist;                    }
    float getVisBearing()     const { return visBearing;                 }
    float getVisBearingDeg()  const { return TO_DEG * visBearing;        }
    float getDistanceSD()     const { return sigma_d;                    }
    float getBearingSD()      const { return sigma_b;                    }
    int getID()               const { return id;                         }
    int getNumPossibilities() const { return possibilities.size();       }
    bool isAmbiguous()        const { return  getNumPossibilities() > 1; }
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
    void addPossibility(LandmarkT p){
        possibilities.push_back(p);
    }
    void setPossibility(LandmarkT p){
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
    float visDist, visBearing;
    float sigma_d, sigma_b;

    // Identity information
    int id;
    std::vector<LandmarkT> possibilities;
};

class CornerObservation : public Observation<VisualCorner,
                                             ConcreteCorner,
                                             CornerLandmark>
{
public:
    CornerObservation(const VisualCorner& _c);
    virtual ~CornerObservation() { }

private:
    float visOrientation;
    float sigma_o;
};

/**
 * @param fo FieldObject that was seen and reported.
 */
template <class VisualT,class ConcreteT,class LandmarkT>
Observation<VisualT, ConcreteT, LandmarkT>::
Observation(const VisualT &_object) :
    visDist(_object.getDistance()), visBearing(_object.getBearing()),
    sigma_d(_object.getDistanceSD()), sigma_b(_object.getBearingSD()),
    id(_object.getID())
{
    typename std::list<const ConcreteT *>::const_iterator i;
    const std::list <const ConcreteT *> * objList =
        _object.getPossibilities();
    for( i = objList->begin(); i != objList->end(); ++i) {
        LandmarkT objectLandmark((**i).getFieldX(),
                                 (**i).getFieldY());
        possibilities.push_back(objectLandmark);
    }
}


typedef Observation<VisualObject,
                    ConcreteLandmark,
                    PointLandmark> PointObservation;

#endif // _Observation_h_DEFINED
