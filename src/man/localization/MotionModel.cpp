#include "MotionModel.h"

#include <ctime>

namespace man {
namespace localization {

MotionModel::MotionModel(float xyNoise_, float hNoise_)
    : rng(time(0))
{
    xyNoise = xyNoise_;
    hNoise = hNoise_;
}

void MotionModel::update(ParticleSet& particles,
                         const messages::RobotLocation& odometry)
{
    // Store the last odometry and set the current one
    lastOdometry.set_x(curOdometry.x());
    lastOdometry.set_y(curOdometry.y());
    lastOdometry.set_h(curOdometry.h());

    curOdometry.set_x(odometry.x());
    curOdometry.set_y(odometry.y());
    curOdometry.set_h(odometry.h());

    // change in the robot frame
    float dX_R = curOdometry.x() - lastOdometry.x();
    float dY_R = curOdometry.y() - lastOdometry.y();
    float dH_R = curOdometry.h() - lastOdometry.h();

    if( (std::fabs(dX_R) > 3.f) || (std::fabs(dY_R) > 3.f) ) {
        // Probably reset odometry somewhere, so skip frame
        std::cout << "Odometry reset, motion frame skipped in loc" << std::endl;
        return;
    }

    float dX, dY, dH;
    ParticleIt iter;
    for(iter = particles.begin(); iter != particles.end(); iter++)
    {
        Particle* particle = &(*iter);

        // Rotate from the robot frame to the global to add the translation
        float sinh, cosh;
        sincosf(curOdometry.h() - particle->getLocation().h(), &sinh, &cosh);

        dX = (cosh*dX_R + sinh*dY_R);
        dY = (cosh*dY_R - sinh*dX_R);
        dH = dH_R;

        // First add delta odometry, then add noise
        particle->shift(dX, dY, dH);
        noiseShift(particle);
    }
}

void MotionModel::noiseShift(Particle* particle)
{
    // Three gaussians with zero mean and standard deviations set by the
    // probalistic motion model
    boost::normal_distribution<> xGaussian(0, xyNoise);
    boost::normal_distribution<> yGaussian(0, xyNoise);
    boost::normal_distribution<> hGaussian(0, hNoise);

    boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > xNoise(rng, xGaussian);
    boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > yNoise(rng, yGaussian);
    boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > hNoise(rng, hGaussian);

    // Sample from gaussians and shift particle
    messages::RobotLocation noise;
    noise.set_x(xNoise());
    noise.set_y(yNoise());
    noise.set_h(NBMath::subPIAngle(hNoise()));

    particle->shift(noise);
}

} // namespace localization
} // namespace man
