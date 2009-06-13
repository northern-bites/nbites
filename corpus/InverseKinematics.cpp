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

#define TEMP 0.0f
//bodily dimensions
const float THL = TEMP;
const float TIL = TEMP;
const float FH = TEMP;
const float HOY = TEMP;
const float HOZ= TEMP;

//angles


//fillable floats
float sqrt2 = .22f;
float
    j_1_1,j_1_2,j_1_3,
    j_2_1,j_2_2,j_2_3,
    j_3_1,j_3_2,j_3_3;

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


        switch(id){
        case LLEG_CHAIN:
            x = -THL*(cosHYP*sinHP+cosHP*cosHRPlusPiFourth*sinHYP)-TIL*(cosKP*(cosHYP*sinHP+cosHP*cosHRPlusPiFourth*sinHYP)+(cosHP*cosHYP-cosHRPlusPiFourth*sinHP*sinHYP)*sinKP)-FH*(cosAR*(sinAP*(cosKP*(cosHP*cosHYP-cosHRPlusPiFourth*sinHP*sinHYP)-(cosHYP*sinHP+cosHP*cosHRPlusPiFourth*sinHYP)*sinKP)+cosAP*(cosKP*(cosHYP*sinHP+cosHP*cosHRPlusPiFourth*sinHYP)+(cosHP*cosHYP-cosHRPlusPiFourth*sinHP*sinHYP)*sinKP))-sinAR*sinHYP*sinHRPlusPiFourth);
            y = HOY-THL*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))-TIL*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2)))-FH*(-sinAR*(cosHRPlusPiFourth/sqrt2+(cosHYP*sinHRPlusPiFourth)/sqrt2)+cosAR*(sinAP*(-sinKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2)))+cosAP*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2)))));
            z = -HOZ-THL*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))-TIL*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2)))-FH*(-sinAR*(-cosHRPlusPiFourth/sqrt2+(cosHYP*sinHRPlusPiFourth)/sqrt2)+cosAR*(sinAP*(-sinKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2)))+cosAP*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2)))));
            break;
        case LANKLE_CHAIN:
            x = -THL*(cosHYP*sinHP+cosHP*cosHRPlusPiFourth*sinHYP)-TIL*(cosKP*(cosHYP*sinHP+cosHP*cosHRPlusPiFourth*sinHYP)+(cosHP*cosHYP-cosHRPlusPiFourth*sinHP*sinHYP)*sinKP);
            y = HOY-THL*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))-TIL*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2)));
            z = -HOZ-THL*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))-TIL*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2)));
            break;
        case RLEG_CHAIN:
            x = -THL*(cosHYP*sinHP+cosHP*cosHRMinusPiFourth*sinHYP)-TIL*(cosKP*(cosHYP*sinHP+cosHP*cosHRMinusPiFourth*sinHYP)+(cosHP*cosHYP-cosHRMinusPiFourth*sinHP*sinHYP)*sinKP)-FH*(cosAR*(sinAP*(cosKP*(cosHP*cosHYP-cosHRMinusPiFourth*sinHP*sinHYP)-(cosHYP*sinHP+cosHP*cosHRMinusPiFourth*sinHYP)*sinKP)+cosAP*(cosKP*(cosHYP*sinHP+cosHP*cosHRMinusPiFourth*sinHYP)+(cosHP*cosHYP-cosHRMinusPiFourth*sinHP*sinHYP)*sinKP))-sinAR*sinHYP*sinHRMinusPiFourth);
            y = -HOY-THL*((sinHP*sinHYP)/sqrt2+cosHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))-TIL*(cosKP*((sinHP*sinHYP)/sqrt2+cosHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+sinKP*((cosHP*sinHYP)/sqrt2-sinHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))-FH*(-sinAR*(cosHRMinusPiFourth/sqrt2-(cosHYP*sinHRMinusPiFourth)/sqrt2)+cosAR*(sinAP*(-sinKP*((sinHP*sinHYP)/sqrt2+cosHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+cosKP*((cosHP*sinHYP)/sqrt2-sinHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))+cosAP*(cosKP*((sinHP*sinHYP)/sqrt2+cosHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+sinKP*((cosHP*sinHYP)/sqrt2-sinHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))));
            z = -HOZ-THL*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))-TIL*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))-FH*(-sinAR*(cosHRMinusPiFourth/sqrt2+(cosHYP*sinHRMinusPiFourth)/sqrt2)+cosAR*(sinAP*(-sinKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))+cosAP*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))));
            break;
        case RANKLE_CHAIN:
            x = -THL*(cosHYP*sinHP+cosHP*cosHRMinusPiFourth*sinHYP)-TIL*(cosKP*(cosHYP*sinHP+cosHP*cosHRMinusPiFourth*sinHYP)+(cosHP*cosHYP-cosHRMinusPiFourth*sinHP*sinHYP)*sinKP)-FH*(cosAR*(sinAP*(cosKP*(cosHP*cosHYP-cosHRMinusPiFourth*sinHP*sinHYP)-(cosHYP*sinHP+cosHP*cosHRMinusPiFourth*sinHYP)*sinKP)+cosAP*(cosKP*(cosHYP*sinHP+cosHP*cosHRMinusPiFourth*sinHYP)+(cosHP*cosHYP-cosHRMinusPiFourth*sinHP*sinHYP)*sinKP))-sinAR*sinHYP*sinHRMinusPiFourth);
            y = -HOY-THL*((sinHP*sinHYP)/sqrt2+cosHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))-TIL*(cosKP*((sinHP*sinHYP)/sqrt2+cosHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+sinKP*((cosHP*sinHYP)/sqrt2-sinHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))-FH*(-sinAR*(cosHRMinusPiFourth/sqrt2-(cosHYP*sinHRMinusPiFourth)/sqrt2)+cosAR*(sinAP*(-sinKP*((sinHP*sinHYP)/sqrt2+cosHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+cosKP*((cosHP*sinHYP)/sqrt2-sinHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))+cosAP*(cosKP*((sinHP*sinHYP)/sqrt2+cosHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+sinKP*((cosHP*sinHYP)/sqrt2-sinHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))));
            z = -HOZ-THL*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))-TIL*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))-FH*(-sinAR*(cosHRMinusPiFourth/sqrt2+(cosHYP*sinHRMinusPiFourth)/sqrt2)+cosAR*(sinAP*(-sinKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))+cosAP*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))));
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


