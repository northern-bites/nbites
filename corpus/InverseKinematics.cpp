#include <cmath>

#include "InverseKinematics.h"

#define USE_ANALYTIC_IK
//#define DEBUG_IK
using namespace boost::numeric;
using namespace NBMath;
using namespace std;


/**
 * Wrapper method for simple IK, which just picks between the various
 * methods of inverse kinematics we use
 */
const Kinematics::IKLegResult Kinematics::simpleLegIK(const ChainID chainID,
                                                      const ufvector3 & legGoal,
                                                      float startAngles []){
#ifdef USE_ANALYTIC_IK
    const ufvector3 footOrientation = CoordFrame3D::vector3D(0,0,0);
    const ufvector3 bodyGoal = CoordFrame3D::vector3D(0,0,0);
    const ufvector3 bodyOrientation = CoordFrame3D::vector3D(0,0,0);
    return analyticLegIK(chainID,legGoal,footOrientation,
                         bodyGoal,bodyOrientation,startAngles[0]);
#else
    return dls(chainID,legGoal,startAngles,REALLY_LOW_ERROR);
#endif
}

const Kinematics::IKLegResult Kinematics::angleXYIK(const ChainID chainID,
                                                    const ufvector3 & legGoal,
                                                    const float bodyAngleX,
                                                    const float bodyAngleY,
                                                    const float HYPAngle){
    const ufvector3 footOrientation = CoordFrame3D::vector3D(0,0,0);
    const ufvector3 bodyGoal = CoordFrame3D::vector3D(0,0,0);
    const ufvector3 bodyOrientation = CoordFrame3D::vector3D(bodyAngleX,
                                                             bodyAngleY,0);
    return analyticLegIK(chainID,legGoal,footOrientation,
                         bodyGoal,bodyOrientation,HYPAngle);
}

const Kinematics::IKLegResult
 Kinematics::legIK(const ChainID chainID,
                   const ufvector3 &footGoal,
                   const ufvector3 &footOrientation,
                   const ufvector3 &bodyGoal,
                   const ufvector3 &bodyOrientation,
                   const float HYPAngle){

#ifdef USE_ANALYTIC_IK
    IKLegResult result = analyticLegIK(chainID,footGoal,footOrientation,
                                       bodyGoal,bodyOrientation);

#ifdef DEBUG_IK
    cout << "IK command with leg"<<chainID <<" :"<<endl
             <<"    tried to put foot to "<<footGoal
             << "      with orientation  "<<footOrientation<<endl
             <<"    tried to put body to "<<bodyGoal
             << "      with orientation  "<<bodyOrientation<<endl;
        cout << "   result angles: {";
        for(int i =0; i<6; i++){cout<<result.angles[i]<<",";}cout<<"}"<<endl;
#endif

#else
    #error "JACOBIAN IK NOT SETUP RIGHT NOW"
#endif
    if(result.outcome != Kinematics::SUCCESS){
        cout << "IK ERROR with leg"<<chainID <<" :"
             <<"    tried to put foot to "<<footGoal
             << "      with orientation  "<<footOrientation<<endl
             <<"    tried to put body to "<<bodyGoal
             << "      with orientation  "<<bodyOrientation<<endl;
    }
    return result;

}

/**
 * This method will destructively clip the chain angles that are passed to it.
 * This means that it will modify the array that was passed by reference
 * and will return nothing. The old values will be lost.
 */
const void Kinematics::clipChainAngles(const ChainID chainID,
                                       float angles[]) {
    switch (chainID) {
    case LLEG_CHAIN:
        for (unsigned int i=0; i<LEG_JOINTS; i++) {
            angles[i] = clip(angles[i],
                             LEFT_LEG_BOUNDS[i][0],LEFT_LEG_BOUNDS[i][1]);
        }
        break;
    case RLEG_CHAIN:
        for (unsigned int i=0; i<LEG_JOINTS; i++) {
            angles[i] = clip(angles[i],
                             RIGHT_LEG_BOUNDS[i][0],RIGHT_LEG_BOUNDS[i][1]);
        }
        break;
    case LARM_CHAIN:
        for (unsigned int i=0; i<ARM_JOINTS; i++) {
            angles[i] = clip(angles[i],
                             LEFT_ARM_BOUNDS[i][0],LEFT_ARM_BOUNDS[i][1]);
        }
        break;
    case RARM_CHAIN:
        for (unsigned int i=0; i<ARM_JOINTS; i++) {
            angles[i] = clip(angles[i],
                             RIGHT_ARM_BOUNDS[i][0],RIGHT_ARM_BOUNDS[i][1]);
        }
        break;
    case HEAD_CHAIN:
    default:
        for (unsigned int i=0; i<HEAD_JOINTS; i++) {
            angles[i] = clip(angles[i],
                             HEAD_BOUNDS[i][0],HEAD_BOUNDS[i][1]);
        }
        break;
    }
}


const float Kinematics::getMinValue(const ChainID id, const int jointNumber) {
    switch (id) {
    case LARM_CHAIN:
        return LEFT_ARM_BOUNDS[jointNumber][0];
    case RARM_CHAIN:
        return RIGHT_ARM_BOUNDS[jointNumber][0];
    case LLEG_CHAIN:
        return LEFT_LEG_BOUNDS[jointNumber][0];
    case RLEG_CHAIN:
        return RIGHT_LEG_BOUNDS[jointNumber][0];
    case HEAD_CHAIN:
    default:
        return HEAD_BOUNDS[jointNumber][0];
    }
    return 0;
}


