#include "MotionSystem.h"

/**
 * Constructor.
 */
MotionSystem::MotionSystem()
    : PF::MotionModel(), moved(false)
{ 
  
}

/**
 * Updates current location and generates a noisy step,
 * with Gaussian white noise added proportional to 
 * the step size.
 *
 * @param dx the change in x.
 * @param dy the change in y.
 * @param da the change in angle.
 */
void MotionSystem::feedStep(float dx, float dy, float da)
{
    moved = true;

    lastStep = Step(dx, dy, da);
}

void MotionSystem::feedStep(Step s)
{
    feedStep(s.dx, s.dy, s.da);
}

/**
 * Generates a noisy version of the given last step.
 * 
 * @return a Step object with noise proportional to
 * the magnitude of the step in every direction.
 */
Step MotionSystem::noisyStep()
{
    return Step(lastStep.dx + PF::sampleNormal(0.0f, std::sqrt(lastStep.dx)),
	        lastStep.dy + PF::sampleNormal(0.0f, std::sqrt(lastStep.dy)),
	        lastStep.da + PF::sampleNormal(0.0f, std::sqrt(lastStep.da)));
}

/**
 * Upates the particle set according to the motion.
 *
 * @return the updated ParticleSet.
 */
PF::ParticleSet MotionSystem::update(PF::ParticleSet particles)
{
    if(moved)
    {
	PF::ParticleIt iter;
	for(iter = particles.begin(); iter != particles.end(); ++iter)
	{
	    Step noisy = noisyStep();
	    (*iter).setX((*iter).getLocation().x + noisy.dx);
	    (*iter).setY((*iter).getLocation().y + noisy.dy);
	    (*iter).setA((*iter).getLocation().angle + noisy.da);
	}
	moved = false;
    }

    return particles;
}
