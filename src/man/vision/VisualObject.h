#ifndef _VisualObject_h_DEFINED
#define _VisualObject_h_DEFINED

#include "VisualDetection.h"
#include "VisualLandmark.h"

namespace man {
namespace vision {

class VisualObject : public VisualLandmark, public VisualDetection
{
public:
    VisualObject(int _id) : VisualLandmark(_id),
                               VisualDetection() { }
    VisualObject(int _id, int _x, int _y,
                 float _distance, float _bearing) :
        VisualLandmark(_id),
        VisualDetection(_x,_y,_distance,_bearing) { }

    VisualObject(const VisualObject& other) :
        VisualLandmark(other), VisualDetection(other) { }

    virtual ~VisualObject(){ }
};

}
}

#endif /* _VisualObject_h_DEFINED */
