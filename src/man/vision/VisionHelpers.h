#ifndef VisionHelpers_h_defined
#include <cstdlib>

namespace man {
namespace vision {

inline int ROUND2(float x) {
	return static_cast<int>( std::floor(x + 0.5f) );
}

/* Calculate the euclidian distance between two points.
 * @param x    x value of point 1
 * @param y    y value of point 1
 * @param x1   x value of point 2
 * @param y1   y value of point 2
 * @return      the distance between the objects
 */
inline float dist(int x, int y, int x1, int y1) {
    return std::sqrt(static_cast<float>(std::abs(x - x1) * std::abs(x - x1) +
										std::abs(y - y1) * std::abs(y - y1) ));
}

/* Finds and returns the midpoint of two numbers.
 * @param a   one number
 * @param b   the other
 * @return    the number halfway between (as int)
 */
inline int midPoint(int a, int b) {
    return a + (b - a) / 2;
}

}
}

#define VisionHelpers_h_defined
#endif // VisionHelpers_h_defined
