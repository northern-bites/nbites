/**
 * Observation.cpp - The landmark observation class. Here we house all those
 * things needed to describe a single landmark sighting.  Observations can be of
 * field objects, corners (line intersections), lines, and possibly other
 * things in the future (i.e. carpet edges)
 *
 * @author Tucker Hermans
 */

#include "Observation.h"
using namespace std;

/**
 * @param fo FieldObject that was seen and reported.
 */
template <class VisualT,class ConcreteT,class LandmarkT>
Observation<VisualT, ConcreteT, LandmarkT>::Observation(const VisualT &_object) :
    visDist(_object.getDistance()), visBearing(_object.getBearing()),
    sigma_d(_object.getDistanceSD()), sigma_b(_object.getBearingSD()),
    id(_object.getID())
{
    typename list<const ConcreteT *>::const_iterator i;
    const list <const ConcreteT *> * objList =
        _object.getPossibleFieldObjects();
    for( i = objList->begin(); i != objList->end(); ++i) {
        LandmarkT objectLandmark((**i).getFieldX(),
                                     (**i).getFieldY());
        possibilities.push_back(objectLandmark);
    }
}

// Observation::Observation(int _ID, float _visDist,
//                          float _visBearing, float _distSD,
//                          float _bearingSD) :
//     visDist(_visDist), visBearing(_visBearing), sigma_d(_distSD),
//     sigma_b(_bearingSD), id(_ID), numPossibilities(0)
// {
// }

/**
 * Deconstructor for our observation
 */
template <class VisualT,class ConcreteT,class LandmarkT>
Observation<VisualT, ConcreteT, LandmarkT>::~Observation() {}
