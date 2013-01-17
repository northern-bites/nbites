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
	    SensorsModule(boost::shared_ptr<AL::ALBroker> broker);

	    virtual ~SensorsModule();

	private:
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
