#ifndef Kinematics_h_DEFINED
#define Kinematics_h_DEFINED

/**
 * This file is strictly specific to the Nao robot.
 * It contains relevant to the physical configuration of the robot including
 * length of links, number of chains, etc.
 * It also supports the creation of rotation and translation matrices.
 */

#include <exception>
#include <string>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
using namespace boost::numeric;

#include <cmath>

namespace Kinematics {

//Conversion constants

  static const float TO_DEG = 180.0f/M_PI;
#ifndef TO_RAD //also defined in almotionproxy.h
  static const float TO_RAD = M_PI/180.0f;
#endif
  static const float M_TO_CM  = 100.0f;
  static const float CM_TO_M  = 0.01f;
  static const float CM_TO_MM = 10.0f;
  static const float MM_TO_CM = 0.1f;

  enum Axis {
    X_AXIS = 0,
    Y_AXIS,
    Z_AXIS,
    W_AXIS
  };

  // -------------------- Helper matrix methods --------------------

  static const ublas::matrix <float> rotation4D(const Axis axis,
						const float angle) {
    ublas::matrix <float> rot = ublas::identity_matrix <float>(4);

    const float sinAngle = sin(angle);
    const float cosAngle = cos(angle);
    if (angle == 0.0) { //OPTIMIZAION POINT
      return rot;
    }
    switch(axis) {
    case X_AXIS:
      rot(Y_AXIS, Y_AXIS) =  cosAngle;
      rot(Y_AXIS, Z_AXIS) = -sinAngle;
      rot(Z_AXIS, Y_AXIS) =  sinAngle;
      rot(Z_AXIS, Z_AXIS) =  cosAngle;
      break;
    case Y_AXIS:
      rot(X_AXIS, X_AXIS) =  cosAngle;
      rot(X_AXIS, Z_AXIS) =  sinAngle;
      rot(Z_AXIS, X_AXIS) = -sinAngle;
      rot(Z_AXIS, Z_AXIS) =  cosAngle;
      break;
    case Z_AXIS:
      rot(X_AXIS, X_AXIS) =  cosAngle;
      rot(X_AXIS, Y_AXIS) = -sinAngle;
      rot(Y_AXIS, X_AXIS) =  sinAngle;
      rot(Y_AXIS, Y_AXIS) =  cosAngle;
      break;
    default:
      break;
    }
    return rot;
  }

  static const ublas::matrix <float> translation4D(const float dx,
						   const float dy,
						   const float dz) {
    ublas::matrix <float> trans = ublas::identity_matrix <float>(4);
    trans(X_AXIS, W_AXIS) = dx;
    trans(Y_AXIS, W_AXIS) = dy;
    trans(Z_AXIS, W_AXIS) = dz;
    return trans;
  }

  static const ublas::vector <float> vector3D(const float x, const float y,
					      const float z) {
    ublas::vector <float> p = ublas::zero_vector <float> (3);
    p(0) = x;
    p(1) = y;
    p(2) = z;
    return p;
  }

  static const ublas::vector <float> vector4D(const float x, const float y,
					      const float z,
					      const float w = 1.0f) {
    ublas::vector <float> p = ublas::zero_vector <float> (4);
    p(0) = x;
    p(1) = y;
    p(2) = z;
    p(3) = w;
    return p;
  }

  enum ChainID {
    HEAD_CHAIN = 0,
    LARM_CHAIN,
    LLEG_CHAIN,
    RLEG_CHAIN,
    RARM_CHAIN
  };

  /// Joint Name constants ///
  enum JointNames {
    HEAD_YAW = 0,
    HEAD_PITCH,
    // LARM,
    L_SHOULDER_PITCH,
    L_SHOULDER_ROLL,
    L_ELBOW_YAW,
    L_ELBOW_ROLL,
    // LLEG,
    L_HIP_YAW_PITCH,
    L_HIP_ROLL,
    L_HIP_PITCH,
    L_KNEE_PITCH,
    L_ANKLE_PITCH,
    L_ANKLE_ROLL,
    // RLEG,
    R_HIP_YAW_PITCH,
    R_HIP_ROLL,
    R_HIP_PITCH,
    R_KNEE_PITCH,
    R_ANKLE_PITCH,
    R_ANKLE_ROLL,
    // RARM,
    R_SHOULDER_PITCH,
    R_SHOULDER_ROLL,
    R_ELBOW_YAW,
    R_ELBOW_ROLL
  };

