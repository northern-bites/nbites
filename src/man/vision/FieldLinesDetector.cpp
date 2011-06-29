#include "FieldLinesDetector.h"

using namespace std;
using boost::shared_ptr;

FieldLinesDetector::FieldLinesDetector() :
    VisualDetector(), edges(), hough(), gradient(new Gradient()),
    houghLines()
{

}

/**
 * Detect field lines and their intersections (aka corners) in the given image
 */
void FieldLinesDetector::detect(const uint16_t *img)
{
    findHoughLines(img);
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
void FieldLinesDetector::findHoughLines(const uint16_t *img)
{
    gradient->reset();
    edges.detectEdges(img, gradient);
    // houghLines = hough.findLines(gradient);
}

/**
 * Using the found hough lines and the gradient image, find the actual
 * field lines in the image.
 */
void FieldLinesDetector::findFieldLines()
{

}

void FieldLinesDetector::setEdgeThreshold(int thresh)
{
    edges.setThreshold(static_cast<uint8_t>(thresh));
}

void FieldLinesDetector::setHoughAcceptThreshold(int thresh)
{
    hough.setAcceptThreshold(thresh);
}
