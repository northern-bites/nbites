#ifndef _HOUGHVISUALLINE_H_
#define _HOUGHVISUALLINE_H_

#include "geom/HoughLine.h"

class HoughVisualLine
{
public:
    HoughVisualLine(const HoughLine& a, const HoughLine& b) :
        mLines(a, b)
    {}

    virtual ~HoughVisualLine() {}

    std::pair<HoughLine, HoughLine> getHoughLines() const { return mLines; }
    bool intersects(const HoughVisualLine& other, point<int>& out) const;

private:
    std::pair<HoughLine, HoughLine> mLines;
};

#endif /* _HOUGHVISUALLINE_H_ */