const float Kinematics::getMaxValue(const ChainID id, const int jointNumber) {
    switch (id) {
    case LARM_CHAIN:
        return LEFT_ARM_BOUNDS[jointNumber][1];
    case RARM_CHAIN:
        return RIGHT_ARM_BOUNDS[jointNumber][1];
    case LLEG_CHAIN:
        return LEFT_LEG_BOUNDS[jointNumber][1];
    case RLEG_CHAIN:
        return RIGHT_LEG_BOUNDS[jointNumber][1];
    case HEAD_CHAIN:
    default: // hack! I don't want to use exceptions in this code.
        return HEAD_BOUNDS[jointNumber][0];
    }
    return 0;
}

const ufvector3 Kinematics::forwardKinematics(const ChainID id,
                                              const float angles[]){
    float x=0.0f,y=0.0f,z=0.0f;
    if(id == LLEG_CHAIN || id == RLEG_CHAIN||
       id == LANKLE_CHAIN || id == RANKLE_CHAIN){
        const float HYP = angles[0];
        const float HR = angles[1];
        const float HP = angles[2];
        const float KP = angles[3];
        const float AP = angles[4];
        const float AR = angles[5];

        float sinHYP,cosHYP,sinHR,cosHR,sinHP,cosHP,sinKP,cosKP,sinAP,cosAP,sinAR,cosAR;
        sincosf(HYP,&sinHYP,&cosHYP);
        sincosf(HR,&sinHR,&cosHR);
        sincosf(HP,&sinHP,&cosHP);
        sincosf(KP,&sinKP,&cosKP);
        sincosf(AP,&sinAP,&cosAP);
        sincosf(AR,&sinAR,&cosAR);

        //Other odd angles:
        const float cosHRPlusPiFourth = std::cos(HR+M_PI_FLOAT*0.25f);
        const float cosHRMinusPiFourth = std::cos(HR-M_PI_FLOAT*0.25f);
        const float sinHRPlusPiFourth = std::sin(HR+M_PI_FLOAT*0.25f);
        const float sinHRMinusPiFourth = std::sin(HR-M_PI_FLOAT*0.25f);
        const float sqrt2 = std::sqrt(2.0f);

        switch(id){
        case LLEG_CHAIN:
            x = -THIGH_LENGTH*(cosHYP*sinHP+cosHP*cosHRPlusPiFourth*sinHYP)-TIBIA_LENGTH*(cosKP*(cosHYP*sinHP+cosHP*cosHRPlusPiFourth*sinHYP)+(cosHP*cosHYP-cosHRPlusPiFourth*sinHP*sinHYP)*sinKP)-FOOT_HEIGHT*(cosAR*(sinAP*(cosKP*(cosHP*cosHYP-cosHRPlusPiFourth*sinHP*sinHYP)-(cosHYP*sinHP+cosHP*cosHRPlusPiFourth*sinHYP)*sinKP)+cosAP*(cosKP*(cosHYP*sinHP+cosHP*cosHRPlusPiFourth*sinHYP)+(cosHP*cosHYP-cosHRPlusPiFourth*sinHP*sinHYP)*sinKP))-sinAR*sinHYP*sinHRPlusPiFourth);
            y = HIP_OFFSET_Y-THIGH_LENGTH*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))-TIBIA_LENGTH*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2)))-FOOT_HEIGHT*(-sinAR*(cosHRPlusPiFourth/sqrt2+(cosHYP*sinHRPlusPiFourth)/sqrt2)+cosAR*(sinAP*(-sinKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2)))+cosAP*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2)))));
            z = -HIP_OFFSET_Z-THIGH_LENGTH*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))-TIBIA_LENGTH*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2)))-FOOT_HEIGHT*(-sinAR*(-cosHRPlusPiFourth/sqrt2+(cosHYP*sinHRPlusPiFourth)/sqrt2)+cosAR*(sinAP*(-sinKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2)))+cosAP*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2)))));
            break;
        case LANKLE_CHAIN:
            x = -THIGH_LENGTH*(cosHYP*sinHP+cosHP*cosHRPlusPiFourth*sinHYP)-TIBIA_LENGTH*(cosKP*(cosHYP*sinHP+cosHP*cosHRPlusPiFourth*sinHYP)+(cosHP*cosHYP-cosHRPlusPiFourth*sinHP*sinHYP)*sinKP);
            y = HIP_OFFSET_Y-THIGH_LENGTH*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))-TIBIA_LENGTH*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2)));
            z = -HIP_OFFSET_Z-THIGH_LENGTH*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))-TIBIA_LENGTH*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2)));
            break;
        case RLEG_CHAIN:
            x = -THIGH_LENGTH*(cosHYP*sinHP+cosHP*cosHRMinusPiFourth*sinHYP)-TIBIA_LENGTH*(cosKP*(cosHYP*sinHP+cosHP*cosHRMinusPiFourth*sinHYP)+(cosHP*cosHYP-cosHRMinusPiFourth*sinHP*sinHYP)*sinKP)-FOOT_HEIGHT*(cosAR*(sinAP*(cosKP*(cosHP*cosHYP-cosHRMinusPiFourth*sinHP*sinHYP)-(cosHYP*sinHP+cosHP*cosHRMinusPiFourth*sinHYP)*sinKP)+cosAP*(cosKP*(cosHYP*sinHP+cosHP*cosHRMinusPiFourth*sinHYP)+(cosHP*cosHYP-cosHRMinusPiFourth*sinHP*sinHYP)*sinKP))-sinAR*sinHYP*sinHRMinusPiFourth);
            y = -HIP_OFFSET_Y-THIGH_LENGTH*((sinHP*sinHYP)/sqrt2+cosHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))-TIBIA_LENGTH*(cosKP*((sinHP*sinHYP)/sqrt2+cosHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+sinKP*((cosHP*sinHYP)/sqrt2-sinHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))-FOOT_HEIGHT*(-sinAR*(cosHRMinusPiFourth/sqrt2-(cosHYP*sinHRMinusPiFourth)/sqrt2)+cosAR*(sinAP*(-sinKP*((sinHP*sinHYP)/sqrt2+cosHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+cosKP*((cosHP*sinHYP)/sqrt2-sinHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))+cosAP*(cosKP*((sinHP*sinHYP)/sqrt2+cosHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+sinKP*((cosHP*sinHYP)/sqrt2-sinHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))));
            z = -HIP_OFFSET_Z-THIGH_LENGTH*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))-TIBIA_LENGTH*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))-FOOT_HEIGHT*(-sinAR*(cosHRMinusPiFourth/sqrt2+(cosHYP*sinHRMinusPiFourth)/sqrt2)+cosAR*(sinAP*(-sinKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))+cosAP*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))));
            break;
        case RANKLE_CHAIN:
            x = -THIGH_LENGTH*(cosHYP*sinHP+cosHP*cosHRMinusPiFourth*sinHYP)-TIBIA_LENGTH*(cosKP*(cosHYP*sinHP+cosHP*cosHRMinusPiFourth*sinHYP)+(cosHP*cosHYP-cosHRMinusPiFourth*sinHP*sinHYP)*sinKP);
            y = -HIP_OFFSET_Y-THIGH_LENGTH*((sinHP*sinHYP)/sqrt2+cosHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))-TIBIA_LENGTH*(cosKP*((sinHP*sinHYP)/sqrt2+cosHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+sinKP*((cosHP*sinHYP)/sqrt2-sinHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)));
            z = -HIP_OFFSET_Z-THIGH_LENGTH*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))-TIBIA_LENGTH*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)));
            break;
        case LARM_CHAIN:
        case RARM_CHAIN:
        case HEAD_CHAIN:
            throw "Should not be possible";
        }


    }else if( id == LARM_CHAIN || id == RARM_CHAIN ){
        // Variables for arms.
        const float SP = angles[0];
        const float SR = angles[1];
        const float EY = angles[2];
        const float ER = angles[3];

        float sinSP, cosSP, sinSR, cosSR, sinEY, cosEY, sinER, cosER;
		sincosf(SP, &sinSP, &cosSP);
		sincosf(SR, &sinSR, &cosSR);
		sincosf(EY, &sinEY, &cosEY);
		sincosf(ER, &sinER, &cosER);
        switch(id){
        case LARM_CHAIN:
            x = LOWER_ARM_LENGTH*sinER*sinEY*sinSP + cosSP*((UPPER_ARM_LENGTH + LOWER_ARM_LENGTH*cosER)*cosSR - LOWER_ARM_LENGTH*cosEY*sinER*sinSR);
            y = SHOULDER_OFFSET_Y + LOWER_ARM_LENGTH*cosEY*cosSR*sinER + (UPPER_ARM_LENGTH + LOWER_ARM_LENGTH*cosER)*sinSR;
            z = SHOULDER_OFFSET_Z + LOWER_ARM_LENGTH*cosSP*sinER*sinEY - (UPPER_ARM_LENGTH + LOWER_ARM_LENGTH*cosER)*cosSR*sinSP + LOWER_ARM_LENGTH*cosEY*sinER*sinSP*sinSR;
            break;
        case RARM_CHAIN:
            x = LOWER_ARM_LENGTH*sinER*sinEY*sinSP + cosSP* ((UPPER_ARM_LENGTH + LOWER_ARM_LENGTH*cosER)*cosSR - LOWER_ARM_LENGTH*cosEY*sinER*sinSR);
            y = - SHOULDER_OFFSET_Y + LOWER_ARM_LENGTH*cosEY*cosSR*sinER + (UPPER_ARM_LENGTH + LOWER_ARM_LENGTH*cosER)*sinSR;
            z = SHOULDER_OFFSET_Z + LOWER_ARM_LENGTH*cosSP*sinER*sinEY - (UPPER_ARM_LENGTH + LOWER_ARM_LENGTH*cosER)*cosSR*sinSP + LOWER_ARM_LENGTH*cosEY*sinER*sinSP*sinSR;
            break;
        case LLEG_CHAIN:
        case RLEG_CHAIN:
        case RANKLE_CHAIN:
        case LANKLE_CHAIN:
        case HEAD_CHAIN:
            throw "Should not be a possible chain id";
        }
    }else if(id == HEAD_CHAIN){
        x = 0.0f;
        y = 0.0f;
        z = NECK_OFFSET_Z;
    }else
        throw "Invalid chain name in InverseKinematics";
    return CoordFrame3D::vector3D(x,y,z);
}


