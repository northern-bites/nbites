
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

#ifndef _ALTranscriber_h_DEFINED
#define _ALTranscriber_h_DEFINED

#include "boost/shared_ptr.hpp"

#include "alcommon/albroker.h"
#include "alcore/alptr.h"
#include "almemoryfastaccess/almemoryfastaccess.h"
#include "alproxies/almemoryproxy.h"

#include "Transcriber.h"
#include "Sensors.h"
#include "AccEKF.h"
/**
 *  This class servers to connect our Sensors class with AlMemory.
 *  It has different mechanisims which can be called
 *  from different threads in order to update sensors at different
 *  frequencies.
 *
 *  The advantage of this model is that similar code does not need to be
 *  duplicated accross Enactors, and we can pull more robot/platform specific
 *  code out of Man
 */


class ALTranscriber : public Transcriber
{
public:
    ALTranscriber(AL::ALPtr<AL::ALBroker> _broker,
                  boost::shared_ptr<Sensors> sensors);
    virtual ~ALTranscriber() {
		printf("ALTranscriber destructor\n");
	};

    virtual void postMotionSensors(){ syncMotionWithALMemory(); }
    virtual void postVisionSensors(){ syncVisionWithALMemory(); }

private: //Members
    AL::ALPtr<AL::ALBroker> broker;
    AL::ALPtr<AL::ALMemoryFastAccess> alfastaccessJoints;
    AL::ALPtr<AL::ALMemoryFastAccess> alfastaccessSensors;
    AL::ALPtr<AL::ALMemoryFastAccess> alfastaccessTemps;
    AL::ALPtr<AL::ALMemoryFastAccess> alfastaccessVision;

    std::vector<float> jointValues;

    AccEKF accelerationFilter;

    float lastAngleX, lastAngleY;
    float lastReadAngleX, lastReadAngleY;
private: //Helper Methods

    //For the motion thread (enactor)
    void initSyncMotionWithALMemory();
    void syncMotionWithALMemory();
    void initSensorBodyJoints();

    //For the vision thread (man)
    void initSyncVisionWithALMemory();
    void syncVisionWithALMemory();

    static const float calibrate_acc_x(const float x);
    static const float calibrate_acc_y(const float y);
    static const float calibrate_acc_z(const float z);
};
#endif
