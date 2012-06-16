#include "MotionSystem.h"

#include "NullStream.h"

#include <fstream>

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

    if (!currentOdometryModel.isValid() && lastOdometryModel.isValid()) {
        //TODO: determine what to do when it's invalid

    } else {
        moved = true;
        deltaOdometry = currentOdometryModel - lastOdometryModel;
        deltaOdometry.theta = NBMath::subPIAngle(deltaOdometry.theta);

        clipDeltaOdometry();

        //        static std::ofstream odometry_out("/home/nao/odometry.out");
        //        odometry_out << deltaOdometry << std::endl;

    }
}

void MotionSystem::clipDeltaOdometry() {
//    if (std::abs(deltaOdometry.x) < 0.000001f) {
//        deltaOdometry.x = 0.000001f;
//  //      debug_loc_odometry << "Warning! Clipped delta odo X";
//    }
//    if (std::abs(deltaOdometry.y) < 0.000001f) {
//        deltaOdometry.y = 0.000001f;
//  //      debug_loc_odometry << "Warning! Clipped delta odo Y";
//    }
//    if (std::abs(deltaOdometry.theta) < 0.000001f) {
//        deltaOdometry.theta = 0.000001f;
//  //      debug_loc_odometry << "Warning! Clipped delta odo theta";
//    }
}

//Odometry offsets standard deviations (not to be confused with odometry MEASUREMENT standard deviation
// -- these don't measure the accuracy of the measurment, just what it's value usually is)
//Computed by Octavian summer 2012 using R and some sample data
float ODO_MEASUREMENT_X_SD = 2.2f;
float ODO_MEASUREMENT_Y_SD = 1.48f;
float ODO_MEASUREMENT_THETA_SD = 0.038f;

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

    DeltaOdometryMeasurement noisyDeltaOdometry;

    //Add noise proportionally to the step size (the bigger the step, the worst
    //the odometry?
    //Filter out big jumps
    if (-5 < noisyDeltaOdometry.x && noisyDeltaOdometry.x < 5) {
        noisyDeltaOdometry.x = deltaOdometry.x + PF::sampleNormal(0.0f, 0.5 + std::abs(deltaOdometry.x)/4);
    } else {
        //replace it with a usual measurement
        noisyDeltaOdometry.x = PF::sampleNormal(0.0f, ODO_MEASUREMENT_X_SD);
    }

    if (-3 < noisyDeltaOdometry.y && noisyDeltaOdometry.y < 3) {
        noisyDeltaOdometry.y = deltaOdometry.y + PF::sampleNormal(0.0f, 0.5 + std::abs(deltaOdometry.y)/4);
    } else {
        //replace it with a usual measurement
        noisyDeltaOdometry.y = PF::sampleNormal(0.0f, ODO_MEASUREMENT_Y_SD);
    }
    if (-.02 < noisyDeltaOdometry.theta && noisyDeltaOdometry.theta < .02) {
        noisyDeltaOdometry.theta = deltaOdometry.theta + PF::sampleNormal(0.0f, M_PI_FLOAT/140.0f +  std::abs(deltaOdometry.theta)/4);
    } else {
        //replace it with a usual measurement
        noisyDeltaOdometry.theta = PF::sampleNormal(0.0f, ODO_MEASUREMENT_THETA_SD);
    }

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

	for(iter = particles.begin(); iter != particles.end(); ++iter)
	{
	    DeltaOdometryMeasurement noisyDeltaOdometry = makeNoisyDeltaOdometry();

	    PF::LocalizationParticle* particle = &(*iter);

	    //TODO: comment this really well and make sure people understand
	    // what odometry represents and what it measures !!!!!
	    // EJ & Octavian

	    //Rotate from the robot frame to the world frame to add the translation
	    float sinh, cosh;
	    sincosf(currentOdometryModel.theta - particle->getLocation().heading, &sinh, &cosh);

//	    debug_loc_odometry << particle->getLocation() << std::endl;

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
