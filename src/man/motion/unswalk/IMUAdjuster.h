#include "utils/Kalman.h"
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

	static const int DONE = 400;

	typedef struct Vec3 {
		float x;
		float y;
		float z;

		Vec3(float _x, float _y, float _z) {
			x = _x;
			y = _y;
			z = _z;
		}

		Vec3() { }
	};

	typedef struct Vec2 {
		float x;
		float y;

		Vec2(float _x, float _y) {
			x = _x;
			y = _y;
		}

		Vec2() { }
	};

	Vec2 gyroBiasProcessNoise;
	Vec2 gyroBiasStandMeasurementNoise;
	Vec2 gyroBiasWalkMeasurementNoise;
	Vec3 accBiasProcessNoise;
	Vec3 accBiasStandMeasurementNoise;
	Vec3 accBiasWalkMeasurementNoise;

		
	  Kalman<float> accXBias; /**< The calibration bias of accX. */
	  Kalman<float> accYBias; /**< The calibration bias of accY. */
	  Kalman<float> accZBias; /**< The calibration bias of accZ. */
	  Kalman<float> gyroXBias; /**< The calibration bias of gyroX. */
	  Kalman<float> gyroYBias; /**< The calibration bias of gyroY. */

	float adjustGyrX(float gyro_value);
	float adjustGyrY(float gyro_value);

	void findAvgOffset(float gyX, float gyY);
	bool isDone() { return counter > DONE; };

	// Take a long running average of the gyroscope's values,  making the assumption
	// that if the robot does not perform any complete revolutions, accurate readings should
	// average to exactly zero
	float old_gyro_zero_x;
	float old_gyro_zero_y;

	int counter;
	bool initted;

	// Value of the gyroscope reading itself should also be averaged over time,
	// in order to remove sharp changes in the gyroscope's values, which could 
	// cause undesirable feedback loops
	float old_filtered_gyro_x;
	float old_filtered_gyro_y;

	// The filtered reading is multipled by an experimentally determined constant,
	// the nod_gyro_ratio, to give an adjustment angle
	static const float NOD_GYRO_RATIO = 0.04;
	// After the pose of the robot is determined by the engine, this adjustment
	// angle is added to particular joint angles in the robot to give an 
	// adjusted pose

	float getGyrXZero() { return old_gyro_zero_x; }
	float getGyrYZero() { return old_gyro_zero_y; }


    // RingBufferWithSumBH<Vector3BH<>, 300> accValues; *< Ringbuffer for collecting the acceleration sensor values of one walking phase or 1 secBH. 
    RingBufferWithSumBH<float, 300> gyroXValues; /**< Ringbuffer for collecting the gyro sensor values of one walking phase or 1 secBH. */
    RingBufferWithSumBH<float, 300> gyroYValues; /**< Ringbuffer for collecting the gyro sensor values of one walking phase or 1 secBH. */


};
}
}