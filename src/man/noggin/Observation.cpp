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
    id(_object.getID()), line_truth(false), numPossibilities(0)
{
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
    id(_corner.getID()), line_truth(false),  numPossibilities(0)
{
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
 * @param cross VisualCros that was seen and reported.
 */
Observation::Observation(VisualCross &_cross) :
    visDist(_cross.getDistance()), visBearing(_cross.getBearing()),
    sigma_d(_cross.getDistanceSD()), sigma_b(_cross.getBearingSD()),
    id(_cross.getID()), line_truth(false), numPossibilities(0)
{
    list <const ConcreteCross *>::const_iterator theIterator;
    const list <const ConcreteCross *> * objList =
        _cross.getPossibleCrosses();
    for( theIterator = objList->begin(); theIterator != objList->end();
         ++theIterator) {
        PointLandmark objectLandmark((**theIterator).getFieldX(),
                                     (**theIterator).getFieldY());
        pointPossibilities.push_back(objectLandmark);
        ++numPossibilities;
    }
}

/**
 * @param l Line that was seen and reported.
 */
Observation::Observation(const VisualLine &_line) :
    visDist(_line.getDistance()), visBearing(_line.getBearing()),
    sigma_d(_line.getDistanceSD()), sigma_b(_line.getBearingSD()),
    id(_line.getID()), line_truth(true), numPossibilities(0)
{
    // Build our possibilitiy list

    list <const ConcreteLine *>::iterator theIterator;
    list <const ConcreteLine *> lineList = _line.getPossibleLines();
    for( theIterator = lineList.begin(); theIterator != lineList.end();
         ++theIterator) {
        LineLandmark addLine((**theIterator).getFieldX1(),
                             (**theIterator).getFieldY1(),
                             (**theIterator).getFieldX2(),
                             (**theIterator).getFieldY2());
        linePossibilities.push_back(addLine);
        ++numPossibilities;
    }

}
Observation::Observation(int _ID, float _visDist,
                         float _visBearing, float _distSD,
                         float _bearingSD, bool _line_truth) :
    visDist(_visDist), visBearing(_visBearing), sigma_d(_distSD),
    sigma_b(_bearingSD), id(_ID), line_truth(_line_truth), numPossibilities(0)
{
}

/**
 * Deconstructor for our observation
 */
Observation::~Observation() {}

void Observation::addPointPossibility(PointLandmark p)
{
    pointPossibilities.push_back(p);
    ++numPossibilities;
}
void Observation::addLinePossibility(LineLandmark l)
{
    linePossibilities.push_back(l);
    ++numPossibilities;
}
