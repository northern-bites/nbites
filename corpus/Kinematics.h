#ifndef Kinematics_h_DEFINED
#define Kinematics_h_DEFINED

/**
 * This file is strictly specific to the Nao robot.
 * It contains infomation relevant to the physical configuration of the robot
 * including:
 * length of links
 * number of chains, etc.
 * It includes method definitions related to the calculation of forward and
 * inverse kinematics.
 * It also supports the creation of rotation and translation matrices.
 * NOTE: All of the lengths are in millimeters.
 */

#include <string.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/lu.hpp>              // for lu_factorize
#include <boost/numeric/ublas/io.hpp>              // for cout
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
        RARM_CHAIN,
        LANKLE_CHAIN,
        RANKLE_CHAIN
    };

	// Interpolation types
	enum InterpolationType {
		INTERPOLATION_SMOOTH = 0,
		INTERPOLATION_LINEAR
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

    enum IKOutcome {
        STUCK = 0,
        SUCCESS = 1
    };

    struct IKLegResult {
        IKOutcome outcome;
        float angles[6];
    };

    enum Motion_IntFlag {
        UNINT_INTR_CMD, // Un-interruptable interrupter command
        INT_INTR_CMD,   // Interruptable interupter command
        UNINT_CMD,      // Un-interruptable command
        INT_CMD         // Interruptable command
    };


    enum SupportLeg{
        BOTH_LEGS = 0,
        RIGHT_LEG,
        LEFT_LEG
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

    /**********       Joint Bounds       ***********/
    static const float HEAD_BOUNDS[2][2] = {{-2.09f,2.09f},{-.785f,.785f}};

    // Order of arm joints: ShoulderPitch, SRoll, ElbowYaw, ERoll
    static const float LEFT_ARM_BOUNDS[][2] = {{-2.09f,2.09f},
                                               {0.0f,1.65f},
                                               {-2.09f,2.09f},
                                               {-1.57f,0.0f}};
    static const float RIGHT_ARM_BOUNDS[][2] = {{-2.09f,2.09f},
                                                {-1.65f,0.0f},
                                                {-2.09f,2.09f},
                                                {0.0f,1.57f}};

    // Order of leg joints: HYPitch HipPitch HipRoll KneePitch APitch ARoll
    static const float LEFT_LEG_BOUNDS[][2] = {{-1.57f,0.0f},
                                               {-1.57f,.436f},
                                               {-.349f,.785f},
                                               {0.0f,2.269f},
                                               {-1.309f,.524f},
                                               {-.785f,.349f}};
    static const float RIGHT_LEG_BOUNDS[][2] = {{-1.57f,0.0f},
                                                {-1.57f,.436f},
                                                {-.785f,.349f},
                                                {0.0f,2.269f},
                                                {-1.309f,.524f},
                                                {-.349f,.785f}};

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


    /*
     * Declarations for constants and methods concerning forward and inverse
     * kinematics.
     */
    static const float dampFactor = 0.4f;
    static const float maxDeltaTheta = 0.5f;
    static const int maxAnkleIterations = 60;
    static const int maxHeelIterations = 20;

    /*
     * This saves me some nasty typing...
     * uBLAS by default uses std::vector as its underlying storage data
     * structure. By overriding the matrix type to use ublas::bounded_array
     * we're forcing uBLAS to use arrays as the storage type which allows all
     * variables to stay off the heap and on the stack. This offers a dramatic
     * increase in performance with the slight inconvenience that the static
     * array needs to be big enough to fit a n by n matrix. The biggest one I
     * handle is 3x3, so a bounded array of size 9 is sufficient.
     */
    typedef ublas::matrix<float,
                          ublas::row_major,
                          ublas::bounded_array<float,9> > ufmatrix3;
    typedef ublas::vector<float, ublas::bounded_array<float,3> > ufvector3;

    const float clip(const float, const float, const float);
    const void clipChainAngles(const ChainID id,
                               float angles[]);
    const float getMinValue(const ChainID id, const int jointNumber);
    const float getMaxValue(const ChainID id, const int jointNumber);
    const ufvector3 forwardKinematics(const ChainID id,
                                      const float angles[]);
    const ufmatrix3 buildHeelJacobian(const ChainID chainID,
                                      const float angles[]);
    const ufmatrix3 buildLegJacobian(const ChainID chainID,
                                     const float angles[]);

    const ufvector3 solve(ufmatrix3 &A,
                          const ufvector3 &b);

    // Both adjustment methods return whether the search was successful.
    // The correct angles required to fulfill the goal are returned through
    // startAngles by reference.
    const bool adjustAnkle(const ChainID chainID,
                           const ufvector3 &goal,
                           float startAngles[],
                           const float maxError);
    const bool adjustHeel(const ChainID chainID,
                          const ufvector3 &goal,
                          float startAngles[],
                          const float maxError);
    const IKLegResult dls(const ChainID chainID,
                          const ufvector3 &goal,
                          const float startAngles[],
                          const float maxError = 1.0f,
                          const float maxHeelError = .1f);
};
#endif
