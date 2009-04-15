#include "RoboGuardian.h"

#include <iostream>
#include <cstdlib>

using namespace std;

#include "almodule.h"
#include "alsentinelproxy.h"
using namespace AL;

#include "Kinematics.h"

#include "StiffnessCommand.h"

const int RoboGuardian::GUARDIAN_FRAME_RATE = 50;
// 1 second * 1000 ms/s * 1000 us/ms
const float RoboGuardian::GUARDIAN_FRAME_LENGTH_uS = 1.0f * 1000.0f * 1000.0f /
    RoboGuardian::GUARDIAN_FRAME_RATE;

const int RoboGuardian::NO_CLICKS = -1;

static const string quiet = " -q ";
static const string sout = "aplay"+quiet;
static const string sdir = "/opt/naoqi/data/wav/";
static const string nbsdir = "/opt/naoqi/modules/etc/audio/";
static const string wav = ".wav";
static const string shutdown_wav = sdir + "shutdown" + wav;
static const string heat_wav = sdir + "heat" + wav;
static const string energy_wav = sdir + "energy" + wav;
static const string mynameis_wav = nbsdir + "mynameis" + wav;
static const string my_address_is_wav = sdir + "my_internet_address_is" + wav;
static const string stiffness_removed_wav = sdir + "emergency_stiffness"+wav;
static const string stiffness_enabled_wav = nbsdir + "stiffness_enabled"+wav;
static const string warning_wav = sdir + "warning"+wav;
static const string falling_wav = nbsdir +"falling"+wav;
static const string wifi_restart_wav = nbsdir +"wifi_restart"+wav;
static const string dot = ".";


//Non blocking!!
void RoboGuardian::playFile(string str)const{
    system((sout+str+" &").c_str());
}



RoboGuardian::RoboGuardian(boost::shared_ptr<Synchro> _synchro,
                           boost::shared_ptr<Sensors> s,
                           AL::ALPtr<AL::ALBroker> _broker,
                           MotionInterface * _interface)
    : Thread(_synchro,"RoboGuardian"), sensors(s),
      broker(_broker),motion_interface(_interface),
      lastTemps(sensors->getBodyTemperatures()),
      lastBatteryCharge(sensors->getBatteryCharge()),
      buttonOnCounter(0),buttonOffCounter(0),
      lastButtonOnCounter(0),lastButtonOffCounter(0),
      buttonClicks(0),
      lastInertial(sensors->getInertial()), fallingFrames(0),
      notFallingFrames(0),fallenCounter(0),numClicks(NO_CLICKS),
      registeredClickThisTime(true),registeredShutdown(false),
      falling(false),fallen(false),
      useFallProtection(false)
{
    pthread_mutex_init(&click_mutex,NULL);
    try{
        ALSentinelProxy sentinel(broker);
        sentinel.enableDefaultActionSimpleClick(false);
        sentinel.enableDefaultActionDoubleClick(false);
        sentinel.enableDefaultActionTripleClick(false);
    }catch(ALError &e){
        cout << "Failed to access the ALSentinel: "<<e.toString()<<endl;
    }
    executeStartupAction();
}
RoboGuardian::~RoboGuardian(){
    pthread_mutex_destroy(&click_mutex);
}



void RoboGuardian::run(){
    Thread::running = true;

    while(Thread::running){

        checkFallProtection();
        checkBatteryLevels();
        checkTemperatures();
        checkButtonPushes();

         usleep(static_cast<useconds_t>(GUARDIAN_FRAME_LENGTH_uS));
    }

    Thread::trigger->off();
}

void RoboGuardian::shutoffGains(){
    playFile(stiffness_removed_wav);
    motion_interface->sendStiffness( new StiffnessCommand(
                                         StiffnessCommand::NO_STIFFNESS));
}

static const float FALL_SPEED_THRESH = 0.04f; //rads/20ms
static const float NOFALL_SPEED_THRESH = 0.01f; //rads/20ms
static const int FALLING_FRAMES_THRESH = 5;
static const float FALLING_ANGLE_THRESH = M_PI_FLOAT/4.0f; //45 degrees
static const float FALLEN_ANGLE_THRESH = M_PI_FLOAT/2.5f; //72 degrees


//Check if the angle is unstable, (ie tending AWAY from zero)
bool isFalling(float angle_pos, float angle_vel){
    if (angle_pos < 0){
        //then we only care if the velocity is negative
        if(angle_vel < -FALL_SPEED_THRESH)
            return true;
    }else{
        if(angle_vel > FALL_SPEED_THRESH)
            return true;
    }
    return false;
}

