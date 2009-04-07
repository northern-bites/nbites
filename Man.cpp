
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

using namespace std;
using namespace AL;
using boost::shared_ptr;


/////////////////////////////////////////
//                                     //
//  Module class function definitions  //
//                                     //
/////////////////////////////////////////

#ifdef NAOQI1
Man::Man (ALPtr<ALBroker> pBroker, std::string pName)
    : ALModule(pBroker,pName),
#else
Man::Man ()
    : ALModule("Man"),
#endif
    // this is not good usage of shared_ptr...  oh well
    Thread(shared_ptr<Synchro>(new Synchro()), "Man"),
    python_prefs(),
#ifdef NAOQI1
    // call the default constructor of all the shared pointers
    log(), camera(), lem(), almemory(), dcm(0),
#else
    // initialize all pointers to NULL or 0
    log(0), camera(0), lem(0),
#endif
    lem_name(""),
    camera_active(false)
{
    // open lems
    initMan();

    // initialize system helper modules
    profiler = shared_ptr<Profiler>(new Profiler(&micro_time));
    //messaging = shared_ptr<Messenger>(new Messenger());
    sensors = shared_ptr<Sensors>(new Sensors());
    pose = shared_ptr<NaoPose>(new NaoPose(sensors));

    // initialize core processing modules
#ifdef USE_MOTION
    enactor = shared_ptr<EnactorT>(new EnactorT(pBroker, sensors));
    motion = shared_ptr<Motion<EnactorT> >(
        new Motion<EnactorT>(synchro, enactor, sensors));
#endif
    vision = shared_ptr<Vision>(new Vision(pose, profiler));
    comm = shared_ptr<Comm>(new Comm(synchro, sensors, vision));
#ifdef USE_NOGGIN
    noggin = shared_ptr<Noggin>(new Noggin(profiler, vision,
                                           motion->getInterface()));
#endif// USE_NOGGIN
    initSyncWithALMemory();
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

#ifdef NAOQI1
        log = getParentBroker()->getLoggerProxy();
#else
        log = ALLoggerProxy::getInstance();
#endif
        log->setVerbosity("info");
    }catch (ALError &e) {
        std::cerr << "Could not create a proxy to ALLogger module" << std::endl;
    }

    // initialize ALMemory for access to stuff like bumpers, etc
#ifdef NAOQI1
    try {
        //almemory = getParentBroker()->getProxy("ALMemory");
        almemory = getParentBroker()->getMemoryProxy();
    } catch(ALError &e){
        cout << "Failed to initialize proxy to ALMemory" << endl;
    }

    // initialize a dcm proxy so we can set values in almemory
    try {
        dcm = new DCMProxy(getParentBroker());
    } catch(ALError &e) {
        cout << "Failed to initialize proxy to DCM" << endl;
    }

#endif

#ifdef USE_VISION
#ifdef NAOQI1
    registerCamera();
    if(camera_active){
        //initCameraSettings(TOP_CAMERA);
        initCameraSettings(BOTTOM_CAMERA);
    }
#else
    initCamera();
#endif
#endif

#ifdef DEBUG_MAN_INITIALIZATION
    printf("  DONE!\n");
#endif
}

