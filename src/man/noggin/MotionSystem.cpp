#include "MotionSystem.h"

/**
 * Constructor.
 */
MotionSystem::MotionSystem()
    : PF::MotionModel(), moved(false)
{ 
  
}

void MotionSystem::setCurrentOdometry(const PF::OdometryMeasurement &current)
{
  moved = true;

  currentOdometry = current;
}

void MotionSystem::setLastOdometry(const PF::OdometryMeasurement &last)
{
  lastOdometry = last;
}

/**
 * Generates a noisy delta odometry measurement. We store absolute
 * odometry measurement to avoid syncronization issues, so we need 
 * to calculate the change and add noise. 
 * 
 * @return a OdometryMeasurement object containing the odometry 
 * delta with gaussian noise added proportional to the step size.
 */
PF::OdometryMeasurement MotionSystem::noisyDeltaOdometry(const PF::OdometryMeasurement &newOdometry)
{
  float deltaX = newOdometry.x - lastOdometry.x;
  float deltaY = newOdometry.y - lastOdometry.y;
  float deltaH = newOdometry.h - lastOdometry.h;
  
  if(deltaX > 0.0001f &&
     deltaY > 0.0001f &&
     deltaH > 0.0001f)
  {
    std::cout << "Updating (delta x, delta y, delta h) = (" 
	      << deltaX << ", " << deltaY << ", " 
	      << deltaH << ")" << std::endl;
  }

  setLastOdometry(newOdometry);

  return PF::OdometryMeasurement(deltaX + PF::sampleNormal(0.0f, std::sqrt(std::abs(deltaX))),
				 deltaY + PF::sampleNormal(0.0f, std::sqrt(std::abs(deltaY))),
				 deltaH + PF::sampleNormal(0.0f, std::sqrt(std::abs(deltaH))));
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
	    PF::OdometryMeasurement noisy = noisyDeltaOdometry(currentOdometry);
	    (*iter).setX((*iter).getLocation().x + noisy.x);
	    (*iter).setY((*iter).getLocation().y + noisy.y);
	    (*iter).setH((*iter).getLocation().heading + noisy.h);
	}
	moved = false;
    }

    return particles;
}
