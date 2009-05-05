
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

#include <iostream>
#include <sstream>
#include <time.h>
#include <sys/time.h>
#include <Python.h>
#include <boost/shared_ptr.hpp>
#include <boost/assign/std/vector.hpp>
using namespace boost::assign;

#include "alvisionimage.h"
#include "alvisiondefinitions.h"

#include "Man.h"
#include "manconfig.h"
#include "corpus/synchro.h"
#include "VisionDef.h"
#include "Common.h"
#include "_ledsmodule.h"

using namespace std;
using namespace AL;
using boost::shared_ptr;


/////////////////////////////////////////
//                                     //
//  Module class function definitions  //
//                                     //
/////////////////////////////////////////

Man::Man (ALPtr<ALBroker> pBroker, std::string pName)
    : ALModule(pBroker,pName),
    // this is not good usage of shared_ptr...  oh well
    Thread(shared_ptr<Synchro>(new Synchro()), "Man"),
    // call the default constructor of all the shared pointers
    log(), camera(), lem(), 
    lem_name(""),
    camera_active(false)
{
    // open lems
    initMan();

    // initialize system helper modules
    profiler = shared_ptr<Profiler>(new Profiler(&micro_time));
    //messaging = shared_ptr<Messenger>(new Messenger());
    sensors = shared_ptr<Sensors>(new Sensors());
    // give python a pointer to the sensors structure. Method defined in
    // Sensors.h
    set_sensors_pointer(sensors);

    setLedsProxy(AL::ALPtr<AL::ALLedsProxy>(new AL::ALLedsProxy(pBroker)));

    transcriber = shared_ptr<Transcriber>(new ALTranscriber(pBroker, sensors));
    imageTranscriber =
        shared_ptr<ImageTranscriber>(new ImageTranscriber(sensors));
    imageTranscriber->setNewImageCallback(
        &ImageSubscriber::notifyNextVisionImage);

    pose = shared_ptr<NaoPose>(new NaoPose(sensors));

    // initialize core processing modules
#ifdef USE_MOTION
    enactor = shared_ptr<EnactorT>(new EnactorT(pBroker, sensors,
                                       transcriber));
    motion = shared_ptr<Motion<EnactorT> >(
        new Motion<EnactorT>(synchro, enactor, sensors));
    guardian = shared_ptr<RoboGuardian>(
        new RoboGuardian(synchro,sensors, pBroker, motion->getInterface()));

    // give python a pointer to the guardian. Method defined in PyRoboguardian.h
    set_guardian_pointer(guardian);
    // initialize python roboguardian module.
#endif
    vision = shared_ptr<Vision>(new Vision(pose, profiler));
    comm = shared_ptr<Comm>(new Comm(synchro, sensors, vision));
#ifdef USE_NOGGIN
    noggin = shared_ptr<Noggin>(new Noggin(profiler, vision,comm, guardian,
                                           motion->getInterface()));
#endif// USE_NOGGIN
}

Man::~Man ()
{
    // stop vision processing, comm, and motion
    Thread::stop();

    // unregister lem
    closeMan();
}

void
Man::initMan()
{
#ifdef DEBUG_MAN_INITIALIZATION
    printf("Man::initializing\n");
    printf("  Binding functions\n");
#endif

    // Describe the module here
    setModuleDescription("Nao robotic soccer player");

    // Define callable methods with there description
    functionName("start", "Man", "Begin environment processing");
    BIND_METHOD(Man::manStart);

    functionName("stop", "Man", "Halt environment processing");
    BIND_METHOD(Man::manStop);

    functionName("trigger_await_on", "Man", "Wait for Man to start");
    BIND_METHOD(Man::manAwaitOn);

    functionName("trigger_await_off", "Man", "Wait for Man to stop");
    BIND_METHOD(Man::manAwaitOff);

    functionName("startProfiling", "Man", "Start vision frame profiling, "
                 "for given number of frames");
    BIND_METHOD(Man::startProfiling);

    functionName("stopProfiling", "Man", "Stop vision frame profiling");
    BIND_METHOD(Man::stopProfiling);

#ifdef DEBUG_MAN_INITIALIZATION
    printf("  Opening proxies\n");
#endif
    try {
        log = getParentBroker()->getLoggerProxy();
        // Possible values are
        // lowDebug, debug, lowInfo, info, warning, error, fatal
        log->setVerbosity("lowDebug");
    }catch (ALError &e) {
        std::cerr << "Could not create a proxy to ALLogger module" << std::endl;
    }

    // initialize ALMemory for access to stuff like bumpers, etc

#ifdef USE_VISION
    registerCamera();
    if(camera_active){
        //initCameraSettings(TOP_CAMERA);
        initCameraSettings(BOTTOM_CAMERA);
    }
#endif

#ifdef DEBUG_MAN_INITIALIZATION
    printf("  DONE!\n");
#endif
}