#ifdef USE_VISION
#ifdef NAOQI1

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
    int fps = VISION_FPS;

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
    // Auto white balance
    try {
        camera->callVoid("setParam", kCameraAutoWhiteBalanceID,
                         DEFAULT_CAMERA_AUTO_WHITEBALANCE);

    } catch (ALError &e){
        log->error("Man", "Couldn't set AutoWhiteBalance");
    }
    // Auto gain
    try {
        camera->callVoid("setParam", kCameraAutoGainID,
                         DEFAULT_CAMERA_AUTO_GAIN);
    } catch (ALError &e){
        log->error("Man", "Couldn't set AutoGain");
    }
    // Set camera defaults
    // brightness
    try {
        camera->callVoid("setParam", kCameraBrightnessID,
                         DEFAULT_CAMERA_BRIGHTNESS);
    } catch (ALError &e){
        log->error("Man", "Couldn't set Brightness ");
    }
    // contrast
    try {
        camera->callVoid("setParam", kCameraContrastID,
                         DEFAULT_CAMERA_CONTRAST);
    } catch (ALError &e){
        log->error("Man", "Couldn't set Contrast");
    }
    // Red chroma
    try {
        camera->callVoid("setParam", kCameraRedChromaID,
                         DEFAULT_CAMERA_REDCHROMA);
    } catch (ALError &e){
        log->error("Man", "Couldn't set RedChroma");
    }
    // Blue chroma
    try {
        camera->callVoid("setParam", kCameraBlueChromaID,
                         DEFAULT_CAMERA_BLUECHROMA);
    } catch (ALError &e){
        log->error("Man", "Couldn't set BlueChroma");
    }
    // Exposure length
    try {
        camera->callVoid("setParam",kCameraExposureID,
                         DEFAULT_CAMERA_EXPOSURE);
    } catch (ALError &e) {
        log->error("Man", "Couldn't set Exposure");
    }
    // Gain
    try {
        camera->callVoid("setParam",kCameraGainID,
                         DEFAULT_CAMERA_GAIN);
    } catch (ALError &e) {
        log->error("Man", "Couldn't set Gain");
    }
    // Saturation
    try {
        camera->callVoid("setParam",kCameraSaturationID,
                         DEFAULT_CAMERA_SATURATION);
    } catch (ALError &e) {
        log->error("Man", "Couldn't set Saturation");
    }
    // Hue
    try {
        camera->callVoid("setParam",kCameraHueID,
                         DEFAULT_CAMERA_HUE);
    } catch (ALError &e) {
        log->error("Man", "Couldn't set Hue");
    }
    // Lens correction X
    try {
        camera->callVoid("setParam",kCameraLensXID,
                         DEFAULT_CAMERA_LENSX);
    } catch (ALError &e) {
        log->error("Man", "Couldn't set Lens Correction X");
    }
    // Lens correction Y
    try {
        camera->callVoid("setParam",kCameraLensXID,
                         DEFAULT_CAMERA_LENSY);
    } catch (ALError &e) {
        log->error("Man", "Couldn't set Lens Correction Y");
    }
}

#else//NAOQI1
void Man::initCamera(){
    camera = NULL;
    lem = NULL;


    try {
        camera = new ALProxy("NaoCam");
    }catch (ALError &e) {
        log->error("Man", "Could not create a proxy to NaoCam module");
        return;
    }

    lem_name = "Man_LEM";
    int format = NAO_IMAGE_SIZE;
    int colorSpace = NAO_COLOR_SPACE;
    int fps = 15;

#ifdef DEBUG_MAN_INITIALIZATION
    printf("  Registering LEM with format=%i colorSpace=%i fps=%i\n", format,
           colorSpace, fps);
#endif

    try {
        lem_name = camera->call<std::string>("register", lem_name, format,
                                             colorSpace, fps);
    } catch (ALError &e) {
        cout << "Failed to initialize the camera"<<endl;
        camera_active = false;
    }


    // Turn off auto settings
    // Auto exposure
    try {
        camera->callVoid("setParam", kCameraAutoExpositionID,
                         DEFAULT_CAMERA_AUTO_EXPOSITION);
    } catch (ALError &e){
        log->error("Man", "Couldn't set AutoExposition");
    }
    // Auto white balance
    try {
        camera->callVoid("setParam", kCameraAutoWhiteBalanceID,
                         DEFAULT_CAMERA_AUTO_WHITEBALANCE);

    } catch (ALError &e){
        log->error("Man", "Couldn't set AutoWhiteBalance");
    }
    // Auto gain
    try {
        camera->callVoid("setParam", kCameraAutoGainID,
                         DEFAULT_CAMERA_AUTO_GAIN);
    } catch (ALError &e){
        log->error("Man", "Couldn't set AutoGain");
    }
    // Set camera defaults
    // brightness
    try {
        camera->callVoid("setParam", kCameraBrightnessID,
                         DEFAULT_CAMERA_BRIGHTNESS);
    } catch (ALError &e){
        log->error("Man", "Couldn't set Brightness ");
    }
    // contrast
    try {
        camera->callVoid("setParam", kCameraContrastID,
                         DEFAULT_CAMERA_CONTRAST);
    } catch (ALError &e){
        log->error("Man", "Couldn't set Contrast");
    }
    // Red chroma
    try {
        camera->callVoid("setParam", kCameraRedChromaID,
                         DEFAULT_CAMERA_REDCHROMA);
    } catch (ALError &e){
        log->error("Man", "Couldn't set RedChroma");
    }
    // Blue chroma
    try {
        camera->callVoid("setParam", kCameraBlueChromaID,
                         DEFAULT_CAMERA_BLUECHROMA);
    } catch (ALError &e){
        log->error("Man", "Couldn't set BlueChroma");
    }
    // Exposure length
    try {
        camera->callVoid("setParam",kCameraExposureID,
                         DEFAULT_CAMERA_EXPOSURE);
    } catch (ALError &e) {
        log->error("Man", "Couldn't set Exposure");
    }
    // Gain
    try {
        camera->callVoid("setParam",kCameraGainID,
                         DEFAULT_CAMERA_GAIN);
    } catch (ALError &e) {
        log->error("Man", "Couldn't set Gain");
    }
    // Saturation
    try {
        camera->callVoid("setParam",kCameraSaturationID,
                         DEFAULT_CAMERA_SATURATION);
    } catch (ALError &e) {
        log->error("Man", "Couldn't set Saturation");
    }
    // Hue
    try {
        camera->callVoid("setParam",kCameraHueID,
                         DEFAULT_CAMERA_HUE);
    } catch (ALError &e) {
        log->error("Man", "Couldn't set Hue");
    }
    // Lens correction X
    try {
        camera->callVoid("setParam",kCameraLensXID,
                         DEFAULT_CAMERA_LENSX);
    } catch (ALError &e) {
        log->error("Man", "Couldn't set Lens Correction X");
    }
    // Lens correction Y
    try {
        camera->callVoid("setParam",kCameraLensXID,
                         DEFAULT_CAMERA_LENSY);
    } catch (ALError &e) {
        log->error("Man", "Couldn't set Lens Correction Y");
    }

}
#endif//NAOQI1
#endif // USE_VISION