int main(){
    float legAngles[] = {0.0f,0.0f,0.0f,
                       0.0f,0.0f,0.0f};
    ufvector3 result = Kinematics::forwardKinematics(Kinematics::LLEG_CHAIN,legAngles);
    cout <<result<<endl;
}

void randomCrap(){
const float HYP = TEMP;
const float HP = TEMP;
const float HR = TEMP;
const float KP = TEMP;
const float AP = TEMP;
const float AR = TEMP;

    float sinHYP,cosHYP,sinHR,cosHR,sinHP,cosHP,sinKP,cosKP,sinAP,cosAP,sinAR,cosAR;
    float cosHRPlusPiFourth = std::cos(HR+M_PI_FLOAT*0.25f);
    float cosHRMinusPiFourth = std::cos(HR-M_PI_FLOAT*0.25f);
    float sinHRPlusPiFourth = std::sin(HR+M_PI_FLOAT*0.25f);
    float sinHRMinusPiFourth = std::sin(HR-M_PI_FLOAT*0.25f);

    float    x,y,z;
//Jacobians
//Left*leg
//NO*HEEL
j_1_1 = -THL*(cosHP*cosHYP-cosHRPlusPiFourth*sinHP*sinHYP)-TIL*(cosKP*(cosHP*cosHYP-cosHRPlusPiFourth*sinHP*sinHYP)+(-cosHYP*sinHP-cosHP*cosHRPlusPiFourth*sinHYP)*sinKP);
j_1_2 = THL*cosHP*sinHYP*sinHRPlusPiFourth-TIL*(-cosHP*cosKP*sinHYP*sinHRPlusPiFourth+sinHP*sinHYP*sinKP*sinHRPlusPiFourth);
j_1_3 = -TIL*(cosKP*(cosHP*cosHYP-cosHRPlusPiFourth*sinHP*sinHYP)-(cosHYP*sinHP+cosHP*cosHRPlusPiFourth*sinHYP)*sinKP);
j_2_1 = -THL*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))-TIL*(sinKP*((sinHP*sinHYP)/sqrt2-cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2)));
j_2_2 = -THL*cosHP*(-cosHRPlusPiFourth/sqrt2-(cosHYP*sinHRPlusPiFourth)/sqrt2)-TIL*(cosHP*cosKP*(-cosHRPlusPiFourth/sqrt2-(cosHYP*sinHRPlusPiFourth)/sqrt2)-sinHP*sinKP*(-cosHRPlusPiFourth/sqrt2-(cosHYP*sinHRPlusPiFourth)/sqrt2));
j_2_3 = -TIL*(-sinKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2)));
j_3_1 = -THL*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))-TIL*(sinKP*((sinHP*sinHYP)/sqrt2-cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2)));
j_3_2 = -THL*cosHP*(cosHRPlusPiFourth/sqrt2-(cosHYP*sinHRPlusPiFourth)/sqrt2)-TIL*(cosHP*cosKP*(cosHRPlusPiFourth/sqrt2-(cosHYP*sinHRPlusPiFourth)/sqrt2)-sinHP*sinKP*(cosHRPlusPiFourth/sqrt2-(cosHYP*sinHRPlusPiFourth)/sqrt2));
j_3_3 = -TIL*(-sinKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2)));

