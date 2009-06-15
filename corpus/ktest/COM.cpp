#include "Kinematics.h"

using namespace Kinematics;
using namespace boost::numeric;
using namespace NBMath;
using namespace std;


/**
 * The inverse of one of the kinematics matrices if the originally is of the
 * form:
 *    R p
 *    0 1 
 * Then the inverse is given as
 *   R^t 
 */

const ufmatrix4 invertKinematicsMatrix(const ufmatrix4 source){
    ufmatrix4 result = source;


}

const ufvector4
calculateCom(const ChainID id,
             const std::vector <float> &angles) {
  ufmatrix4 fullTransform = ublas::identity_matrix <float> (4);
  ufvector4 comPos = CoordFrame4D::vector4D(0,0,0,0);

  const ufvector4 origin = CoordFrame4D::vector4D(0,0,0);
  // Do base transforms
  const int numBaseTransforms = NUM_BASE_TRANSFORMS[id];
  for (int i = 0; i < numBaseTransforms; i++) {
    fullTransform = prod(fullTransform, BASE_TRANSFORMS[id][i]);
  }

  // Do mDH transforms
  const int numTransforms = NUM_JOINTS_CHAIN[id];
  for (int i = 0; i < numTransforms; i++) {
    // Right before we do a transformation, we are in the correct coordianate
    // frame and we need to store it, so we know where all the links of a
    // chain are. We only need to do this if the transformation gives us a new
    // link
    const float *currentmDHParameters = MDH_PARAMS[id];

    //length L - movement along the X(i-1) axis
    if (currentmDHParameters[i*4 + L] != 0) {
      const ufmatrix4 transX =
	CoordFrame4D::translation4D(currentmDHParameters[i*4 + L],0.0f,0.0f);
      fullTransform = prod(fullTransform, transX);
    }

    //twist: - rotate about the X(i-1) axis
    if (currentmDHParameters[i*4 + ALPHA] != 0) {
      const ufmatrix4 rotX =
	CoordFrame4D::rotation4D(CoordFrame4D::X_AXIS,
			       currentmDHParameters[i*4 + ALPHA]);
      fullTransform = prod(fullTransform, rotX);
    }
    //theta - rotate about the Z(i) axis
    if (currentmDHParameters[i*4 + THETA] + angles[i] != 0) {
      const ufmatrix4 rotZ =
	CoordFrame4D::rotation4D(CoordFrame4D::Z_AXIS,
			       currentmDHParameters[i*4 + THETA] +
			       angles[i]);
      fullTransform = prod(fullTransform, rotZ);
    }
    //offset D movement along the Z(i) axis
    if (currentmDHParameters[i*4 + D] != 0) {
      const ufmatrix4 transZ =
	CoordFrame4D::translation4D(0.0f,0.0f,currentmDHParameters[i*4 + D]);
      fullTransform = prod(fullTransform, transZ);
    }

    const float *curInertialPos = INERTIAL_POS[id];

    const ufmatrix4 massEndTrans =
        CoordFrame4D::translation4D(curInertialPos[i*4 + 0],
                                    curInertialPos[i*4 + 1],
                                    curInertialPos[i*4 + 2]);
    const ufmatrix4 curMassTrans = prod(fullTransform,massEndTrans);
    const float curMassProportion = curInertialPos[i*4 + MASS_INDEX]/TOTAL_MASS;
    const ufvector4 thisSegmentWeightedPos =
        prod(curMassTrans,origin)*curMassProportion;
    comPos += thisSegmentWeightedPos;
//         cout << "  fullTransform" << fullTransform<<endl;
//         cout << "  location of this link" << prod(fullTransform,origin)<<endl<<endl;
//         cout << "  cumulative POS " << comPos<<endl;
//         cout << "  this weighted segment is " << thisSegmentWeightedPos <<endl;
//         cout << "  located at " << prod(curMassTrans,origin)<<endl;
//         cout << "  curMass Proportion is" << curMassProportion<<endl;
//         cout << "  massEndTrans " << massEndTrans <<endl;
//         cout << "  curMassTrans "<<curMassTrans<<endl;

  }

  // Do the end transforms
  const int numEndTransforms = NUM_END_TRANSFORMS[id];
  for (int i = 0; i < numEndTransforms; i++) {
    fullTransform = prod(fullTransform, END_TRANSFORMS[id][i]);
  }
  cout << "Weighted COM is " << comPos<<endl;
  return comPos;
}

vector<float> getChainAngles(ChainID id, vector<float> bodyAngles){
    vector<float> result;
    
    for(unsigned int i = chain_first_joint[id]; i <= chain_last_joint[id]; i++ ){
        result.push_back(bodyAngles[i]);
    }
    return result;
}


int main(){
    float bodyJoints[] = 
{
    0.0f,0.0f,
    1.57f,0.26f,0.0f,0.0f,
    0.0f, 0.0f, -0.378f,0.91f,-0.53f,0.0f,
    0.0f, 0.0f, -0.378f,0.91f,-0.53f,0.0f,
    1.57,-0.26f,0.0f,0.0f};
    vector<float> bodyAngles = vector<float>(bodyJoints,&bodyJoints[NUM_JOINTS]);

    ChainID leg = LLEG_CHAIN;
    ufvector4 comPos = CoordFrame4D::vector4D(CHEST_MASS_X,0,CHEST_MASS_Z)*(CHEST_MASS_g/TOTAL_MASS);
    for(unsigned int i = 0; i < NUM_CHAINS; i++){
        comPos+= calculateCom((ChainID)i, getChainAngles((ChainID) i,
                                                         bodyAngles));
        cout << endl;
    }
    cout << "final comPos is"<<comPos <<endl;
}
