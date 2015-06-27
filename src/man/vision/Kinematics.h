#pragma once

#include "RoboGrams.h"
#include "PMotion.pb.h"

#include <utility>
#include <tuple>

namespace man {
namespace vision {
 
// TODO documentation
// TODO add aperature to torso center Y computation
class Kinematics {
public:
    Kinematics(bool topCamera = true);
    void joints(const messages::JointAngles& newJoints);
    double tilt() { compute(); return tilt_; }
    double wx0() { compute(); return wx0_; }
    double wy0() { compute(); return wy0_; }
    double wz0() { compute(); return wz0_; }
    double azimuth() { compute(); return azimuth_; }

private:
    void compute();
    std::tuple<double, double, double> computeForLeg(double anklePitch, double kneePitch,
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
    messages::JointAngles joints_;

    double tilt_;
    double wx0_;
    double wy0_;
    double wz0_;
    double azimuth_;
};

}
}