#ifdef USE_VISION

void
Man::registerCamera() {
    try {
        camera = getParentBroker()->getProxy("NaoCam");
        camera_active =true;
    }catch (ALError &e) {
        log->error("Man", "Could not create a proxy to NaoCam module");
        camera_active =false;
        return;
    }

    lem_name = "Man_LEM";
    int format = NAO_IMAGE_SIZE;
    int colorSpace = NAO_COLOR_SPACE;
    int fps = DEFAULT_CAMERA_FRAMERATE;

    int resolution = format;



#ifdef DEBUG_MAN_INITIALIZATION
    printf("  Registering LEM with format=%i colorSpace=%i fps=%i\n", format,
           colorSpace, fps);
#endif

    try {
        lem_name = camera->call<std::string>("register", lem_name, format,
                                             colorSpace, fps);
        cout << "Registered Camera: " << lem_name << " successfully"<<endl;
    } catch (ALError &e) {
        cout << "Failed to register camera" << lem_name << endl;
        camera_active = false;
//         SleepMs(500);

//         try {
//             printf("LEM failed once, trying again\n");
//             lem_name = camera->call<std::string>("register", lem_name, format,
//                                                  colorSpace, fps);
//         }catch (ALError &e2) {
//             log->error("Man", "Could not call the register method of the NaoCam "
//                        "module\n" + e2.toString());
//             return;
//         }
    }

}

