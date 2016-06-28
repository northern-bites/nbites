#pragma once

#include "PMotion.pb.h"
#include <alproxies/dcmproxy.h>

namespace boss {
namespace enactor {
class Enactor
{
public:
    Enactor(boost::shared_ptr<AL::DCMProxy> dcm_);
    ~Enactor();

    void command(messages::JointAngles angles, messages::JointAngles stiffness);
    void noStiff();
    bool manDied();


private:
    void initEnactor();

    boost::shared_ptr<AL::DCMProxy> dcm;

    AL::ALValue jointCommand;
    AL::ALValue stiffnessCommand;

    static const int numJoints = 21;
    double lastSet[numJoints];
};
    
}
}
