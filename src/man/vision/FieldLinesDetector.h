#ifndef _FieldLinesDetector_h_DEFINED
#define _FieldLinesDetector_h_DEFINED

#include <list>

#include "VisualDetector.h"
#include "VisualCorner.h"
#include "VisualLine.h"
#include "EdgeDetector.h"
#include "HoughSpace.h"
#include "Profiler.h"

class FieldLinesDetector : VisualDetector
{
public:
    FieldLinesDetector(boost::shared_ptr<Profiler> p);
    virtual ~FieldLinesDetector() { };

    // VisualDetector interface
    virtual void detect(int upperBound, const uint16_t *img);

    // FieldLines interface
    std::list<VisualLine> getLines() const {
        return std::list<VisualLine>();
    }

    std::list<VisualCorner> getCorners() const {
        return std::list<VisualCorner>();
    }

    // Parameter Interface
    void setEdgeThreshold(int thresh);
    void setHoughAcceptThreshold(int thresh);

    Gradient getEdges() const { return gradient; }
    std::list<HoughLine> getHoughLines() const { return houghLines; }

private:
    void findHoughLines(int upperBound, const uint16_t *img);
    void findFieldLines();

private:
    EdgeDetector edges;
    HoughSpace hough;
    Gradient gradient;
    std::list<HoughLine> houghLines;
};

#endif /* _FieldLinesDetector_h_DEFINED */
