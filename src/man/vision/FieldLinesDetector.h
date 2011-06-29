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
    FieldLinesDetector();
    virtual ~FieldLinesDetector() { };

    // VisualDetector interface
    virtual void detect(const uint16_t *img);

    // FieldLines interface
    std::list<VisualLine> getLines(){ return std::list<VisualLine>(); }
    std::list<VisualCorner> getCorners(){ return std::list<VisualCorner>(); }

    // Parameter Interface
    void setEdgeThreshold(int thresh);
    void setHoughAcceptThreshold(int thresh);

private:
    void findHoughLines(const uint16_t *img);
    void findFieldLines();

private:
    EdgeDetector edges;
    HoughSpace hough;
    boost::shared_ptr<Gradient> gradient;
    std::list<HoughLine> houghLines;
};

#endif /* _FieldLinesDetector_h_DEFINED */
