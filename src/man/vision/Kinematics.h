#pragma once

#include "RoboGrams.h"
#include "PMotion.pb.h"

#include <utility>

namespace man {
namespace vision {
 
// TODO documentation
class Kinematics {
public:
    Kinematics(bool topCamera = true);
    void setJointAngles(const messages::JointAngles& joints_);
    double tilt() { compute(); return tilt_; } 
    double wz0() { compute(); return wz0_; } 

private:
    void compute();
    std::pair<double, double> computeForLeg(double anklePitch, double kneePitch,
                                            double hipPitch, double neckPitch) const;

    struct NaoConstants {
        double groundToAnkle;
        double ankleToKnee;
        double kneeToHip;
        double hipToNeck;
        double neckToCameraZ;
        double neckToCameraX;
        double opticalAxisOffset;

        // TODO json settings
        NaoConstants(bool topCamera = true) {
            groundToAnkle = 4.519;
            ankleToKnee = 10.290;
            kneeToHip = 10.000;
            hipToNeck = 21.15;
            if (topCamera) {
                neckToCameraZ = 6.364;
                neckToCameraX = 5.871;
                opticalAxisOffset = 0.020943951;
            } else {
                neckToCameraZ = 1.774;
                neckToCameraX = 5.071;
                opticalAxisOffset = 0.692895713;
            }
        }
    };

    bool needCompute;
    NaoConstants constants;
    messages::JointAngles joints;

    double tilt_;
    double wz0_;
};

}
}
