#ifndef JointMassConstants_h_DEFINED
#define JointMassConstants_h_DEFINED

#include <boost/numeric/ublas/matrix.hpp>

using namespace boost::numeric;
using namespace NBMath;

class Joint
{
public:
	float mass;
	ufmatrix4 offset;

	Joint(const float _mass, const float x_off, const float y_off,
		  const float z_off)
		: mass(_mass)
		{
		offset = CoordFrame4D::translation4D(x_off,
											 y_off,
											 z_off);
	}
};

// locally expressed constants (with respect to an individual joint
// and the GLOBAL coordinate frame)
// current as of 8/15/10 from
// http://robocup.aldebaran-robotics.com/docs/site_en/reddoc/hardware
// ** all values for the LEFT side of the robot **

static const float CHEST_MASS_X = -4.8f;
static const float CHEST_MASS_Y = 0.06f;
static const float CHEST_MASS_Z = 127.27f; // 85 + 42.27

static const float NECK_MASS_X = -0.03f;
static const float NECK_MASS_Y = 0.18f;
static const float NECK_MASS_Z = -25.73f;

static const float HEAD_MASS_X = 3.83f;
static const float HEAD_MASS_Y = -0.93f;
static const float HEAD_MASS_Z = 51.56f;

static const float SHOULDER_MASS_X = -1.78f;
static const float SHOULDER_MASS_Y = -25.07f;
static const float SHOULDER_MASS_Z = 0.19f;

static const float BICEP_MASS_X = 20.67f;
static const float BICEP_MASS_Y = 3.88f;
static const float BICEP_MASS_Z = 3.62f;

static const float ELBOW_MASS_X = -25.73f;
static const float ELBOW_MASS_Y = -0.01f;
static const float ELBOW_MASS_Z = -0.2f;

static const float FOREARM_MASS_X = 69.92f;
static const float FOREARM_MASS_Y = 0.96f;
static const float FOREARM_MASS_Z = -1.14f;

static const float PELVIS_MASS_X = -7.17f;
static const float PELVIS_MASS_Y = -11.87f;
static const float PELVIS_MASS_Z = 27.05f;

static const float HIP_MASS_X = -16.49f;
static const float HIP_MASS_Y = 0.29f;
static const float HIP_MASS_Z = -4.75f;

static const float THIGH_MASS_X = 1.31f;
static const float THIGH_MASS_Y = 2.01f;
static const float THIGH_MASS_Z = -53.86f;

static const float TIBIA_MASS_X = 4.71f;
static const float TIBIA_MASS_Y = 2.10f;
static const float TIBIA_MASS_Z = -48.91f;

static const float ANKLE_MASS_X = 1.42f;
static const float ANKLE_MASS_Y = 0.28f;
static const float ANKLE_MASS_Z = 6.38f;

static const float FOOT_MASS_X = 24.89f;
static const float FOOT_MASS_Y = 3.30f;
static const float FOOT_MASS_Z = -32.08f;

//Weight constants
static const float CHEST_MASS_g = 1026.28f;
static const float NECK_MASS_g = 59.59f;
static const float HEAD_MASS_g = 476.71f;
static const float SHOULDER_MASS_g = 69.84f;
static const float BICEP_MASS_g = 121.66f;
static const float ELBOW_MASS_g = 59.59f;
static const float FOREARM_MASS_g = 112.82f;
static const float PELVIS_MASS_g = 72.44f;
static const float HIP_MASS_g = 135.30f;
static const float THIGH_MASS_g  = 397.98f;
static const float TIBIA_MASS_g  = 297.06f;
static const float ANKLE_MASS_g = 138.92f;
static const float FOOT_MASS_g  = 163.04f;
static const float TOTAL_MASS  =  // ~4699.88g
	CHEST_MASS_g + HEAD_MASS_g + NECK_MASS_g +
	2.0f*(SHOULDER_MASS_g + BICEP_MASS_g + ELBOW_MASS_g + FOREARM_MASS_g +
		  PELVIS_MASS_g + HIP_MASS_g + THIGH_MASS_g + TIBIA_MASS_g +
		  ANKLE_MASS_g + FOOT_MASS_g);
#endif
