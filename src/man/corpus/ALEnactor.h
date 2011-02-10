
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

#ifndef _ALEnactor_h_DEFINED
#define _ALEnactor_h_DEFINED

#include <boost/shared_ptr.hpp>
#include "alcommon/albroker.h"
#include "alcore/alerror.h"
#include "alcore/alptr.h"
#include "almemoryfastaccess/almemoryfastaccess.h"
#include "alproxies/almemoryproxy.h"
#include "alproxies/almotionproxy.h"
#include "alcommon/alproxy.h"
#include "alproxies/dcmproxy.h"

#include "motionconfig.h"
#include "Sensors.h"
#include "ThreadedMotionEnactor.h"
#include "MotionSwitchboard.h"
#include "Transcriber.h"

using namespace std;

class ALEnactor : public ThreadedMotionEnactor {
public:
    ALEnactor(boost::shared_ptr<Sensors> s,
              boost::shared_ptr<Synchro> synchro,
              boost::shared_ptr<Transcriber> t,
              AL::ALPtr<AL::ALBroker> _pbroker )
        : ThreadedMotionEnactor(synchro,"ALEnactor"),
          broker(_pbroker), sensors(s),
          transcriber(t), almotion_link(false){

        try {
            almotion = broker->getMotionProxy();
            almotion_link = true;
        } catch(AL::ALError &e){
            cout << "Failed to initialize proxy to ALMotion!!!" << endl;
            cout << "Please turn on 'motion' in autoload.ini and restart naoqi"
                 << endl;
            almotion_link = false;
        }

    };
    virtual ~ALEnactor() { };

    void run();

    void sendCommands();
    void postSensors();

private:
    void sendJoints();
    void sendHardness();

private:
    AL::ALPtr<AL::ALBroker> broker;
    AL::ALPtr<AL::ALMotionProxy>  almotion;
    boost::shared_ptr<Sensors> sensors;
    boost::shared_ptr<Transcriber> transcriber;
    std::vector<float> motionCommandAngles;
    std::vector<float>  motionCommandStiffness;
    static const int MOTION_FRAME_RATE;
    static const float MOTION_FRAME_LENGTH_uS; // in microseconds
    static const float MOTION_FRAME_LENGTH_S; // in seconds

    bool almotion_link;
};

#endif//_ALEnactor_h_DEFINED
