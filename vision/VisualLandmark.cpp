#include "VisualLandmark.h"


VisualLandmark::VisualLandmark(certainty _idCertainty,
                               distanceCertainty _distanceCertainty,
                               ConcreteLandmark* _concreteLandmark)
    : idCertainty(_idCertainty), distanceCert(_distanceCertainty),
      concreteLandmark(_concreteLandmark) {
}


// Pairwise copy the member attributes of other
VisualLandmark::VisualLandmark(const VisualLandmark& other)
    : idCertainty(other.idCertainty), distanceCert(other.distanceCert),
      concreteLandmark(other.concreteLandmark) {
}

// No memory needs to be freed
VisualLandmark::~VisualLandmark() {}
