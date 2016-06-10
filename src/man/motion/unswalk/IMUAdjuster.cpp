#include "IMUAdjuster.h"
#include <iostream>

namespace man 
{
namespace motion 
{
	IMUAdjuster::IMUAdjuster() {
		old_gyro_zero_x = 0.0f;
		old_gyro_zero_y = 0.0f;
		old_filtered_gyro_x = 0.0f;
		old_filtered_gyro_y = 0.0f;

	}

	IMUAdjuster::~IMUAdjuster()
	{

	}


	float IMUAdjuster::adjustGyrX(float gyro_value) {
		std::cout << "OLD ZERO X: " << old_gyro_zero_x << std::endl;
		old_gyro_zero_x = old_gyro_zero_x * 0.99 + gyro_value * 0.01;
		old_filtered_gyro_x = old_filtered_gyro_x * 0.8 + gyro_value * 0.2;
		std::cout << "NEW ZERO X: " << old_gyro_zero_x << std::endl;
		return old_filtered_gyro_x;
	}

	float IMUAdjuster::adjustGyrY(float gyro_value) {
		std::cout << "OLD ZERO Y: " << old_gyro_zero_x << std::endl;

		old_gyro_zero_x = old_gyro_zero_x * 0.99 + gyro_value * 0.01;
		old_filtered_gyro_x = old_filtered_gyro_x * 0.8 + gyro_value * 0.2;
		std::cout << "NEW ZERO Y: " << old_gyro_zero_x << std::endl;
		return old_filtered_gyro_y;
	}

}

}