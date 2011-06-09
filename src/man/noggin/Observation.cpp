#include "Observation.h"

template <>
void Observation<CornerLandmark>::initPossibilities(const VisualObject& _object)
{
    std::list<const ConcreteLandmark *>::const_iterator i;
    const std::list <const ConcreteLandmark *> * objList =
        _object.getPossibilities();
    for( i = objList->begin(); i != objList->end(); ++i) {
        CornerLandmark objectLandmark((**i).getFieldX(),
                                      (**i).getFieldY(),
                                      (**i).getFieldAngle());
        possibilities.push_back(objectLandmark);
    }
}