/**
 * Method to watch the robots rotation and detect falls
 * publishes the result of this computation to other classes.
 *
 * Also builds in the ability to shutoff gains when a fall is detected,
 * but this still has some problems. Mainly, it sometimes triggers a fall
 * when the robot is rotated back up again, but only when the robot is 
 * 'over rotated' during the righting.  This feature can be enabled
 * by calling the currently private 'enableFallProtection'.
 * Also, currently the robot will print whenever it believes it is in
 * the process of falling. This will allow us to monitor how well this code
 * works.
 *
 */
void RoboGuardian::checkFallProtection(){
    const Inertial inertial  = sensors->getInertial();

    /***** Determine if the robot is in the process of FALLING ****/
    //Using just the magnitude:
    const float angleMag = std::sqrt(std::pow(inertial.angleX,2) +
                                     std::pow(inertial.angleY,2));
    const float lastAngleMag = std::sqrt(std::pow(lastInertial.angleX,2) +
                                         std::pow(lastInertial.angleY,2));

    const float angleSpeed = angleMag - lastAngleMag;
    const bool falling_critical_angle = angleMag > FALLING_ANGLE_THRESH;

    if(isFalling(angleMag,angleSpeed)){
        fallingFrames += 1;
        notFallingFrames = 0;
    }else if( angleSpeed < NOFALL_SPEED_THRESH){
        fallingFrames = 0;
        notFallingFrames +=1;
    }


    //If the robot has been falling for a while, and the robot is inclined
    //already at a 45 degree angle, than we know we are falling
    if (fallingFrames > FALLING_FRAMES_THRESH && falling_critical_angle){
        //Execute protection? Just print for now and we'll see how this works
        falling = true;
    }else if(notFallingFrames > FALLING_FRAMES_THRESH){
        falling = false;
    }

    /***** Determine if the robot has FALLEN OVER *****/
    const bool fallen_now =
        std::abs(inertial.angleX) > FALLEN_ANGLE_THRESH ||
        std::abs(inertial.angleY) > FALLEN_ANGLE_THRESH;

    if(fallen_now)
        fallenCounter +=1;
    else
        fallenCounter = 0;

    static const int FALLEN_FRAMES_THRESH  = 2;
    if(fallenCounter >= FALLEN_FRAMES_THRESH ){
        fallen = true;
#ifdef DEBUG_GUARDIAN_FALLING
        cout << "Robot has fallen" <<endl;
#endif
    }

    if(fallingFrames == FALLING_FRAMES_THRESH){
        if(useFallProtection){
            shutoffGains();
            cout << "Disabling Gains!!! due to falling" <<endl;
        }
#define DEBUG_GUARDIAN_FALLING
#ifdef DEBUG_GUARDIAN_FALLING
        cout << Thread::name <<": OH NO! I think I'm falling" <<endl;
        playFile(falling_wav);
#endif
    }

    lastInertial  = inertial;
}


// We print each time the battery looses ten percent of charge
//once the charge gets to %30 percent of capacity, we start to say "energy"
void RoboGuardian::checkBatteryLevels(){
    //Constants on 0-10 scale, sensor reading is on 0-1.0 scale
    static const float LOW_BATTERY_VALUE = 3.0f;
    static const float EMPTY_BATTERY_VALUE = 0.7f; //start nagging below 7%

    //sometimes we get weird values from the battery (like -9.8...)
    const float newBatteryCharge =  sensors->getBatteryCharge();
    if(newBatteryCharge < 0 || newBatteryCharge > 1.0){
#ifdef GUARDIAN_DEBUG_BATTERY
        cout<< Thread::name<<": Got bad battery charge of "<< newBatteryCharge
        <<endl;
#endif
        return;
    }

    if(newBatteryCharge != lastBatteryCharge){

        //covert to a 0 - 10 scale
        const float newLevel = floor(newBatteryCharge*10.0f);
        const float oldLevel = floor(lastBatteryCharge*10.0f);
        if(oldLevel != newLevel && oldLevel > newLevel &&
            newLevel - oldLevel <= 2.0f){
            cout << Thread::name << ": Battery charge is now at "
                 << 10.0f * newBatteryCharge
                 << " (was "<<oldLevel<<")"<<endl;

            if (newLevel <= LOW_BATTERY_VALUE){
                playFile(energy_wav);
            }else if(newLevel <= EMPTY_BATTERY_VALUE){
                playFile(energy_wav); playFile(energy_wav);
            }
        }
    }

    lastBatteryCharge = newBatteryCharge;
}

