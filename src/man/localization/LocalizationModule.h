/*
 * @brief  The localization module class. Takes input from motion and vision for
 *             calculations, also an inPortal for resetting
 *
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   February 2013
 */
#pragma once

#include "DebugConfig.h"

/** Messages **/
#include "RoboGrams.h"
#include "VisionField.pb.h"
#include "RobotLocation.pb.h"
#include "ParticleSwarm.pb.h"

/** Filter Headers **/
#include "SensorModel.h"
#include "MotionModel.h"
#include "ParticleFilter.h"
#include "VisionSystem.h"
#include "MotionSystem.h"

#include <boost/shared_ptr.hpp>

namespace man
{
namespace localization
{
/**
 * @class LocalizationModule
 */
class LocalizationModule : public portals::Module
{
public:
    LocalizationModule();
    ~LocalizationModule();

    /** In Portals **/
    portals::InPortal<messages::RobotLocation> motionInput;
    portals::InPortal<messages::VisionField> visionInput;
    portals::InPortal<messages::RobotLocation> resetInput;

    /** Out Portals **/
    portals::OutPortal<messages::RobotLocation> output;
    portals::OutPortal<messages::ParticleSwarm> particleOutput;

    float lastMotionTimestamp;
    float lastVisionTimestamp;

protected:
    /**
     * @brief Update inputs, calculate new state of the filter
     */
    void run_();

    /**
     * @brief Updates the current robot pose estimate given
     *        the most recent motion control inputs and
     *        measurements taken.
     */
    void update();

    ParticleFilter * particleFilter;
    long long lastReset;

    // Previous information
    messages::RobotLocation lastOdometry;
    messages::RobotLocation curOdometry;
    messages::RobotLocation deltaOdometry;
};
} // namespace localization
} // namespace man
