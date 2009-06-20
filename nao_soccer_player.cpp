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

#include "TTMan.h"
#include "WBEnactor.h"
#include "WBImageTranscriber.h"
#include "WBTranscriber.h"
#include "WBLights.h"

using boost::shared_ptr;

typedef Man WBMan;

static shared_ptr<WBMan> man;
static shared_ptr<Sensors> sensors;
static shared_ptr<Synchro> synchro;
static shared_ptr<WBTranscriber> transcriber;
static shared_ptr<WBImageTranscriber> imageTranscriber;
static shared_ptr<WBEnactor> enactor;
static shared_ptr<Lights> lights;


void WBCreateMan(){

    synchro = shared_ptr<Synchro>(new Synchro());
    sensors = shared_ptr<Sensors>(new Sensors);
    transcriber = shared_ptr<WBTranscriber>(new WBTranscriber(sensors));
    imageTranscriber =
        shared_ptr<WBImageTranscriber>
        (new WBImageTranscriber(sensors));

    enactor = shared_ptr<WBEnactor>(new WBEnactor(sensors,
                                                  transcriber));
    lights  = shared_ptr<Lights>(new WBLights());

    man = boost::shared_ptr<WBMan> (new WBMan(sensors,
                                              transcriber,
                                              imageTranscriber,
                                              enactor,
                                              synchro,
                                              lights));
    man->startSubThreads();
}

void WBDestroyMan(){
    man->stopSubThreads();
}


int main() {

  wb_robot_init();

  WBCreateMan();
  int time_step = static_cast<int>(wb_robot_get_basic_time_step());

  // forever
  for (;;) {
    wb_robot_step(time_step);

    //step motion
    enactor->sendCommands();
    enactor->postSensors();
    //step vision
    transcriber->postVisionSensors();
    imageTranscriber->waitForImage();

    //step motion (2nd time)
    usleep(2000);
    enactor->sendCommands();
    enactor->postSensors();

  }

  WBDestroyMan();
  // never reached
  return 0;
}


/* Remember these:
 * wb servo set motor force() 
 *
 * Foot sensors: RFsrFL, RFsrFR, RFsrBR, RFsrBL
 *               LFsrFL, LFsrFR, LFsrBR, LFsrBL
 */

//GPS Example
//   #include <webots/gps.h>
//   WbDeviceTag gps = wb_robot_get_device("gps");
//   wb_gps_enable(gps,40);
//   const double * gps_vals = wb_gps_get_values(gps);
//   printf("Robot is at (%f,%f,%f) \n",
//          gps_vals[0],gps_vals[1],gps_vals[2]);
