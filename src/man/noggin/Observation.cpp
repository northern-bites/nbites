/**
 * Observation.cpp - The landmark observation class. Here we house all those
 * things needed to describe a single landmark sighting.  Observations can be of
 * field objects, corners (line intersections), lines, and possibly other
 * things in the future (i.e. carpet edges)
 *
 * @author Tucker Hermans
 */

#include "Observation.h"
#include "VisualCorner.h"
using namespace std;

// template <class VisualT,class ConcreteT,class LandmarkT>
// Observation<VisualT, ConcreteT, LandmarkT>::~Observation() {}
CornerObservation::CornerObservation(const VisualCorner& _c) :
    Observation<VisualCorner, ConcreteCorner, CornerLandmark>(_c),
    visOrientation(_c.getPhysicalOrientation()),
    sigma_o(_c.getPhysicalOrientationSD())
{

}