const ufmatrix3 Kinematics::buildJacobians(const ChainID id,
                               const float angles[]){
    const float HYP = angles[0];
    const float HR = angles[1];
    const float HP = angles[2];
    const float KP = angles[3];
    const float AP = angles[4];
    const float AR = angles[5];

    float sinHYP,cosHYP,sinHR,cosHR,sinHP,cosHP,sinKP,cosKP,sinAP,cosAP,sinAR,cosAR;
    sincosf(HYP,&sinHYP,&cosHYP);
    sincosf(HR,&sinHR,&cosHR);
    sincosf(HP,&sinHP,&cosHP);
    sincosf(KP,&sinKP,&cosKP);
    sincosf(AP,&sinAP,&cosAP);
    sincosf(AR,&sinAR,&cosAR);

    //Other odd angles:
    const float cosHRPlusPiFourth = std::cos(HR+M_PI_FLOAT*0.25f);
    const float cosHRMinusPiFourth = std::cos(HR-M_PI_FLOAT*0.25f);
    const float sinHRPlusPiFourth = std::sin(HR+M_PI_FLOAT*0.25f);
    const float sinHRMinusPiFourth = std::sin(HR-M_PI_FLOAT*0.25f);
    const float sqrt2 = std::sqrt(2.0f);
    if( id == LANKLE_CHAIN){
//Jacobians
//Left*leg
//NO*HEEL

        const float j_1_1 = THIGH_LENGTH*cosHP*sinHYP*sinHRPlusPiFourth-TIBIA_LENGTH*(-cosHP*cosKP*sinHYP*sinHRPlusPiFourth+sinHP*sinHYP*sinKP*sinHRPlusPiFourth);
        const float j_1_2 = -THIGH_LENGTH*(cosHP*cosHYP-cosHRPlusPiFourth*sinHP*sinHYP)-TIBIA_LENGTH*(cosKP*(cosHP*cosHYP-cosHRPlusPiFourth*sinHP*sinHYP)+(-cosHYP*sinHP-cosHP*cosHRPlusPiFourth*sinHYP)*sinKP);
        const float j_1_3 = -TIBIA_LENGTH*(cosKP*(cosHP*cosHYP-cosHRPlusPiFourth*sinHP*sinHYP)-(cosHYP*sinHP+cosHP*cosHRPlusPiFourth*sinHYP)*sinKP);
        const float j_2_1 = -THIGH_LENGTH*cosHP*(-cosHRPlusPiFourth/sqrt2-(cosHYP*sinHRPlusPiFourth)/sqrt2)-TIBIA_LENGTH*(cosHP*cosKP*(-cosHRPlusPiFourth/sqrt2-(cosHYP*sinHRPlusPiFourth)/sqrt2)-sinHP*sinKP*(-cosHRPlusPiFourth/sqrt2-(cosHYP*sinHRPlusPiFourth)/sqrt2));
        const float j_2_2 = -THIGH_LENGTH*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))-TIBIA_LENGTH*(sinKP*((sinHP*sinHYP)/sqrt2-cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2)));
        const float j_2_3 = -TIBIA_LENGTH*(-sinKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2)));
        const float j_3_1 = -THIGH_LENGTH*cosHP*(cosHRPlusPiFourth/sqrt2-(cosHYP*sinHRPlusPiFourth)/sqrt2)-TIBIA_LENGTH*(cosHP*cosKP*(cosHRPlusPiFourth/sqrt2-(cosHYP*sinHRPlusPiFourth)/sqrt2)-sinHP*sinKP*(cosHRPlusPiFourth/sqrt2-(cosHYP*sinHRPlusPiFourth)/sqrt2));
        const float j_3_2 = -THIGH_LENGTH*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))-TIBIA_LENGTH*(sinKP*((sinHP*sinHYP)/sqrt2-cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2)));
        const float j_3_3 = -TIBIA_LENGTH*(-sinKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2)));

        ufmatrix3 jacobian(3,3);
        jacobian(0,0) = j_1_1;  jacobian(0,1) = j_1_2;  jacobian(0,2) = j_1_3;
        jacobian(1,0) = j_2_1;  jacobian(1,1) = j_2_2;  jacobian(1,2) = j_2_3;
        jacobian(2,0) = j_3_1;  jacobian(2,1) = j_3_2;  jacobian(2,2) = j_3_3;
        return jacobian;

    }else if(id == LLEG_CHAIN){
//WITH*HEEL
        const float j_1_1 = -FOOT_HEIGHT*cosAR*(cosAP*(cosKP*(cosHP*cosHYP-cosHRPlusPiFourth*sinHP*sinHYP)-(cosHYP*sinHP+cosHP*cosHRPlusPiFourth*sinHYP)*sinKP)-sinAP*(cosKP*(cosHYP*sinHP+cosHP*cosHRPlusPiFourth*sinHYP)+(cosHP*cosHYP-cosHRPlusPiFourth*sinHP*sinHYP)*sinKP));
        const float j_1_2 = -FOOT_HEIGHT*(-sinAR*(sinAP*(cosKP*(cosHP*cosHYP-cosHRPlusPiFourth*sinHP*sinHYP)-(cosHYP*sinHP+cosHP*cosHRPlusPiFourth*sinHYP)*sinKP)+cosAP*(cosKP*(cosHYP*sinHP+cosHP*cosHRPlusPiFourth*sinHYP)+(cosHP*cosHYP-cosHRPlusPiFourth*sinHP*sinHYP)*sinKP))-cosAR*sinHYP*sinHRPlusPiFourth);
        const float j_2_1 = -FOOT_HEIGHT*cosAR*(cosAP*(-sinKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2)))-sinAP*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))));
        const float j_2_2 = -FOOT_HEIGHT*(-cosAR*(cosHRPlusPiFourth/sqrt2+(cosHYP*sinHRPlusPiFourth)/sqrt2)-sinAR*(sinAP*(-sinKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2)))+cosAP*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2)))));
        const float j_3_1 = -FOOT_HEIGHT*cosAR*(cosAP*(-sinKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2)))-sinAP*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))));
        const float j_3_2 = -FOOT_HEIGHT*(-cosAR*(-cosHRPlusPiFourth/sqrt2+(cosHYP*sinHRPlusPiFourth)/sqrt2)-sinAR*(sinAP*(-sinKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2)))+cosAP*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2)))));

        ufmatrix3 jacobian(3,2);
        jacobian(0,0) = j_1_1;  jacobian(0,1) = j_1_2;
        jacobian(1,0) = j_2_1;  jacobian(1,1) = j_2_2;
        jacobian(2,0) = j_3_1;  jacobian(2,1) = j_3_2;
        return jacobian;
    }else if(id == RANKLE_CHAIN){
//Right*leg
//NO*HEEL
        const float j_1_1 = THIGH_LENGTH*cosHP*sinHYP*sinHRPlusPiFourth-TIBIA_LENGTH*(-cosHP*cosKP*sinHYP*sinHRPlusPiFourth+sinHP*sinHYP*sinKP*sinHRPlusPiFourth);
        const float j_1_2 = -THIGH_LENGTH*(cosHP*cosHYP-cosHRPlusPiFourth*sinHP*sinHYP)-TIBIA_LENGTH*(cosKP*(cosHP*cosHYP-cosHRPlusPiFourth*sinHP*sinHYP)+(-cosHYP*sinHP-cosHP*cosHRPlusPiFourth*sinHYP)*sinKP);
        const float j_1_3 = -TIBIA_LENGTH*(cosKP*(cosHP*cosHYP-cosHRPlusPiFourth*sinHP*sinHYP)-(cosHYP*sinHP+cosHP*cosHRPlusPiFourth*sinHYP)*sinKP);
        const float j_2_1 = -THIGH_LENGTH*cosHP*(-cosHRPlusPiFourth/sqrt2-(cosHYP*sinHRPlusPiFourth)/sqrt2)-TIBIA_LENGTH*(cosHP*cosKP*(-cosHRPlusPiFourth/sqrt2-(cosHYP*sinHRPlusPiFourth)/sqrt2)-sinHP*sinKP*(-cosHRPlusPiFourth/sqrt2-(cosHYP*sinHRPlusPiFourth)/sqrt2));
        const float j_2_2 = -THIGH_LENGTH*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))-TIBIA_LENGTH*(sinKP*((sinHP*sinHYP)/sqrt2-cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2)));
        const float j_2_3 = -TIBIA_LENGTH*(-sinKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2)));
        const float j_3_1 = -THIGH_LENGTH*cosHP*(cosHRPlusPiFourth/sqrt2-(cosHYP*sinHRPlusPiFourth)/sqrt2)-TIBIA_LENGTH*(cosHP*cosKP*(cosHRPlusPiFourth/sqrt2-(cosHYP*sinHRPlusPiFourth)/sqrt2)-sinHP*sinKP*(cosHRPlusPiFourth/sqrt2-(cosHYP*sinHRPlusPiFourth)/sqrt2));
        const float j_3_2 = -THIGH_LENGTH*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))-TIBIA_LENGTH*(sinKP*((sinHP*sinHYP)/sqrt2-cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2)));
        const float j_3_3 = -TIBIA_LENGTH*(-sinKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2)));

        ufmatrix3 jacobian(3,3);
        jacobian(0,0) = j_1_1;  jacobian(0,1) = j_1_2;  jacobian(0,2) = j_1_3;
        jacobian(1,0) = j_2_1;  jacobian(1,1) = j_2_2;  jacobian(1,2) = j_2_3;
        jacobian(2,0) = j_3_1;  jacobian(2,1) = j_3_2;  jacobian(2,2) = j_3_3;
        return jacobian;

    }else if(id == RLEG_CHAIN){
//WITH*HEEL
        const float j_1_1 = -FOOT_HEIGHT*cosAR*(cosAP*(cosKP*(cosHP*cosHYP-cosHRMinusPiFourth*sinHP*sinHYP)-(cosHYP*sinHP+cosHP*cosHRMinusPiFourth*sinHYP)*sinKP)-sinAP*(cosKP*(cosHYP*sinHP+cosHP*cosHRMinusPiFourth*sinHYP)+(cosHP*cosHYP-cosHRMinusPiFourth*sinHP*sinHYP)*sinKP));
        const float j_1_2 = -FOOT_HEIGHT*(-sinAR*(sinAP*(cosKP*(cosHP*cosHYP-cosHRMinusPiFourth*sinHP*sinHYP)-(cosHYP*sinHP+cosHP*cosHRMinusPiFourth*sinHYP)*sinKP)+cosAP*(cosKP*(cosHYP*sinHP+cosHP*cosHRMinusPiFourth*sinHYP)+(cosHP*cosHYP-cosHRMinusPiFourth*sinHP*sinHYP)*sinKP))-cosAR*sinHYP*sinHRMinusPiFourth);
        const float j_2_1 = -FOOT_HEIGHT*cosAR*(cosAP*(-sinKP*((sinHP*sinHYP)/sqrt2+cosHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+cosKP*((cosHP*sinHYP)/sqrt2-sinHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))-sinAP*(cosKP*((sinHP*sinHYP)/sqrt2+cosHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+sinKP*((cosHP*sinHYP)/sqrt2-sinHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))));
        const float j_2_2 = -FOOT_HEIGHT*(-cosAR*(cosHRMinusPiFourth/sqrt2-(cosHYP*sinHRMinusPiFourth)/sqrt2)-sinAR*(sinAP*(-sinKP*((sinHP*sinHYP)/sqrt2+cosHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+cosKP*((cosHP*sinHYP)/sqrt2-sinHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))+cosAP*(cosKP*((sinHP*sinHYP)/sqrt2+cosHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+sinKP*((cosHP*sinHYP)/sqrt2-sinHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))));
        const float j_3_1 = -FOOT_HEIGHT*cosAR*(cosAP*(-sinKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))-sinAP*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))));
        const float j_3_2 = -FOOT_HEIGHT*(-cosAR*(cosHRMinusPiFourth/sqrt2+(cosHYP*sinHRMinusPiFourth)/sqrt2)-sinAR*(sinAP*(-sinKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))+cosAP*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))));
        ufmatrix3 jacobian(3,2);
        jacobian(0,0) = j_1_1;  jacobian(0,1) = j_1_2;
        jacobian(1,0) = j_2_1;  jacobian(1,1) = j_2_2;
        jacobian(2,0) = j_3_1;  jacobian(2,1) = j_3_2;
        return jacobian;
    }else
        throw "Not a valid chain to get a jacobian from";
}

