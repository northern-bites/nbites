#ifndef COMKinematics_h_DEFINED
#define COMKinematics_h_DEFINED

#include "Kinematics.h"
#include "JointMassConstants.h"

namespace Kinematics{
    const NBMath::ufvector4 getCOMc(const std::vector<float> bodyAngles);

    void buildJointTransforms(const float angles[]);

    void buildHeadNeck(const int start, const float angles[]);
    void buildArmChain(const int start, const float side, const float angles[]);
    void buildLegChain(const int start, const float side, const float angles[]);
};

#endif
