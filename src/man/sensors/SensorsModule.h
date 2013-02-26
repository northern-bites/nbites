/**
 * @brief  Module that serves as an interface between the low-level
 *         NAOQi sensor data acquisition, and responsible for retrieving
 *         and maintaining information about the state of the sonar,
 *         joints, inertia, FSRs, and buttons.
 * @author Ellis Ratner <eratner@bowdoin.edu>
 * @date   January 2013
 */
#pragma once

#include <vector>
#include <string>
#include <iostream>

#include "RoboGrams.h"
#include "SensorTypes.h"

#include "JointAngles.pb.h"
#include "ButtonState.pb.h"
#include "FootBumperState.pb.h"
#include "InertialState.pb.h"
#include "SonarState.pb.h"

#include <alcommon/albroker.h>
#include <alproxies/almemoryproxy.h>
#include <alproxies/dcmproxy.h>
#include <almemoryfastaccess/almemoryfastaccess.h>

#include <boost/shared_ptr.hpp>

namespace man
{
    namespace sensors
    {
	/**
	 * @class SensorsModule
	 */
	class SensorsModule : public portals::Module
	{
	public:
	    /**
	     * @brief Constructor must take a pointer to the broker
	     *        passed to Man in order to communicate via
	     *        proxies to the DCM and ALMemory.
	     */
	    SensorsModule(boost::shared_ptr<AL::ALBroker> broker);

	    virtual ~SensorsModule();

	    /*
	     * These portals enable other modules to get sensory
	     * information.
	     */

	    // Joints.
	    portals::OutPortal<messages::JointAngles> jointsOutput_;

	    // Chestboard button.
	    portals::OutPortal<messages::ButtonState> chestboardButtonOutput_;
	    // Foot bumpers.
	    portals::OutPortal<messages::FootBumperState> footbumperOutput_;

	    // Inertial sensors.
	    portals::OutPortal<messages::InertialState> inertialsOutput_;

	    // Sonars.
	    portals::OutPortal<messages::SonarState> sonarsOutput_;

	private:
	    /*
	     * Methods used to communicate with the NAO hardware
	     * through the NAOqi software interface.
	     */

	    /**
	     * @brief Initialize sensor aliases for fast access.
	     */
	    void initializeSensorFastAccess();

	    /**
	     * @brief By default, the DCM does not write ultrasonic
	     *        sensor values to ALMemory, so we must notify
	     *        it to do so if we need sonar readings.
	     */
	    void initializeSonarValues();

	    /**
	     * @brief Updates the sensor readings. All readings are
	     *        stored and indexed appropriately in a vector.
	     */
	    void updateSensorValues();

	    /*
	     * Methods to update the messages provided by the
	     * out portals.
	     */

	    /**
	     * @brief Updates the joint angles message.
	     */
	    void updateJointsMessage();

	    /**
	     * @brief Updates the chestboard button message.
	     */
	    void updateChestboardButtonMessage();

	    /**
	     * @brief Updates the footbumper button message.
	     */
	    void updateFootbumperMessage();

	    /**
	     * @brief Updates the inertial sensors message.
	     */
	    void updateInertialsMessage();

	    /**
	     * @brief Updates the sonars message.
	     */
	    void updateSonarsMessage();

	    /**
	     * @brief The main run routine, primarily updates sensor
	     *        readings.
	     */
	    void run_();

	    boost::shared_ptr<AL::ALBroker> broker_;
	    boost::shared_ptr<AL::ALMemoryFastAccess> fastMemoryAccess_;
	    std::vector<float> sensorValues_;
	    std::vector<std::string> sensorKeys_;
	};
    } // namespace sensors
} // namespace man
