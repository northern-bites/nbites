/**
 * Observation.cpp - The landmark observation class. Here we house all those
 * things needed to describe a single landmark sighting.  Observations can be of
 * field objects, corners (line intersections), lines, and possibly other
 * things in the future (i.e. carpet edges)
 *
 * @author Tucker Hermans
 */

#include "Observation.h"

/**
 * @param fo FieldObject that was seen and reported.
 */
Observation::Observation(FieldObject &_object)
{
    // We aren't a line
    line_truth = false;
    // pointPossibilities = ;
    // numPossibilities = ;
}

/**
 * @param c Corner that was seen and reported.
 */
Observation::Observation(VisualCorner &_corner)
{
    // We aren't a line
    line_truth = false;

    // Get basic vision information
    visDist = _corner.getDistance();
    visBearing = _corner.getBearing();
    sigma_d = _corner.getDistanceSD();
    sigma_b = _corner.getBearingSD();

    // Build our possibilitiy list
    numPossibilities = 0;

    list <const ConcreteCorner *>::iterator theIterator;
    list <const ConcreteCorner *> cornerList = _corner.getPossibleCorners();
    for( theIterator = cornerList.begin(); theIterator != cornerList.end();
         ++theIterator) {
        PointLandmark cornerLandmark;
        cornerLandmark.x = (*theIterator).getFieldX();
        cornerLandmark.y = (*theIterator).getFieldY();
        pointPossibilities.push_back(cornerLandmark);
        ++numPossibilities;
    }
}

/**
 * @param l Line that was seen and reported.
 */
Observation::Observation(VisualLine &_line)
{
    // We're a line
    line_truth = true;

    // Get basic vision information
    visDist = _corner.getDistance();
    visBearing = _corner.getBearing();
    sigma_d = _corner.getDistanceSD();
    sigma_b = _corner.getBearingSD();

    // Build our possibilitiy list
    numPossibilities = 0;

    list <const ConcreteLine *>::iterator theIterator;
    list <const ConcreteLine *> lineList = _line.getPossibleLines();
    for( theIterator = lineList.begin(); theIterator != lineList.end();
         ++theIterator) {
        LineLandmark addLine;
        cornerLandmark.x = (*theIterator).getFieldX();
        cornerLandmark.y = (*theIterator).getFieldY();
        linePossibilities.push_back(addLine);
        ++numPossibilities;
    }

}


/**
 * Deconstructor for our observation
 */
virtual Observation::~Observation()
{
    if (line_truth) {
    } else {
    }
}
