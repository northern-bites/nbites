#pragma once

#include "utils/SPLDefs.hpp"
#include "utils/angles.hpp"

/* Useful, but not in math.h */
#define PI_8 (M_PI_4 / 2.0)

/* Don't allow robot to exceed these coordinates */
#define FIELD_X_CLIP ((FIELD_LENGTH / 2.0f) + FIELD_LENGTH_OFFSET / 2.0f)
#define FIELD_Y_CLIP ((FIELD_WIDTH / 2.0f) + FIELD_WIDTH_OFFSET / 2.0f)

/* Max number of other robots on the field */
#define MAX_ROBOT_OBSTACLES 7

#define OFFNAO_TEAM TEAM_BLUE

/* Dimensions that are filtered by the KF */
typedef enum {
   X = 0,
   Y,
   THETA,
   STATE_VEC_DIM
} StateVector;

// FIELD_LENGTH away -- more likely to be on the field, more dynamic!!!
#define FIELD_DIAGONAL \
   sqrt(SQUARE(FULL_FIELD_WIDTH) + SQUARE(FULL_FIELD_LENGTH))

#define IS_VALID_DIST(dist) \
   (dist <= FIELD_DIAGONAL)

// Number of domensions in the main distribution. Includes robot pose, ball pos, ball vec, and
// robot poses of our 4 teammates.
#define MAIN_DIM 19

// Number of dimensions in the shared distribution. Includes the robot pose, ball pos, ball vel.
#define SHARED_DIM 7

/* Returns the minimum heading between 2 angles given in radians */
/* TODO(yanjinz) refactorme! */
static inline float minHeadingDiff(float thetaA, float thetaB) {
   float minTheta = MIN_ANGLE_360(RAD2DEG(thetaA),
                                  RAD2DEG(thetaB));
   if (minTheta < -180) minTheta += 360;
   if (minTheta > 180) minTheta -= 360;
   return minTheta;
}

#define LOCALISATION_DEBUG false