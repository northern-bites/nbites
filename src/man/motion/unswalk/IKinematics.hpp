#pragma once

#include <iostream>

namespace IKinematics
{
struct NaoLegChain
{
   float hip;
   float thighPitch;
   float thighRoll;
   float kneePitch;
   float anklePitch;
   float ankleRoll;
   friend std::ostream &operator <<(std::ostream &out, const NaoLegChain &chain);
};

struct NaoFootTarget
{
   float x, y, z;
   float pitch, roll, yaw;
   friend std::ostream &operator <<(std::ostream &out, const NaoFootTarget &target);
};


NaoLegChain NaoSolve(const NaoFootTarget &target, float left = -1.f);

std::ostream &operator <<(std::ostream &out, const NaoLegChain &chain);
std::ostream &operator <<(std::ostream &out, const NaoFootTarget &target);



} /* namespace IKinematics */

