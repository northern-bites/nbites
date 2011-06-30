
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


#include "MotionEnactor.h"
#include "ImageSubscriber.h"
#include "ImageTranscriber.h"
#include "Transcriber.h"
#include "Lights.h"
#include "Speech.h"
#include "Common.h"
#include "Profiler.h"
#include "Sensors.h"
#include "Vision.h"
#include "Noggin.h"
#include "Comm.h"
#include "Motion.h"
#include "NaoPose.h"
#include "memory/Memory.h"
#include "memory/log/LoggingBoard.h"
#include "synchro.h"
#include "RoboGuardian.h"

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
    Man(boost::shared_ptr<Profiler> _profiler,
        boost::shared_ptr<Sensors> _sensors,
        boost::shared_ptr<Transcriber> _transcriber,
        boost::shared_ptr<ImageTranscriber> _imageTranscriber,
        boost::shared_ptr<MotionEnactor> _enactor,
        boost::shared_ptr<Synchro> synchro,
        boost::shared_ptr<Lights> _lights,
        boost::shared_ptr<Speech> _speech);
    // destructor
    virtual ~Man();

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
    virtual void startSubThreads();
    virtual void stopSubThreads();

private:
    // run Vision and call Noggin's main loop function
    void processFrame(void);

    void notifyNextVisionImage();

  //
  // Variables
  //
public:
    boost::shared_ptr<Profiler> profiler;
    boost::shared_ptr<Sensors> sensors;
    boost::shared_ptr<Transcriber> transcriber;
    boost::shared_ptr<ImageTranscriber> imageTranscriber;
    boost::shared_ptr<MotionEnactor> enactor;
    boost::shared_ptr<RoboGuardian> guardian;

    //boost::shared_ptr<Synchro> synchro;
    // Sub-module instances
    // ** ORDER MATTERS HERE **
    //   if the modules are not instantiated in this order, some dependedcies
    //   (i.e. the Python modules exported) will not be available by the time
    //   other modules are imported
    boost::shared_ptr<NaoPose> pose;
#ifdef USE_MOTION
    boost::shared_ptr<Motion> motion;
#endif
    boost::shared_ptr<Vision> vision;
    boost::shared_ptr<Comm> comm;
    boost::shared_ptr<man::memory::Memory> memory;
    boost::shared_ptr<man::memory::log::LoggingBoard> loggingBoard;

#ifdef USE_NOGGIN
    boost::shared_ptr<Noggin> noggin;
#endif// USE_NOGGIN
    boost::shared_ptr<Lights> lights;
    boost::shared_ptr<Speech> speech;

};


#endif
