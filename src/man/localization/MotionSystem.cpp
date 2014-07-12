#include "MotionSystem.h"

namespace man {
namespace localization {

static const float FRICTION_FACTOR_X = 1.f;
static const float FRICTION_FACTOR_Y = 1.f;
static const float FRICTION_FACTOR_H = 1.f;

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
                          const messages::RobotLocation& odometry,
                          float error)
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
        //Probably reset odometry somewhere so skip a frame
        return;
    }

    float dX, dY, dH;
    ParticleIt iter;
    for(iter = particles.begin(); iter != particles.end(); iter++)
    {
        Particle* particle = &(*iter);

        // Rotate from the robot frame to the global to add the translation
        float sinh, cosh;
        sincosf(curOdometry.h() - particle->getLocation().h(),
                &sinh, &cosh);

        dX = (cosh*dX_R + sinh*dY_R) * FRICTION_FACTOR_X;
        dY = (cosh*dY_R - sinh*dX_R) * FRICTION_FACTOR_Y;
        dH = dH_R                    * FRICTION_FACTOR_H;

        particle->shift(dX, dY, dH);

        noiseShiftWithOdo(particle, dX, dY, dH, error);
    }
}

void MotionSystem::noiseShiftWithOdo(Particle* particle, float dX, float dY, float dH, float error) {

    int lostShiftFactor = (int) (error / 10.f);

    // How x,y,h factors when deciding ranges
    float xF = 5.f + (float) lostShiftFactor;
    float yF = 5.f + (float) lostShiftFactor;
    float hF = 5.f + (float) lostShiftFactor;

    float xL, xU, yL, yU, hL, hU;

    if ((std::fabs(dX * xF) - .2f) < 0.001f) {
        xL = -.2f;
        xU =  .2f;
    }
    else if (dX >0) {
        xL = -1.f * dX * xF;
        xU = dX * xF;
    }
    else {//dX <0
        xL = dX * xF;
        xU = -1.f * dX * xF;
    }

    if ((std::fabs(dY * yF) - .2f) < 0.001f) {
        yL = -.2f;
        yU =  .2f;
    }
    else if (dY >0) {
        yL = -1.f * dY * yF;
        yU = dY * yF;
    }
    else { //dY <0
        yL = dY * yF;
        yU = -1.f * dY * yF;
    }

    if ((std::fabs(dH * hF) - .025f) < 0.001f) {
        hL = -.025f;
        hU =  .025f;
    }
    else if (dH >0) {
        hL = -1.f * dH * hF;
        hU = dH * hF;
    }
    else { //dH <0
        hL = dH * hF;
        hU = -1.f * dH * hF;
    }

    if (error > 23) {
        xL = -.3f;
        xU =  .3f;
        yL = -.3f;
        yU =  .3f;
        hL = -.07f;
        hU =  .07f;
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
