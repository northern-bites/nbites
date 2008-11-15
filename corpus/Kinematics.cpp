#include <iostream>
#include "Kinematics.h"

const float Kinematics::clip(const float value,
                             const float minValue, const float maxValue) {
    if (value > maxValue)
        return maxValue;
    else if (value < minValue)
        return minValue;
    else
        return value;
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
        for (int i=0; i<LEG_JOINTS; i++) {
            angles[i] = clip(angles[i],
                             LEFT_LEG_BOUNDS[i][0],LEFT_LEG_BOUNDS[i][1]);
        }
        break;
    case RLEG_CHAIN:
        for (int i=0; i<LEG_JOINTS; i++) {
            angles[i] = clip(angles[i],
                             RIGHT_LEG_BOUNDS[i][0],RIGHT_LEG_BOUNDS[i][1]);
        }
        break;
    case LARM_CHAIN:
        for (int i=0; i<ARM_JOINTS; i++) {
            angles[i] = clip(angles[i],
                             LEFT_ARM_BOUNDS[i][0],LEFT_ARM_BOUNDS[i][1]);
        }
        break;
    case RARM_CHAIN:
        for (int i=0; i<ARM_JOINTS; i++) {
            angles[i] = clip(angles[i],
                             RIGHT_ARM_BOUNDS[i][0],RIGHT_ARM_BOUNDS[i][1]);
        }
        break;
    case HEAD_CHAIN:
    default:
        for (int i=0; i<HEAD_JOINTS; i++) {
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


/* Perform forward kinematics on a set of angles.
   Returns an x,y,z point where the end of a limb would be with a given
   chainID and chain angles. The returned value is a 3 by 1 vector.
*/
const Kinematics::ufvector3 Kinematics::
forwardKinematics(const ChainID id,
                  const float angles[]) {
    ufvector3 finalPoint(3);
    float x=0,y=0,z=0;

    // Variables for legs. These will save computation by storing certain useful
    // things only once
    float HYP, HP, HR, KP, AP, AR, sinHYP, cosHYP, sinHP, cosHP, sinHR, cosHR,
        sinKP, cosKP, sinAP, cosAP, sinAR, cosAR, root2, sinAPplusKP, cosAPplusKP;

    // Variables for arms.
    float SP, SR, EY, ER, sinSP, cosSP, sinSR, cosSR, sinEY, cosEY, sinER, cosER;

    switch(id) {
    case LARM_CHAIN:
        /* Extract the correct angles from the chainAngles tuple.
           Sines and cosines get reused a lot, so calculate them once.
           # SP - shoulder pitch
           # SR - shoulder roll 
           # EY - elbow yaw
           # ER - elbow roll */
        SP = angles[0];
        SR = angles[1];
        EY = angles[2];
        ER = angles[3];
        sinSP = sin(SP);
        cosSP = cos(SP);
        sinSR = sin(SR);
        cosSR = cos(SR);
        sinEY = sin(EY);
        cosEY = cos(EY);
        sinER = sin(ER);
        cosER = cos(ER);

        /* These are precalculated functions that take some joint
           angles and return single coordinate components. Pretty sweet.
           If you want to see the derivation of these formulas, take a look
           at the method 'calcLinkTransforms'.
           The formulas were precalculated using Mathematica and the notebook
           in which this was done is in svn under nao_robocup/kinematicsTester */
        x = LOWER_ARM_LENGTH*sinER*sinEY*sinSP + cosSP*((UPPER_ARM_LENGTH + LOWER_ARM_LENGTH*cosER)*cosSR - LOWER_ARM_LENGTH*cosEY*sinER*sinSR);

        y = SHOULDER_OFFSET_Y + LOWER_ARM_LENGTH*cosEY*cosSR*sinER + (UPPER_ARM_LENGTH + LOWER_ARM_LENGTH*cosER)*sinSR;

        z = SHOULDER_OFFSET_Z + LOWER_ARM_LENGTH*cosSP*sinER*sinEY - (UPPER_ARM_LENGTH + LOWER_ARM_LENGTH*cosER)*cosSR*sinSP + LOWER_ARM_LENGTH*cosEY*sinER*sinSP*sinSR;

        break;

    case RARM_CHAIN:
        SP = angles[0];
        SR = angles[1];
        EY = angles[2];
        ER = angles[3];
        sinSP = sin(SP);
        cosSP = cos(SP);
        sinSR = sin(SR);
        cosSR = cos(SR);
        sinEY = sin(EY);
        cosEY = cos(EY);
        sinER = sin(ER);
        cosER = cos(ER);

        x = LOWER_ARM_LENGTH*sinER*sinEY*sinSP + cosSP* ((UPPER_ARM_LENGTH + LOWER_ARM_LENGTH*cosER)*cosSR - LOWER_ARM_LENGTH*cosEY*sinER*sinSR);

        y = - SHOULDER_OFFSET_Y + LOWER_ARM_LENGTH*cosEY*cosSR*sinER + (UPPER_ARM_LENGTH + LOWER_ARM_LENGTH*cosER)*sinSR;

        z = SHOULDER_OFFSET_Z + LOWER_ARM_LENGTH*cosSP*sinER*sinEY - (UPPER_ARM_LENGTH + LOWER_ARM_LENGTH*cosER)*cosSR*sinSP + LOWER_ARM_LENGTH*cosEY*sinER*sinSP*sinSR;

        break;

    case LLEG_CHAIN:
        HYP = angles[0];
        HP = angles[1];
        HR = angles[2];
        KP = angles[3];
        AP = angles[4];
        AR = angles[5];
        sinHYP = sin(HYP);
        cosHYP = cos(HYP);
        sinHP = sin(HP);
        cosHP = cos(HP);
        sinHR = sin(HR);
        cosHR = cos(HR);
        sinKP = sin(KP);
        cosKP = cos(KP);
        sinAP = sin(AP);
        cosAP = cos(AP);
        sinAR = sin(AR);
        cosAR = cos(AR);
        root2 = sqrt(2);
        sinAPplusKP = sin(AP + KP);
        cosAPplusKP = cos(AP + KP);

        // uglyyyyyyy
        x = .5f*(-cosHR*(root2*(cosHP*(THIGH_LENGTH + TIBIA_LENGTH*cosKP + FOOT_HEIGHT*cosAR*cosAPplusKP) - FOOT_HEIGHT*sinAR)*sinHYP + 2*cosHYP*sinHP*(THIGH_LENGTH + (TIBIA_LENGTH + FOOT_HEIGHT*cosAP*cosAR)*cosKP - FOOT_HEIGHT*cosAR*sinAP*sinKP)) + root2*sinHYP*((THIGH_LENGTH + TIBIA_LENGTH*cosKP + FOOT_HEIGHT*cosHP*sinAR)*sinHR + TIBIA_LENGTH*sinHP*sinKP + FOOT_HEIGHT*cosAR*(cosKP*(sinAP*sinHP + cosAP*sinHR) + (cosAP*sinHP - sinAP*sinHR)*sinKP)) + 2*cosHYP*(FOOT_HEIGHT*sinAR*sinHP*sinHR - cosHP*(TIBIA_LENGTH*sinKP + FOOT_HEIGHT*cosAR*sinAPplusKP)));

        y = .5f*(2*HIP_OFFSET_Y + THIGH_LENGTH*sinHR + THIGH_LENGTH*cosHYP*sinHR + TIBIA_LENGTH*cosKP*sinHR + TIBIA_LENGTH*cosHYP*cosKP*sinHR - 2*FOOT_HEIGHT*cosHP*sinAR*sinHR*sin(HYP/2.f)*sin(HYP/2.f) - root2*FOOT_HEIGHT*sinAR*sinHP*sinHR*sinHYP - TIBIA_LENGTH*sinHP*sinKP + TIBIA_LENGTH*cosHYP*sinHP*sinKP + root2*TIBIA_LENGTH*cosHP*sinHYP*sinKP + cosHR*(FOOT_HEIGHT*(1 + cosHYP)*sinAR + (2*cosHP*sin(HYP/2.f)*sin(HYP/2.f) + root2*sinHP*sinHYP)*(THIGH_LENGTH + (TIBIA_LENGTH + FOOT_HEIGHT*cosAP*cosAR)*cosKP - FOOT_HEIGHT*cosAR*sinAP*sinKP)) + FOOT_HEIGHT*cosAR*(cosKP*((-1 + cosHYP)*sinAP*sinHP + cosAP*(1 + cosHYP)*sinHR) - 2*(cos(HYP/2.)*cos(HYP/2.f))*sinAP*sinHR*sinKP - 2*cosAP*sinHP*(sin(HYP/2.f)*sin(HYP/2.f))*sinKP + root2*cosHP*sinHYP*sinAPplusKP));

        z = .5f*(-2*HIP_OFFSET_Z + FOOT_HEIGHT*cosAR*cosKP*sinAP*sinHP + FOOT_HEIGHT*cosAR*cosHYP*cosKP*sinAP*sinHP - THIGH_LENGTH*sinHR + THIGH_LENGTH*cosHYP*sinHR - TIBIA_LENGTH*cosKP*sinHR - FOOT_HEIGHT*cosAP*cosAR*cosKP*sinHR + TIBIA_LENGTH*cosHYP*cosKP*sinHR + FOOT_HEIGHT*cosAP*cosAR*cosHYP*cosKP*sinHR - root2*FOOT_HEIGHT*sinAR*sinHP*sinHR*sinHYP + TIBIA_LENGTH*sinHP*sinKP + FOOT_HEIGHT*cosAP*cosAR*sinHP*sinKP + TIBIA_LENGTH*cosHYP*sinHP*sinKP + FOOT_HEIGHT*cosAP*cosAR*cosHYP*sinHP*sinKP + FOOT_HEIGHT*cosAR*sinAP*sinHR*sinKP - FOOT_HEIGHT*cosAR*cosHYP*sinAP*sinHR*sinKP + cosHR*(FOOT_HEIGHT*(-1 + cosHYP)*sinAR + root2*sinHP*sinHYP*(THIGH_LENGTH + (TIBIA_LENGTH + FOOT_HEIGHT*cosAP*cosAR)*cosKP - FOOT_HEIGHT*cosAR*sinAP*sinKP)) + cosHP*(FOOT_HEIGHT*(1 + cosHYP)*sinAR*sinHR - 2*cosHR*(cos(HYP/2.)*cos(HYP/2.))*(THIGH_LENGTH + (TIBIA_LENGTH + FOOT_HEIGHT*cosAP*cosAR)*cosKP - FOOT_HEIGHT*cosAR*sinAP*sinKP) + root2*sinHYP*(TIBIA_LENGTH*sinKP + FOOT_HEIGHT*cosAR*sinAPplusKP)));
        break;

    case RLEG_CHAIN:
        HYP = angles[0];
        HP = angles[1];
        HR = angles[2];
        KP = angles[3];
        AP = angles[4];
        AR = angles[5];
        sinHYP = sin(HYP);
        cosHYP = cos(HYP);
        sinHP = sin(HP);
        cosHP = cos(HP);
        sinHR = sin(HR);
        cosHR = cos(HR);
        sinKP = sin(KP);
        cosKP = cos(KP);
        sinAP = sin(AP);
        cosAP = cos(AP);
        sinAR = sin(AR);
        cosAR = cos(AR);
        root2 = sqrt(2);
        sinAPplusKP = sin(AP + KP);
        cosAPplusKP = cos(AP + KP);

        x = .5*(-cosHR*(root2*(cosHP*(THIGH_LENGTH + TIBIA_LENGTH*cosKP + FOOT_HEIGHT*cosAR*cosAPplusKP) + FOOT_HEIGHT*sinAR)*sinHYP + 2*cosHYP*sinHP*(THIGH_LENGTH + (TIBIA_LENGTH + FOOT_HEIGHT*cosAP*cosAR)*cosKP - FOOT_HEIGHT*cosAR*sinAP*sinKP)) + root2*sinHYP*(-(THIGH_LENGTH + TIBIA_LENGTH*cosKP - FOOT_HEIGHT*cosHP*sinAR)*sinHR + TIBIA_LENGTH*sinHP*sinKP + FOOT_HEIGHT*cosAR*(cosKP*(sinAP*sinHP - cosAP*sinHR) + (cosAP*sinHP + sinAP*sinHR)*sinKP)) + 2*cosHYP*(FOOT_HEIGHT*sinAR*sinHP*sinHR - cosHP*(TIBIA_LENGTH*sinKP + FOOT_HEIGHT*cosAR*sinAPplusKP)));

        y = .5*(-2*HIP_OFFSET_Y + THIGH_LENGTH*sinHR + THIGH_LENGTH*cosHYP*sinHR + TIBIA_LENGTH*cosKP*sinHR + TIBIA_LENGTH*cosHYP*cosKP*sinHR + 2*FOOT_HEIGHT*cosHP*sinAR*sinHR*sin(HYP/2.f)*sin(HYP/2.f) + root2*FOOT_HEIGHT*sinAR*sinHP*sinHR*sinHYP + TIBIA_LENGTH*sinHP*sinKP - TIBIA_LENGTH*cosHYP*sinHP*sinKP - root2*TIBIA_LENGTH*cosHP*sinHYP*sinKP + cosHR*(FOOT_HEIGHT*(1 + cosHYP)*sinAR - (2*cosHP*sin(HYP/2.)*sin(HYP/2.f) + root2*sinHP*sinHYP)*(THIGH_LENGTH + (TIBIA_LENGTH + FOOT_HEIGHT*cosAP*cosAR)*cosKP - FOOT_HEIGHT*cosAR*sinAP*sinKP)) + FOOT_HEIGHT*cosAR*(cosKP*(-(-1 + cosHYP)*sinAP*sinHP + cosAP*(1 + cosHYP)*sinHR) - 2*(cos(HYP/2.)*cos(HYP/2.))*sinAP*sinHR*sinKP + 2*cosAP*sinHP*(sin(HYP/2.f)*sin(HYP/2.f))*sinKP - root2*cosHP*sinHYP*sinAPplusKP));

        z = 0.5*(-2*HIP_OFFSET_Z + FOOT_HEIGHT*cosAR*cosKP*sinAP*sinHP + FOOT_HEIGHT*cosAR*cosHYP*cosKP*sinAP*sinHP + THIGH_LENGTH*sinHR - THIGH_LENGTH*cosHYP*sinHR + TIBIA_LENGTH*cosKP*sinHR + FOOT_HEIGHT*cosAP*cosAR*cosKP*sinHR - TIBIA_LENGTH*cosHYP*cosKP*sinHR - FOOT_HEIGHT*cosAP*cosAR*cosHYP*cosKP*sinHR - root2*FOOT_HEIGHT*sinAR*sinHP*sinHR*sinHYP + TIBIA_LENGTH*sinHP*sinKP + FOOT_HEIGHT*cosAP*cosAR*sinHP*sinKP + TIBIA_LENGTH*cosHYP*sinHP*sinKP + FOOT_HEIGHT*cosAP*cosAR*cosHYP*sinHP*sinKP - FOOT_HEIGHT*cosAR*sinAP*sinHR*sinKP + FOOT_HEIGHT*cosAR*cosHYP*sinAP*sinHR*sinKP + cosHR*((FOOT_HEIGHT - FOOT_HEIGHT*cosHYP)*sinAR + root2*sinHP*sinHYP*(THIGH_LENGTH + (TIBIA_LENGTH + FOOT_HEIGHT*cosAP*cosAR)*cosKP - FOOT_HEIGHT*cosAR*sinAP*sinKP)) + cosHP*(FOOT_HEIGHT*(1 + cosHYP)*sinAR*sinHR - 2*cosHR*(cos(HYP/2.f)*cos(HYP/2.f))*(THIGH_LENGTH + (TIBIA_LENGTH + FOOT_HEIGHT*cosAP*cosAR)*cosKP - FOOT_HEIGHT*cosAR*sinAP*sinKP) + root2*sinHYP*(TIBIA_LENGTH*sinKP + FOOT_HEIGHT*cosAR*sinAPplusKP)));

        break;

    case LANKLE_CHAIN:
        HYP = angles[0];
        HP = angles[1];
        HR = angles[2];
        KP = angles[3];
        sinHYP = sin(HYP);
        cosHYP = cos(HYP);
        sinHP = sin(HP);
        cosHP = cos(HP);
        sinHR = sin(HR);
        cosHR = cos(HR);
        sinKP = sin(KP);
        cosKP = cos(KP);
        root2 = sqrt(2);
            
        x = ((THIGH_LENGTH + TIBIA_LENGTH*cosKP)*sinHR*sinHYP)/root2 - .5*cosHR*(THIGH_LENGTH + TIBIA_LENGTH*cosKP)*(2*cosHYP*sinHP + root2*cosHP*sinHYP) - TIBIA_LENGTH*cosHP*cosHYP*sinKP + (TIBIA_LENGTH*sinHP*sinHYP*sinKP)/root2;

        y = .5*(2*HIP_OFFSET_Y + 2*(cos(HYP/2.f)*cos(HYP/2.f))*(THIGH_LENGTH + TIBIA_LENGTH*cosKP)*sinHR + root2*THIGH_LENGTH*cosHR*sinHP*sinHYP + root2*TIBIA_LENGTH*cosHR*cosKP*sinHP*sinHYP - TIBIA_LENGTH*sinHP*sinKP + TIBIA_LENGTH*cosHYP*sinHP*sinKP + cosHP*(2*cosHR*(THIGH_LENGTH + TIBIA_LENGTH*cosKP)*(sin(HYP/2.f)*sin(HYP/2.f)) + root2*TIBIA_LENGTH*sinHYP*sinKP));
            
        z = .5*(-2*HIP_OFFSET_Z - 2*(THIGH_LENGTH + TIBIA_LENGTH*cosKP)*sinHR*(sin(HYP/2.f)*sin(HYP/2.f)) + root2*THIGH_LENGTH*cosHR*sinHP*sinHYP + root2*TIBIA_LENGTH*cosHR*cosKP*sinHP*sinHYP + TIBIA_LENGTH*sinHP*sinKP + TIBIA_LENGTH*cosHYP*sinHP*sinKP + cosHP*(-2*cosHR*(cos(HYP/2.)*cos(HYP/2.))*(THIGH_LENGTH + TIBIA_LENGTH*cosKP) + root2*TIBIA_LENGTH*sinHYP*sinKP));

        break;

    case RANKLE_CHAIN:
    default:
        HYP = angles[0];
        HP = angles[1];
        HR = angles[2];
        KP = angles[3];
        sinHYP = sin(HYP);
        cosHYP = cos(HYP);
        sinHP = sin(HP);
        cosHP = cos(HP);
        sinHR = sin(HR);
        cosHR = cos(HR);
        sinKP = sin(KP);
        cosKP = cos(KP);
        root2 = sqrt(2);

        x = -(((THIGH_LENGTH + TIBIA_LENGTH*cosKP)*sinHR*sinHYP)/root2) - .5*cosHR*(THIGH_LENGTH + TIBIA_LENGTH*cosKP)*(2*cosHYP*sinHP + root2*cosHP*sinHYP) - TIBIA_LENGTH*cosHP*cosHYP*sinKP + (TIBIA_LENGTH*sinHP*sinHYP*sinKP)/root2;

        y = .5*(-2*HIP_OFFSET_Y + 2*(cos(HYP/2.)*cos(HYP/2.))*(THIGH_LENGTH + TIBIA_LENGTH*cosKP)*sinHR - root2*THIGH_LENGTH*cosHR*sinHP*sinHYP - root2*TIBIA_LENGTH*cosHR*cosKP*sinHP*sinHYP + TIBIA_LENGTH*sinHP*sinKP - TIBIA_LENGTH*cosHYP*sinHP*sinKP - cosHP*(2*cosHR*(THIGH_LENGTH + TIBIA_LENGTH*cosKP)*(sin(HYP/2.)*sin(HYP/2.)) + root2*TIBIA_LENGTH*sinHYP*sinKP));
            
        z = .5*(-2*HIP_OFFSET_Z + 2*(THIGH_LENGTH + TIBIA_LENGTH*cosKP)*sinHR*(sin(HYP/2.)*sin(HYP/2.)) + root2*THIGH_LENGTH*cosHR*sinHP*sinHYP + root2*TIBIA_LENGTH*cosHR*cosKP*sinHP*sinHYP + TIBIA_LENGTH*sinHP*sinKP + TIBIA_LENGTH*cosHYP*sinHP*sinKP + cosHP*(-2*cosHR*(cos(HYP/2.)*cos(HYP/2.))*(THIGH_LENGTH + TIBIA_LENGTH*cosKP) + root2*TIBIA_LENGTH*sinHYP*sinKP));

        break;
    }
    finalPoint(0) = x;
    finalPoint(1) = y;
    finalPoint(2) = z;

    return finalPoint;
}


const Kinematics::ufmatrix3 Kinematics::
buildHeelJacobian(const ChainID chainID,
                  const float angles[]) {
    const float HYP = angles[0];
    const float HP = angles[1];
    const float HR = angles[2];
    const float KP = angles[3];
    const float AP = angles[4];
    const float AR = angles[5];

    const float sinHYP = sin(HYP);
    const float cosHYP = cos(HYP);
    const float sinHP = sin(HP);
    const float cosHP = cos(HP);
    const float sinHR = sin(HR);
    const float cosHR = cos(HR);
    const float sinKP = sin(KP);
    const float cosKP = cos(KP);
    const float sinAP = sin(AP);
    const float cosAP = cos(AP);
    const float sinAR = sin(AR);
    const float cosAR = cos(AR);
    const float sinAPplusKP = sin(AP + KP);
    const float cosAPplusKP = cos(AP + KP);
    const float root2 = sqrt(2.0f);

    if (chainID == LLEG_CHAIN) {
        const float j_1_1 = -.5*FOOT_HEIGHT*cosAR*(sinAP*(root2*cosKP*sinHR*sinHYP - cosHR*cosKP*(2*cosHYP*sinHP + root2*cosHP*sinHYP) - 2*cosHP*cosHYP*sinKP + root2*sinHP*sinHYP*sinKP) - cosAP*(root2*cosKP*sinHP*sinHYP + 2*cosHR*cosHYP*sinHP*sinKP - root2*sinHR*sinHYP*sinKP + cosHP*(-2*cosHYP*cosKP + root2*cosHR*sinHYP*sinKP)));

        const float j_1_2 = .5*FOOT_HEIGHT*(cosAR*(2*cosHYP*sinHP*sinHR + root2*(cosHR + cosHP*sinHR)*sinHYP) + cosAP*sinAR*(cosHR*cosKP*(2*cosHYP*sinHP + root2*cosHP*sinHYP) - root2*sinHYP*(cosKP*sinHR + sinHP*sinKP)) - sinAR*(root2*cosKP*sinAP*sinHP*sinHYP - root2*sinAP*sinHR*sinHYP*sinKP + cosHR*sinAP*(2*cosHYP*sinHP + root2*cosHP*sinHYP)*sinKP - 2*cosHP*cosHYP*sinAPplusKP));

        const float j_2_1 = .5*FOOT_HEIGHT*cosAR*(-sinAP*(cosKP*((1 + cosHYP)*sinHR + root2*cosHR*sinHP*sinHYP) + (-1 + cosHYP)*sinHP*sinKP) + cosAP*((-1 + cosHYP)*cosKP*sinHP - ((1 + cosHYP)*sinHR + root2*cosHR*sinHP*sinHYP)*sinKP) + cosHP*(root2*cosAPplusKP*sinHYP - 2*cosHR*(sin(HYP/2.f)*sin(HYP/2.f))*sinAPplusKP));

        const float j_2_2 = .5*FOOT_HEIGHT*(cosAR*(cosHR*(1 + cosHYP) + sinHR*(cosHP*(-1 + cosHYP) - root2*sinHP*sinHYP)) + sinAR*(sinAP*(cosKP*(sinHP - cosHYP*sinHP - root2*cosHP*sinHYP) + (-cosHP*cosHR*(-1 + cosHYP) + sinHR + cosHYP*sinHR + root2*cosHR*sinHP*sinHYP)*sinKP) - cosAP*(cosKP*(sinHR + cosHYP*sinHR + root2*cosHR*sinHP*sinHYP) + (-1 + cosHYP)*sinHP*sinKP + cosHP*(-cosHR*(-1 + cosHYP)*cosKP + root2*sinHYP*sinKP))));

        const float j_3_1 = .5*FOOT_HEIGHT*cosAR*(-sinAP*(cosKP*((-1 + cosHYP)*sinHR + root2*cosHR*sinHP*sinHYP) + (1 + cosHYP)*sinHP*sinKP) + cosAP*((1 + cosHYP)*cosKP*sinHP + (sinHR - cosHYP*sinHR - root2*cosHR*sinHP*sinHYP)*sinKP) + cosHP*(root2*cosAPplusKP*sinHYP + 2*cosHR*(cos(HYP/2.f)*cos(HYP/2.f))*sinAPplusKP));

        const float j_3_2 = .5*FOOT_HEIGHT*(cosAR*(cosHR*(-1 + cosHYP) + sinHR*(cosHP*(1 + cosHYP) - root2*sinHP*sinHYP)) + sinAR*(-sinAP*(cosKP*((1 + cosHYP)*sinHP + root2*cosHP*sinHYP) + (cosHP*cosHR*(1 + cosHYP) + sinHR - cosHYP*sinHR - root2*cosHR*sinHP*sinHYP)*sinKP) + cosAP*(cosKP*(sinHR - cosHYP*sinHR - root2*cosHR*sinHP*sinHYP) - (1 + cosHYP)*sinHP*sinKP + cosHP*(cosHR*(1 + cosHYP)*cosKP - root2*sinHYP*sinKP))));

        ufmatrix3 jacobian(3,2);
        jacobian(0,0) = j_1_1;  jacobian(0,1) = j_1_2;
        jacobian(1,0) = j_2_1;  jacobian(1,1) = j_2_2;
        jacobian(2,0) = j_3_1;  jacobian(2,1) = j_3_2;
        return jacobian;
    }

    else if (chainID == RLEG_CHAIN) {

        const float j_1_1 = .5*FOOT_HEIGHT*cosAR*(sinAP*(root2*cosKP*sinHR*sinHYP + cosHR*cosKP*(2*cosHYP*sinHP + root2*cosHP*sinHYP) + 2*cosHP*cosHYP*sinKP - root2*sinHP*sinHYP*sinKP) + cosAP*(root2*cosKP*sinHP*sinHYP + 2*cosHR*cosHYP*sinHP*sinKP + root2*sinHR*sinHYP*sinKP + cosHP*(-2*cosHYP*cosKP + root2*cosHR*sinHYP*sinKP)));

        const float j_1_2 = .5*FOOT_HEIGHT*(cosAR*(2*cosHYP*sinHP*sinHR + root2*(-cosHR + cosHP*sinHR)*sinHYP) + cosAP*sinAR*(cosHR*cosKP*(2*cosHYP*sinHP + root2*cosHP*sinHYP) + root2*sinHYP*(cosKP*sinHR - sinHP*sinKP)) - sinAR*(root2*cosKP*sinAP*sinHP*sinHYP + root2*sinAP*sinHR*sinHYP*sinKP + cosHR*sinAP*(2*cosHYP*sinHP + root2*cosHP*sinHYP)*sinKP - 2*cosHP*cosHYP*sinAPplusKP));

        const float j_2_1 = -.5*FOOT_HEIGHT*cosAR*(sinAP*(cosKP*((1 + cosHYP)*sinHR - root2*cosHR*sinHP*sinHYP) - (-1 + cosHYP)*sinHP*sinKP) + cosAP*((-1 + cosHYP)*cosKP*sinHP + ((1 + cosHYP)*sinHR - root2*cosHR*sinHP*sinHYP)*sinKP) + cosHP*(root2*cosAPplusKP*sinHYP - 2*cosHR*(sin(HYP/2.f)*sin(HYP/2.f))*sinAPplusKP));

        const float j_2_2 = .5*FOOT_HEIGHT*(cosAR*(cosHR*(1 + cosHYP) + sinHR*(cosHP - cosHP*cosHYP + root2*sinHP*sinHYP)) + sinAR*(sinAP*(cosKP*((-1 + cosHYP)*sinHP + root2*cosHP*sinHYP) + (cosHP*cosHR*(-1 + cosHYP) + sinHR + cosHYP*sinHR - root2*cosHR*sinHP*sinHYP)*sinKP) + cosAP*(-cosKP*(sinHR + cosHYP*sinHR - root2*cosHR*sinHP*sinHYP) + (-1 + cosHYP)*sinHP*sinKP + cosHP*(-cosHR*(-1 + cosHYP)*cosKP + root2*sinHYP*sinKP))));

        const float j_3_1 = .5*FOOT_HEIGHT*cosAR*(sinAP*(cosKP*((-1 + cosHYP)*sinHR - root2*cosHR*sinHP*sinHYP) - (1 + cosHYP)*sinHP*sinKP) + cosAP*((1 + cosHYP)*cosKP*sinHP + ((-1 + cosHYP)*sinHR - root2*cosHR*sinHP*sinHYP)*sinKP) + cosHP*(root2*cosAPplusKP*sinHYP + 2*cosHR*(cos(HYP/2.f)*cos(HYP/2.0f))*sinAPplusKP));
    
        const float j_3_2 = -.5*FOOT_HEIGHT*(cosAR*(cosHR*(-1 + cosHYP) - sinHR*(cosHP*(1 + cosHYP) - root2*sinHP*sinHYP)) + sinAR*(sinAP*(cosKP*((1 + cosHYP)*sinHP + root2*cosHP*sinHYP) + (cosHP*cosHR*(1 + cosHYP) + (-1 + cosHYP)*sinHR - root2*cosHR*sinHP*sinHYP)*sinKP) + cosAP*(cosKP*(sinHR - cosHYP*sinHR + root2*cosHR*sinHP*sinHYP) + (1 + cosHYP)*sinHP*sinKP - cosHP*(cosHR*(1 + cosHYP)*cosKP - root2*sinHYP*sinKP))));

        ufmatrix3 jacobian(3,2);
        jacobian(0,0) = j_1_1;  jacobian(0,1) = j_1_2;
        jacobian(1,0) = j_2_1;  jacobian(1,1) = j_2_2;
        jacobian(2,0) = j_3_1;  jacobian(2,1) = j_3_2;
        return jacobian;
    }
    else
        throw "Wrong chain";
}
const Kinematics::ufmatrix3 Kinematics::buildLegJacobian(const ChainID chainID,
                                                         const float angles[]) {
    const float HYP = angles[0];
    const float HP = angles[1];
    const float HR = angles[2];
    const float KP = angles[3];

    const float sinHYP = sin(HYP);
    const float cosHYP = cos(HYP);
    const float sinHP = sin(HP);
    const float cosHP = cos(HP);
    const float sinHR = sin(HR);
    const float cosHR = cos(HR);
    const float sinKP = sin(KP);
    const float cosKP = cos(KP);
    const float root2 = sqrt(2.0f);

    if (chainID == LLEG_CHAIN) {
        const float j_1_1 = .5*(sinHP*(root2*cosHR*(THIGH_LENGTH + TIBIA_LENGTH*cosKP)*sinHYP + 2*TIBIA_LENGTH*cosHYP*sinKP) + cosHP*(-2*cosHR*cosHYP*(THIGH_LENGTH + TIBIA_LENGTH*cosKP) + root2*TIBIA_LENGTH*sinHYP*sinKP));

        const float j_1_2 = .5*(THIGH_LENGTH + TIBIA_LENGTH*cosKP)*(2*cosHYP*sinHP*sinHR + root2*(cosHR + cosHP*sinHR)*sinHYP);

        const float j_1_3 = .5*TIBIA_LENGTH*(root2*cosKP*sinHP*sinHYP + (2*cosHR*cosHYP*sinHP - root2*sinHR*sinHYP)*sinKP + cosHP*(-2*cosHYP*cosKP + root2*cosHR*sinHYP*sinKP));

        const float j_2_1 = .5*(cosHR*(THIGH_LENGTH + TIBIA_LENGTH*cosKP)*((-1 + cosHYP)*sinHP + root2*cosHP*sinHYP) + TIBIA_LENGTH*(cosHP*(-1 + cosHYP) - root2*sinHP*sinHYP)*sinKP);

        const float j_2_2 = .5*(THIGH_LENGTH + TIBIA_LENGTH*cosKP)*(cosHR*(1 + cosHYP) + sinHR*(cosHP*(-1 + cosHYP) - root2*sinHP*sinHYP));

        const float j_2_3 = .5*TIBIA_LENGTH*(cosKP*((-1 + cosHYP)*sinHP + root2*cosHP*sinHYP) + (cosHP*cosHR*(-1 + cosHYP) - (1 + cosHYP)*sinHR - root2*cosHR*sinHP*sinHYP)*sinKP);

        const float j_3_1 = .5*(cosHR*(THIGH_LENGTH + TIBIA_LENGTH*cosKP)*((1 + cosHYP)*sinHP + root2*cosHP*sinHYP) + TIBIA_LENGTH*(cosHP*(1 + cosHYP) - root2*sinHP*sinHYP)*sinKP);

        const float j_3_2 = .5*(THIGH_LENGTH + TIBIA_LENGTH*cosKP)*(cosHR*(-1 + cosHYP) + sinHR*(cosHP*(1 + cosHYP) - root2*sinHP*sinHYP));

        const float j_3_3 = .5*TIBIA_LENGTH*(cosKP*((1 + cosHYP)*sinHP + root2*cosHP*sinHYP) + (cosHP*cosHR*(1 + cosHYP) + sinHR - cosHYP*sinHR - root2*cosHR*sinHP*sinHYP)*sinKP);

        ufmatrix3 jacobian(3,3);
        jacobian(0,0) = j_1_1;  jacobian(0,1) = j_1_2;  jacobian(0,2) = j_1_3;
        jacobian(1,0) = j_2_1;  jacobian(1,1) = j_2_2;  jacobian(1,2) = j_2_3;
        jacobian(2,0) = j_3_1;  jacobian(2,1) = j_3_2;  jacobian(2,2) = j_3_3;
        return jacobian;
    }

    else if (chainID == RLEG_CHAIN) {
        const float j_1_1 = -THIGH_LENGTH*cosHR*(cosHP*cosHYP - (sinHP*sinHYP)/root2) - TIBIA_LENGTH*(cosHR*cosKP*(cosHP*cosHYP - (sinHP*sinHYP)/root2) + (-cosHYP*sinHP - (cosHP*sinHYP)/root2)*sinKP);

        const float j_1_2 = -THIGH_LENGTH*((cosHR*sinHYP)/root2 - sinHR*(cosHYP*sinHP + (cosHP*sinHYP)/root2)) - TIBIA_LENGTH*cosKP*((cosHR*sinHYP)/root2 - sinHR*(cosHYP*sinHP + (cosHP*sinHYP)/root2));

        const float j_1_3 = -TIBIA_LENGTH*(cosKP*(cosHP*cosHYP - (sinHP*sinHYP)/root2) - ((sinHR*sinHYP)/root2 + cosHR*(cosHYP*sinHP + (cosHP*sinHYP)/root2))*sinKP);

        const float j_2_1 = -THIGH_LENGTH*cosHR*((-.5 + cosHYP*.5)*sinHP + (cosHP*sinHYP)/root2) - TIBIA_LENGTH*(cosHR*cosKP*((-.5 + cosHYP*.5)*sinHP + (cosHP*sinHYP)/root2) + (cosHP*(-.5 + cosHYP*.5) - (sinHP*sinHYP)/root2)*sinKP);

        const float j_2_2 = -THIGH_LENGTH*(cosHR*(-.5 - cosHYP*.5) - sinHR*(-cosHP*(-.5 + cosHYP*.5) + (sinHP*sinHYP)/root2)) - TIBIA_LENGTH*cosKP*(cosHR*(-.5 - cosHYP*.5) - sinHR*(-cosHP*(-.5 + cosHYP*.5) + (sinHP*sinHYP)/root2));

        const float j_2_3 = -TIBIA_LENGTH*(cosKP*((-.5 + cosHYP*.5)*sinHP + (cosHP*sinHYP)/root2) - ((-.5 - cosHYP*.5)*sinHR + cosHR*(-cosHP*(-.5 + cosHYP*.5) + (sinHP*sinHYP)/root2))*sinKP);

        const float j_3_1 = -THIGH_LENGTH*cosHR*((-.5 - cosHYP*.5)*sinHP - (cosHP*sinHYP)/root2) - TIBIA_LENGTH*(cosHR*cosKP*((-.5 - cosHYP*.5)*sinHP - (cosHP*sinHYP)/root2) + (cosHP*(-.5 - cosHYP*.5) + (sinHP*sinHYP)/root2)*sinKP);

        const float j_3_2 = -THIGH_LENGTH*(cosHR*(-.5 + cosHYP*.5) - sinHR*(-cosHP*(-.5 - cosHYP*.5) - (sinHP*sinHYP)/root2)) - TIBIA_LENGTH*cosKP*(cosHR*(-.5 + cosHYP*.5) - sinHR*(-cosHP*(-.5 - cosHYP*.5) - (sinHP*sinHYP)/root2));

        const float j_3_3 = -TIBIA_LENGTH*(cosKP*((-.5 - cosHYP*.5)*sinHP - (cosHP*sinHYP)/root2) - ((-.5 + cosHYP*.5)*sinHR + cosHR*(-cosHP*(-.5 - cosHYP*.5) - (sinHP*sinHYP)/root2))*sinKP);

        ufmatrix3 jacobian(3,3);
        jacobian(0,0) = j_1_1;  jacobian(0,1) = j_1_2;  jacobian(0,2) = j_1_3;
        jacobian(1,0) = j_2_1;  jacobian(1,1) = j_2_2;  jacobian(1,2) = j_2_3;
        jacobian(2,0) = j_3_1;  jacobian(2,1) = j_3_2;  jacobian(2,2) = j_3_3;
        return jacobian;
    }
    else
        throw "Wrong chain";
}

// Solve the linear system Ax=b for the vector x.
// NOTE: This method is hard coded to work for 3x3 matrices and 3-vectors.
//       We can get superior performance this way.
const Kinematics::ufvector3 Kinematics::solve(ufmatrix3 &A,
                                              const ufvector3 &b) {
    ublas::permutation_matrix
        <float, ublas::bounded_array<float, 9> >
        P(A.size1());
    int singularRow = lu_factorize(A, P);
    if (singularRow != 0) {
        // TODO: This case needs to be dealt with
        throw "the system had no solution";
    }
    ufvector3 result(A.size2());
    result.assign(b);
    lu_substitute(A, P, result);
    return result;
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
        const ufmatrix3 j = buildLegJacobian(chainID, startAngles);
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
            buildHeelJacobian(chainID, startAngles);
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

void hackJointOrder(float angles[]) {
    float temp = angles[1];
    angles[1] = angles[2];
    angles[2] = temp;
}

/*
 * NOTE on a giant hack:
 *   This method still uses the old joint order: HYP, HP, HR, KP,...
 *   Since then, the HP and HR angle values have switched positions. In order
 *   to fully fix this, we need to redo forwardKinematics() in Mathematica.
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
    hackJointOrder(currentAngles);


    // The optimization method hits a singularity if the leg is perfectly
    // straight, so we can virtually bend the knee .3 radians and go around
    // that.
    currentAngles[3] = .3;

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

    hackJointOrder(currentAngles);

    IKLegResult result;
    result.outcome = (ankleSuccess && heelSuccess ? SUCCESS : STUCK);
    memcpy(result.angles,currentAngles,LEG_JOINTS*sizeof(float));
    return result;
}



//#ifdef 0
//Usage example, code for offline testing:

int main() {
    // George's test code
    Kinematics::IKLegResult result;
    for (int i=0; i<1; i++) {
        //float startAngles[] = {0,0,0,0.2,0,0};
        float startAngles[] = {0,-0.78,0,0.95,-0.2,0};
        Kinematics::ufvector3 goal(3);
        goal(0) = 0;
        goal(1) = 70;
        goal(2) = -310;
        //bool jointMask[4] = {true,true,true,true};
        result = Kinematics::dls(Kinematics::LLEG_CHAIN,
                                 goal,
                                 startAngles,
                                 //jointMask,
                                 1.0f, 0.1f);
    }
    std::cout << "Outcome: " << (result.outcome == Kinematics::SUCCESS ?
                                 "success" : "stuck") << std::endl;
    std::cout << "Angles: ";
    for(int i=0; i<6; i++) {
        std::cout << result.angles[i] << " ";
    }
    std::cout << std::endl;
    return 0;
}
//#endif

