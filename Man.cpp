
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

#include "alvisionimage.h"
#include "alvisiondefinitions.h"

#include "Man.h"
#include "manconfig.h"
#include "corpus/synchro.h"

using namespace std;
using namespace AL;
using namespace boost;


// reference to the running instance
shared_ptr<Man> lMan;

/////////////////////////////////////////
//                                     //
//  Module class function definitions  //
//                                     //
/////////////////////////////////////////

static long long
micro_time (void)
{
    // Needed for microseconds which we convert to milliseconds
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return tv.tv_sec * MICROS_PER_SECOND + tv.tv_usec;
}

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
      profiler(&micro_time), sensors(),
#ifdef USE_MOTION
#ifdef NAOQI1
      motion(getParentBroker()->getMotionProxy(),synchro, &sensors),
#else
      motion(AL::ALMotionProxy::getInstance(),synchro, &sensors),
#endif
#endif
      vision(new NaoPose(&sensors), &profiler),
      comm(synchro, &sensors, &vision),
      noggin(&sensors, &profiler, &vision),
      camera_active(false)
{
    // open lems
    initMan();
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
        log->setVerbosity("warning");
    }catch (ALError &e) {
        std::cerr << "Could not create a proxy to ALLogger module" << std::endl;
    }

//initialize ALMemory for access to stuff like bumpers, etc
#ifdef NAOQI1
    try{
        almemory = getParentBroker()->getProxy("ALMemory");
    }catch(ALError &e){
        cout << "Failed to initialize proxy to ALMemory" <<endl;
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
void Man::syncWithALMemory() {
    // FSR update
    float frontLeft = 0.0f, frontRight = 0.0f,
        rearLeft = 0.0f, rearRight = 0.0f;
    try {
        frontLeft = static_cast<float>(almemory->call<ALValue>(
            "getData",string(
                "Device/SubDeviceList/LFoot/FSR/FrontLeft/Sensor/Value"), 0));
        frontRight = static_cast<float>(almemory->call<ALValue>(
            "getData",string(
                "Device/SubDeviceList/LFoot/FSR/FrontRight/Sensor/Value"), 0));
        rearLeft = static_cast<float>(almemory->call<ALValue>(
            "getData",string(
                "Device/SubDeviceList/LFoot/FSR/RearLeft/Sensor/Value"), 0));
        rearRight = static_cast<float>(almemory->call<ALValue>(
            "getData",string(
                "Device/SubDeviceList/LFoot/FSR/RearRight/Sensor/Value"), 0));
    } catch(ALError &e) {
        cout << "Failed to read left foot FSR values" << endl;
    }
    sensors.setLeftFootFSR(frontLeft, frontRight, rearLeft, rearRight);

    try {
        frontLeft = static_cast<float>(almemory->call<ALValue>(
            "getData",string(
                "Device/SubDeviceList/RFoot/FSR/FrontLeft/Sensor/Value"), 0));
        frontRight = static_cast<float>(almemory->call<ALValue>(
            "getData",string(
                "Device/SubDeviceList/RFoot/FSR/FrontRight/Sensor/Value"), 0));
        rearLeft = static_cast<float>(almemory->call<ALValue>(
            "getData",string(
                "Device/SubDeviceList/RFoot/FSR/RearLeft/Sensor/Value"), 0));
        rearRight = static_cast<float>(almemory->call<ALValue>(
            "getData",string(
                "Device/SubDeviceList/RFoot/FSR/RearRight/Sensor/Value"), 0));
    } catch(ALError &e) {
        cout << "Failed to read right foot FSR values" << endl;
    }
    sensors.setRightFootFSR(frontLeft, frontRight, rearLeft, rearRight);

    // Foot bumper update
    float leftFootBumperLeft  = 0.0f, leftFootBumperRight  = 0.0f;
    float rightFootBumperLeft = 0.0f, rightFootBumperRight = 0.0f;
    try {
        leftFootBumperLeft = static_cast<float>(almemory->call<ALValue>(
            "getData",string(
                "Device/SubDeviceList/LFoot/Bumper/Left/Sensor/Value"), 0));
        leftFootBumperRight = static_cast<float>(almemory->call<ALValue>(
            "getData",string(
                "Device/SubDeviceList/LFoot/Bumper/Right/Sensor/Value"), 0));
        rightFootBumperLeft = static_cast<float>(almemory->call<ALValue>(
            "getData",string(
                "Device/SubDeviceList/RFoot/Bumper/Left/Sensor/Value"), 0));
        rightFootBumperRight = static_cast<float>(almemory->call<ALValue>(
            "getData",string(
                "Device/SubDeviceList/RFoot/Bumper/Right/Sensor/Value"), 0));
    } catch(ALError &e) {
        cout << "Failed to read bumper values" <<endl;
    }
    sensors.setLeftFootBumper(leftFootBumperLeft, leftFootBumperRight);
    sensors.setRightFootBumper(rightFootBumperLeft, rightFootBumperRight);
}


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
    int fps = 15;

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


    const int CAM_PARAM_RETRIES = 3;

    for (int i=0;i<CAM_PARAM_RETRIES;++i){
        try {
            // Turn of auto settings
            camera->callVoid("setParam", kCameraAutoExpositionID,
                             DEFAULT_CAMERA_AUTO_EXPOSURE);
            break;
        }catch (ALError &e){
            log->error("Man", "Couldn't set AutoExposition %d",
                       DEFAULT_CAMERA_AUTO_EXPOSURE);
        }
    }

    for (int i=0;i<CAM_PARAM_RETRIES;++i){
        try {
            camera->callVoid("setParam", kCameraAutoWhiteBalanceID,
                             DEFAULT_CAMERA_AUTO_WHITE);
            break;
        }catch (ALError &e){
            log->error("Man", "Couldn't set AutoWhiteBalance %d",
                       DEFAULT_CAMERA_AUTO_WHITE);
        }
    }

    for (int i=0;i<CAM_PARAM_RETRIES;++i){
        try {
            camera->callVoid("setParam", kCameraAutoGainID, 0);
            break;
        }catch (ALError &e){
            log->error("Man", "Couldn't set AutoGain 0");
        }
    }

    // Our settings -- currently all mid-way between extremes, or off
    // currently off, as if messes up the image

    //cout << "Setting Brightness" << endl;

    for (int i=0;i<CAM_PARAM_RETRIES;++i){
        try {
            camera->callVoid("setParam", kCameraBrightnessID, 128);
            break;
        }catch (ALError &e){
            log->error("Man", "Couldn't set Brightness 128");
        }
    }

    for (int i=0;i<CAM_PARAM_RETRIES;++i){
        try {
            camera->callVoid("setParam", kCameraContrastID, 64);
            break;
        }catch (ALError &e){
            log->error("Man", "Couldn't set Contrast 64");
        }
    }

    for (int i=0;i<CAM_PARAM_RETRIES;++i){
        try {
            camera->callVoid("setParam", kCameraRedChromaID, 72);
            break;
        }catch (ALError &e){
            log->error("Man", "Couldn't set RedChroma 72");
        }
    }

    for (int i=0;i<CAM_PARAM_RETRIES;++i){
        try {
            camera->callVoid("setParam", kCameraBlueChromaID,131);
            break;
        }catch (ALError &e){
            log->error("Man", "Couldn't set BlueChroma 131");
        }
    }

    for (int i=0;i<CAM_PARAM_RETRIES;++i){
        try {
            camera->callVoid("setParam",kCameraExposureID,1);
            break;
        }catch (ALError &e){
            log->error("Man", "Couldn't set Exposure 0");
        }
    }

}
#endif//NAOQI1
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
    if (comm.start() != 0)
        cerr << "Comm failed to start" << endl;
    else
        comm.getTrigger()->await_on();

