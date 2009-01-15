
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


static const int NUM_HACK_FRAMES = 20;
static const int MIN_HACK_BALL_WIDTH = 20;
#define DEBUG_VISION_HACK

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
#ifdef NAOQI1
      motion(getParentBroker()->getMotionProxy(),synchro, &sensors),
#else
      motion(AL::ALMotionProxy::getInstance(),synchro, &sensors),
#endif
      vision(new NaoPose(&sensors), &profiler),
      comm(synchro, &sensors, &vision),
      //BREAKS NAOQI1.0
#ifndef NAOQI1
      noggin(&sensors, &profiler, &vision),
#endif
      frame_counter(0), saved_frames(0), hack_frames(0),
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

    functionName("visionHack", "Man", "H4ck v1SI0n 2 pI3ce5");
    BIND_METHOD(Man::visionHack);
    //functionName("helloWorld", "Man", "Test");
    //BIND_METHOD(Man::helloWorld);

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

#ifdef USE_VISION
    initCamera();
#endif

#ifdef DEBUG_MAN_INITIALIZATION
    printf("  DONE!\n");
#endif
}

#ifdef USE_VISION
#ifdef NAOQI1
void
Man::initCamera(){

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

#ifdef DEBUG_MAN_INITIALIZATION
    printf("  Registering LEM with format=%i colorSpace=%i fps=%i\n", format,
           colorSpace, fps);
#endif

    try {
        lem_name = camera->call<std::string>("register", lem_name, format,
                                             colorSpace, fps);
    } catch (ALError &e) {
        SleepMs(500);
    }

    try {
        printf("LEM failed once, trying again\n");
        lem_name = camera->call<std::string>("register", lem_name, format,
                                             colorSpace, fps);
    }catch (ALError &e2) {
        log->error("Man", "Could not call the register method of the NaoCam "
                   "module\n" + e2.toString());
        return;
    }


    try {
        lem =getParentBroker()->getProxy(lem_name);
    }catch (ALError &e) {
        log->error("Man", "Could not create the proxy for the Layer Extracator "
                   "Module, name " + lem_name);
    }

    const int CAM_PARAM_RETRIES = 3;

    for (int i=0;i<CAM_PARAM_RETRIES;++i){
        try {
            // Turn of auto settings
            camera->callVoid("setParam", kCameraAutoExpositionID, 0);
            break;
        }catch (ALError &e){
            log->error("Man", "Couldn't set AutoExposition 0");
        }
    }

    for (int i=0;i<CAM_PARAM_RETRIES;++i){
        try {
            camera->callVoid("setParam", kCameraAutoWhiteBalanceID, 0);
            break;
        }catch (ALError &e){
            log->error("Man", "Couldn't set AutoWhiteBalance 0");
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
            camera->callVoid("setParam",kCameraExposureID,0);
            break;
        }catch (ALError &e){
            log->error("Man", "Couldn't set Exposure 0");
        }
    }

}

#else//NAOQI1

void
Man::initCamera(){
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
        SleepMs(500);
    }

    try {
        printf("LEM failed once, trying again\n");
        lem_name = camera->call<std::string>("register", lem_name, format,
                                             colorSpace, fps);
    }catch (ALError &e2) {
        log->error("Man", "Could not call the register method of the NaoCam "
                   "module\n" + e2.toString());
        return;
    }

    try {
        lem= new ALProxy(lem_name);
    }catch (ALError &e) {
        log->error("Man", "Could not create the proxy for the Layer Extracator "
                   "Module, name " + lem_name);
    }

    const int CAM_PARAM_RETRIES = 3;

    for (int i=0;i<CAM_PARAM_RETRIES;++i){
        try {
            // Turn of auto settings
            camera->callVoid("setParam", kCameraAutoExpositionID, 0);
            break;
        }catch (ALError &e){
            log->error("Man", "Couldn't set AutoExposition 0");
        }
    }

    for (int i=0;i<CAM_PARAM_RETRIES;++i){
        try {
            camera->callVoid("setParam", kCameraAutoWhiteBalanceID, 0);
            break;
        }catch (ALError &e){
            log->error("Man", "Couldn't set AutoWhiteBalance 0");
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
            camera->callVoid("setParam",kCameraExposureID,0);
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
        try {
            camera->callVoid("unregister", lem_name);
        }catch (ALError &e) {
            log->error("Man", "Could not call the inregister method of the NaoCam "
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

    frame_counter = 0;
#ifdef USE_VISION
    visionHack();
#endif

    while (running) {

#ifdef USE_VISION
        // Wait for signal
        //image_sig->await();
        // wait for and retrieve the latest image
        if(camera_active)
            waitForImage();
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

        // Image logging
        //if (frame_counter % 6 == 0)
        //  saveFrame();
        //frame_counter++;

#ifdef USE_VISION
        // Perform hack to correct flipped image resulting from driver errors
        if (hack_frames > 0)
            hackFrame();
#endif

        // Process current frame
        processFrame();

        // Make sure messages are printed
        fflush(stdout);

        // Broadcast a signal that we have finished processing this frame
        //vision_sig->signal();
    }

    // Finished with run loop, stop sub-threads and exit
    motion.stop();
    motion.getTrigger()->await_off();
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
            image = (ALVisionImage*) (camera->call<int>("getImageLocal",lem_name));
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
    //BREAKS NAOQI1.0
#ifndef NAOQI1
    noggin.runStep();
#endif

    PROF_EXIT(&profiler, P_FINAL);
    PROF_NFRAME(&profiler);

    PROF_ENTER(&profiler, P_GETIMAGE);

}

void
Man::saveFrame(){
    int MAX_FRAMES = 150;
    if (saved_frames > MAX_FRAMES)
        return;

    string EXT(".NFRM");
    string BASE("/");
    int NUMBER = saved_frames;
    string FOLDER("/tmp/frames");
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

void
Man::visionHack()
{
#ifdef DEBUG_VISION_HACK
    cout << "Starting vision hack" << endl;
#endif
    hack_frames = NUM_HACK_FRAMES;
    balls_seen = 0;
}

void
Man::hackFrame()
{
    hack_frames--;

    // perform image checks
    if (vision.ball->getDist() > 0 &&
        vision.ball->getWidth() > MIN_HACK_BALL_WIDTH)
        balls_seen++;

    // we're done hacking, make a decision
    if (hack_frames == 0) {
#ifdef DEBUG_VISION_HACK
        cout << "Vison hack over" << endl;
#endif
        if (balls_seen < 3) {
#ifdef DEBUG_VISION_HACK
            cout << "Swapping image" << endl;
#endif
            vision.thresh->swapUV();

        }else {
#ifdef DEBUG_VISION_HACK
            cout << "Not swapping" << endl;
#endif
        }
    }
}

PythonPreferences::PythonPreferences ()
{
    // Initialize interpreter
    if (!Py_IsInitialized())
        Py_Initialize();
    //Breaks NaoQi1.0
#ifndef NAOQI1
    modifySysPath();
#endif
}

void
PythonPreferences::modifySysPath ()
{
    // Enter the current working directory into the python module path
    //
#if ROBOT(NAO)
    char *cwd = "/opt/naoqi/modules/lib";
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


