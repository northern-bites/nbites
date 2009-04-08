
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

#ifdef NAOQI1

#include <boost/shared_ptr.hpp>
#include "alerror.h"
#include "almotionproxy.h"
#include "almemoryproxy.h"
#include "alptr.h"
#include "albroker.h"
#include "alproxy.h"
#include "dcmproxy.h"
#include "almemoryfastaccess.h"

#include "motionconfig.h"
#include "Sensors.h"
#include "ThreadedMotionEnactor.h"
#include "MotionSwitchboard.h"
#include "Transcriber.h"

class ALEnactor : public ThreadedMotionEnactor {
public:
    ALEnactor(AL::ALPtr<AL::ALBroker> _pbroker, boost::shared_ptr<Sensors> s,
              boost::shared_ptr<Transcriber> t)
        : ThreadedMotionEnactor(), broker(_pbroker), sensors(s),
          transcriber(t){
        try{
            alfastaccess =
                AL::ALPtr<ALMemoryFastAccess >(new ALMemoryFastAccess());
        } catch(AL::ALError &e){
            cout << "Failed to initialize proxy to ALFastAccess"<<endl;
        }
        try {
            almemory = broker->getMemoryProxy();
        } catch(AL::ALError &e){
            cout << "Failed to initialize proxy to ALMemory" << endl;
        }
        try {
            almotion = broker->getMotionProxy();
        } catch(AL::ALError &e){
            cout << "Failed to initialize proxy to ALMotion" << endl;
        }
        try {
            dcm = AL::ALPtr<AL::DCMProxy>(new AL::DCMProxy(broker));
        } catch(AL::ALError &e) {
            cout << "Failed to initialize proxy to DCM" << endl;
        }
        //starting out we want to set our motion angles to the sensed position
        motionCommandAngles = almotion->getBodyAngles();
        sensors->setMotionBodyAngles(motionCommandAngles);
        sensors->setBodyAngles(motionCommandAngles);
#ifndef OFFLINE
        initSyncWithALMemory();
#endif
    };
    virtual ~ALEnactor() { };

    virtual void run();
    virtual void sendJoints();
    virtual void postSensors();

private:
    void syncWithALMemory();
    void initSyncWithALMemory();
private:
    AL::ALPtr<AL::ALBroker> broker;
    AL::ALPtr<AL::ALMotionProxy>  almotion;
    AL::ALPtr<AL::ALMemoryProxy>  almemory;
    AL::ALPtr<ALMemoryFastAccess> alfastaccess;
    AL::ALPtr<AL::DCMProxy> dcm;
    boost::shared_ptr<Sensors> sensors;
    boost::shared_ptr<Transcriber> transcriber;
    std::vector<float> motionCommandAngles;
    std::vector<float>  motionCommandStiffness;
    static const int MOTION_FRAME_RATE;
    static const float MOTION_FRAME_LENGTH_uS; // in microseconds
    static const float MOTION_FRAME_LENGTH_S; // in seconds
};

#endif //NAOQI1

#endif//_ALEnactor_h_DEFINED
