#include "COMKinematics.h"

using namespace NBMath;
using namespace std;
using namespace boost::numeric;

//#define DEBUG_COM
//#define DEBUG_COM_VERBOSE

ufmatrix4 limbs[Kinematics::NUM_JOINTS]; // transform to the origin of each limb

const ufvector4
Kinematics::getCOMc(const vector<float> bodyAngles) {
	//copy the body angles to an array
	float angles[NUM_JOINTS];
	for(unsigned int i = 0; i< NUM_JOINTS; i++){
		angles[i] = bodyAngles[i];
	}

	buildJointTransforms(angles);

	// start with the chest CoM
	ufvector4 partialComPos = chestMass.offset * chestMass.mass;

#ifdef DEBUG_COM_VERBOSE
	cout << "Chest COM " << partialComPos/chestMass.mass << endl;
#endif

	// add each joint's mass relative to origin (0,0,0)
	for(unsigned int joint = 0; joint < NUM_JOINTS; joint++) {
		const ufvector4 partial = (prod(limbs[joint], jointMass[joint].offset)
								   *jointMass[joint].mass);
		partialComPos += partial;

#ifdef DEBUG_COM_VERBOSE
		cout << "joint: " << joint <<" pos " << partial/jointMass[joint].mass;
		cout << " angle: " << angles[joint] << endl;
#endif
	}

#ifdef DEBUG_COM
	cout << "Body Com " << partialComPos/TOTAL_MASS <<endl;
#endif

	return partialComPos/TOTAL_MASS;
}

/*
 * Builds full transforms from the robot origin (0,0,0) to the local
 * origin of each joint, based on a given set of joint angles this
 * allows us to add each limb's mass as the product
 * transform(matrix) * localOffset(vector) * mass(scalar)
 * The sum of these divided by the total mass is the CoM
 *
 * NOTE: matrix multiplication isn't associative, so if you change
 * things try not to cause awlful sanity-ruining bugs
 */
void Kinematics::buildJointTransforms(const float angles[]) {
	float side = 1.0f; // Left Side ? 1 : -1
	int start = 0;

	// head & neck
	buildHeadNeck(start, angles);

	// left arm chain
	start = 2;
	buildArmChain(start, side, angles);

	// left leg chain
	start = 6;
	buildLegChain(start, side, angles);

	// right leg chain
	side = -1.0f;
	start = 12;
	buildLegChain(start, side, angles);

	// right arm chain
	start = 18;
	buildArmChain(start, side, angles);
}

void Kinematics::buildHeadNeck(const int start, const float angles[]) {
	using namespace CoordFrame4D;

	// neck (head yaw)
	limbs[start] = prod(translation4D(0, 0, NECK_OFFSET_Z),
						rotation4D(Z_AXIS, angles[0]));
	// head (head pitch)
	limbs[start + 1] = prod(rotation4D(Y_AXIS, -angles[1]),
							limbs[start]);
}

void Kinematics::buildArmChain(const int start, const float side, const float angles[]) {
	using namespace CoordFrame4D;
	ufmatrix4 temp; // for multiple transformations, ublas hates nested prod calls

	// shoulder pitch
	limbs[start] = prod(translation4D(0, SHOULDER_OFFSET_Y*side, SHOULDER_OFFSET_Z),
						rotation4D(Y_AXIS, -angles[start]));
	// shoulder roll
	limbs[start + 1] = prod(rotation4D(Z_AXIS, angles[start + 1]*side),
							limbs[start]);
	// elbow yaw
	temp = prod(rotation4D(X_AXIS, angles[start + 2]*side),
				limbs[start + 1]);
	limbs[start + 2] = prod(translation4D(UPPER_ARM_LENGTH, 0, 0),
							temp);
	// elbow roll
	limbs[start + 3] = prod(rotation4D(Z_AXIS, -angles[start + 3]*side),
							limbs[start + 2]);
}

// See: buildArmChain
void Kinematics::buildLegChain(const int start, const float side, const float angles[]) {
	using namespace CoordFrame4D;
	ufmatrix4 temp; // for multiple transformations, ublas hates nested prod calls

	// hip yaw pitch
	temp = prod(rotation4D(X_AXIS, M_PI_FLOAT/4*-side),
				rotation4D(Z_AXIS, angles[start]*-side));
    limbs[start] = prod(translation4D(0.0f, HIP_OFFSET_Y*side, -HIP_OFFSET_Z),
						temp);
	// hip roll
	limbs[start + 1] = prod(rotation4D(X_AXIS, (angles[start + 1] + M_PI_FLOAT/4)*side),
							limbs[start]);
	// hip pitch
	limbs[start + 2] = prod(rotation4D(Y_AXIS, angles[start + 2]),
							limbs[start + 1]);
	// knee pitch
	temp = prod(rotation4D(Y_AXIS, angles[start + 3]),
				limbs[start + 2]);
	limbs[start + 3] = prod(translation4D(0.0f, 0.0f, -THIGH_LENGTH),
							temp);
 	// ankle pitch
	temp = prod(rotation4D(Y_AXIS, angles[start + 4]),
				limbs[start + 3]);
	limbs[start + 4] = prod(translation4D(0.0f, 0.0f, -TIBIA_LENGTH),
							temp);
	// ankle roll
	limbs[start + 5] = prod(rotation4D(X_AXIS, angles[start + 5] * -side),
							limbs[start + 4]);
}
