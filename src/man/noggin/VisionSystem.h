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

/**
 * Holds a known landmark.
 */
struct Landmark
{
    Landmark(float X = 0.0f, float Y = 0.0f, std::string w = "UNKNOWN")
    : x(X), y(Y), what(w)
    { }
    
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

 private:
    LandmarkMap map;
};

#endif // VISION_SYSTEM_H
