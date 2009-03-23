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
Observation::Observation(VisualFieldObject &_object) :
    visDist(_object.getDistance()), visBearing(_object.getBearing()),
    sigma_d(_object.getDistanceSD()), sigma_b(_object.getBearingSD()),
    id(_object.getID()), line_truth(false)
{

    // Initialize to 0 possibilities
    numPossibilities = 0;

    list <const ConcreteFieldObject *>::const_iterator theIterator;
    const list <const ConcreteFieldObject *> * objList =
        _object.getPossibleFieldObjects();
    for( theIterator = objList->begin(); theIterator != objList->end();
         ++theIterator) {
        PointLandmark objectLandmark((**theIterator).getFieldX(),
                                     (**theIterator).getFieldY());
        pointPossibilities.push_back(objectLandmark);
        ++numPossibilities;
    }
}

/**
 * @param c Corner that was seen and reported.
 */
Observation::Observation(const VisualCorner &_corner) :
    visDist(_corner.getDistance()), visBearing(_corner.getBearing()),
    sigma_d(_corner.getDistanceSD()), sigma_b(_corner.getBearingSD()),
    id(_corner.getID()), line_truth(false)
{
    // Build our possibilitiy list
    numPossibilities= 0;

    list <const ConcreteCorner *>::iterator theIterator;
    list <const ConcreteCorner *> cornerList = _corner.getPossibleCorners();
    for( theIterator = cornerList.begin(); theIterator != cornerList.end();
         ++theIterator) {
        PointLandmark cornerLandmark((**theIterator).getFieldX(),
                                     (**theIterator).getFieldY());
        pointPossibilities.push_back(cornerLandmark);
        ++numPossibilities;
    }
}

/**
 * @param l Line that was seen and reported.
 */
Observation::Observation(const VisualLine &_line) :
    visDist(_line.getDistance()), visBearing(_line.getBearing()),
    sigma_d(_line.getDistanceSD()), sigma_b(_line.getBearingSD()),
    // id(_line.getID()),
    line_truth(true)
{
    // Build our possibilitiy list
    numPossibilities = 0;

    list <const ConcreteLine *>::iterator theIterator;
    list <const ConcreteLine *> lineList = _line.getPossibleLines();
    for( theIterator = lineList.begin(); theIterator != lineList.end();
         ++theIterator) {
        LineLandmark addLine((**theIterator).getFieldX1(),
                             (**theIterator).getFieldY1(),
                             (**theIterator).getFieldX1(),
                             (**theIterator).getFieldY1());
        linePossibilities.push_back(addLine);
        ++numPossibilities;
    }

}

/**
 * Deconstructor for our observation
 */
Observation::~Observation() {}
