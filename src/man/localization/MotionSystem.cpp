#include "MotionSystem.h"

namespace man
{
namespace localization
{
MotionSystem::MotionSystem(float xAndYNoise_, float hNoise_)
{
    xAndYNoise = xAndYNoise_;
    hNoise = hNoise_;
}
MotionSystem::~MotionSystem(){}

void MotionSystem::resetNoise(float xyNoise_, float hNoise_)
{
    xAndYNoise = xyNoise_;
    hNoise = hNoise_;
}

/**
 * Updates the particle set according to the motion.
 *
 * @return the updated ParticleSet.
 */
void MotionSystem::update(ParticleSet& particles,
                          const messages::RobotLocation& deltaMotionInfo,
                          bool nearMid)
{
    if((fabs(deltaMotionInfo.x()) > 3.f) || (fabs(deltaMotionInfo.y()) > 3.f)) {
        std::cout << "LOCALIZATION WARNING:\t Sanity check missed an unhelpful odometry frame\n"
                  << "( Delta X , Delta Y ):\t(" << deltaMotionInfo.x() << " , " << deltaMotionInfo.y()
                  << ")" << std::endl << std::endl;
    }

    float dX, dY, dH;

    ParticleIt iter;
    for(iter = particles.begin(); iter != particles.end(); iter++)
    {
        Particle* particle = &(*iter);

        float sinh, cosh;
        sincosf(deltaMotionInfo.h() - particle->getLocation().h(),
                &sinh, &cosh);
        dX = cosh*deltaMotionInfo.x() + sinh*deltaMotionInfo.y();
        dY = cosh*deltaMotionInfo.y() - sinh*deltaMotionInfo.x();
        dH = NBMath::subPIAngle(deltaMotionInfo.h());

        particle->shift(dX, dY, dH);

        noiseShiftWithOdo(particle, dX, dY, dH);
        //randomlyShiftParticle(particle, nearMid);
    }
}

void MotionSystem::noiseShiftWithOdo(Particle* particle, float dX, float dY, float dH) {
    float xF = 5.f;
    float yF = 5.f;
    float hF = 4.f;

    float xL, xU, yL, yU, hL, hU;

    if (dX >0) {
        xL = -1.f * dX * xF;
        xU = dX * xF;
    }
    else if (dX <0) {
        xL = dX * xF;
        xU = -1.f * dX * xF;
    }
    else {
        xL = -.1f;
        xU = .1f;
    }

    if (dY >0) {
        yL = -1.f * dY * yF;
        yU = dY * yF;
    }
    else if (dY <0) {
        yL = dY * yF;
        yU = -1.f * dY * yF;
    }
    else {
        yL = -.1f;
        yU = .1f;
    }

    if (dH >0) {
        hL = -1.f * dH * hF;
        hU = dH * hF;
    }
    else if (dH <0) {
        hL = dH * hF;
        hU = -1.f * dH * hF;
    }
    else {
        hL = -.03f;
        hU =  .03f;
    }

    boost::uniform_real<float> xRange(xL, xU);
    boost::uniform_real<float> yRange(yL, yU);
    boost::uniform_real<float> hRange(hL, hU);

    boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > xShift(rng, xRange);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > yShift(rng, yRange);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > hShift(rng, hRange);

    particle->shift(xShift(), yShift(), hShift());

}

void MotionSystem::randomlyShiftParticle(Particle* particle, bool nearMid)
{
    float pumpNoise = 1.f;
    if (nearMid)
        pumpNoise = 2.f;

    // TODO: This should be experimentally determined
    boost::uniform_real<float> coordRange(-1.f * xAndYNoise * pumpNoise, xAndYNoise * pumpNoise);
    boost::uniform_real<float> headRange (-1.f * hNoise    , hNoise);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > coordNoise(rng, coordRange);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > headNoise(rng, headRange);

    // Determine random noise and shift the particle
    messages::RobotLocation noise;
    noise.set_x(coordNoise());
    noise.set_y(coordNoise());
    noise.set_h(NBMath::subPIAngle(headNoise()));
    particle->shift(noise);
}

} // namespace localization
} // namespace man
