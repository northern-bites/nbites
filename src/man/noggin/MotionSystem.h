/**
 * Implements the interface between the localization system
 * and the motion system for procuring odometry measurement
 * data.
 *
 * @author Ellis Ratner <eratner@bowdoin.edu>
 * @fixed Octavian Neamtu #hotaldebarangirl #hahathisisfunny
 */

#pragma once

#include "EKFStructs.h"
#include "ParticleFilter.h"

//TODO: just make MotionModel the odometry model
typedef MotionModel OdometryModel;
typedef DeltaMotionModel DeltaOdometryMeasurement;

/**
 * @class MotionSystem
 * 
 * Responsible for updating particles based on an odometry
 * measurement from the motion system.
 */
class MotionSystem : public PF::MotionModel
{
 public:
    MotionSystem();
    ~MotionSystem() { }

    /**
     * Incorporate motion odometry measurements
     * The odometryModel can potentially be invalid
     */
    void motionUpdate(const OdometryModel& odometryModel);
    PF::ParticleSet update(PF::ParticleSet particles) const;
    const OdometryModel& getLastOdometry() const { return currentOdometryModel; }
    const DeltaOdometryMeasurement& getDeltaOdometry() const { return deltaOdometry; }

    /**
     * When a robot falls, it tends to rotate, altering its
     * heading in a manner not measureable by conventional
     * odometry. Therefore, we will account for this by adding
     * additional noise/uncertainty to the heading following
     * a fall. 
     *
     * @param fallen Whether or not the robot is fallen. 
     */
    void setFallen(bool fallen);

 private:
    DeltaOdometryMeasurement makeNoisyDeltaOdometry() const;
    void clipDeltaOdometry();

 private:
    mutable bool moved;
    mutable bool robotFallen;
    OdometryModel currentOdometryModel;
    OdometryModel lastOdometryModel;
    DeltaOdometryMeasurement deltaOdometry;

};
