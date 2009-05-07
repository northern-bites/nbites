
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

#ifndef _Man_h_DEFINED
#define _Man_h_DEFINED

#include <pthread.h>
#include <signal.h>

#include <boost/shared_ptr.hpp>

#include "manconfig.h"

#include "alxplatform.h"
#include "altools.h"
#include "albroker.h"
#include "almodule.h"
#include "alloggerproxy.h"
#include "almemoryproxy.h"
#include "almemoryfastaccess.h"
#include "alptr.h"

#include "dcmproxy.h"

#ifdef USE_DCM

#  if defined USE_DCM && defined MAN_IS_REMOTE
#    error "DCM not compatible with remote!!!"
#  endif

#include "NaoEnactor.h"
typedef NaoEnactor EnactorT;
#else
#include "ALEnactor.h"
typedef ALEnactor EnactorT;
#endif


#include "ALTranscriber.h"
#include "ImageSubscriber.h"
#include "ALImageTranscriber.h"
#include "Common.h"
#include "Profiler.h"
#include "Sensors.h"
#include "Comm.h"
#include "Vision.h"
#include "Noggin.h"
#include "Motion.h"
#include "NaoPose.h"
#include "synchro.h"
#include "RoboGuardian.h"
#include "PyRoboGuardian.h"
#include "PySensors.h"


/**
 * The Naoqi module to run our main Nao robot system.
 *
 * @author Jeremy R. Fishman
 * @author Bowdoin College Northern Bites
 */
class Man : public ImageSubscriber
{
public:

    // contructors
    Man(AL::ALPtr<AL::ALBroker> pBroker, std::string pName);

    // destructor
    virtual ~Man();

    //
    // ALModule methods
    //

    //
    // Our methods
    //

    // Profiling methods
    void startProfiling(int nframes) {
       profiler->reset();
       profiler->profileFrames(nframes);
    }
    void stopProfiling() {
       profiler->profiling = false;
    }

    // start/stop called by manmodule
    void manStart();
    void manStop();

private:
    // run Vision and call Noggin's main loop function
    void processFrame(void);

    void notifyNextVisionImage();

  //
  // Variables
  //
public:
    boost::shared_ptr<Synchro> synchro;
    // Sub-module instances
    // ** ORDER MATTERS HERE **
    //   if the modules are not instantiated in this order, some dependedcies
    //   (i.e. the Python modules exported) will not be available by the time
    //   other modules are imported
    boost::shared_ptr<Profiler> profiler;
    boost::shared_ptr<Sensors> sensors;
    boost::shared_ptr<Transcriber> transcriber;
    boost::shared_ptr<ALImageTranscriber> imageTranscriber;
    boost::shared_ptr<NaoPose> pose;
#ifdef USE_MOTION
    boost::shared_ptr<EnactorT> enactor;
    boost::shared_ptr<Motion<EnactorT> > motion;
    boost::shared_ptr<RoboGuardian> guardian;
#endif
    boost::shared_ptr<Vision> vision;
    boost::shared_ptr<Comm> comm;
#ifdef USE_NOGGIN
    boost::shared_ptr<Noggin> noggin;
#endif// USE_NOGGIN

};


#endif
