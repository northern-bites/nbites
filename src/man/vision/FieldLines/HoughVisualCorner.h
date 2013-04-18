#ifndef _HOUGHVISUALCORNER_H_
#define _HOUGHVISUALCORNER_H_

#include "Structs.h"

namespace man {
namespace vision {

class HoughVisualCorner
{
public:
    HoughVisualCorner(int x, int y) : mImagePoint(x,y) {}
    HoughVisualCorner(const point<int>& pt) : mImagePoint(pt) {}
    virtual ~HoughVisualCorner() {}

    point<int> getImageLocation() const { return mImagePoint; }

private:
    point<int> mImagePoint;
};

}
}

#endif /* _HOUGHVISUALCORNER_H_ */
