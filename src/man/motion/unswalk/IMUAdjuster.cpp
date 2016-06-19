#include "IMUAdjuster.h"
#include <iostream>

namespace man 
{
namespace motion 
{
	static const float FRAME_SCALE = 1.0; //0.01;
	static const float OLD_SCALE = 0.8;

	IMUAdjuster::IMUAdjuster() {
		initted = false;
		falling = false;

		gyr_zero_x = 0.0f;
		gyr_zero_y = 0.0f;
		adj_gyr_x = 0.0f;
		adj_gyr_y = 0.0f;
		counter = 0;

	}

	IMUAdjuster::~IMUAdjuster() { }

	void IMUAdjuster::findAvgOffset(float gyX, float gyY, float acc_x, float acc_y, float acc_z) 
	{
		gyr_zero_x = gyr_zero_x * 0.99 + gyX * 0.01;
		gyr_zero_y = gyr_zero_y * 0.99 + gyY * 0.01;

		if (counter <= DONE + 1) {
			counter++;
		}

	}

	void IMUAdjuster::adjustIMUs(float gyr_x, float gyr_y, float angle_x, float angle_y) {
		gyr_x = gyr_x - gyr_zero_x;
		adj_gyr_x = adj_gyr_x * OLD_SCALE + gyr_x * (1.0 - OLD_SCALE);

		gyr_y = gyr_y - gyr_zero_y;
		adj_gyr_y = adj_gyr_y * OLD_SCALE + gyr_y * (1.0 - OLD_SCALE);

		if (angle_x >= fallDownAngleX || angle_y >= fallDownAngleY) {
			// std::cout << "FALLING IS TRUUUUE\n";
			falling = true;
		} else {
			falling = false;
		}


		// accXBuffer.add(acc_x);
		// accYBuffer.add(acc_y);
		// accZBuffer.add(acc_z);
		// float accXaverage(accXBuffer.getAverage());
		// float accYaverage(accYBuffer.getAverage());
		// float accZaverage(accZBuffer.getAverage());
		// float accelerationAngleXZ(atan2(accZaverage, accXaverage));
		// float accelerationAngleYZ(atan2(accZaverage, accYaverage));



		// acc_x = acc_x - acc_zero_x;
		// adj_acc_x = adj_acc_x * OLD_SCALE + acc_x * (1.0 - OLD_SCALE);

		// acc_y = acc_y - acc_zero_y;
		// adj_acc_y = adj_acc_y * OLD_SCALE + acc_y * (1.0 - OLD_SCALE);

		// acc_z = acc_z - acc_zero_z;
		// adj_acc_z = adj_acc_z * OLD_SCALE + acc_z * (1.0 - OLD_SCALE);


		// std::cout << "NEW ZERO GY X: " << gyr_zero_x << std::endl;
		// std::cout << "NEW ZERO GY Y: " << gyr_zero_y << std::endl;
		// std::cout << "NEW ZERO acc_zero_x: " << acc_zero_x << std::endl;
		// std::cout << "NEW ZERO acc_zero_y: " << acc_zero_y << std::endl;
		// std::cout << "NEW ZERO acc_zero_z: " << acc_zero_z << std::endl << std::endl;


		// std::cout << "NEW ZERO GY X: " << gyr_zero_x << std::endl;
		// std::cout << "NEW ZERO GY Y: " << gyr_zero_y << std::endl;
		// std::cout << "NEW ZERO acc_zero_x: " << acc_zero_x << std::endl;
		// std::cout << "NEW ZERO acc_zero_y: " << acc_zero_y << std::endl;
		// std::cout << "NEW ZERO acc_zero_z: " << acc_zero_z << std::endl;



	}

	// float IMUAdjuster::adjustGyrX(float gyro_value) {
	// 	// gyroXValues.add(gyro_value);

