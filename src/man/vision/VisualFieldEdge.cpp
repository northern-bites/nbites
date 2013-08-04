/* This class contains a struct with three values.  These values are the
   perceived distance to the edge of the field from x = IMAGE_WIDTH / 4,
   x = IMAGE_WIDTH / 2 and x =  3 * IMAGE_WIDTH / 4.  Normally these are
   based on pixEstimated distances to the convex hull calculated in Field.cpp
   In some cases, however, it cannot be calculated (e.g. the robot is looking
   down and the field extends to the top of the image.  In these cases
   a value of UNKNOWN_DISTANCE will be returned.
 */

#include "VisualFieldEdge.h"

using namespace std;

namespace man {
namespace vision {

VisualFieldEdge::VisualFieldEdge() {
	distances.left = UNKNOWN_DISTANCE;
	distances.center = UNKNOWN_DISTANCE;
	distances.right = UNKNOWN_DISTANCE;
}

Basic_Shape VisualFieldEdge::getShape() {
	if (distances.left < distances.center) {
		if (distances.left < distances.right) {
			return RISING_RIGHT;
		} else {
			return WEDGE;
		}
	} else if (distances .left > distances.center) {
		if (distances.right < distances.center) {
			return RISING_LEFT;
		} else {
			return FLAT;
		}
	}
	return FLAT;
}

}
}