#ifdef USE_MOTION
// Start Motion thread (it handles its own threading
    if (motion.start() != 0)
        cerr << "Motion failed to start" << endl;
    //else
    //  motion.getStart()->await();
#endif

#ifdef DEBUG_MAN_THREADING
    cout << "  run :: Signalling start" << endl;
#endif

    running = true;
    trigger->on();

    while (running) {

#ifdef USE_VISION
        // Wait for signal
        //image_sig->await();
        // wait for and retrieve the latest image
        if(camera_active)
            waitForImage();
        else
            SleepMs(500);
#else
        // simulate vision frame rate
        SleepMs(500);
#endif // USE_VISION

        // Break out of loop if thread should exit
        if (!running)
            break;

        // Synchronize noggin's information about joint angles with the motion
        // thread's information
        sensors.updatePython();
        sensors.updateVisionAngles();
#ifdef NAOQI1
#ifndef OFFLINE
        // This call syncs all sensors values: bumpers, fsr, inertial, etc.
        syncWithALMemory();

        const FootBumper leftFootBumper(sensors.getLeftFootBumper());
        const FootBumper rightFootBumper(sensors.getRightFootBumper());

        bool temp = leftFootBumper.left || leftFootBumper.right;

        /*
        cout << "leftFootBumper: "
             << boolalpha << temp << endl;
        */

        /*
        if (leftFootBumper.left || leftFootBumper.right ||
            rightFootBumper.left || rightFootBumper.right) {
            saveFrame();
        }
        */

        // testing the fsr values we get from ALMemory
        const FSR leftFoot(sensors.getLeftFootFSR());
        const FSR rightFoot(sensors.getRightFootFSR());

        cout << "Left foot:" << endl
             << "    FL: " << leftFoot.frontLeft
             << "    FR: " << leftFoot.frontRight
             << "    RL: " << leftFoot.rearLeft
             << "    RR: " << leftFoot.rearRight
             << endl;

        cout << "Right foot:" << endl
             << "    FL: " << rightFoot.frontLeft
             << "    FR: " << rightFoot.frontRight
             << "    RL: " << rightFoot.rearLeft
             << "    RR: " << rightFoot.rearRight
             << endl;

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
    }

