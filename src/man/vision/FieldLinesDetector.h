#ifndef _FieldLinesDetector_h_DEFINED
#define _FieldLinesDetector_h_DEFINED

#include <list>
#include <vector>

#include "EdgeDetector.h"
#include "geom/HoughLine.h"
#include "HoughSpace.h"
#include "Profiler.h"
#include "VisualCorner.h"
#include "VisualDetector.h"
#include "VisualLine.h"

class FieldLinesDetector : VisualDetector
{
public:
    FieldLinesDetector();
    virtual ~FieldLinesDetector() { };

    // VisualDetector interface
    virtual void detect(int upperBound,
                        int* field_edge,
                        const uint16_t *img);

    // Parameter Interface
    void setEdgeThreshold(int thresh);
    void setHoughAcceptThreshold(int thresh);

    Gradient* getEdges() { return &gradient; }
    std::list<HoughLine> getHoughLines() const;

    // FieldLines interface
    std::vector<VisualLine>* getLines() {
        return &lines;
    };
    std::list<VisualCorner>* getCorners() {
        return &corners;
    }

private:
    void findHoughLines(int upperBound,
                        int* field_edge,
                        const uint16_t *img);
    void findFieldLines();

private:
    EdgeDetector edges;
    Gradient gradient;

    std::list<std::pair<HoughLine, HoughLine> > houghLines;
    std::vector<VisualLine> lines;
    std::list<VisualCorner> corners;
    boost::shared_ptr<HoughSpace> hough;
};

#endif /* _FieldLinesDetector_h_DEFINED */