  enum Motion_IntFlag {
    UNINT_INTR_CMD, // Un-interruptable interrupter command
    INT_INTR_CMD,   // Interruptable interupter command
    UNINT_CMD,      // Un-interruptable command
    INT_CMD         // Interruptable command
  };

  static const unsigned int HEAD_JOINTS = 2;
  static const unsigned int ARM_JOINTS = 4;
  static const unsigned int LEG_JOINTS = 6;
  static const unsigned int NUM_CHAINS = 5;

  static const unsigned int NUM_JOINTS = HEAD_JOINTS + ARM_JOINTS*2 +
                                                       LEG_JOINTS*2;
  static const unsigned int NUM_BODY_JOINTS = ARM_JOINTS*2 + LEG_JOINTS*2;
  static const unsigned int chain_lengths[NUM_CHAINS] = {2, 4, 6, 6, 4};

  static const std::string CHAIN_STRINGS[NUM_CHAINS] =
    { "Head",
      "LArm",
      "LLeg",
      "RLeg",
      "RArm" };

  /**********    Bodily dimensions     ***********/

  static const float SHOULDER_OFFSET_Y = 98.0f;
  static const float UPPER_ARM_LENGTH = 90.0f;
  static const float LOWER_ARM_LENGTH = 145.0f;
  static const float SHOULDER_OFFSET_Z = 100.0f;
  static const float THIGH_LENGTH = 100.0f;
  static const float TIBIA_LENGTH = 100.0f;
  static const float NECK_OFFSET_Z = 126.5f;
  static const float HIP_OFFSET_Y = 50.0f;
  static const float HIP_OFFSET_Z = 85.0f;
  static const float FOOT_HEIGHT = 46.0f;

  // Camera
  static const float CAMERA_OFF_X = 59.25f; // in millimeters
  static const float CAMERA_OFF_Z = 68.0f;  // in millimeters

  /**********      mDH parameters      ***********/

  enum mDHNames {
    ALPHA = 0,
    L,
    THETA,
    D
  };

  //                                  (alpha,  a ,  theta ,   d  )
  const float HEAD_MDH_PARAMS[2][4] = {{0.0f , 0.0f,  0.0f , 0.0f},
				       {-M_PI/2, 0.0f, -M_PI/2 , 0.0f}};

  const float LEFT_ARM_MDH_PARAMS[4][4] = {{-M_PI/2,0.0f,0.0f,0.0f},
					   { M_PI/2,0.0f,M_PI/2,0.0f},
					   { M_PI/2,0.0f,0.0f,UPPER_ARM_LENGTH},
					   {-M_PI/2,0.0f,0.0f,0.0f}};

  const float LEFT_LEG_MDH_PARAMS[6][4] = {{ -3*M_PI/4, 0.0f,  -M_PI/2, 0.0f},
					   { -M_PI/2,   0.0f,   M_PI/4, 0.0f},
					   { M_PI/2,    0.0f,     0.0f, 0.0f},
					   //{ M_PI/2,-THIGH_LENGTH,0.0f, 0.0f},
					   {   0.0f,-THIGH_LENGTH,0.0f, 0.0f},
					   {   0.0f,-TIBIA_LENGTH,0.0f, 0.0f},
					   {-M_PI/2,    0.0f,     0.0f, 0.0f}};

  const float RIGHT_LEG_MDH_PARAMS[6][4]= {{ -M_PI/4,  0.0f,   -M_PI/2, 0.0f},
					   { -M_PI/2,   0.0f,  -M_PI/4, 0.0f},
					   {  M_PI/2,    0.0f,    0.0f, 0.0f},
					   //{  M_PI/2,-THIGH_LENGTH,0.0f,0.0f},
					   { 0.0f,-THIGH_LENGTH,0.0f, 0.0f},
					   {0.0f,-TIBIA_LENGTH,0.0f,0.0f},
					   {-M_PI/2,0.0f,0.0f,0.0f}};

