/*
 * File:         nao_soccer_player_red.c
 * Description:  This is dummy controller
 *               In principle this directory provides the files for the Java controller example for Robotstadium.
 *               However in the case Java is not installed, this tiny C controller will demonstrates a minimal walking behavior.
 *               You can safely remove this source file and the corresponding executable file if you want to work on the Java example.
 * Author:       yvan.bourquin - www.cyberbotics.com
 */

#include <iostream>
#include <string>
#include <cstdlib>
using namespace std;

#include <webots/robot.h>
#include <webots/utils/motion.h>
#include <webots/accelerometer.h>
#include <webots/gyro.h>
#include <webots/servo.h>
#include <webots/touch_sensor.h>

#include <webots/camera.h>

//#include "Man.h"

int main() {
  wb_robot_init();

  int time_step = static_cast<int>(wb_robot_get_basic_time_step());

  // load and start forward motion
  WbMotionRef forwards = wbu_motion_new("../motions/Forwards50.motion");
  wbu_motion_set_loop(forwards, true);
  wbu_motion_play(forwards);

  // These are all the sensors tags that we will need
  WbDeviceTag acc = wb_robot_get_device("accelerometer");
  WbDeviceTag gyro = wb_robot_get_device("gyro");
  /*
  WbDeviceTag us1 = wb_robot_get_device("US/TopRight");
  WbDeviceTag us2 = wb_robot_get_device("US/BottomRight");
  WbDeviceTag us3 = wb_robot_get_device("US/TopLeft");
  WbDeviceTag us4 = wb_robot_get_device("US/BottomLeft");
  */
  WbDeviceTag an_fsr = wb_robot_get_device("LFsrFR");
  string angle = "LKneePitch";
  WbDeviceTag angleTag = wb_robot_get_device(angle.c_str());
  //double kneeAngle = wb_servo_get_position(angleTag);
  //WbDeviceTag servos[

  // WbDeviceTag camera_servo
  WbDeviceTag camera = wb_robot_get_device("camera");

  wb_accelerometer_enable (acc, 20);
  wb_gyro_enable (gyro, 20);
  wb_servo_enable_position (angleTag, 20);
  wb_touch_sensor_enable (an_fsr, 20);
  wb_camera_enable(camera, 20);


  // forever
  for (;;) {
    printf("testing balls balls\n");

    wb_robot_step(time_step);
    const double *acc_values = wb_accelerometer_get_values (acc);
    printf("accelerometers: x: %f, y: %f, z: %f\n",
	   acc_values[0], acc_values[1], acc_values[2]);

    const double *gyro_values = wb_gyro_get_values (gyro);
    printf("gyros: x: %f, y: %f\n", gyro_values[0], gyro_values[1]);

    double kneeAngle = wb_servo_get_position(angleTag);
    cout << "kneeAngle: " << kneeAngle << endl;

    double an_fsr_value = wb_touch_sensor_get_value(an_fsr);
    cout << "an fsr: " << an_fsr_value << endl;

    const unsigned char *image = wb_camera_get_image (camera);
    cout << "third value in the image: " << (int)image[80 * 119 * 3] << endl;
  }

  // never reached
  return 0;
}


/* Remember these:
 * wb servo set motor force() 
 *
 * Foot sensors: RFsrFL, RFsrFR, RFsrBR, RFsrBL
 *               LFsrFL, LFsrFR, LFsrBR, LFsrBL
 */