const bool Kinematics::adjustAnkle(const ChainID chainID,
                                   const ufvector3 &goal,
                                   float startAngles[],
                                   const float maxError = .1) {
    const ufmatrix3 dampenMatrix =
        ublas::identity_matrix<float> (3)*(dampFactor*dampFactor);

    ChainID ankleChainID = (chainID == LLEG_CHAIN ?
                            LANKLE_CHAIN : RANKLE_CHAIN);

    // for error calculations
    float dist_e;
    int iterations = 0;

    while (iterations < maxAnkleIterations) {
        iterations++;
        // Define the Jacobian that describes the linear approximation at the
        // current angle values.
        const ufmatrix3 j = buildJacobians(ankleChainID, startAngles);
        const ufmatrix3 j_t = trans(j);

        const ufvector3 currentAnklePosition =
            forwardKinematics(ankleChainID,
                              startAngles);
        const ufvector3 e = goal - currentAnklePosition;

        // Check if we have gotten close enough
        dist_e = norm_2(e);

        if (dist_e < maxError)
            return true;

        ufmatrix3 temp = prod(j, j_t);
        temp += dampenMatrix;
        // Now we need to find a vector that we'll call 'result' such that
        // temp*f = e. The solve method is a local implementation and not part
        // of the library (as dumb as that may seem).
        const ufvector3 result = solve(temp, e);
        // Now we multiply j_t by result and we get delta_theta
        ufvector3 ankleDeltaTheta = prod(j_t, result);

        startAngles[1] += clip(ankleDeltaTheta(0),
                               -maxDeltaTheta,
                               maxDeltaTheta);
        startAngles[2] += clip(ankleDeltaTheta(1),
                               -maxDeltaTheta,
                               maxDeltaTheta);
        startAngles[3] += clip(ankleDeltaTheta(2),
                               -maxDeltaTheta,
                               maxDeltaTheta);
        clipChainAngles(chainID, startAngles);
    }

    return false;
}