//WITH*HEEL
j_1_1 = -FH*cosAR*(cosAP*(cosKP*(cosHP*cosHYP-cosHRPlusPiFourth*sinHP*sinHYP)-(cosHYP*sinHP+cosHP*cosHRPlusPiFourth*sinHYP)*sinKP)-sinAP*(cosKP*(cosHYP*sinHP+cosHP*cosHRPlusPiFourth*sinHYP)+(cosHP*cosHYP-cosHRPlusPiFourth*sinHP*sinHYP)*sinKP));
j_1_2 = -FH*(-sinAR*(sinAP*(cosKP*(cosHP*cosHYP-cosHRPlusPiFourth*sinHP*sinHYP)-(cosHYP*sinHP+cosHP*cosHRPlusPiFourth*sinHYP)*sinKP)+cosAP*(cosKP*(cosHYP*sinHP+cosHP*cosHRPlusPiFourth*sinHYP)+(cosHP*cosHYP-cosHRPlusPiFourth*sinHP*sinHYP)*sinKP))-cosAR*sinHYP*sinHRPlusPiFourth);
j_1_3 = -FH*cosAR*(cosAP*(-sinKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2)))-sinAP*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))));
j_2_1 = -FH*(-cosAR*(cosHRPlusPiFourth/sqrt2+(cosHYP*sinHRPlusPiFourth)/sqrt2)-sinAR*(sinAP*(-sinKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2)))+cosAP*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2-sinHRPlusPiFourth/sqrt2)))));
j_2_2 = -FH*cosAR*(cosAP*(-sinKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2)))-sinAP*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))));
j_2_3 = -FH*(-cosAR*(-cosHRPlusPiFourth/sqrt2+(cosHYP*sinHRPlusPiFourth)/sqrt2)-sinAR*(sinAP*(-sinKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2)))+cosAP*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRPlusPiFourth)/sqrt2+sinHRPlusPiFourth/sqrt2)))));


