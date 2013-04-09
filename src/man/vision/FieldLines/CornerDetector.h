#ifndef _CORNERDETECTOR_H_
#define _CORNERDETECTOR_H_

#include <vector>
#include "HoughVisualCorner.h"

namespace man {
namespace vision {

class HoughVisualLine;

class CornerDetector
{
public:
    CornerDetector() {}
    virtual ~CornerDetector() {}

    virtual void detect(int upperBound,
                        int * field_edge,
                        const std::vector<HoughVisualLine>& lines);

    std::vector<HoughVisualCorner> getCorners() { return mCorners; }

private:
    std::vector<HoughVisualCorner> mCorners;
};

}
}

#endif /* _CORNERDETECTOR_H_ */
