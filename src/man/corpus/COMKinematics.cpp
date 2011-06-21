#include "COMKinematics.h"

using namespace NBMath;
using namespace std;
using namespace boost::numeric;

//#define DEBUG_COM
//#define DEBUG_COM_VERBOSE
//#define DEBUG_COM_TRANSFORMS

ufmatrix4 limbs[Kinematics::NUM_MASS_PIECES]; // transform to the origin of each limb

const ufvector4
Kinematics::getCOMc(const vector<float> bodyAngles) {
    if (bodyAngles.size() != Kinematics::NUM_JOINTS)
	cout << "bodyAngles vector input to Kinematics::getCOMc\n";

    // copy the body angles to an array
    float angles[NUM_MASS_PIECES];

    unsigned int joint = 0;
    for(unsigned int i = 0; i < NUM_MASS_PIECES; i++){
	// add two dummy values for the hands
	// leave the rest of the angles intact
	if (i == 6 || i == 23) {
	    angles[i] = -1.0f;
	    continue;
	}

	angles[i] = bodyAngles[joint];
	joint++;
    }

    buildJointTransforms(angles);

    // start with the chest CoM
    ufvector4 partialComPos = chestMass.offset * chestMass.mass;

#ifdef DEBUG_COM_VERBOSE
    cout << "Chest COM " << partialComPos/chestMass.mass << endl;
#endif

    // add each joint's mass relative to origin (0,0,0)
    for(unsigned int joint = 0; joint < NUM_MASS_PIECES; ++joint) {
#ifdef DEBUG_COM_TRANSFORMS
// will give us access to the position in x,y,z space each transform goes to
// without adding in the mass at the joint's (local) CoM
	const ufvector4 no_offset = CoordFrame4D::vector4D(1.0f, 1.0f, 1.0f);
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
    start = 7;
    buildLegChain(start, side, angles);

    // right leg chain
    side = -1.0f;
    start = 13;
    buildLegChain(start, side, angles);

    // right arm chain
    start = 19;
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

/*
 * The Z coordinate is likely wrong here, but we're more concerned with the x/y
 * so at the moment I'm not going to worry about it -Nathan 11/8/10
 */
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
    limbs[start + 2] = prod(translation4D(UPPER_ARM_LENGTH, 0.0f, 0.0f),
                            temp);
    // elbow roll
    limbs[start + 3] = prod(rotation4D(Z_AXIS, -angles[start + 3]*side),
                            limbs[start + 2]);
    // hand (no joint angle here)
    limbs[start + 4] = prod(translation4D(HAND_OFFSET_X, 0.0f, HAND_OFFSET_Z),
                            limbs[start + 3]);
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
