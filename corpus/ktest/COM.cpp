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
 *   R^t -R^td
 *    0    1
 */

const ufmatrix4 invertKinematicsMatrix(const ufmatrix4 source){
    const ufmatrix3 Rt = trans(subrange(source,0,3,0,3));
    const ufvector3 Rtd = -prod(Rt,
                                CoordFrame3D::vector3D(source(0,3),
                                                       source(1,3),
                                                       source(2,3)));
    ufmatrix4 result = ublas::identity_matrix<float>(4);
    subrange(result,0,3,0,3) = Rt;
    result(0,3) = Rtd(0);
    result(1,3) = Rtd(1);
    result(2,3) = Rtd(2);
    return result;
}

const ufvector4
calculateChainCom(const ChainID id,
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

  }

  // Do the end transforms
  const int numEndTransforms = NUM_END_TRANSFORMS[id];
  for (int i = 0; i < numEndTransforms; i++) {
    fullTransform = prod(fullTransform, END_TRANSFORMS[id][i]);
  }
  return comPos;
}


const ufvector4
calculateBodyCom_f(const ChainID id,
                   const std::vector <float> &angles,
                   const ufvector4 partialBodyCom_c) {
  ufmatrix4 fullTransform = ublas::identity_matrix <float> (4);
  ufvector4 partialCom_c = CoordFrame4D::vector4D(0,0,0,0);

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
    partialCom_c += thisSegmentWeightedPos;


  }

  // Do the end transforms
  const int numEndTransforms = NUM_END_TRANSFORMS[id];
  for (int i = 0; i < numEndTransforms; i++) {
    fullTransform = prod(fullTransform, END_TRANSFORMS[id][i]);
  }
  
  const ufmatrix4 cf_Transform = invertKinematicsMatrix(fullTransform);
  cout << "com in Body frame " << partialCom_c+partialBodyCom_c<<endl;
  ufvector4 bodyCom_f  = prod(cf_Transform,(partialCom_c+partialBodyCom_c));
  return bodyCom_f;
}

vector<float> getChainAngles(ChainID id, vector<float> bodyAngles){
    vector<float> result;
    
    for(unsigned int i = chain_first_joint[id]; i <= chain_last_joint[id]; i++ ){
        result.push_back(bodyAngles[i]);
    }
    return result;
}



void comControl(ChainID support_id){
    float bodyJoints[] = 
{
    0.0f,0.0f,
    1.57f,0.26f,0.0f,0.0f,
    0.0f, 0.0f, -0.0f,0.0f,-0.0f,0.0f,
    0.0f, 0.0f, -0.0f,0.0f,-0.0f,0.0f,
    1.57,-0.26f,0.0f,0.0f};
    vector<float> bodyAngles = vector<float>(bodyJoints,&bodyJoints[NUM_JOINTS]);

    //Calculate the COM of the rest of the leg
    ufvector4 partialComPos = CoordFrame4D::vector4D(CHEST_MASS_X,0,CHEST_MASS_Z)*(CHEST_MASS_g/TOTAL_MASS);
    for(unsigned int i = 0; i < NUM_CHAINS; i++){
        if((ChainID) i == support_id)
            continue;
        partialComPos+= calculateChainCom((ChainID)i, getChainAngles((ChainID) i,
                                                                      bodyAngles));
    }


    //Now calculate the COM in the support foot frame
    ufvector4 com_f = calculateBodyCom_f(support_id,getChainAngles(support_id,bodyAngles)
                                         ,partialComPos);
    cout <<"Com in foot frame " <<com_f<<endl;
}


void comTest(){
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
        comPos+= calculateChainCom((ChainID)i, getChainAngles((ChainID) i,
                                                         bodyAngles));
        cout << endl;
    }
    cout << "final comPos is"<<comPos <<endl;


}

int main(){
    comControl(LLEG_CHAIN);
}