	// 	std::cout << "NEW ZERO X: " << gyr_zero_x << std::endl;
	// 	std::cout << "NEW ZERO Y: " << gyr_zero_y << std::endl;
	// 	float new_filtered_gyro = gyro_value - gyr_zero_x;
	// 	adj_gyr_x = adj_gyr_x * OLD_SCALE + new_filtered_gyro * (1.0 - OLD_SCALE);
	// 	return adj_gyr_x;

		
	// 	return gyro_value - gyr_zero_x;

	// 	std::cout << "adj_gyr_x: " << adj_gyr_x << std::endl;
	// 	std::cout << "adj_gyr_x * OLD_SCALE: " << adj_gyr_x * OLD_SCALE << std::endl;
	// 	std::cout << "old adj_gyr_y: " << adj_gyr_y << std::endl;
	// 	adj_gyr_x = adj_gyr_x * OLD_SCALE + gyro_value * (1.0 - OLD_SCALE) - gyr_zero_x;
	// 	return adj_gyr_x * FRAME_SCALE;
	// }

	// float IMUAdjuster::adjustGyrY(float gyro_value) {
	// 	// gyroYValues.add(gyro_value);

	// 	float new_filtered_gyro = gyro_value - gyr_zero_y;
	// 	adj_gyr_y = adj_gyr_y * OLD_SCALE + new_filtered_gyro * (1.0 - OLD_SCALE);
	// 	return adj_gyr_y;

	// 	return gyro_value - gyr_zero_y;

	// 	adj_gyr_y = adj_gyr_y * OLD_SCALE + gyro_value * (1.0 - OLD_SCALE) - gyr_zero_y;
	// 	return adj_gyr_y * FRAME_SCALE;
	// }

	void BHCalibrate() {

		// if (!initted) {
		// 	initted = true;
		// 	accXBias.init(collection.accAvg.x, sqrBH(accBiasMeasurementNoise.x));
	 //        accYBias.init(collection.accAvg.y, sqrBH(accBiasMeasurementNoise.y));
	 //        accZBias.init(collection.accAvg.z, sqrBH(accBiasMeasurementNoise.z));
	 //        gyroXBias.init(collection.gyroAvg.x, sqrBH(gyroBiasMeasurementNoise.x));
	 //        gyroYBias.init(collection.gyroAvg.y, sqrBH(gyroBiasMeasurementNoise.y));
		// }
  //       else
  //       {
  //         accXBias.update(collection.accAvg.x, sqrBH(accBiasMeasurementNoise.x));
  //         accYBias.update(collection.accAvg.y, sqrBH(accBiasMeasurementNoise.y));
  //         accZBias.update(collection.accAvg.z, sqrBH(accBiasMeasurementNoise.z));
  //         gyroXBias.update(collection.gyroAvg.x, sqrBH(gyroBiasMeasurementNoise.x));
  //         gyroYBias.update(collection.gyroAvg.y, sqrBH(gyroBiasMeasurementNoise.y));
  //       } 

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


        //   calibrated = true;
        //   accXBias.init(collection.accAvg.x, sqrBH(accBiasMeasurementNoise.x));
        //   accYBias.init(collection.accAvg.y, sqrBH(accBiasMeasurementNoise.y));
        //   accZBias.init(collection.accAvg.z, sqrBH(accBiasMeasurementNoise.z));
        //   gyroXBias.init(collection.gyroAvg.x, sqrBH(gyroBiasMeasurementNoise.x));
        //   gyroYBias.init(collection.gyroAvg.y, sqrBH(gyroBiasMeasurementNoise.y));
        // }
        // else
        // {
        //   accXBias.update(collection.accAvg.x, sqrBH(accBiasMeasurementNoise.x));
        //   accYBias.update(collection.accAvg.y, sqrBH(accBiasMeasurementNoise.y));
        //   accZBias.update(collection.accAvg.z, sqrBH(accBiasMeasurementNoise.z));
        //   gyroXBias.update(collection.gyroAvg.x, sqrBH(gyroBiasMeasurementNoise.x));
        //   gyroYBias.update(collection.gyroAvg.y, sqrBH(gyroBiasMeasurementNoise.y));
        // }        


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