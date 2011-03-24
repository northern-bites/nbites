#ifndef JointMassConstants_h_DEFINED
#define JointMassConstants_h_DEFINED

#include <boost/numeric/ublas/matrix.hpp>

using namespace boost::numeric;
using namespace NBMath;

class Joint
{
public:
	const float mass;
	ufvector4 offset;

	Joint(const float _mass, const float x_off, const float y_off,
		  const float z_off)
		: mass(_mass)
        {
            offset = CoordFrame4D::vector4D(x_off, y_off, z_off);
        }
};

namespace Kinematics {

    /* Not sure why we need this, CoM transformations seemed to be working
     * before the 3.3 upgrade. In any case, when moving from the C frame to
     * the I frame there is a 20mm offset in the y direction. If joint_com_y
     * looks broken in the debug graphs, tweak this (or find the real problem).
     */
    static const float COM_I_Y_OFFSET = -20.0f;

// All masses in grams
// locally expressed constants (with respect to an individual joint
// and the GLOBAL coordinate frame)
// current as of 3/22/11 from
// http://robocup.aldebaran-robotics.com/docs/site_en/reddoc/hardware
// ** all values for the RIGHT side of the robot **

	static const float CHEST_MASS_g = 1039.48f;
	static const float CHEST_MASS_X = -4.15f;
	static const float CHEST_MASS_Y = 0.07f;
	static const float CHEST_MASS_Z = 42.58f;

	static const float NECK_MASS_g = 59.30f;
	static const float NECK_MASS_X = -0.02f;
	static const float NECK_MASS_Y = 0.17f;
	static const float NECK_MASS_Z = -25.56f;

	static const float HEAD_MASS_g = 520.65f;
	static const float HEAD_MASS_X = 1.20f;
	static const float HEAD_MASS_Y = -0.84f;
	static const float HEAD_MASS_Z = 53.53f;

	static const float SHOULDER_MASS_g = 69.96f;
	static const float SHOULDER_MASS_X = -1.78f;
	static const float SHOULDER_MASS_Y = 24.96f;
	static const float SHOULDER_MASS_Z = 0.18f;

	static const float BICEP_MASS_g = 123.09f;
	static const float BICEP_MASS_X = 18.85f;
	static const float BICEP_MASS_Y = -5.77f;
	static const float BICEP_MASS_Z = 0.65f;

	static const float ELBOW_MASS_g = 59.71f;
	static const float ELBOW_MASS_X = -25.60f;
	static const float ELBOW_MASS_Y = 0.01f;
	static const float ELBOW_MASS_Z = -0.19f;

	static const float FOREARM_MASS_g = 77.24f;
	static const float FOREARM_MASS_X = 25.56f;
	static const float FOREARM_MASS_Y = -2.73f;
	static const float FOREARM_MASS_Z = 0.96f;

	static const float HAND_MASS_g = 185.00f;
	static const float HAND_MASS_X = 65.36f;
	static const float HAND_MASS_Y = -0.34f;
	static const float HAND_MASS_Z = -0.02f;

	static const float PELVIS_MASS_g = 71.17f;
	static const float PELVIS_MASS_X = -7.66f;
	static const float PELVIS_MASS_Y = 12.00f;
	static const float PELVIS_MASS_Z = 27.17f;

	static const float HIP_MASS_g = 135.30f;
	static const float HIP_MASS_X = -16.49f;
	static const float HIP_MASS_Y = -0.29f;
	static const float HIP_MASS_Z = -4.75f;

	static const float THIGH_MASS_g  = 394.21f;
	static const float THIGH_MASS_X = 1.32f;
	static const float THIGH_MASS_Y = -2.35f;
	static const float THIGH_MASS_Z = -53.52f;

	static const float TIBIA_MASS_g  = 291.59f;
	static const float TIBIA_MASS_X = 4.22f;
	static const float TIBIA_MASS_Y = -2.52f;
	static const float TIBIA_MASS_Z = -48.68f;

	static const float ANKLE_MASS_g = 138.92f;
	static const float ANKLE_MASS_X = 1.42f;
	static const float ANKLE_MASS_Y = -0.28f;
	static const float ANKLE_MASS_Z = 6.38f;

