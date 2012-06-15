#include "MotionSystem.h"

#include "NullStream.h"

#define DEBUG_LOC_ODOMETRY
#ifdef DEBUG_LOC_ODOMETRY
#define debug_loc_odometry std::cout
#else
#define debug_loc_odometry (*NullStream::NullInstance())
#endif

/**
 * Constructor. No shit :).
 */
MotionSystem::MotionSystem()
    : moved(false) {
    
}

void MotionSystem::motionUpdate(const OdometryModel& odometryModel) {

    lastOdometryModel = currentOdometryModel;
    currentOdometryModel = odometryModel;

//    debug_loc_odometry << "Current " << currentOdometryModel << std::endl
//                       << "Last "<< lastOdometryModel << std::endl;

    if (currentOdometryModel.isValid()) {
        moved = true;
        deltaOdometry = currentOdometryModel - lastOdometryModel;
        deltaOdometry.theta = NBMath::subPIAngle(deltaOdometry.theta);
    } else {
        //TODO: determine what to do when it's invalid
    }

}


/**
 * Generates a noisy delta odometry measurement. We store absolute
 * odometry measurement to avoid syncronization issues, so we need 
 * to calculate the change and add noise. 
 * 
 * @return a OdometryMeasurement object containing the odometry 
 * delta with gaussian noise added proportional to the step size.
 */
DeltaOdometryMeasurement MotionSystem::makeNoisyDeltaOdometry() const
{




//  if (deltaOdometry.x < 0.000001f) {
//      deltaOdometry.x = 0.000001f;
//      debug_loc_odometry << "Warning! Clipped delta odo X";
//  }
//  if (deltaOdometry.y < 0.000001f) {
//      deltaOdometry.y = 0.000001f;
//      debug_loc_odometry << "Warning! Clipped delta odo Y";
//  }
//  if (deltaOdometry.theta < 0.000001f) {
//      deltaOdometry.theta = 0.000001f;
//      debug_loc_odometry << "Warning! Clipped delta odo theta";
//  }

    DeltaOdometryMeasurement noisyDeltaOdometry;

  //Add noise inverse proportionally to the step size (the bigger the step, the worst
  //the odometry)
//  noisyDeltaOdometry.x = deltaOdometry.x
//          + 1/PF::sampleNormal(0.0f, std::sqrt(1/std::abs(deltaOdometry.x)));
//  noisyDeltaOdometry.y = deltaOdometry.y
//          + 1/PF::sampleNormal(0.0f, std::sqrt(1/std::abs(deltaOdometry.y)));
//  noisyDeltaOdometry.theta = NBMath::subPIAngle(deltaOdometry.theta
//          + 1/PF::sampleNormal(0.0f, std::sqrt(1/std::abs(deltaOdometry.theta))));
    noisyDeltaOdometry = deltaOdometry;

    return noisyDeltaOdometry;
}

/**
 * Upates the particle set according to the motion.
 *
 * @return the updated ParticleSet.
 */
PF::ParticleSet MotionSystem::update(PF::ParticleSet particles) const
{
    if(moved)
    {
	PF::ParticleIt iter;
	float angle;
	float angle2;
	float angle3;
	for(iter = particles.begin(); iter != particles.end(); ++iter)
	{
	    DeltaOdometryMeasurement noisyDeltaOdometry = makeNoisyDeltaOdometry();

	    PF::LocalizationParticle* particle = &(*iter);

	    //Rotate from the robot frame to the world frame to add the translation
	    float sinh, cosh;
	    sincosf(currentOdometryModel.theta - particle->getLocation().heading, &sinh, &cosh);

//	    debug_loc_odometry << particle->getLocation() << std::endl;

	    angle = NBMath::subPIAngle(particle->getLocation().heading + noisyDeltaOdometry.theta);
	    angle2 = noisyDeltaOdometry.theta;

	    // Assume that
	    particle->setX(particle->getLocation().x + cosh * noisyDeltaOdometry.x + sinh * noisyDeltaOdometry.y);
	    particle->setY(particle->getLocation().y + cosh * noisyDeltaOdometry.y - sinh * noisyDeltaOdometry.x);
	    //Rotation is just added
	    particle->setH(NBMath::subPIAngle(particle->getLocation().heading + noisyDeltaOdometry.theta));

//	    debug_loc_odometry << cosh << " " << sinh << std::endl <<
//	                          noisyDeltaOdometry << std::endl;
	}

	moved = false;
    }

    return particles;
}