//Right*leg
//NO*HEEL
j_1_1 = -THL*(cosHP*cosHYP-cosHRMinusPiFourth*sinHP*sinHYP)-TIL*(cosKP*(cosHP*cosHYP-cosHRMinusPiFourth*sinHP*sinHYP)+(-cosHYP*sinHP-cosHP*cosHRMinusPiFourth*sinHYP)*sinKP)-FH*cosAR*(cosAP*(cosKP*(cosHP*cosHYP-cosHRMinusPiFourth*sinHP*sinHYP)+(-cosHYP*sinHP-cosHP*cosHRMinusPiFourth*sinHYP)*sinKP)+sinAP*(cosKP*(-cosHYP*sinHP-cosHP*cosHRMinusPiFourth*sinHYP)-(cosHP*cosHYP-cosHRMinusPiFourth*sinHP*sinHYP)*sinKP));
j_1_2 = THL*cosHP*sinHYP*sinHRMinusPiFourth-TIL*(-cosHP*cosKP*sinHYP*sinHRMinusPiFourth+sinHP*sinHYP*sinKP*sinHRMinusPiFourth)-FH*(-cosHRMinusPiFourth*sinAR*sinHYP+cosAR*(sinAP*(cosKP*sinHP*sinHYP*sinHRMinusPiFourth+cosHP*sinHYP*sinKP*sinHRMinusPiFourth)+cosAP*(-cosHP*cosKP*sinHYP*sinHRMinusPiFourth+sinHP*sinHYP*sinKP*sinHRMinusPiFourth)));
j_1_3 = -TIL*(cosKP*(cosHP*cosHYP-cosHRMinusPiFourth*sinHP*sinHYP)-(cosHYP*sinHP+cosHP*cosHRMinusPiFourth*sinHYP)*sinKP)-FH*cosAR*(cosAP*(cosKP*(cosHP*cosHYP-cosHRMinusPiFourth*sinHP*sinHYP)-(cosHYP*sinHP+cosHP*cosHRMinusPiFourth*sinHYP)*sinKP)+sinAP*(-cosKP*(cosHYP*sinHP+cosHP*cosHRMinusPiFourth*sinHYP)-(cosHP*cosHYP-cosHRMinusPiFourth*sinHP*sinHYP)*sinKP));
j_2_1 = -THL*((cosHP*sinHYP)/sqrt2-sinHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))-TIL*(sinKP*(-(sinHP*sinHYP)/sqrt2-cosHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+cosKP*((cosHP*sinHYP)/sqrt2-sinHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))-FH*cosAR*(cosAP*(sinKP*(-(sinHP*sinHYP)/sqrt2-cosHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+cosKP*((cosHP*sinHYP)/sqrt2-sinHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))+sinAP*(cosKP*(-(sinHP*sinHYP)/sqrt2-cosHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))-sinKP*((cosHP*sinHYP)/sqrt2-sinHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))));
j_2_2 = -THL*cosHP*(-cosHRMinusPiFourth/sqrt2+(cosHYP*sinHRMinusPiFourth)/sqrt2)-TIL*(cosHP*cosKP*(-cosHRMinusPiFourth/sqrt2+(cosHYP*sinHRMinusPiFourth)/sqrt2)-sinHP*sinKP*(-cosHRMinusPiFourth/sqrt2+(cosHYP*sinHRMinusPiFourth)/sqrt2))-FH*(-sinAR*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)+cosAR*(sinAP*(-cosKP*sinHP*(-cosHRMinusPiFourth/sqrt2+(cosHYP*sinHRMinusPiFourth)/sqrt2)-cosHP*sinKP*(-cosHRMinusPiFourth/sqrt2+(cosHYP*sinHRMinusPiFourth)/sqrt2))+cosAP*(cosHP*cosKP*(-cosHRMinusPiFourth/sqrt2+(cosHYP*sinHRMinusPiFourth)/sqrt2)-sinHP*sinKP*(-cosHRMinusPiFourth/sqrt2+(cosHYP*sinHRMinusPiFourth)/sqrt2))));
j_2_3 = -TIL*(-sinKP*((sinHP*sinHYP)/sqrt2+cosHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+cosKP*((cosHP*sinHYP)/sqrt2-sinHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))-FH*cosAR*(cosAP*(-sinKP*((sinHP*sinHYP)/sqrt2+cosHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+cosKP*((cosHP*sinHYP)/sqrt2-sinHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))+sinAP*(-cosKP*((sinHP*sinHYP)/sqrt2+cosHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))-sinKP*((cosHP*sinHYP)/sqrt2-sinHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))));
j_3_1 = -THL*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))-TIL*(sinKP*((sinHP*sinHYP)/sqrt2-cosHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))-FH*cosAR*(cosAP*(sinKP*((sinHP*sinHYP)/sqrt2-cosHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))+sinAP*(cosKP*((sinHP*sinHYP)/sqrt2-cosHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))-sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))));
j_3_2 = -THL*cosHP*(-cosHRMinusPiFourth/sqrt2-(cosHYP*sinHRMinusPiFourth)/sqrt2)-TIL*(cosHP*cosKP*(-cosHRMinusPiFourth/sqrt2-(cosHYP*sinHRMinusPiFourth)/sqrt2)-sinHP*sinKP*(-cosHRMinusPiFourth/sqrt2-(cosHYP*sinHRMinusPiFourth)/sqrt2))-FH*(-sinAR*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)+cosAR*(sinAP*(-cosKP*sinHP*(-cosHRMinusPiFourth/sqrt2-(cosHYP*sinHRMinusPiFourth)/sqrt2)-cosHP*sinKP*(-cosHRMinusPiFourth/sqrt2-(cosHYP*sinHRMinusPiFourth)/sqrt2))+cosAP*(cosHP*cosKP*(-cosHRMinusPiFourth/sqrt2-(cosHYP*sinHRMinusPiFourth)/sqrt2)-sinHP*sinKP*(-cosHRMinusPiFourth/sqrt2-(cosHYP*sinHRMinusPiFourth)/sqrt2))));
j_3_3 = -TIL*(-sinKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))-FH*cosAR*(cosAP*(-sinKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))+sinAP*(-cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))-sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))));

