#include <iostream>
#include <cstdlib>

#include "RoboGuardian.h"
using namespace std;

#include "Kinematics.h"

#include "StiffnessCommand.h"

const int RoboGuardian::GUARDIAN_FRAME_RATE = 50;
// 1 second * 1000 ms/s * 1000 us/ms
const float RoboGuardian::GUARDIAN_FRAME_LENGTH_uS = 1.0f * 1000.0f * 1000.0f /
                                                RoboGuardian::GUARDIAN_FRAME_RATE;


RoboGuardian::RoboGuardian(boost::shared_ptr<Synchro> _synchro,
                           boost::shared_ptr<Sensors> s,
                           AL::ALPtr<AL::ALBroker> _broker,
                           MotionInterface * _interface)
    : Thread(_synchro,"RoboGuardian"), sensors(s),
      broker(_broker),motion_interface(_interface),
      lastTemps(sensors->getBodyTemperatures()),
      buttonOnCounter(0),buttonOffCounter(0),
      lastButtonOnCounter(0),lastButtonOffCounter(0),
      buttonClicks(0), registeredClickThisTime(true)
{
}
RoboGuardian::~RoboGuardian(){}



void RoboGuardian::run(){
    Thread::running = true;

    while(Thread::running){

        checkTemperatures();
        checkButtonPushes();

         usleep(static_cast<useconds_t>(GUARDIAN_FRAME_LENGTH_uS));
    }

    Thread::trigger->off();
}


void RoboGuardian::checkTemperatures(){
    static const float HIGH_TEMP = 40.0f; //deg C
    static const float TEMP_THRESHOLD = 1.0f; //deg C

    vector<float> newTemps = sensors->getBodyTemperatures();

    for(unsigned int joint = 0; joint < Kinematics::NUM_JOINTS; joint++){
        const float tempDiff = newTemps[joint] - lastTemps[joint];
        if(newTemps[joint] >= HIGH_TEMP && tempDiff >= TEMP_THRESHOLD){
            cout << Thread::name << "::" << "TEMPERATURE-WARNING:"
                 << "\t" << Kinematics::JOINT_STRINGS[joint]
                 << "'s temperature increased by " <<tempDiff <<" to "
                 << newTemps[joint] <<" deg C"<<endl;
        }
    }

}



void RoboGuardian::checkButtonPushes(){
    //These assume were running 50 fps
    static const int SINGLE_CLICK_ACTIVE_MIN = 5;
    static const int SINGLE_CLICK_ACTIVE_MAX = 50;
    static const int SINGLE_CLICK_INACTIVE_MIN = 4;
    static const int SINGLE_CLICK_INACTIVE_MAX = 18;
    static const float PUSHED = 1.0f;
    static const int SHUTDOWN_THRESH = GUARDIAN_FRAME_RATE * 3;

    float buttonPush = sensors->getChestButton();

    bool push_done =false;
    bool unpush_done = false;

    if(buttonPush == PUSHED){

        buttonOnCounter += 1;
        if(buttonOffCounter > 0){
            lastButtonOffCounter = buttonOffCounter;
            buttonOffCounter  = 0;
        }
        registeredClickThisTime = false;
    }else{
        if(buttonOnCounter >0){
            lastButtonOnCounter = buttonOnCounter;
            buttonOnCounter = 0;
        }
        buttonOffCounter  +=1;
    }

    //detect if a click occured sometime in the past
    if( lastButtonOnCounter >= SINGLE_CLICK_ACTIVE_MIN &&
        lastButtonOnCounter <= SINGLE_CLICK_ACTIVE_MAX &&
        buttonOffCounter >= SINGLE_CLICK_INACTIVE_MIN &&
        buttonOffCounter <= SINGLE_CLICK_INACTIVE_MAX &&
        !registeredClickThisTime){
        buttonClicks += 1;
        cout << "Registered a click, waiting to see if there are more"<<endl;
        registeredClickThisTime = true;
   }

    if( buttonOffCounter > SINGLE_CLICK_INACTIVE_MAX &&
         buttonClicks > 0){
        cout << "Processing " <<buttonClicks <<" clicks"<<endl;
        executeClickAction(buttonClicks);
        buttonClicks = 0;
    }

    if(buttonOnCounter > SHUTDOWN_THRESH && !registeredClickThisTime){
        registeredClickThisTime = true;
        executeShutdownAction();
    }

}


void RoboGuardian::executeClickAction(int numClicks){
    switch(numClicks){

    case 1:
        //Say IP Address
        break;
    case 2:
        cout << "Registered two clicks, disabling the gains" <<endl;
        motion_interface->sendStiffness( new StiffnessCommand(0.0f));
        break;
    case 6:
        //Easter EGG!
        break;
    default:
        //nothing
        break;
    }

}


void RoboGuardian::executeShutdownAction(){
    cout << "Shutting down the robot NOW!!!"<<endl;
    system("shutdown -h now &");
}
