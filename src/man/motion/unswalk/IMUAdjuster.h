#include "utils/Kalman.h"
#include <math.h>
#include "utils/RingBufferWithSum.h"
// #include "utils/Vector3.h"

namespace man
{
namespace motion 
{

class IMUAdjuster 
{
public:
	IMUAdjuster();
	~IMUAdjuster();

	static const int DONE = 500;

	// Vec2 gyroBiasProcessNoise;
	// Vec2 gyroBiasStandMeasurementNoise;
	// Vec2 gyroBiasWalkMeasurementNoise;
	// Vec3 accBiasProcessNoise;
	// Vec3 accBiasStandMeasurementNoise;
	// Vec3 accBiasWalkMeasurementNoise;

		
	  // Kalman<float> accXBias; /**< The calibration bias of accX. */
	  // Kalman<float> accYBias; /**< The calibration bias of accY. */
	  // Kalman<float> accZBias; *< The calibration bias of accZ. 
	  // Kalman<float> gyroXBias; /**< The calibration bias of gyroX. */
	  // Kalman<float> gyroYBias; /**< The calibration bias of gyroY. */

	void adjustIMUs(float gyr_x, float gyr_y, float angle_x, float angle_y);

	void findAvgOffset(float gyX, float gyY, float acc_x, float acc_y, float acc_z);
	bool isDone() { return counter >= DONE; };

	float getGyrXZero() { return gyr_zero_x; }
	float getGyrYZero() { return gyr_zero_y; }

	float getGyrX() { return adj_gyr_x; }
	float getGyrY() { return adj_gyr_y; }

	bool isFalling() { return falling; }

	void reset();

private:

	// Take a long running average of the gyroscope's values,  making the assumption
	// that if the robot does not perform any complete revolutions, accurate readings should
	// average to exactly zero
	bool falling;

	float gyr_zero_x;
	float gyr_zero_y;

	int counter;
	bool initted;

	// Value of the gyroscope reading itself should also be averaged over time,
	// in order to remove sharp changes in the gyroscope's values, which could 
	// cause undesirable feedback loops
	float adj_gyr_x;
	float adj_gyr_y;

	static const float fallDownAngleX = 25 * M_PI / 180;
	static const float fallDownAngleY = 50 * M_PI / 180;
	static const float onGroundAngle = 75 * M_PI / 180;

	void init();

    // RingBufferWithSumBH<Vector3BH<>, 300> accValues; *< Ringbuffer for collecting the acceleration sensor values of one walking phase or 1 secBH. 
    // RingBufferWithSumBH<float, 15> accXBuffer; // *< Ringbuffer for collecting the gyro sensor values of one walking phase or 1 secBH. 
    // RingBufferWithSumBH<float, 15> accYBuffer; *< Ringbuffer for collecting the gyro sensor values of one walking phase or 1 secBH. 
    // RingBufferWithSumBH<float, 15> accZBuffer; /**< Ringbuffer for collecting the gyro sensor values of one walking phase or 1 secBH. */


};
}
}