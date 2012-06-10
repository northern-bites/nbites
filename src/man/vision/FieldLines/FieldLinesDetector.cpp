#include "FieldLinesDetector.h"
#include <stdio.h>
#include <boost/make_shared.hpp>
#include "HoughSpace.h"
#include "EdgeDetector.h"
#include "Gradient.h"

using namespace std;
using boost::shared_ptr;

FieldLinesDetector::FieldLinesDetector() :
    mEdges(new EdgeDetector),
    mGradient(new Gradient),
    mHoughLines()
{
    mHough = HoughSpace::create();
}

/**
 * Detect field lines and their intersections (aka corners) in the given image
 */
void FieldLinesDetector::detect(int upperBound,
                                int* field_edge,
                                const uint16_t *img)
{
    // For safety (in case horizon is too low), scan from above the
    // given upperbound
    upperBound -= 10;

    // Only use values within the image
    upperBound = min(max(0, upperBound), IMAGE_HEIGHT-3);

    findHoughLines(upperBound, field_edge, img);
    findFieldLines();
}

/**
 * Computes the gradient over the Y Channel of the image and
 * runs a hough transform to find all the pairs of hough space lines
 * in an image.
 *
 * Side effects: Updates gradient with current image's gradient values,
 *               updates list of hough space lines
 */
void FieldLinesDetector::findHoughLines(int upperBound,
                                        int* field_edge,
                                        const uint16_t *img)
{
    mGradient->reset();
    mEdges->detectEdges(upperBound, field_edge, img, *mGradient);
    mHoughLines = mHough->findLines(*mGradient);
}

/**
 * Using the found hough lines and the gradient image, find the actual
 * field lines in the image.
 */
void FieldLinesDetector::findFieldLines()
{
    mLines.clear();
    list<pair<HoughLine, HoughLine> >::const_iterator hl;
    for(hl = mHoughLines.begin(); hl != mHoughLines.end(); ++hl){
        mLines.push_back(HoughVisualLine(hl->first, hl->second));
    }
}

void FieldLinesDetector::setEdgeThreshold(int thresh)
{
    mEdges->setThreshold(static_cast<uint8_t>(thresh));
}

void FieldLinesDetector::setHoughAcceptThreshold(int thresh)
{
    mHough->setAcceptThreshold(thresh);
}

list<HoughLine> FieldLinesDetector::getHoughLines() const
{
    list<HoughLine> lines;
    list<pair<HoughLine, HoughLine> >::const_iterator i;
    for(i = mHoughLines.begin(); i != mHoughLines.end(); ++i){
        lines.push_back(i->first);
        lines.push_back(i->second);
    }
    return lines;
}