const bool Kinematics::adjustHeel(const ChainID chainID,
                                  const ufvector3 &goal,
                                  float startAngles[],
                                  const float maxError = .1) {
    ChainID ankleChainID = (chainID == LLEG_CHAIN ?
                            LANKLE_CHAIN : RANKLE_CHAIN);
    const ufmatrix3 dampenMatrix =
        ublas::identity_matrix<float> (3)*(dampFactor*dampFactor);

    int iterations = 0;
    float dist_e_heel;

    while (iterations < maxHeelIterations) {
        iterations++;
        const ufmatrix3 jHeel =
            buildJacobians(chainID, startAngles);
        const ufmatrix3 jHeel_t = trans(jHeel);

        const ufvector3 currentHeelPosition =
            forwardKinematics(chainID,startAngles);
        const ufvector3 eHeel = goal - currentHeelPosition;

        dist_e_heel = norm_2(eHeel);
        if (dist_e_heel < maxError)
            return true;

        ufmatrix3 temp = prod(jHeel, jHeel_t);
        temp += dampenMatrix;

        const ufvector3 result = solve(temp, eHeel);
        // Now we multiply j_t by result and we get delta_theta
        ufvector3 heelDeltaTheta = prod(jHeel_t, result);

        startAngles[4] += clip(heelDeltaTheta(0),
                                 -maxDeltaTheta,
                                 maxDeltaTheta);
        startAngles[5] += clip(heelDeltaTheta(1),
                                 -maxDeltaTheta,
                                 maxDeltaTheta);
        clipChainAngles(chainID,startAngles);
    }

    return false;
}

