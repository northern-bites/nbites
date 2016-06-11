
namespace man
{
namespace motion 
{

class IMUAdjuster 
{
public:
	IMUAdjuster();
	~IMUAdjuster();

	static const int DONE = 300;

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


};
}
}