void
Man::initCameraSettings(int whichCam){

    int currentCam =  camera->call<int>( "getParam", kCameraSelectID );
    if (whichCam != currentCam){
        camera->callVoid( "setParam", kCameraSelectID,whichCam);
        SleepMs(CAMERA_SLEEP_TIME);
        currentCam =  camera->call<int>( "getParam", kCameraSelectID );
        if (whichCam != currentCam){
            cout << "Failed to switch to camera "<<whichCam
                 <<" retry in " << CAMERA_SLEEP_TIME <<" ms" <<endl;
            SleepMs(CAMERA_SLEEP_TIME);
            currentCam =  camera->call<int>( "getParam", kCameraSelectID );
            if (whichCam != currentCam){
                cout << "Failed to switch to camera "<<whichCam
                     <<" ... returning, no parameters initialized" <<endl;
                return;
            }
        }
        cout << "Switched to camera " << whichCam <<" successfully"<<endl;
    }

    // Turn off auto settings
    // Auto exposure
    try {
        camera->callVoid("setParam", kCameraAutoExpositionID,
                         DEFAULT_CAMERA_AUTO_EXPOSITION);
    } catch (ALError &e){
        log->error("Man", "Couldn't set AutoExposition");
    }
	int param = camera->call<int>("getParam", kCameraAutoExpositionID);
	// if that didn't work, then try again
	if (param != DEFAULT_CAMERA_AUTO_EXPOSITION) {
		try {
			camera->callVoid("setParam", kCameraAutoExpositionID,
							 DEFAULT_CAMERA_AUTO_EXPOSITION);
		} catch (ALError &e){
			log->error("Man", "Couldn't set AutoExposition AGAIN");
		}
	}
    // Auto white balance
    try {
        camera->callVoid("setParam", kCameraAutoWhiteBalanceID,
                         DEFAULT_CAMERA_AUTO_WHITEBALANCE);

    } catch (ALError &e){
        log->error("Man", "Couldn't set AutoWhiteBalance");
    }
	param = camera->call<int>("getParam", kCameraAutoWhiteBalanceID);
	if (param != DEFAULT_CAMERA_AUTO_WHITEBALANCE) {
		try {
			camera->callVoid("setParam", kCameraAutoWhiteBalanceID,
							 DEFAULT_CAMERA_AUTO_WHITEBALANCE);
		} catch (ALError &e){
			log->error("Man", "Couldn't set AutoWhiteBalance AGAIN");
		}
	}
    // Auto gain
    try {
        camera->callVoid("setParam", kCameraAutoGainID,
                         DEFAULT_CAMERA_AUTO_GAIN);
    } catch (ALError &e){
        log->error("Man", "Couldn't set AutoGain");
    }
	param = camera->call<int>("getParam", kCameraAutoGainID);
	if (param != DEFAULT_CAMERA_AUTO_GAIN) {
		try {
			camera->callVoid("setParam", kCameraAutoGainID,
							 DEFAULT_CAMERA_AUTO_GAIN);
		} catch (ALError &e){
			log->error("Man", "Couldn't set AutoGain AGAIN");
		}
	}
    // Set camera defaults
    // brightness
    try {
        camera->callVoid("setParam", kCameraBrightnessID,
                         DEFAULT_CAMERA_BRIGHTNESS);
    } catch (ALError &e){
        log->error("Man", "Couldn't set Brightness ");
    }
	param = camera->call<int>("getParam", kCameraBrightnessID);
	if (param != DEFAULT_CAMERA_BRIGHTNESS) {
		try {
			camera->callVoid("setParam", kCameraBrightnessID,
							 DEFAULT_CAMERA_BRIGHTNESS);
		} catch (ALError &e){
			log->error("Man", "Couldn't set BRIGHTNESS AGAIN");
		}
	}
    // contrast
    try {
        camera->callVoid("setParam", kCameraContrastID,
                         DEFAULT_CAMERA_CONTRAST);
    } catch (ALError &e){
        log->error("Man", "Couldn't set Contrast");
    }
	param = camera->call<int>("getParam", kCameraContrastID);
	if (param != DEFAULT_CAMERA_CONTRAST) {
		try {
			camera->callVoid("setParam", kCameraContrastID,
							 DEFAULT_CAMERA_CONTRAST);
		} catch (ALError &e){
			log->error("Man", "Couldn't set Contrast AGAIN");
		}
	}
    // Red chroma
    try {
        camera->callVoid("setParam", kCameraRedChromaID,
                         DEFAULT_CAMERA_REDCHROMA);
    } catch (ALError &e){
        log->error("Man", "Couldn't set RedChroma");
    }
	param = camera->call<int>("getParam", kCameraRedChromaID);
	if (param != DEFAULT_CAMERA_REDCHROMA) {
		try {
			camera->callVoid("setParam", kCameraRedChromaID,
							 DEFAULT_CAMERA_REDCHROMA);
		} catch (ALError &e){
			log->error("Man", "Couldn't set RedChroma AGAIN");
		}
	}
    // Blue chroma
    try {
        camera->callVoid("setParam", kCameraBlueChromaID,
                         DEFAULT_CAMERA_BLUECHROMA);
    } catch (ALError &e){
        log->error("Man", "Couldn't set BlueChroma");
    }
	param = camera->call<int>("getParam", kCameraBlueChromaID);
	if (param != DEFAULT_CAMERA_BLUECHROMA) {
		try {
			camera->callVoid("setParam", kCameraBlueChromaID,
							 DEFAULT_CAMERA_BLUECHROMA);
		} catch (ALError &e){
			log->error("Man", "Couldn't set BlueChroma AGAIN");
		}
	}
    // Exposure length
    try {
        camera->callVoid("setParam",kCameraExposureID,
                         DEFAULT_CAMERA_EXPOSURE);
    } catch (ALError &e) {
        log->error("Man", "Couldn't set Exposure");
    }
	param = camera->call<int>("getParam", kCameraExposureID);
	if (param != DEFAULT_CAMERA_EXPOSURE) {
		try {
			camera->callVoid("setParam", kCameraExposureID,
							 DEFAULT_CAMERA_EXPOSURE);
		} catch (ALError &e){
			log->error("Man", "Couldn't set Exposure AGAIN");
		}
	}
    // Gain
    try {
        camera->callVoid("setParam",kCameraGainID,
                         DEFAULT_CAMERA_GAIN);
    } catch (ALError &e) {
        log->error("Man", "Couldn't set Gain");
    }
	param = camera->call<int>("getParam", kCameraGainID);
	if (param != DEFAULT_CAMERA_GAIN) {
		try {
			camera->callVoid("setParam", kCameraGainID,
							 DEFAULT_CAMERA_GAIN);
		} catch (ALError &e){
			log->error("Man", "Couldn't set Gain AGAIN");
		}
	}
    // Saturation
    try {
        camera->callVoid("setParam",kCameraSaturationID,
                         DEFAULT_CAMERA_SATURATION);
    } catch (ALError &e) {
        log->error("Man", "Couldn't set Saturation");
    }
	param = camera->call<int>("getParam", kCameraSaturationID);
	if (param != DEFAULT_CAMERA_SATURATION) {
		try {
			camera->callVoid("setParam", kCameraSaturationID,
							 DEFAULT_CAMERA_SATURATION);
		} catch (ALError &e){
			log->error("Man", "Couldn't set Saturation AGAIN");
		}
	}
    // Hue
    try {
        camera->callVoid("setParam",kCameraHueID,
                         DEFAULT_CAMERA_HUE);
    } catch (ALError &e) {
        log->error("Man", "Couldn't set Hue");
    }
	param = camera->call<int>("getParam", kCameraHueID);
	if (param != DEFAULT_CAMERA_HUE) {
		try {
			camera->callVoid("setParam", kCameraHueID,
							 DEFAULT_CAMERA_HUE);
		} catch (ALError &e){
			log->error("Man", "Couldn't set Hue AGAIN");
		}
	}
    // Lens correction X
    try {
        camera->callVoid("setParam",kCameraLensXID,
                         DEFAULT_CAMERA_LENSX);
    } catch (ALError &e) {
        log->error("Man", "Couldn't set Lens Correction X");
    }
	param = camera->call<int>("getParam", kCameraLensXID);
	if (param != DEFAULT_CAMERA_LENSX) {
		try {
			camera->callVoid("setParam", kCameraLensXID,
							 DEFAULT_CAMERA_LENSX);
		} catch (ALError &e){
			log->error("Man", "Couldn't set Lens Correction X AGAIN");
		}
	}
    // Lens correction Y
    try {
        camera->callVoid("setParam",kCameraLensYID,
                         DEFAULT_CAMERA_LENSY);
    } catch (ALError &e) {
        log->error("Man", "Couldn't set Lens Correction Y");
    }
	param = camera->call<int>("getParam", kCameraLensYID);
	if (param != DEFAULT_CAMERA_LENSY) {
		try {
			camera->callVoid("setParam", kCameraLensYID,
							 DEFAULT_CAMERA_LENSY);
		} catch (ALError &e){
			log->error("Man", "Couldn't set Lens Correction Y AGAIN");
		}
	}
}


