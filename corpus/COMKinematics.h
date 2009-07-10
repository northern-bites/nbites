#ifndef COMKinematics_h_DEFINED
#define COMKinematics_h_DEFINED

#include "Kinematics.h"

namespace Kinematics{

  const NBMath::ufvector4
    getCOMc(const std::vector<float> bodyAngles);

  const NBMath::ufvector4
    slowCalculateChainCom(const ChainID id,
			  const float angles[]);
 
};

#endif