/*
 *
 */
const Kinematics::IKLegResult
Kinematics::dls(const ChainID chainID,
                const ufvector3 &goal,
                const float startAngles[],
                const float maxError,
                const float maxHeelError) {
    ChainID ankleChainID = (chainID == LLEG_CHAIN ?
                            LANKLE_CHAIN : RANKLE_CHAIN);

    ufvector3 ankleGoal(3);
    ankleGoal.assign(goal);
    // the ankle should be FOOT_HEIGHT above what the final goal is
    ankleGoal(2) = ankleGoal(2) + FOOT_HEIGHT;

    float currentAngles[LEG_JOINTS];
    memcpy(currentAngles, startAngles, LEG_JOINTS*sizeof(float));

    // The optimization method hits a singularity if the leg is perfectly
    // straight, so we can virtually bend the knee .3 radians and go around
    // that.

    if( fabs(currentAngles[3]) < .2f )
        currentAngles[3] = .2f;

    bool ankleSuccess =
        adjustAnkle(chainID, ankleGoal, currentAngles, maxError);

    // calculate the position of the heel. It should be FOOT_HEIGHT below
    // where the ankle managed to go.
    const ufvector3 currentAnklePosition =
        forwardKinematics(ankleChainID,currentAngles);
    ufvector3 heelGoal(3);
    heelGoal.assign(currentAnklePosition);
    heelGoal(2) = heelGoal(2) - FOOT_HEIGHT;

    bool heelSuccess =
        adjustHeel(chainID, heelGoal, currentAngles, maxHeelError);

    IKLegResult result;
    result.outcome = (ankleSuccess && heelSuccess ? SUCCESS : STUCK);
    memcpy(result.angles,currentAngles,LEG_JOINTS*sizeof(float));
    return result;
}