#endif // USE_VISION


void
Man::closeMan() {
#ifdef USE_VISION
    if(camera_active){
        cout << "lem_name = " << lem_name << endl;
        try {
            camera->callVoid("unregister", lem_name);
        }catch (ALError &e) {
            log->error("Man", "Could not call the unregister method of the NaoCam "
                       "module");
        }
    }
#endif
}


void
Man::run ()
{

#ifdef DEBUG_MAN_THREADING
    cout << "Man::running" << endl;
#endif

    // Start Comm thread (it handles its own threading
    if (comm->start() != 0)
        cerr << "Comm failed to start" << endl;
    else
        comm->getTrigger()->await_on();

#ifdef USE_MOTION
// Start Motion thread (it handles its own threading
    if (motion->start() != 0)
        cerr << "Motion failed to start" << endl;
    //else
    //  motion->getStart()->await();
    if(guardian->start() != 0)
        cout << "RoboGuardian failed to start" << endl;
#endif

#ifdef DEBUG_MAN_THREADING
    cout << "  run :: Signalling start" << endl;
#endif

    running = true;
    trigger->on();

    while (running) {
        //start timer
        const long long startTime = micro_time();
#ifdef USE_VISION
        // Wait for signal
        //image_sig->await();
        // wait for and retrieve the latest image
        if(camera_active)
            waitForImage();
#endif // USE_VISION

        // Break out of loop if thread should exit
        if (!running)
            break;

        // Synchronize noggin's information about joint angles with the motion
        // thread's information
        sensors->updateVisionAngles();

        transcriber->postVisionSensors();

        // Process current frame
        processFrame();

        //Release the camera image
        if(camera_active)
            releaseImage();

        // Make sure messages are printed
        fflush(stdout);

        // Broadcast a signal that we have finished processing this frame
        //vision_sig->signal();

        //stop timer
        const long long processTime = micro_time() - startTime;
        //sleep until next frame
        if (processTime > VISION_FRAME_LENGTH_uS){
            cout << "Time spent in Man loop longer than frame length: "
                 << processTime <<endl;
            //Don't sleep at all
        } else{
            usleep(static_cast<useconds_t>(VISION_FRAME_LENGTH_uS
                                           -processTime));
        }
    }

#ifdef USE_MOTION
    // Finished with run loop, stop sub-threads and exit
    motion->stop();
    motion->getTrigger()->await_off();
    guardian->stop();
    guardian->getTrigger()->await_off();
#endif
    comm->stop();
    comm->getTrigger()->await_off();
    // @jfishman - tool will not exit, due to socket blocking
    //comm->getTOOLTrigger()->await_off();

#ifdef DEBUG_MAN_THREADING
    cout << "  run :: Signalling stop" << endl;
#endif

    // Signal stop event
    running = false;
    trigger->off();

}

