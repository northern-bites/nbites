
#include <iostream>

 #include "BalancingArm.h"

BalancingArm::BalancingArm(Kinematics::ChainID id, COMPreview::ptr com)
    : chain(id),
      comPreview(com)
{

}

BalancingArm::~BalancingArm() {

}

std::vector<float> BalancingArm::getNextJoints() {
    std::vector<float> angles;

    for (int i = 0; i < 4; ++i)
	angles.push_back(0.0f);

    return angles;
}