/**
 * The following method implements the analytic (exact) IK solution for the Nao
 * which Dr. Dan Lee from UPenn was so gracious to show me. The code is ported
 * from Matlab.
 *
 * Note that there are several frames of reference which are important to
 * understanding this approach:
 *  F -- the coordinate frame aligned with the bottom of the foot in question,
 *       located directly beneath the heel
 *  C -- the coordinate frame aligned with the body, located at the bellybutton
 *  O -- an aritrary origin, which may coincide with F, C or neither
 *
 *  When referencing 4x4 homogeneous matrices, the convention is to label them
 *  ab_Transform which means ab_Tranform*Va = Vb, that is, the ab transform
 *  moves points from the a coordinate frame into the b coordinate frame.
 *
 * Triangles:
 *  TTL --  THIGH_LENGTH, TIBIA_LENTH, legLenth triangle, where legLength
 *          is the distance between the ankle and the hip
 *
 * Overview - first we find all the angles in the knee, and ankle
 *            then we tackle the hip
 */
//#define DEBUG_ANA
const Kinematics::IKLegResult Kinematics::analyticLegIK(const ChainID chainID,
                                      const ufvector3 &footGoal,
                                      const ufvector3 &footOrientation,
                                      const ufvector3 &bodyGoal,
                                      const ufvector3 &bodyOrientation,
                                      const float givenHYPAngle)
{
    bool success = true;
#ifdef DEBUG_ANA
    cout << "anaIK inputs:"<<endl
         <<"  footGoal: "<<footGoal<<endl
         <<"  footOrientation: "<<footOrientation<<endl
         <<"  bodyGoal: "<<bodyGoal<<endl
         <<"  bodyOrientation: "<<bodyOrientation<<endl
         <<"  HYP Angle: " <<givenHYPAngle<<endl;
#endif
    //fo - translate from f to o
    const ufmatrix4 fo_Transform = CoordFrame4D::get6DTransform(footGoal(0),
                                                footGoal(1),footGoal(2),
                                                footOrientation(0),
                                                footOrientation(1),
                                                footOrientation(2));
    //co - translate from c to o
    const ufmatrix4 co_Transform = CoordFrame4D::get6DTransform(bodyGoal(0),
                                                bodyGoal(1),bodyGoal(2),
                                                bodyOrientation(0),
                                                bodyOrientation(1),
                                                bodyOrientation(2));
#ifdef DEBUG_ANA
    cout << "fo_Transform: "<<endl<< "  "<<fo_Transform<<endl;
    cout << "co_Transform: "<<endl<< "  "<<co_Transform<<endl;
#endif
    //fc - translate from f to o to c
    const ufmatrix4 fc_Transform =
        prod(CoordFrame4D::invertHomogenous(co_Transform),fo_Transform);

    //cf - translate from c to o to f
    const ufmatrix4 cf_Transform =
        prod(CoordFrame4D::invertHomogenous(fo_Transform),co_Transform);

#ifdef DEBUG_ANA
    cout << "cf_Transform: "<<endl<< "  "<<cf_Transform<<endl;
    cout << "fc_Transform: "<<endl<< "  "<<fc_Transform<<endl;
#endif

    const float leg_sign = (chainID == LLEG_CHAIN ? 1.0f : -1.0f);

    //The location of the hip rotation center in the C frame
    const ufvector4 hipOffset_c = CoordFrame4D::vector4D(0.0f,
                                                         leg_sign*HIP_OFFSET_Y,
                                                         -HIP_OFFSET_Z);
    const ufvector4 ankleOffset_f =CoordFrame4D::vector4D(0.0f,
                                                          0.0f,
                                                          FOOT_HEIGHT);

    //Find the location of the hip in the F frame that is shifted
    //to the ankle from the bottom of the foot
    const ufvector4 hipPosition_fprime =
        prod(cf_Transform,hipOffset_c) - ankleOffset_f;

#ifdef DEBUG_ANA
    cout<< "Hip position in fprime: "<< hipPosition_fprime<<endl;
#endif

    //squared dist from ankle to hip
    const float legLength = norm_2(hipPosition_fprime);
    const float legLengthSq = std::pow(legLength,2);
    if(legLength > THIGH_LENGTH+TIBIA_LENGTH)
        success = false;
#ifdef DEBUG_ANA
    cout<< "LegLength: "<< legLength << ", sqrd = "<<legLengthSq<<endl;
#endif

    //Using the law of cosines to find knee pitch in TTL triangle
    const float kneeCosine =
        (legLengthSq -TIBIA_LENGTH*TIBIA_LENGTH - THIGH_LENGTH*THIGH_LENGTH)/
                  (2.0f*TIBIA_LENGTH*THIGH_LENGTH);
#ifdef DEBUG_ANA
    cout<< "KneeCosine: "<<kneeCosine
        << " unclipped cos"<< std::acos(kneeCosine)<<endl;
#endif

    const float KP = std::acos(std::min(std::max(kneeCosine,-1.0f),
                                               1.0f));
#ifdef DEBUG_ANA
    cout<< "Calculated KP: "<<KP<<endl;
#endif
    //Now, we can find the ankle roll using only the position of hip in f:
    const float AR = std::atan2(hipPosition_fprime(CoordFrame4D::Y_AXIS),
                                hipPosition_fprime(CoordFrame4D::Z_AXIS));

#ifdef DEBUG_ANA
    cout<< "Calculated AR: "<<AR<<endl;
#endif

    //To find AP, we first use the law of sines to find angle opposite
    //the THIGH in the TTL tri.
    //Also, note, even though the TTL triangle is not in the plane XZ plane of
    //the F frame, the following still works, since scaling the triangle into
    //that frame creates a similar triangle with the same angles
    const float pitch0 = std::asin(THIGH_LENGTH*std::sin(KP)/legLength);
    const float AP =
        std::asin(-hipPosition_fprime(CoordFrame4D::X_AXIS)/legLength) - pitch0;

#ifdef DEBUG_ANA
    cout<< "Calculated AP: "<<AP<<endl;
#endif

    float tempHYP = givenHYPAngle;
    //If the HYP was not passed in, we need to find it:
    if(givenHYPAngle == HYP_NOT_SET){
        //find the rotation-only transform from C to F back to Hip
        const ufmatrix3 cf_Rot  = subrange(cf_Transform,0,3,0,3);
        const ufmatrix3 temp =
            prod(CoordFrame3D::rotation3D(CoordFrame3D::Y_AXIS,
                                          AP+KP),
                 CoordFrame3D::rotation3D(CoordFrame3D::X_AXIS,
                                          AR));
        const ufmatrix3 cfh_Transform =
            prod(temp,
                 cf_Rot);

        // next, grab the hipYawPitch angle from the cfh_Transform matrix.
        // What? that's right!
        // Here's how it works. The rHip rotation describes C->after_hip
        // transform. If we assume that the HYP was the only joint one could use
        // in the hip, then to modify rHip to be a C->C transform (i.e. I),
        // you could do the following:
        // find the the matrix RHYP, such that I = RHYP*rHip, let
        // RHYP^-1 = rHip
        // If you find RHYP = Rotx[-3Pi/4].Rotx[HYP].Rotx[3Pi/4] (for left),
        // then you can evaluate (symbolicaly) RHYP^-1 with a transpose,
        // and see that to solve for HYP, you can apply the formulas below
        // neat stuff...
        if(chainID == LLEG_CHAIN){
            tempHYP =
                std::atan2(std::sqrt(2.0f)*cfh_Transform(CoordFrame3D::Y_AXIS,
                                                         CoordFrame3D::X_AXIS),
                           cfh_Transform(CoordFrame3D::Y_AXIS,
                                         CoordFrame3D::Y_AXIS) +
                           cfh_Transform(CoordFrame3D::Y_AXIS,
                                         CoordFrame3D::Z_AXIS));
        }else{
            tempHYP =
                std::atan2(-std::sqrt(2.0f)*cfh_Transform(CoordFrame3D::Y_AXIS,
                                                         CoordFrame3D::X_AXIS),
                           cfh_Transform(CoordFrame3D::Y_AXIS,
                                         CoordFrame3D::Y_AXIS) -
                           cfh_Transform(CoordFrame3D::Y_AXIS,
                                         CoordFrame3D::Z_AXIS));
        }
    }
    const float HYP = tempHYP;
#ifdef DEBUG_ANA
    cout<< "Calculated HYP: "<<HYP<<endl;
#endif

    //Now we are left only to find the HipRoll and HipPitch

    //Find the location of the ankle in a C frame shifted to hip
    const ufvector4 anklePosition_cprime = prod(fc_Transform,
                                                ankleOffset_f) - hipOffset_c;

    //Now, we have already found HYP, so we will shift the cprime
    //frame to the d frame. The d frame is positioned at the hip,
    //and parrallel to the cprime EXCEPT for the HYP rotation, which is added:
    const ufvector4 anklePosition_d = prod(( chainID == LLEG_CHAIN ?
                                             rotationHYPLeftInv(HYP) :
                                             rotationHYPRightInv(HYP)),
                                           anklePosition_cprime);


    //Finding the hip Roll easy in the d frame:
    const float HR = std::atan2(anklePosition_d(CoordFrame4D::Y_AXIS),
                                -anklePosition_d(CoordFrame4D::Z_AXIS));
#ifdef DEBUG_ANA
    cout<< "Calculated HR: "<<HR<<endl;
#endif

    //Again, using the law of sines, we can find the angle accross from
    //TIBIA_LENGTH in the triangle TTL:
    const float pitch1 = std::asin(TIBIA_LENGTH*std::sin(KP)/legLength);
    const float HP = std::asin(-anklePosition_d(CoordFrame4D::X_AXIS)
                               /legLength) - pitch1;
#ifdef DEBUG_ANA
    cout<< "Calculated HP: "<<HP<<endl;
#endif

    //Setup the return value:
    IKLegResult result;

    result.angles[0] = HYP;
    result.angles[1] = HR;
    result.angles[2] = HP;
    result.angles[3] = KP;
    result.angles[4] = AP;
    result.angles[5] = AR;
    result.outcome = (success ? SUCCESS : STUCK);
    return result;
}

