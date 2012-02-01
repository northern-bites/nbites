
// This file is part of Man, a robotic perception, locomotion, and
// team strategy application created by the Northern Bites RoboCup
// team of Bowdoin College in Brunswick, Maine, for the Aldebaran
// Nao robot.
//
// Man is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Man is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.

#include "ALEnactor.h"

#include <iostream>
#include <boost/assign/std/vector.hpp>
using namespace boost::assign;
using namespace AL;

#include "Kinematics.h"
using namespace Kinematics;
using namespace std;

//#define SPEEDY_ENACTOR

void ALEnactor::run() {

    long long currentTime;
    struct timespec interval, remainder;
    while (running) {
        currentTime = process_micro_time();
            sendCommands();
            //Once we've sent the most calculated joints
            postSensors();

        const long long zero = 0;
        const long long processTime = process_micro_time() - currentTime;

#if ! defined OFFLINE || ! defined SPEEDY_ENACTOR
        if (processTime > MOTION_FRAME_LENGTH_uS){
            std::cout << "Time spent in ALEnactor longer than frame length: "
                      << processTime << std::endl;
            //Don't sleep at all
        } else{
            interval.tv_sec = 0;
            interval.tv_nsec = static_cast<long int>(
                static_cast<long long int>(MOTION_FRAME_LENGTH_uS)
                - processTime);
            nanosleep(&interval,&remainder);
        }
#endif

    }
}
void ALEnactor::sendCommands(){
    if(!switchboard)
        return;
    if(!almotion_link){
        return;
    }
    sendJoints();
    sendHardness();
}

void ALEnactor::sendJoints(){
    // Get the angles we want to go to this frame from the switchboard
    motionCommandAngles = switchboard->getNextJoints();

#ifdef DEBUG_ENACTOR_JOINTS
    for (unsigned int i=0; i<motionCommandAngles.size();i++)
        cout << "result of joint " << i << " is "
             << motionCommandAngles.at(i) << endl;
#endif

#ifndef NO_ACTUAL_MOTION
    //almotion->setAngles(motionCommandAngles);
#endif

}

void ALEnactor::sendHardness(){
    //Get the hardness we need to send on to lower level
    motionCommandStiffness = switchboard->getNextStiffness();

    //NOTE: in AL Enactor, we set each joint stiffness individually - this is
    //      probably quite slow
    for(unsigned int joint = 0; joint < NUM_JOINTS; joint ++){

        const float chainStiffness = motionCommandStiffness[joint];
        const std::string name = JOINT_STRINGS[joint];
#ifndef NO_ACTUAL_MOTION
        //almotion->setStiffness(name ,chainStiffness);
#endif
    }
}

void ALEnactor::postSensors() {
    if(!switchboard)
        return;
    //Each frame, we need to store which commmands were sent,
    //also, we need to ask the transcriber to copy
    //the relevant information out of ALMemory into sensors
    sensors->setMotionBodyAngles(motionCommandAngles);

    transcriber->postMotionSensors();
    switchboard->signalNextFrame();
}

