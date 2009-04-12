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
#ifdef DEBUG_GUARDIAN_CLICKS
        cout << "Registered a click, waiting to see if there are more"<<endl;
#endif
        registeredClickThisTime = true;
   }

    if( buttonOffCounter > SINGLE_CLICK_INACTIVE_MAX &&
         buttonClicks > 0){
#ifdef DEBUG_GUARDIAN_CLICKS
        cout << "Processing " <<buttonClicks <<" clicks"<<endl;
#endif
        executeClickAction(buttonClicks);
        buttonClicks = 0;
    }

    if(buttonOnCounter > SHUTDOWN_THRESH && !registeredClickThisTime){
        registeredClickThisTime = true;
        executeShutdownAction();
    }

}


static const string quiet = " -q ";
static const string sout = "aplay"+quiet;
static const string sdir = "/opt/naoqi/data/wav/";
static const string nbsdir = "/opt/naoqi/modules/etc/audio/";
static const string wav = ".wav";
static const string shutdown_wav = sdir + "shutdown" + wav;
static const string mynameis_wav = nbsdir + "mynameis" + wav;
static const string my_address_is_wav = sdir + "my_internet_address_is" + wav;
static const string dot = ".";


void playFile(string str){
    system((sout+str).c_str());
}


void RoboGuardian::executeClickAction(int numClicks){
    switch(numClicks){

    case 1:
        //Say IP Address
        speakIPAddress();
        break;
    case 2:
        cout << "Registered two clicks, disabling the gains" <<endl;
        motion_interface->sendStiffness( new StiffnessCommand(0.0f));
        break;
    case 6:
        //Easter EGG!
        playFile(nbsdir+"easter_egg.wav");
        break;
    default:
        //nothing
        break;
    }

}


void RoboGuardian::executeStartupAction(){
//Blank for now

}

void RoboGuardian::executeShutdownAction(){
    cout << "Shutting down the robot NOW!!!"<<endl;
    playFile(shutdown_wav);
    system("shutdown -h now &");
}

string getHostName(){
    char name[40];
    name[0] ='\0';
    gethostname(name,39);
    return string(name);
}

void RoboGuardian::speakIPAddress(){
    //Currently we poll the broker. If this breaks in the future
    //you can try to call /opt/naoqi/bin/ip.sh or
    //parse the output of if config yourself
    const string IP = broker->getIP();
    const string host = getHostName();

    cout << "Hello, my name is " + host
         <<". My internet address is "<< IP<<endl;
    playFile(mynameis_wav);
    playFile(nbsdir+host+wav);
    playFile(my_address_is_wav);
    for (unsigned int i = 0; i < IP.size(); i++){
        char digit = IP[i];
        if(digit == dot[0])
            playFile(sdir+"point"+wav);
        else
            playFile(sdir+digit+wav);

    }
}
