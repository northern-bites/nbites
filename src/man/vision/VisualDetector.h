#ifndef _VisualDetector_h_DEFINED
#define _VisualDetector_h_DEFINED

#include <stdint.h>

namespace man {
namespace vision {
/**
 * Base class for visual object detectors.
 *
 * Provides simple interface:
 *
 *    - detect(image): Detects objects (type
 *      detected depends on derived type) in the given image
 *
 *    - identify(Context): Identifies the objects in an image given all the
 *                         context of that image
 *
 */
class VisualDetector
{
public:
    VisualDetector() { };
    virtual ~VisualDetector() { };

    virtual void detect(int upperBound,
                        int * field_edge,
                        const uint16_t * image) = 0;

    // @TODO: Implement!
    // virtual void identify(Context *context) = 0;
};

}
}

#endif /* _VisualDetector_h_DEFINED */