#ifdef USE_MOTION
    // Finished with run loop, stop sub-threads and exit
    motion.stop();
    motion.getTrigger()->await_off();
#endif
    comm.stop();
    comm.getTrigger()->await_off();
    // @jfishman - tool will not exit, due to socket blocking
    //comm.getTOOLTrigger()->await_off();

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

        SleepMs(100);
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
            sensors.lockImage();
            sensors.setImage(data);
            sensors.releaseImage();
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
            sensors.lockImage();
            sensors.setImage(data);
            sensors.releaseImage();
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
        vision.copyImage(sensors.getImage());
#endif
    PROF_EXIT(&profiler, P_GETIMAGE);

    PROF_ENTER(&profiler, P_FINAL);
#ifdef USE_VISION
    if(camera_active)
        vision.notifyImage();
#endif

#ifdef DEBUG_BALL_DETECTION
    if (vision.ball->getWidth() > 0)
        printf("We see the ball!\n");
    else
        printf("No ball in this frame\n");
#endif

    // run Python behaviors
#ifdef USE_NOGGIN
    noggin.runStep();
#endif

    PROF_EXIT(&profiler, P_FINAL);
    PROF_NFRAME(&profiler);

    PROF_ENTER(&profiler, P_GETIMAGE);

}

void
Man::saveFrame(){
    static int saved_frames = 0;
    int MAX_FRAMES = 150;
    if (saved_frames > MAX_FRAMES)
        return;

    string EXT(".NFRM");
    string BASE("/");
    int NUMBER = saved_frames;
    string FOLDER("/home/root/frames");
    stringstream FRAME_PATH;

    FRAME_PATH << FOLDER << BASE << NUMBER << EXT;
    fstream fout(FRAME_PATH.str().c_str(), ios_base::out);

    // Retrive joints
    vector<float> joints = sensors.getVisionBodyAngles();

    // Lock and write imag1e
    sensors.lockImage();
    fout.write(reinterpret_cast<const char*>(sensors.getImage()),
               IMAGE_BYTE_SIZE);
    sensors.releaseImage();

    // Write joints
    for (vector<float>::const_iterator i = joints.begin(); i < joints.end(); i++)
        fout << " " << *i;

    fout.close();
    saved_frames++;
    cout << "Saved frame #" << saved_frames << endl;
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