	static const float FOOT_MASS_g  = 161.75f;
	static const float FOOT_MASS_X = 25.40f;
	static const float FOOT_MASS_Y = -3.32f;
	static const float FOOT_MASS_Z = -32.41f;

	static const float TOTAL_MASS  =  // ~4879g
		CHEST_MASS_g + HEAD_MASS_g + NECK_MASS_g +
		2.0f*(SHOULDER_MASS_g + BICEP_MASS_g + ELBOW_MASS_g + FOREARM_MASS_g +
			  HAND_MASS_g + PELVIS_MASS_g + HIP_MASS_g + THIGH_MASS_g + TIBIA_MASS_g +
			  ANKLE_MASS_g + FOOT_MASS_g);

// put the constants and offsets together into Joint objects
// invert Y offset for left arm & leg
	const Joint chestMass = Joint(CHEST_MASS_g, CHEST_MASS_X,
								  CHEST_MASS_Y, CHEST_MASS_Z);

	const Joint jointMass[] = {
		// neck & head
		Joint(NECK_MASS_g, NECK_MASS_X, NECK_MASS_Y, NECK_MASS_Z),
		Joint(HEAD_MASS_g, HEAD_MASS_X, HEAD_MASS_Y, HEAD_MASS_Z),
		// LARM
		Joint(SHOULDER_MASS_g, SHOULDER_MASS_X, -SHOULDER_MASS_Y, SHOULDER_MASS_Z),
		Joint(BICEP_MASS_g, BICEP_MASS_X, -BICEP_MASS_Y, BICEP_MASS_Z),
		Joint(ELBOW_MASS_g, ELBOW_MASS_X, -ELBOW_MASS_Y, ELBOW_MASS_Z),
		Joint(FOREARM_MASS_g, FOREARM_MASS_X, -FOREARM_MASS_Y, FOREARM_MASS_Z),
		Joint(HAND_MASS_g, HAND_MASS_X, -HAND_MASS_Y, HAND_MASS_Z),
		// LLEG
		Joint(PELVIS_MASS_g, PELVIS_MASS_X, -PELVIS_MASS_Y, PELVIS_MASS_Z),
		Joint(HIP_MASS_g, HIP_MASS_X, -HIP_MASS_Y, HIP_MASS_Z),
		Joint(THIGH_MASS_g, THIGH_MASS_X, -THIGH_MASS_Y, THIGH_MASS_Z),
		Joint(TIBIA_MASS_g, TIBIA_MASS_X, -TIBIA_MASS_Y, TIBIA_MASS_Z),
		Joint(ANKLE_MASS_g, ANKLE_MASS_X, -ANKLE_MASS_Y, ANKLE_MASS_Z),
		Joint(FOOT_MASS_g, FOOT_MASS_X, -FOOT_MASS_Y, FOOT_MASS_Z),
		// RLEG
		Joint(PELVIS_MASS_g, PELVIS_MASS_X, PELVIS_MASS_Y, PELVIS_MASS_Z),
		Joint(HIP_MASS_g, HIP_MASS_X, HIP_MASS_Y, HIP_MASS_Z),
		Joint(THIGH_MASS_g, THIGH_MASS_X, THIGH_MASS_Y, THIGH_MASS_Z),
		Joint(TIBIA_MASS_g, TIBIA_MASS_X, TIBIA_MASS_Y, TIBIA_MASS_Z),
		Joint(ANKLE_MASS_g, ANKLE_MASS_X, ANKLE_MASS_Y, ANKLE_MASS_Z),
		Joint(FOOT_MASS_g, FOOT_MASS_X, FOOT_MASS_Y, FOOT_MASS_Z),
		// RARM
		Joint(SHOULDER_MASS_g, SHOULDER_MASS_X, SHOULDER_MASS_Y, SHOULDER_MASS_Z),
		Joint(BICEP_MASS_g, BICEP_MASS_X, BICEP_MASS_Y, BICEP_MASS_Z),
		Joint(ELBOW_MASS_g, ELBOW_MASS_X, ELBOW_MASS_Y, ELBOW_MASS_Z),
		Joint(FOREARM_MASS_g, FOREARM_MASS_X, FOREARM_MASS_Y, FOREARM_MASS_Z),
		Joint(HAND_MASS_g, HAND_MASS_X, HAND_MASS_Y, HAND_MASS_Z),
	};

} // namespace Kinematics

#endif
