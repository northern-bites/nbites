#include "IMUAdjuster.h"
#include <iostream>

namespace man 
{
namespace motion 
{
	static const float FRAME_SCALE = 1.0; //0.01;
	static const float OLD_SCALE = 0.8;

	IMUAdjuster::IMUAdjuster() {
		gyroBiasProcessNoise = Vec2(0.05f, 0.05f);
		gyroBiasStandMeasurementNoise = Vec2(0.01f, 0.01f);
		gyroBiasWalkMeasurementNoise = Vec2(0.1f, 0.1f);
		accBiasProcessNoise = Vec3(0.1f, 0.1f, 0.1f);
		accBiasStandMeasurementNoise = Vec3(0.1f, 0.1f, 0.1f);
		accBiasWalkMeasurementNoise = Vec3(1.f, 1.f, 1.f);

		old_gyro_zero_x = 0.0f;
		old_gyro_zero_y = 0.0f;
		old_filtered_gyro_x = 0.0f;
		old_filtered_gyro_y = 0.0f;
		counter = 0;

	}

	IMUAdjuster::~IMUAdjuster() { }

	void IMUAdjuster::findAvgOffset(float gyX, float gyY) 
	{
		old_gyro_zero_x = old_gyro_zero_x * 0.99 + gyX * 0.01;
		old_gyro_zero_y = old_gyro_zero_y * 0.99 + gyY * 0.01;
		// std::cout << "NEW ZERO X: " << old_gyro_zero_x << std::endl;
		// std::cout << "NEW ZERO Y: " << old_gyro_zero_y << std::endl;
		counter++;

	}

	float IMUAdjuster::adjustGyrX(float gyro_value) {
		// std::cout << "NEW ZERO X: " << old_gyro_zero_x << std::endl;
		// std::cout << "NEW ZERO Y: " << old_gyro_zero_y << std::endl;
		old_filtered_gyro_x = old_filtered_gyro_x * OLD_SCALE + gyro_value * (1.0 - OLD_SCALE) - old_gyro_zero_x;
		return old_filtered_gyro_x * FRAME_SCALE;
	}

	float IMUAdjuster::adjustGyrY(float gyro_value) {
		old_filtered_gyro_y = old_filtered_gyro_y * OLD_SCALE + gyro_value * (1.0 - OLD_SCALE) - old_gyro_zero_y;
		return old_filtered_gyro_y * FRAME_SCALE;
	}

	void BHCalibrate() {


		// const Vector2BH<> gyro = Vector2BH<>(theSensorDataBH.data[SensorDataBH::gyroX], theSensorDataBH.data[SensorDataBH::gyroY]);
	 //  const Vector3BH<> acc = Vector3BH<>(theSensorDataBH.data[SensorDataBH::accX], theSensorDataBH.data[SensorDataBH::accY], theSensorDataBH.data[SensorDataBH::accZ]);
	 
	 //  // it's prediction time!
	 //  if(lastTime && calibrated)
	 //  {
	 //    const float timeDiff = float(theFrameInfoBH.time - lastTime) * 0.001f; // in seconds
	 //    accXBias.predict(0.f, sqrBH(accBiasProcessNoise.x * timeDiff));
	 //    accYBias.predict(0.f, sqrBH(accBiasProcessNoise.y * timeDiff));
	 //    accZBias.predict(0.f, sqrBH(accBiasProcessNoise.z * timeDiff));
	 //    gyroXBias.predict(0.f, sqrBH(gyroBiasProcessNoise.x * timeDiff));
	 //    gyroYBias.predict(0.f, sqrBH(gyroBiasProcessNoise.y * timeDiff));
	 //  }


	    // inertiaSensorData.gyro.x = gyro.x - gyroXBias.value;
	    // inertiaSensorData.gyro.y = gyro.y - gyroYBias.value;
	    // inertiaSensorData.acc.x = acc.x - accXBias.value;
	    // inertiaSensorData.acc.y = acc.y - accYBias.value;
	    // inertiaSensorData.acc.z = acc.z - accZBias.value;

	    // inertiaSensorData.gyro.x *= theSensorCalibrationBH.gyroXGain;
	    // inertiaSensorData.gyro.y *= theSensorCalibrationBH.gyroYGain;
	    // inertiaSensorData.acc.x *= theSensorCalibrationBH.accXGain;
	    // inertiaSensorData.acc.y *= theSensorCalibrationBH.accYGain;
	    // inertiaSensorData.acc.z *= theSensorCalibrationBH.accZGain;
  



	}

}

}