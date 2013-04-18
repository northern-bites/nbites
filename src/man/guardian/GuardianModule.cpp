#include "GuardianModule.h"

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <google/protobuf/descriptor.h>

#include "SoundPaths.h"
#include "Profiler.h"

//#define DEBUG_GUARDIAN_FALLING

namespace man{
namespace guardian{

static const float FALL_SPEED_THRESH = 0.03f; //rads/20ms
static const float NOFALL_SPEED_THRESH = 0.01f; //rads/20ms
static const int FALLING_FRAMES_THRESH = 3;
static const int FALLING_RESET_FRAMES_THRESH = 10;
static const float FALLING_ANGLE_THRESH = M_PI_FLOAT/5.0f; //36.0 degrees
static const float FALLEN_ANGLE_THRESH = M_PI_FLOAT/3.0f; //72 degrees

const int GuardianModule::NO_CLICKS = -1;

GuardianModule::GuardianModule()
    : portals::Module(),
      stiffnessControlOutput(base()),
      initialStateOutput(base()),
      advanceStateOutput(base()),
      printJointsOutput(base()),
      switchTeamOutput(base()),
      switchKickOffOutput(base()),
      feetOnGroundOutput(base()),
      fallStatusOutput(base()),
      audioOutput(base()),
      chestButton( new ClickableButton(GUARDIAN_FRAME_RATE) ),
      leftFootButton( new ClickableButton(GUARDIAN_FRAME_RATE) ),
      rightFootButton( new ClickableButton(GUARDIAN_FRAME_RATE) ),
      useFallProtection(true),
      audioQueue()
{
}

GuardianModule::~GuardianModule()
{
}

void GuardianModule::run_()
{
    PROF_ENTER(P_ROBOGUARDIAN);

    temperaturesInput.latch();
    chestButtonInput.latch();
    footBumperInput.latch();
    inertialInput.latch();
    fsrInput.latch();
    batteryInput.latch();

    countButtonPushes();
    checkFalling();
    checkFallen();
    checkFeetOnGround();
    checkBatteryLevels();
    checkTemperatures();
    processFallingProtection();
    processChestButtonPushes();
    processFootBumperPushes();
    checkAudio();
    frameCount++;
    PROF_EXIT(P_ROBOGUARDIAN);
}

void GuardianModule::countButtonPushes()
{
    chestButton->updateFrame(chestButtonInput.message().pressed());
    leftFootButton->updateFrame( footBumperInput.message().l_foot_bumper_left() .pressed() ||
                                 footBumperInput.message().l_foot_bumper_right().pressed()   );
    rightFootButton->updateFrame(footBumperInput.message().r_foot_bumper_left() .pressed() ||
                                 footBumperInput.message().r_foot_bumper_right().pressed()   );
}

/**
 * Method to watch the robots rotation and detect falls
 * publishes the result of this computation to other classes.
 *
 * Also builds in the ability to shutoff gains when a fall is detected,
 * but this still has some problems. Mainly, it sometimes triggers a fall
 * when the robot is rotated back up again, but only when the robot is
 * 'over rotated' during the righting.  This feature can be enabled
 * by calling 'enableFallProtection'.
 * Also, currently the robot will print whenever it believes it is in
 * the process of falling. This will allow us to monitor how well this code
 * works.
 *
 */
void GuardianModule::checkFalling()
{
    if (!useFallProtection)
    {
        return;
    }

    struct Inertial inertial = {inertialInput.message().angle_x(),
                                 inertialInput.message().angle_y() };

    /***** Determine if the robot is in the process of FALLING ****/
    //Using just the magnitude:
    const float angleMag = sqrtf(std::pow(inertial.angleX,2) +
                                 std::pow(inertial.angleY,2));
    const float lastAngleMag = sqrtf(std::pow(lastInertial.angleX,2) +
                                     std::pow(lastInertial.angleY,2));

    const float angleSpeed = angleMag - lastAngleMag;
    const bool falling_critical_angle = angleMag > FALLING_ANGLE_THRESH;

    if(isFalling(angleMag, angleSpeed))
    {
        // If falling, increment falling frames counter.
        fallingFrames += 1;
        notFallingFrames = 0;
    }
    else
    {
        // Otherwise, not falling.
        fallingFrames = 0;
        notFallingFrames += 1;
    }

    // if(angleMag >= FALLING_ANGLE_THRESH)
    // {
    //     std::cout << "angleSpeed " << angleSpeed
    //               << " and angleMag" << angleMag << std::endl
    //               << "  fallingFrames is " << fallingFrames
    //               << "  notFallingFrames is " << notFallingFrames
    //               << "  and critical angle is " << falling_critical_angle
    //               << std::endl;
    // }

    //If the robot has been falling for a while, and the robot is inclined
    //already at a 45 degree angle, than we know we are falling
    if (fallingFrames > FALLING_FRAMES_THRESH)
    {
        // When falling, execute the fall protection method.
        //cout << "GuardianModule::checkFalling() : FALLING!" << endl;
        falling = true;
        //processFallingProtection(); // Should be called later in run_()
    }
    else if(notFallingFrames > FALLING_FRAMES_THRESH)
    {
        falling = false;
    }

    // To calculate the angular speed of the fall next time.
    lastInertial  = inertial;
}

//Check if the angle is unstable, (ie tending AWAY from zero)
bool GuardianModule::isFalling(float angle_pos, float angle_vel)
{
    // Test falling based on angle (note that angle_pos is assumed to be
    // the mag. of the angle).
    if (angle_pos >= FALLING_ANGLE_THRESH)
    {
        // std::cout << "GuardianModule::isFalling() : angle_pos == " << angle_pos
        //           << ", angle_vel == " << angle_vel << std::endl;
        return true;
    }
    else if(angle_vel > FALL_SPEED_THRESH)
    {
            return true;
    }
    return false;
}

void GuardianModule::checkFallen()
{
    if (!useFallProtection)
        return;

    struct Inertial inertial = {inertialInput.message().angle_x(),
                                 inertialInput.message().angle_y() };

    /***** Determine if the robot has FALLEN OVER *****/
    const bool fallen_now =
        std::abs(inertial.angleX) > FALLEN_ANGLE_THRESH ||
        std::abs(inertial.angleY) > FALLEN_ANGLE_THRESH;

    //cout << inertial.angleX << " " <<  inertial.angleY << endl;

    if(fallen_now)
        fallenCounter +=1;
    else
        fallenCounter = 0;

    static const int FALLEN_FRAMES_THRESH = 2;

    fallen = fallenCounter > FALLEN_FRAMES_THRESH;

#ifdef DEBUG_GUARDIAN_FALLING
    if (fallen)
        std::cout << "Robot has fallen" << std::endl;
#endif
}

/**
 * Method to check whether or not one of the robot's feet is on the
 * ground. This is used to stop the motion engine (primarily) when we
 * pick the robot up.
 *
 * We sum up the value of all the FSRs on both feet, and check the sum
 * against a threshold. If the sum is under this threshold for more than
 * GROUND_FRAMES_THRESH then we set feetOnGround=false
 *
 */

void GuardianModule::checkFeetOnGround()
{
    //this can be higher than the falling thresholds since stopping the walk
    //engine is less critical
    static const int GROUND_FRAMES_THRESH = 10;
    // lower pthan this, the robot is off the ground
    static const float onGroundFSRThresh = 1.0f;

    /* If the FSRs are broken, we don't want to accidentally assume that we're
       off the ground (ruins SweetMoves, walking, etc) so this method will stop
       early and feetOnGround will always be true */
    // @TODO currently no check for this. May want to incorperate
    // error detection in Sensors Module. March 2, 2013
    // if (sensors->percentBrokenFSR() > 0)
    // {
    //     feetOnGround = true;
    //     return;
    // }


    float leftSum  = fsrInput.message().lfl() +
        fsrInput.message().lfr() +
        fsrInput.message().lrl() +
        fsrInput.message().lrr();
    float rightSum = fsrInput.message().rfl() +
        fsrInput.message().rfr() +
        fsrInput.message().rrl() +
        fsrInput.message().rrr();


    // printf("left: %f, right: %f, total: %f\n",
    //        leftSum, rightSum, (leftSum + rightSum));

    // buffer the transition in both directions
    if (feetOnGround)
    {
        if (leftSum + rightSum < onGroundFSRThresh)
        {
            groundOffCounter++;
        }
        else
        {
            groundOffCounter = 0;
        }
    }
    else
    {
        if (leftSum + rightSum > onGroundFSRThresh)
        {
            groundOnCounter++;
        }
        else
        {
            groundOnCounter = 0;
        }
    }

    if (groundOffCounter > GROUND_FRAMES_THRESH)
    {
        feetOnGround = false;
        groundOnCounter = groundOffCounter = 0;

        portals::Message<messages::FeetOnGround> msg(0);
        msg.get()->set_on_ground(false);
        feetOnGroundOutput.setMessage(msg);
    }
    else if (groundOnCounter > GROUND_FRAMES_THRESH)
    {
        feetOnGround = true;
        groundOnCounter = groundOffCounter = 0;

        portals::Message<messages::FeetOnGround> msg(0);
        msg.get()->set_on_ground(true);
        feetOnGroundOutput.setMessage(msg);
    }
}

// We print each time the battery looses ten percent of charge
//once the charge gets to %30 percent of capacity, we start to say "energy"
void GuardianModule::checkBatteryLevels()
{
    //Constants on 0-100 scale, sensor reading is on 0-1.0 scale
    static const float LOW_BATTERY_VALUE = 30.0f;
    static const float EMPTY_BATTERY_VALUE = 10.0f; //start nagging below 10%

    const float newBatteryCharge = batteryInput.message().charge();
    if(newBatteryCharge < 0 || newBatteryCharge > 1.0)
    {
        std::cout << "Guardian:: Somehow getting battery current instead..."
                  << std::endl;
        return;
    }

    if(newBatteryCharge != lastBatteryCharge)
    {
        //covert to a % scale
        const float newLevel = floorf(newBatteryCharge*100.0f);
        const float oldLevel = floorf(lastBatteryCharge*100.0f);
        if(oldLevel != newLevel && oldLevel > newLevel &&
           oldLevel - newLevel >= 10.0f)
        {
            std::cout << "Guardian:: Battery charge is now at "
                      << 10.0f * newBatteryCharge
                      << " (was "<< oldLevel <<")"<< std::endl;

            if (newLevel <= EMPTY_BATTERY_VALUE)
            {
                std::cout << "Guardian:: Battery charge is critically "
                          << "low!! PLUG ME IN!!!!!!!!!" << std::endl;
                playFile(energy_wav);
            }
            else if(newLevel <= LOW_BATTERY_VALUE)
            {
                playFile(energy_wav);
            }
        }
    }
    lastBatteryCharge = newBatteryCharge;
}

void GuardianModule::checkTemperatures()
{
    static const float HIGH_TEMP = 40.0f; //deg C
    static const float TEMP_THRESHOLD = 1.0f; //deg C
    static const float REALLY_HIGH_TEMP = 50.0f; //deg C

    std::vector<float> newTemps = vectorizeTemperatures(temperaturesInput.message());

    if (frameCount == 0)
    {
        lastTemps = newTemps;
        return;
    }

    bool sayWarning = false;
    for(int joint = 0;
        joint < temperaturesInput.message().GetDescriptor()->field_count();
        joint++)
    {
        const float tempDiff = newTemps[joint] - lastTemps[joint];
        if(newTemps[joint] >= HIGH_TEMP && tempDiff >= TEMP_THRESHOLD &&
           process_micro_time() - lastHeatPrintWarning > TIME_BETWEEN_HEAT_WARNINGS)
        {
            lastHeatPrintWarning = process_micro_time();
            std::cout << "Guardian:: TEMP-WARNING: "
                      << temperaturesInput.message().GetDescriptor()->field(joint)->name()
                      << " is at " << std::setprecision(1)
                      << newTemps[joint] <<" deg C"<< std::setprecision(6)
                      << std::endl;
            if(newTemps[joint] >= REALLY_HIGH_TEMP)
            {
                sayWarning = true;
            }
        }
    }
    if(sayWarning &&
       process_micro_time() - lastHeatAudioWarning > TIME_BETWEEN_HEAT_WARNINGS)
    {
        playFile(heat_wav);
        lastHeatAudioWarning = process_micro_time();
    }

    lastTemps = newTemps;
}

std::vector<float> GuardianModule::vectorizeTemperatures(const messages::JointAngles& temps)
{
    std::vector<float> result;

    result.push_back(temps.head_yaw());
    result.push_back(temps.head_pitch());

    result.push_back(temps.l_shoulder_pitch());
    result.push_back(temps.l_shoulder_roll());
    result.push_back(temps.l_elbow_yaw());
    result.push_back(temps.l_elbow_roll());
    result.push_back(temps.l_wrist_yaw());
    result.push_back(temps.l_hand());

    result.push_back(temps.r_shoulder_pitch());
    result.push_back(temps.r_shoulder_roll());
    result.push_back(temps.r_elbow_yaw());
    result.push_back(temps.r_elbow_roll());
    result.push_back(temps.r_wrist_yaw());
    result.push_back(temps.r_hand());

    result.push_back(temps.l_hip_yaw_pitch());
    result.push_back(temps.r_hip_yaw_pitch());

    result.push_back(temps.l_hip_roll());
    result.push_back(temps.l_hip_pitch());
    result.push_back(temps.l_knee_pitch());
    result.push_back(temps.l_ankle_pitch());
    result.push_back(temps.l_ankle_roll());

    result.push_back(temps.r_hip_roll());
    result.push_back(temps.r_hip_pitch());
    result.push_back(temps.r_knee_pitch());
    result.push_back(temps.r_ankle_pitch());
    result.push_back(temps.r_ankle_roll());

    return result;
}

void GuardianModule::processFallingProtection()
{
    portals::Message<messages::FallStatus> status(0);
    if(useFallProtection && falling)
    {
        if (!registeredFalling)
        {
            std::cout << "Guardian: OH NO! I'm falling."
                      << " Removing stiffness." << std::endl;
#ifdef DEBUG_GUARDIAN_FALLING
            playFile(falling_wav);
#endif
        }

        registeredFalling = true;

        status.get()->set_falling(true);
    }
    else if (registeredFalling && notFallingFrames > FALLING_RESET_FRAMES_THRESH)
    {
        registeredFalling = false;

        status.get()->set_falling(false);
    }
    if (fallen)
    {
        status.get()->set_fallen(true);
        if (inertialInput.message().angle_y() > 0)
        {
            status.get()->set_on_front(true);
        }
        else
        {
            status.get()->set_on_front(false);
        }
    }
    else
    {
        status.get()->set_fallen(false);
    }

    fallStatusOutput.setMessage(status);

//     if(fallingFrames == FALLING_FRAMES_THRESH && falling_critical_angle)
//     {
//         if(useFallProtection)
//         {
//             shutoffGains();
//             std::cout << "Disabling Gains!!! due to falling" << std::endl;
//         }
// #ifdef DEBUG_GUARDIAN_FALLING
//         std::cout <<"Guardian: OH NO! I think I'm falling" << std::endl;
//         playFile(falling_wav);
// #endif
//     }

}

void GuardianModule::processChestButtonPushes()
{
    static const int SHUTDOWN_THRESH = 3;

    //Then, process our actions locally
    if(chestButton->getClickLength() == 0.0f )
    {
        registeredShutdown  = false;
    }
    else if(chestButton->getClickLength() > SHUTDOWN_THRESH &&
            !registeredShutdown)
    {
        registeredShutdown = true;
        executeShutdownAction();
    }

    if(executeChestClickAction(chestButton->peekNumClicks()))
    {
        chestButton->getAndClearNumClicks();
    }
}

void GuardianModule::processFootBumperPushes()
{
    if(executeLeftFootClickAction(leftFootButton->peekNumClicks()))
    {
        leftFootButton->getAndClearNumClicks();
    }

    if(executeRightFootClickAction(rightFootButton->peekNumClicks()))
    {
        rightFootButton->getAndClearNumClicks();
    }
}

void GuardianModule::executeShutdownAction()
{
    std::cout << "Guardian is attempting a shutdown..."<< std::endl;
    playFile(shutdown_wav);
    if(system("sudo shutdown -h now &") != 0)
        std::cout << "Roboguardian could not shutdown system." << std::endl;
}

bool GuardianModule::executeChestClickAction(int nClicks)
{
    switch(nClicks)
    {
    case NO_CLICKS:
        return false;
        break;
    case 1:
        advanceState();
        break;
    case 2:
        shutoffGains();
        break;
    case 3:
        enableGains();
        break;
    case 4:
        initialState();
        break;
    case 5:
        printJointAngles();
        break;
    case 7:
        break;
    case 9:
        //Easter EGG!
        playFile(nbsdir+"easter_egg.wav");
        break;
    default:
        //nothing
        return false;
        break;
    }
    return true;
}

bool GuardianModule::executeLeftFootClickAction(int nClicks)
{
    switch(nClicks)
    {
    case NO_CLICKS:
        return false;
        break;
    case 1:
        switchTeams();
        break;
    default:
        return false;
        break;
    }
    return true;
}

bool GuardianModule::executeRightFootClickAction(int nClicks)
{
    switch(nClicks)
    {
    case NO_CLICKS:
        return false;
        break;
    case 1:
        switchKickOff();
        break;
    default:
        return false;
        break;
    }
    return true;
}

void GuardianModule::shutoffGains(){
    std::cout << "Guardian::shutoffGains()" << std::endl;

    portals::Message<messages::StiffnessControl> command(0);
    command.get()->set_remove(true);
    stiffnessControlOutput.setMessage(command);

    playFile(stiffness_removed_wav);
}

void GuardianModule::enableGains()
{
    std::cout << "Guardian::enableGains()" << std::endl;

    portals::Message<messages::StiffnessControl> command(0);
    command.get()->set_remove(false);
    stiffnessControlOutput.setMessage(command);

    playFile(stiffness_enabled_wav);
}

void GuardianModule::initialState()
{
    std::cout << "Guardian::initialState()" << std::endl;

    portals::Message<messages::Toggle> command(0);
    command.get()->set_toggle(!lastInitial);
    initialStateOutput.setMessage(command);

    lastInitial = !lastInitial;
}

void GuardianModule::advanceState()
{
    portals::Message<messages::Toggle> command(0);
    command.get()->set_toggle(!lastAdvance);
    advanceStateOutput.setMessage(command);

    lastAdvance = !lastAdvance;
}

void GuardianModule::printJointAngles()
{
    portals::Message<messages::Toggle> command(0);
    command.get()->set_toggle(!lastPrint);
    printJointsOutput.setMessage(command);

    lastPrint = !lastPrint;
}

void GuardianModule::switchTeams()
{
    portals::Message<messages::Toggle> command(0);
    command.get()->set_toggle(!lastTeamSwitch);
    switchTeamOutput.setMessage(command);

    lastTeamSwitch = !lastTeamSwitch;
}

void GuardianModule::switchKickOff()
{
    portals::Message<messages::Toggle> command(0);
    command.get()->set_toggle(!lastKickOffSwitch);
    switchKickOffOutput.setMessage(command);

    lastKickOffSwitch = !lastKickOffSwitch;
}

void GuardianModule::reloadMan()
{
}


void GuardianModule::playFile(std::string str)
{
    audioQueue.push(str);
}

void GuardianModule::checkAudio()
{
    if (audioQueue.empty())
    {
        portals::Message<messages::AudioCommand> command(0);
        audioOutput.setMessage(command);
    }
    else
    {
        std::string str = audioQueue.front();
        portals::Message<messages::AudioCommand> command(0);
        command.get()->set_audio_file(str);
        audioOutput.setMessage(command);
        audioQueue.pop();
    }
}

}
}