#ifdef NAOQI1
void Man::initSyncWithALMemory() {
    try{
        alfastaccess =
            ALPtr<ALMemoryFastAccess >(new ALMemoryFastAccess());
    } catch(AL::ALError &e){
        cout << "Failed to initialize proxy to ALFastAccess"<<endl;
    }

    vector<string> varNames;
    varNames += string("Device/SubDeviceList/LFoot/Bumper/Left/Sensor/Value"),
        string("Device/SubDeviceList/LFoot/Bumper/Right/Sensor/Value"),
        string("Device/SubDeviceList/RFoot/Bumper/Left/Sensor/Value"),
        string("Device/SubDeviceList/RFoot/Bumper/Right/Sensor/Value"),
        string("Device/SubDeviceList/US/Sensor/Value"),
        string("Device/SubDeviceList/US/Actuator/Value");

    alfastaccess->ConnectToVariables(getParentBroker(),varNames);
}

void Man::syncWithALMemory() {

    static vector<float> varValues(6, 0.0f);
    alfastaccess->GetValues(varValues);

    /*
    cout << "****** Sensors values ******" << endl;
    for (int i = 0; i < 6; i++) {
        cout << varValues[i] <<endl;
    }
    cout << endl;
    */

    // cycle the ultra sound mode (MAYBE THIS DOESN'T WORK)
    static int counter = 0;
    try {
        // This is testing code which sends a new value to the actuator every
        // 20 frames. It also cycles the ultrasound mode between the four
        // possibilities. See docs.
        ALValue commands;
        int setMode = counter / 5;

        commands.arraySetSize(3);
        commands[0] = string("US/Actuator/Value");
        commands[1] = string("Merge");
        commands[2].arraySetSize(1);
        commands[2][0].arraySetSize(2);
        // the current mode - changes every 5 frames
        commands[2][0][0] = static_cast<float>(setMode);
        commands[2][0][1] = dcm->getTime(250);

        // set the mode only once every 4 frames because it responds slowly anyway
        // but this rate needs to change if we don't run vision at 15 fps
        if (counter % 4 == 0)
            dcm->set(commands);

        counter++;

        if (counter > 20)
            counter = 0;

    } catch(ALError &e) {
        cout << "Failed to set ultrasound mode. Reason: "
             << e.toString() << endl;
    }

    const float leftFootBumperLeft  = varValues[0],
        leftFootBumperRight  = varValues[1];
    const float rightFootBumperLeft = varValues[2],
        rightFootBumperRight = varValues[3];

    const float ultraSoundDist = varValues[4];
    const int ultraSoundMode = static_cast<int>(varValues[5]);

    sensors->
        setVisionSensors(FootBumper(leftFootBumperLeft, leftFootBumperRight),
                         FootBumper(rightFootBumperLeft, rightFootBumperRight),
                         ultraSoundDist,
                         // UltraSoundMode is just an enum
                         static_cast<UltraSoundMode> (ultraSoundMode));

}

