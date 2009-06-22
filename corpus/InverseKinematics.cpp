#include <cmath>

#include "InverseKinematics.h"


using namespace boost::numeric;
using namespace NBMath;
using namespace std;

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
 */

const Kinematics::IKLegResult Kinematics::analyticLegIK(const ChainID chainID,
                                      const ufvector3 &footGoal,
                                      const ufvector3 &footOrientation,
                                      const ufvector3 &bodyGoal,
                                      const ufvector3 &bodyOrientation,
                                      const float givenHYPAngle)
{

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
    //fc - translate from f to o to c
    const ufmatrix4 fc_Transform =
        prod(CoordFrame4D::invertHomogenous(co_Transform),fo_Transform);

    //cf - translate from c to o to f
    const ufmatrix4 cf_Transform =
        prod(CoordFrame4D::invertHomogenous(fo_Transform),co_Transform);

    IKLegResult result;
    return result;
}
