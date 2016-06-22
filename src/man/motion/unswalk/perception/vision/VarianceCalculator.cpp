#include "VarianceCalculator.hpp"
#include "utils/basic_maths.hpp"
#include "utils/angles.hpp"
#include <cmath>

using namespace std;

void VarianceCalculator::setVariance(vector<BallInfo> &balls) {
   for (vector<BallInfo>::iterator ball = balls.begin(); ball != balls.end(); ++ball) {
      setDistanceHeadingVariance(ball->rr);
   }
}

void VarianceCalculator::setVariance(vector<PostInfo> &posts) {
   for (vector<PostInfo>::iterator post = posts.begin(); post != posts.end(); ++post) {
      setDistanceHeadingVariance(post->rr);
   }
}

void VarianceCalculator::setVariance(vector<FieldFeatureInfo> &features) {
   for (vector<FieldFeatureInfo>::iterator feature = features.begin(); feature != features.end(); ++feature) {
      setDistanceHeadingOrientationVariance(feature->rr);
   }
}
void VarianceCalculator::setDistanceHeadingVariance(RRCoord &rr) {
   int camera_height = 500;
   float phi = atan(rr.distance() / camera_height);
   rr.var.setZero();
   rr.var(0,0) = SQUARE(500 + camera_height*(tan(phi + UNSWDEG2RAD(3)) - tan(phi)));
   rr.var(1,1) = SQUARE(UNSWDEG2RAD(10));
}

void VarianceCalculator::setDistanceHeadingOrientationVariance(RRCoord &rr) {
   setDistanceHeadingVariance(rr);
   rr.var(2,2) = SQUARE(UNSWDEG2RAD(4));
}
