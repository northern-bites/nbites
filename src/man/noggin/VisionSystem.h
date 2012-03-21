/**
 * Implements an interface between localization and the vision
 * system to apply visual landmark measurements to the 
 * particles. 
 *
 * @author Ellis Ratner <eratner@bowdoin.edu>
 */
#ifndef VISION_SYSTEM_H
#define VISION_SYSTEM_H

#include <string>
#include "ParticleFilter.h"
#include "ConcreteFieldObject.h"

/**
 * Holds a known landmark.
 */
struct Landmark
{
    Landmark(float X = 0.0f, float Y = 0.0f, std::string w = "UNKNOWN")
    : x(X), y(Y), what(w)
    { }

    /**
     * Constructs a Landmark from a ConcreteFieldObject.
     */
    Landmark(const ConcreteFieldObject * fieldObject)
    {
        x    = fieldObject->getFieldX();
	y    = fieldObject->getFieldY();
        what = fieldObject->toString();
    }
    
    float x;
    float y;
    std::string what;

    friend std::ostream& operator<<(std::ostream& out, Landmark l)
    {
	out << "Landmark \"" << l.what << "\" at (" << l.x << ", "
	    << l.y << ") \n";
	return out;
    }
};

/**
 * Helper function to construct a list of Landmark possibilities from
 * a VisualFieldObject. 
 * @param fieldObjects
 * @return a vector of landmarks.
 */
std::vector<Landmark> constructLandmarks(VisualFieldObject fieldObject)
{
    std::vector<Landmark> landmarks;

    const std::list<const ConcreteFieldObject *> * possibilities = 
      fieldObject.getPossibilities();

    std::list<const ConcreteFieldObject *>::const_iterator iter;
    for(iter = possibilities.begin(); iter != possibilities.end(); ++iter) 
    {
        // Construct landmarks from possibilities.
        Landmark l(*iter);
	landmarks.push_back(l);
    }

    return landmarks;
}

/**
 * Holds a single observation.
 */
struct Observation
{
    Observation(std::vector<Landmark> p, float dist = 0.0f, float theta = 0.0f)
    : possibilities(p), distance(dist), angle(theta)
    { }

    bool isAmbiguous() const { return possibilities.size() > 1 ? true : false; }

    std::vector<Landmark> possibilities;
    float distance;
    float angle;
};

typedef std::vector<Landmark> LandmarkMap;

/**
 * @class VisionSystem
 */
class VisionSystem : public PF::SensorModel
{
 public:
    VisionSystem(LandmarkMap m);
    ~VisionSystem() { }

    PF::ParticleSet update(PF::ParticleSet particles);

    bool hasNewObservations() const { return hasNewObservations; }
    
    void feedObservations(std::vector<Observation> newObs);

 private:
    LandmarkMap map;
    bool hasNewObservations;
    std::vector<Observation> currentObservations;
};

#endif // VISION_SYSTEM_H