#endif //NAOQI1

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
        sensors->updatePython();
        sensors->updateVisionAngles();
#ifdef NAOQI1
#ifndef OFFLINE
        syncWithALMemory(); // update sensors with foot bumpers and ultrasound.
#endif
#endif

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

#ifdef NAOQI1
void
Man::waitForImage ()
{
    try {
        const unsigned char *data;
#ifndef MAN_IS_REMOTE
        ALVisionImage *image = NULL;
#else
        ALValue image;
        image.arraySetSize(6);
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
            image = lem->call<ALValue>("getImageRemote");
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
#else//NAOQI1
void
Man::waitForImage ()
{
    try {
        const unsigned char *data;
#ifndef MAN_IS_REMOTE
        ALVisionImage *image = NULL;
#else
        ALValue image;
        image.arraySetSize(6);
#endif

        SleepMs(100);
        data = NULL;
#ifndef MAN_IS_REMOTE
#ifdef DEBUG_IMAGE_REQUESTS
        printf("Requesting local image of size %ix%i, color space %i\n",
               IMAGE_WIDTH, IMAGE_HEIGHT, NAO_COLOR_SPACE);
#endif

        // Attempt to retrive the next image
        try {
            image = (ALVisionImage*) (lem->call<int>("fetchNextLocal"));
        }catch (ALError &e) {
            log->error("NaoMain", "Could not call the fetchNextLocal method of the "
                       "NaoCam module");
        }
        if (image != NULL)
            data = image->getFrame();

#else
#ifdef DEBUG_IMAGE_REQUESTS
        printf("Requesting remote image of size %ix%i, color space %i\n",
               IMAGE_WIDTH, IMAGE_HEIGHT, NAO_COLOR_SPACE);
#endif

        // Attempt to retrive the next image
        try {
            image = lem->call<ALValue>("fetchNextRemote");
        }catch (ALError &e) {
            log->error("NaoMain", "Could not call the fetchNextRemote method of the "
                       "NaoCam module");
        }
        data = static_cast<const unsigned char *>(image[5].GetBinary());

#endif

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
#endif//NAOQI1

#ifdef NAOQI1
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
#endif

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

PythonPreferences::PythonPreferences ()
{
    // Initialize interpreter
    if (!Py_IsInitialized())
        Py_Initialize();

    modifySysPath();

}

void
PythonPreferences::modifySysPath ()
{
    // Enter the current working directory into the python module path
    //
#if ROBOT(NAO)
#ifndef OFFLINE
    char *cwd = "/opt/naoqi/modules/lib";
#else
    char *cwd = "/usr/local/nao/modules/lib";
#endif
#else
    const char *cwd = get_current_dir_name();
#endif

#ifdef DEBUG_NOGGIN_INITIALIZATION
    printf("  Adding %s to sys.path\n", cwd);
#endif

    PyObject *sys_module = PyImport_ImportModule("sys");
    if (sys_module == NULL) {
        fprintf(stderr, "** Error importing sys module: **");
        if (PyErr_Occurred())
            PyErr_Print();
        else
            fprintf(stderr, "** No Python exception information available **");
    }else {
        PyObject *dict = PyModule_GetDict(sys_module);
        PyObject *path = PyDict_GetItemString(dict, "path");
        PyList_Append(path, PyString_FromString(cwd));
        Py_DECREF(sys_module);
    }

#if !ROBOT(NAO)
    free(cwd);
#endif

}