//WITH*HEEL
j_1_1 = -FH*cosAR*(cosAP*(cosKP*(cosHP*cosHYP-cosHRMinusPiFourth*sinHP*sinHYP)-(cosHYP*sinHP+cosHP*cosHRMinusPiFourth*sinHYP)*sinKP)-sinAP*(cosKP*(cosHYP*sinHP+cosHP*cosHRMinusPiFourth*sinHYP)+(cosHP*cosHYP-cosHRMinusPiFourth*sinHP*sinHYP)*sinKP));
j_1_2 = -FH*(-sinAR*(sinAP*(cosKP*(cosHP*cosHYP-cosHRMinusPiFourth*sinHP*sinHYP)-(cosHYP*sinHP+cosHP*cosHRMinusPiFourth*sinHYP)*sinKP)+cosAP*(cosKP*(cosHYP*sinHP+cosHP*cosHRMinusPiFourth*sinHYP)+(cosHP*cosHYP-cosHRMinusPiFourth*sinHP*sinHYP)*sinKP))-cosAR*sinHYP*sinHRMinusPiFourth);
j_1_3 = -FH*cosAR*(cosAP*(-sinKP*((sinHP*sinHYP)/sqrt2+cosHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+cosKP*((cosHP*sinHYP)/sqrt2-sinHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))-sinAP*(cosKP*((sinHP*sinHYP)/sqrt2+cosHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+sinKP*((cosHP*sinHYP)/sqrt2-sinHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))));
j_2_1 = -FH*(-cosAR*(cosHRMinusPiFourth/sqrt2-(cosHYP*sinHRMinusPiFourth)/sqrt2)-sinAR*(sinAP*(-sinKP*((sinHP*sinHYP)/sqrt2+cosHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+cosKP*((cosHP*sinHYP)/sqrt2-sinHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))+cosAP*(cosKP*((sinHP*sinHYP)/sqrt2+cosHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+sinKP*((cosHP*sinHYP)/sqrt2-sinHP*(-(cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))));
j_2_2 = -FH*cosAR*(cosAP*(-sinKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))-sinAP*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))));
j_2_3 = -FH*(-cosAR*(cosHRMinusPiFourth/sqrt2+(cosHYP*sinHRMinusPiFourth)/sqrt2)-sinAR*(sinAP*(-sinKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+cosKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))+cosAP*(cosKP*(-(sinHP*sinHYP)/sqrt2+cosHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2))+sinKP*(-(cosHP*sinHYP)/sqrt2-sinHP*((cosHYP*cosHRMinusPiFourth)/sqrt2-sinHRMinusPiFourth/sqrt2)))));

}