void
Man::waitForImage ()
{
    try {
        const unsigned char *data;
#ifndef MAN_IS_REMOTE
        ALVisionImage *image = NULL;
#else
        ALValue image;
        image.arraySetSize(7);
#endif

        data = NULL;
#ifndef MAN_IS_REMOTE
#ifdef DEBUG_IMAGE_REQUESTS
        printf("Requesting local image of size %ix%i, color space %i\n",
               IMAGE_WIDTH, IMAGE_HEIGHT, NAO_COLOR_SPACE);
#endif

        // Attempt to retrive the next image
        try {
            image = (ALVisionImage*) (camera->call<int>("getDirectRawImageLocal",lem_name));
        }catch (ALError &e) {
            log->error("NaoMain", "Could not call the getImageLocal method of the "
                       "NaoCam module");
        }
        if (image != NULL)
            data = image->getFrame();

#ifdef DEBUG_IMAGE_REQUESTS
        //You can get some informations of the image.
        int width = image->fWidth;
        int height = image->fHeight;
        int nbLayers = image->fNbLayers;
        int colorSpace = image->fColorSpace;
        long long timeStamp = image->fTimeStamp;
        int seconds = (int)(timeStamp/1000000LL);
        printf("Retrieved an image of dimensions %ix%i, color space %i,"
               "with %i layers and a time stamp of %is \n",
               width, height, colorSpace,nbLayers,seconds);
#endif

#else//Frame is remote:
#ifdef DEBUG_IMAGE_REQUESTS
        printf("Requesting remote image of size %ix%i, color space %i\n",
               IMAGE_WIDTH, IMAGE_HEIGHT, NAO_COLOR_SPACE);
#endif

        // Attempt to retrive the next image
        try {
            image = camera->call<ALValue>("getDirectRawImageRemote",
                                          lem_name);
        }catch (ALError &e) {
            log->error("NaoMain", "Could not call the getImageRemote method of the "
                       "NaoCam module");
        }

        data = static_cast<const unsigned char*>(image[6].GetBinary());
#ifdef DEBUG_IMAGE_REQUESTS
        //You can get some informations of the image.
        int width = (int) image[0];
        int height = (int) image[1];
        int nbLayers = (int) image[2];
        int colorSpace = (int) image[3];
        long long timeStamp = ((long long)(int)image[4])*1000000LL +
            ((long long)(int)image[5]);
        int seconds = (int)(timeStamp/1000000LL);
        printf("Retrieved an image of dimensions %ix%i, color space %i,"
               "with %i layers and a time stamp of %is \n",
               width, height, colorSpace,nbLayers,seconds);
#endif

#endif//IS_REMOTE

        if (data != NULL) {
            // Update Sensors image pointer
            sensors->lockImage();
            sensors->setImage(data);
            sensors->releaseImage();
        }

    }catch (ALError &e) {
        log->error("NaoMain", "Caught an error in run():\n" + e.toString());
    }
}


void Man::releaseImage(){
  //Now you have finished with the image, you have to release it in the V.I.M.
  try
  {
    camera->call<int>( "releaseDirectRawImage", lem_name );
  }catch( ALError& e)
  {
    log->error( "Man", "could not call the releaseImage method of the NaoCam module" );
  }
}


void
Man::processFrame ()
{
#ifdef USE_VISION
    //  This is called from Python right now
    if(camera_active)
        vision->copyImage(sensors->getImage());
#endif
    PROF_EXIT(profiler.get(), P_GETIMAGE);

    PROF_ENTER(profiler.get(), P_FINAL);
#ifdef USE_VISION
    if(camera_active)
        vision->notifyImage();
#endif

    // run Python behaviors
#ifdef USE_NOGGIN
    noggin->runStep();
#endif

    PROF_EXIT(profiler.get(), P_FINAL);
    PROF_NFRAME(profiler.get());

    PROF_ENTER(profiler.get(), P_GETIMAGE);

}


void Man::notifyNextVisionImage() {

}