ufmatrix4 Kinematics::rotationHYPLeftInv(const float HYP){
    float sinHYP, cosHYP;
    sincosf(HYP,&sinHYP,&cosHYP);
    const float sqrt2 = std::sqrt(2.0f);

    ufmatrix4 r  = ublas::identity_matrix<float>(4);

    r(0,0) = cosHYP;
    r(0,1) = -sinHYP/sqrt2;
    r(0,2) = -sinHYP/sqrt2;

    r(1,0) = sinHYP/sqrt2;
    r(1,1) = 0.5f+cosHYP/2;
    r(1,2) = -0.5f+cosHYP/2;

    r(2,0) = sinHYP/sqrt2;
    r(2,1) = -0.5f+cosHYP/2;
    r(2,2) = 0.5f+cosHYP/2;

    return r;
}


ufmatrix4 Kinematics::rotationHYPRightInv(const float HYP){
    float sinHYP, cosHYP;
    sincosf(HYP,&sinHYP,&cosHYP);
    const float sqrt2 = std::sqrt(2.0f);

    ufmatrix4 r  = ublas::identity_matrix<float>(4);

    r(0,0) = cosHYP;
    r(0,1) = sinHYP/sqrt2;
    r(0,2) = -sinHYP/sqrt2;

    r(1,0) = -sinHYP/sqrt2;
    r(1,1) = 0.5f+cosHYP/2;
    r(1,2) = 0.5f-cosHYP/2;

    r(2,0) = sinHYP/sqrt2;
    r(2,1) = 0.5f-cosHYP/2;
    r(2,2) = 0.5f+cosHYP/2;

    return r;
}


