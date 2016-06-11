#include "IMUAdjuster.h"
#include <iostream>

namespace man 
{
namespace motion 
{
	static const float FRAME_SCALE = 0.01;
	static const float OLD_SCALE = 0.7;

	IMUAdjuster::IMUAdjuster() {
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
		old_filtered_gyro_x = old_filtered_gyro_x * OLD_SCALE + gyro_value * (1.0 - OLD_SCALE) + old_gyro_zero_x;
		return old_filtered_gyro_x * FRAME_SCALE;
	}

	float IMUAdjuster::adjustGyrY(float gyro_value) {
		old_filtered_gyro_y = old_filtered_gyro_y * OLD_SCALE + gyro_value * (1.0 - OLD_SCALE) + old_gyro_zero_y;
		return old_filtered_gyro_y * FRAME_SCALE;
	}

}

}