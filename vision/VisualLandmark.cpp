#include "VisualLandmark.h"


VisualLandmark::VisualLandmark(int _x, int _y, float _distance,
                               float _bearing,
                               certainty _idCertainty,
                               certainty _distanceCertainty,
                               ConcreteLandmark* _concreteLandmark) {
    x = _x;
    y = _y;
    distance = _distance;
    bearing = _bearing;
    distanceSD =0.0f; // NEED TO REDEFINE THIS BASED ON A VARIANCE FUNCTION!!!
    bearingSD = 0.0f; // NEED TO REDEFINE THIS BASED ON A VARIANCE FUNCTION!!!
    idCertainty = _idCertainty;
    distanceCertainty = _distanceCertainty;
    concreteLandmark = _concreteLandmark;
}


// Pairwise copy the member attributes of other
VisualLandmark::VisualLandmark(const VisualLandmark& other) {
    x = other.x;
    y = other.y;
    distance = other.distance;
    bearing = other.bearing;
    distanceSD = other.distanceSD;
    bearingSD = other.bearingSD;
    concreteLandmark = other.concreteLandmark;
    idCertainty = other.idCertainty;
    distanceCertainty = other.distanceCertainty;
}

// No memory needs to be freed
VisualLandmark::~VisualLandmark() {}