void RoboGuardian::checkTemperatures(){
    static const float HIGH_TEMP = 40.0f; //deg C
    static const float TEMP_THRESHOLD = 1.0f; //deg C
    static const float REALLY_HIGH_TEMP = 50.0f; //deg C

    vector<float> newTemps = sensors->getBodyTemperatures();

    bool sayWarning = false;
    for(unsigned int joint = 0; joint < Kinematics::NUM_JOINTS; joint++){
        const float tempDiff = newTemps[joint] - lastTemps[joint];
        if(newTemps[joint] >= HIGH_TEMP && tempDiff >= TEMP_THRESHOLD){
            cout << Thread::name << "::" << "TEMPERATURE-WARNING:"
                 << "\t" << Kinematics::JOINT_STRINGS[joint]
                 << " is at"
                 << newTemps[joint] <<" deg C"<<endl;
            if(newTemps[joint] >= REALLY_HIGH_TEMP){
                sayWarning = true;
            }
        }
    }
    if(sayWarning){
        playFile(heat_wav);
    }
    lastTemps = newTemps;
}

//HACK/NOTE: the sensors we rely on are only updated in the vision thread
// so they will often times be old
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
        registeredShutdown=false;
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

    if(buttonOnCounter > SHUTDOWN_THRESH && !registeredShutdown){
        registeredShutdown = true;
        executeShutdownAction();
    }

}

void RoboGuardian::executeClickAction(int nClicks){
#ifdef DEBUG_GUARDIAN_CLICKS
    cout << "Processing "<<numClicks<< " clicks"<<endl;
#endif
    //NOTE: Please upade wiki/NaoChestButtonInterface when this is changed!!!
    switch(nClicks){

    case 1:
        //Say IP Address
        speakIPAddress();
        break;
    case 2:
        shutoffGains();
        break;
    case 5:
        playFile(stiffness_enabled_wav);
        motion_interface->sendStiffness( new StiffnessCommand(
                                             StiffnessCommand::FULL_STIFFNESS));
        break;
    case 7:
        resetWifiConnection();
        break;
    case 9:
        //Easter EGG!
        playFile(nbsdir+"easter_egg.wav");
        break;
    default:
        //nothing
        cout << Thread::name <<" is leaving "<<nClicks<<" clicks for someone else"<<endl;
        pthread_mutex_lock(&click_mutex);
        numClicks = nClicks;
        pthread_mutex_unlock(&click_mutex);
        break;
    }

}


void RoboGuardian::executeStartupAction() const{
//Blank for now

}

void RoboGuardian::executeShutdownAction()const {
    cout << Thread::name<<" is shutting down the robot NOW!!!"<<endl;
    playFile(shutdown_wav);
    system("shutdown -h now &");
}

string RoboGuardian::getHostName()const {
    char name[40];
    name[0] ='\0';
    gethostname(name,39);
    return string(name);
}

void RoboGuardian::speakIPAddress()const {
    //Currently we poll the broker. If this breaks in the future
    //you can try to call /opt/naoqi/bin/ip.sh or
    //parse the output of if config yourself
    const string IP = broker->getIP();
    const string host = getHostName();

    string speech_command = sout;
    speech_command += " "+mynameis_wav;
    speech_command += " "+nbsdir+host+wav;
    speech_command += " "+my_address_is_wav;



    for (unsigned int i = 0; i < IP.size(); i++){
        char digit = IP[i];
        if(digit == dot[0])
            speech_command += " "+sdir+"point"+wav;
        else
            speech_command += " "+sdir+digit+wav;

    }
    speech_command += " &";
    cout <<Thread::name<<" is speaking: My name is "<<host
         << " my internet address is "
         <<IP<<endl;

    system(speech_command.c_str());
}


int RoboGuardian::getNumChestClicks() const{
    pthread_mutex_lock(&click_mutex);
    const int nClicks = numClicks;
    numClicks = NO_CLICKS;
    pthread_mutex_unlock(&click_mutex);
    return nClicks;
}


void RoboGuardian::resetWifiConnection(){
    playFile(wifi_restart_wav);
    system("/etc/init.d/wireless restart &");
}
