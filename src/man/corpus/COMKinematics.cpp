#include "COMKinematics.h"

using namespace NBMath;
using namespace std;
using namespace boost::numeric;
using namespace CoordFrame4D;

//#define DEBUG_COM
//#define DEBUG_COM_VERBOSE
//#define DEBUG_COM_TRANSFORMS

ufmatrix4 limbs[Kinematics::NUM_JOINTS]; // transform to the origin of each limb

const ufvector4
Kinematics::getCOMc(const vector<float> bodyAngles) {
    if (bodyAngles.size() != Kinematics::NUM_JOINTS)
	cout << "bad bodyAngles vector input to Kinematics::getCOMc\n";

    // copy the body angles to an array
    float angles[Kinematics::NUM_JOINTS];

    for(unsigned int i = 0; i < Kinematics::NUM_JOINTS; i++)
	angles[i] = bodyAngles[i];

    buildJointTransforms(angles);

    // start with the chest CoM
    ufvector4 partialComPos = chestMass.offset * chestMass.mass;

#ifdef DEBUG_COM_VERBOSE
    cout << "Chest COM " << partialComPos/chestMass.mass << endl;
#endif

    // add each joint's mass relative to origin (0,0,0)
    for(unsigned int joint = 0; joint < Kinematics::NUM_JOINTS; ++joint) {
#ifdef DEBUG_COM_TRANSFORMS
// will give us access to the position in x,y,z space each transform goes to
// without adding in the mass at the joint's (local) CoM
	const ufvector4 no_offset = vector4D(1.0f, 1.0f, 1.0f);
	const ufvector4 partial = (prod(limbs[joint], no_offset)
				   *jointMass[joint].mass);
#else
	const ufvector4 partial = (prod(limbs[joint], jointMass[joint].offset)
				   *jointMass[joint].mass);
#endif
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
 * origin of each joint, based on a given set of joint angles. This
 * allows us to add each limb's mass as the product
 * transform(matrix) * localOffset(vector) * mass(scalar)
 * The sum of these divided by the total mass is the CoM
 *
 * NOTE: matrix multiplication isn't associative, so if you change
 * things try not to cause awlful sanity-ruining bugs
 */
void Kinematics::buildJointTransforms(const float angles[]) {
    float side = 1.0f; // Left Side ? 1 : -1
    int start = 0; // indicates where in limbs[] each function starts

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
    // neck (head yaw)
    limbs[start] = get6DTransform(0.0f, 0.0f, NECK_OFFSET_Z,
				  0.0f, 0.0f, angles[HEAD_YAW]);
    // head (head pitch)
    limbs[start + 1] = prod(rotation4D(Y_AXIS, angles[HEAD_PITCH]),
			    limbs[start]);
}

void Kinematics::buildArmChain(const int start, const float side, const float angles[]) {
    // shoulder pitch (shoulder)
    limbs[start] = get6DTransform(0.0f, SHOULDER_OFFSET_Y*side, SHOULDER_OFFSET_Z,
				  0.0f, -angles[start], 0.0f);
    // shoulder roll (bicep)
    limbs[start + 1] = prod(rotation4D(Z_AXIS, angles[start + 1]),
			    limbs[start]);
    // elbow yaw (elbow)
    limbs[start + 2] = prod(get6DTransform(UPPER_ARM_LENGTH, 0.0f, 0.0f,
					   angles[start + 2], 0.0f, 0.0f),
			    limbs[start + 1]);
    // elbow roll (hand/forearm together)
    limbs[start + 3] = prod(rotation4D(Z_AXIS, angles[start + 3]),
                            limbs[start + 2]);
}

// See: buildArmChain
void Kinematics::buildLegChain(const int start, const float side, const float angles[]) {
    // hip yaw pitch (pelvis)
    limbs[start] = get6DTransform(0.0f, HIP_OFFSET_Y*side, -HIP_OFFSET_Z,
				  M_PI_FLOAT/4*side, 0.0f, angles[start]);
    // hip roll (hip)
    limbs[start + 1] = prod(rotation4D(X_AXIS,
				       (angles[start + 1] + M_PI_FLOAT/4*side)),
                            limbs[start]);
    // hip pitch (thigh)
    limbs[start + 2] = prod(rotation4D(Y_AXIS, angles[start + 2]),
                            limbs[start + 1]);
    // knee pitch (tibia)
    limbs[start + 3] = prod(get6DTransform(0.0f, 0.0f, -THIGH_LENGTH,
					   0.0f, angles[start + 3], 0.0f),
			    limbs[start + 2]);
    // ankle pitch (ankle)
    limbs[start + 4] = prod(get6DTransform(0.0f, 0.0f, -TIBIA_LENGTH,
					   0.0f, angles[start + 4], 0.0f),
			    limbs[start + 3]);
    // ankle roll (foot)
    limbs[start + 5] = prod(rotation4D(X_AXIS, angles[start + 5]),
			    limbs[start + 4]);
}