  const float RIGHT_ARM_MDH_PARAMS[4][4] = {{-M_PI/2, 0.0f,0.0f,0.0f},
					    { M_PI/2, 0.0f,M_PI/2,0.0f},
					    { M_PI/2, 0.0f,0.0f,UPPER_ARM_LENGTH},
					    {-M_PI/2, 0.0f,0.0f,0.0f}};

  static const float* MDH_PARAMS[NUM_CHAINS] = {&HEAD_MDH_PARAMS[0][0],
						&LEFT_ARM_MDH_PARAMS[0][0],
						&LEFT_LEG_MDH_PARAMS[0][0],
						&RIGHT_LEG_MDH_PARAMS[0][0],
						&RIGHT_ARM_MDH_PARAMS[0][0]};

  //Base transforms to get from body center to beg. of chain
  static const ublas::matrix <float> HEAD_BASE_TRANSFORMS[1]
    = { translation4D( 0.0f,
		       0.0f,
		       NECK_OFFSET_Z ) };

  static const ublas::matrix <float> LEFT_ARM_BASE_TRANSFORMS[1]
    = { translation4D( 0.0f,
		       SHOULDER_OFFSET_Y,
		       SHOULDER_OFFSET_Z ) };

  static const ublas::matrix <float> LEFT_LEG_BASE_TRANSFORMS[1]
    ={ translation4D( 0.0f,
		      HIP_OFFSET_Y,
		      -HIP_OFFSET_Z ) };

  static const ublas::matrix <float> RIGHT_LEG_BASE_TRANSFORMS[1]
    ={ translation4D( 0.0f,
		      -HIP_OFFSET_Y,
		      -HIP_OFFSET_Z ) };

  static const ublas::matrix <float> RIGHT_ARM_BASE_TRANSFORMS[1]
    ={ translation4D( 0.0f,
		      -SHOULDER_OFFSET_Y,
		      SHOULDER_OFFSET_Z ) };

  static const ublas::matrix <float> * BASE_TRANSFORMS[NUM_CHAINS] =
    { &HEAD_BASE_TRANSFORMS[0],
      &LEFT_ARM_BASE_TRANSFORMS[0],
      &LEFT_LEG_BASE_TRANSFORMS[0],
      &RIGHT_LEG_BASE_TRANSFORMS[0],
      &RIGHT_ARM_BASE_TRANSFORMS[0] };

  //Base transforms to get from body center to beg. of chain
  static const ublas::matrix <float> HEAD_END_TRANSFORMS[3]
    = { rotation4D(X_AXIS, M_PI/2),
        rotation4D(Y_AXIS,M_PI/2),
	translation4D(CAMERA_OFF_X, 0, CAMERA_OFF_Z) };

  static const ublas::matrix <float> LEFT_ARM_END_TRANSFORMS[2]
    = { rotation4D(Z_AXIS, -M_PI/2),
        translation4D(LOWER_ARM_LENGTH,0.0f,0.0f) };

  static const ublas::matrix <float> LEFT_LEG_END_TRANSFORMS[3]
    = { rotation4D(Z_AXIS, M_PI),
	rotation4D(Y_AXIS, -M_PI/2),
	translation4D(0.0f,
		      0.0f,
		      -FOOT_HEIGHT) };

  static const ublas::matrix <float> RIGHT_LEG_END_TRANSFORMS[3] =
    LEFT_LEG_END_TRANSFORMS;

  static const ublas::matrix <float> RIGHT_ARM_END_TRANSFORMS[2] =
    LEFT_ARM_END_TRANSFORMS;


  static const ublas::matrix <float> * END_TRANSFORMS[NUM_CHAINS] =
    { &HEAD_END_TRANSFORMS[0],
      &LEFT_ARM_END_TRANSFORMS[0],
      &LEFT_LEG_END_TRANSFORMS[0],
      &RIGHT_LEG_END_TRANSFORMS[0],
      &RIGHT_ARM_END_TRANSFORMS[0] };
  static const int NUM_BASE_TRANSFORMS[NUM_CHAINS] = {1,1,1,1,1};
  static const int NUM_END_TRANSFORMS[NUM_CHAINS] = {3,2,3,3,2};
  static const int NUM_JOINTS_CHAIN[NUM_CHAINS] = {2,4,6,6,4};

};
#endif
