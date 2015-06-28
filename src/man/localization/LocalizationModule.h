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
#include "Vision.pb.h"
#include "RobotLocation.pb.h"
#include "ParticleSwarm.pb.h"
#include "GameState.pb.h"
#include "BallModel.pb.h"

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
    portals::InPortal<messages::FieldLines>    linesInput;
    portals::InPortal<messages::Corners>    cornersInput;
    portals::InPortal<messages::RobotLocation> resetInput[2];
    portals::InPortal<messages::GameState>     gameStateInput;
    portals::InPortal<messages::FilteredBall>  ballInput;

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
    long long lastReset[2];
    messages::RobotLocation curOdometry;
    messages::FieldLines curLines;
    messages::Corners curCorners;
    messages::FilteredBall curBall;

private:
    size_t log_index;
};

} // namespace localization
} // namespace man
