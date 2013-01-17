#include "SensorsModule.h"

namespace man
{
    namespace sensors
    {
	SensorsModule::SensorsModule(boost::shared_ptr<AL::ALBroker> broker)
	    : portals::Module(), broker_(broker), fastMemoryAccess_(new AL::ALMemoryFastAccess()),
	      sensorValues_(NUM_SENSOR_VALUES), sensorKeys_(NUM_SENSOR_VALUES)
	{
	    std::cout << "SensorsModule : Constructor." << std::endl;

	    // Initialize the Aldebaran fast access memory interface 
	    // to quickly read sensor values from memory. 
	    initializeSensorFastAccess();
	}

	SensorsModule::~SensorsModule()
	{
	    std::cout << "SensorsModule : Destructor." << std::endl;
	}
	
	void SensorsModule::initializeSensorFastAccess()
	{
	    // There are 26 joints.
	    int i = 0;
	    for(; i < 26; ++i)
	    {
		sensorKeys_[i] = std::string("Device/SubDeviceList/") + SensorNames[i] + std::string("/Position/Sensor/Value");
	    }
	    // There are 8 FSR sensors.
	    // (Left foot)
	    for(; i < 30; ++i)
	    {
		sensorKeys_[i] = std::string("Device/SubDeviceList/LFoot/FSR/") + SensorNames[i] + std::string("/Sensor/Value");
	    }
	    // (Right foot)
	    for(; i < 34; ++i)
	    {
		sensorKeys_[i] = std::string("Device/SubDeviceList/RFoot/FSR/") + SensorNames[i] + std::string("/Sensor/Value");
	    }
	    // There are 7 inertial sensors.
	    for(; i < 41; ++i)
	    {
		sensorKeys_[i] = std::string("Device/SubDeviceList/InertialSensor/") + SensorNames[i] + std::string("/Sensor/Value");
	    }
	    // There are 2 important sonars.
	    sensorKeys_[i] = std::string("Device/SubDeviceList/US/Left/Sensor/Value");
	    i++;
	    sensorKeys_[i] = std::string("Device/SubDeviceList/US/Right/Sensor/Value");
	    i++;
	    // There are 4 foot bumpers.
	    sensorKeys_[i] = std::string("Device/SubDeviceList/LFoot/Bumper/Left/Sensor/Value");
	    i++;
	    sensorKeys_[i] = std::string("Device/SubDeviceList/LFoot/Bumper/Right/Sensor/Value");
	    i++;
	    sensorKeys_[i] = std::string("Device/SubDeviceList/RFoot/Bumper/Left/Sensor/Value");
	    i++;
	    sensorKeys_[i] = std::string("Device/SubDeviceList/RFoot/Bumper/Right/Sensor/Value");
	    i++;
	    // There is a single chest button. 
	    sensorKeys_[i] = std::string("Device/SubDeviceList/Chestboard/Button/Sensor/Value");

	    fastMemoryAccess_->ConnectToVariables(broker_, sensorKeys_);

	    std::cout << "SensorsModule : Sensor keys initialized." << std::endl;
	    for(std::vector<std::string>::iterator iter = sensorKeys_.begin();
		iter != sensorKeys_.end();
		++iter)
	    {
		std::cout << *iter << std::endl;
	    }
	}

	void SensorsModule::initializeSonarValues()
	{
	    // Get a proxy to the DCM.
	    boost::shared_ptr<AL::DCMProxy> dcmProxy = broker_->getDcmProxy();
	    if(dcmProxy != 0)
	    {
		try
		{
		    // For DCM::set see http://www.aldebaran-robotics.com/documentation/naoqi/sensors/dcm-api.html#DCMProxy::set__AL::ALValueCR
		    AL::ALValue dcmSonarCommand;
		    
		    dcmSonarCommand.arraySetSize(3);
		    dcmSonarCommand[0] = std::string("Device/SubDeviceList/US/Actuator/Value"); // Device name.
		    dcmSonarCommand[1] = std::string("ClearAll"); // Delete all timed commands before adding this one. 

		    dcmSonarCommand[2].arraySetSize(1); // A list of (1) timed-commands.
		    dcmSonarCommand[2][0].arraySetSize(2);
		    dcmSonarCommand[2][0][0] = 68.0; // The command itself. 
		    dcmSonarCommand[2][0][1] = dcmProxy->getTime(0); // The DCM time for the command to be applied. 
		    
		    // Send the timed command to the sonars.
		    dcmProxy->set(dcmSonarCommand);
		} 
		catch(AL::ALError& e)
		{
		    std::cout << "SensorsModule : Failed to initialize sonars, " << e.toString() << std::endl;
		}
	    }
	}

	void SensorsModule::updateSensorValues()
	{
	    std::cout << "SensorsModule : Retrieving sensor values from NAOqi." << std::endl;
	    fastMemoryAccess_->GetValues(sensorValues_);
	    std::cout << "SensorsModule : Sensor values " << std::endl;
	    for(int i = 0; i < NUM_SENSOR_VALUES; ++i)
	    {
		std::cout << SensorNames[i] << " = " << sensorValues_[i] << std::endl;
	    }
	}

	void SensorsModule::run_()
	{
	    updateSensorValues();
	}
    } // namespace sensors
} // namespace man
