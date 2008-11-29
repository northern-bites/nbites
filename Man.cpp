
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


static const int NUM_HACK_FRAMES = 20;
static const int MIN_HACK_BALL_WIDTH = 20;
#define DEBUG_VISION_HACK

// reference to the running instance
Man *lMan;

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

Man::Man ()
  : ALModule("Man"),
    // this is not good usage of shared_ptr...  oh well
    Thread(boost::shared_ptr<Synchro>(new Synchro()), "Man"),
    python_prefs(),
    profiler(&micro_time), sensors(),
    motion(&sensors),
    vision(new NaoPose(&sensors), &profiler),
    comm(&sensors, &vision),
    noggin(&sensors, &profiler, &vision),
    frame_counter(0), saved_frames(0), hack_frames(0)
{
  // open lems
  initModule();
}

Man::~Man ()
{
  // stop vision processing, comm, and motion
  stop();

  // unregister lem
  closeModule();
}

void
Man::initModule()
{
#ifdef DEBUG_MAN_INITIALIZATION
  printf("Man::initializing\n");
  printf("  Binding functions\n");
#endif

  // Describe the module here
  setModuleDescription("Nao robotic soccer player");

  // Define callable methods with there description
  //functionName("start", "Man", "Begin environment processing");
  //BIND_METHOD(Man::start);

  //functionName("stop", "Man", "Halt environment processing");
  //BIND_METHOD(Man::stop);

  functionName("startProfiling", "Man", "Start vision frame profiling, "
    "for given number of frames");
  BIND_METHOD(Man::startProfiling);

  functionName("stopProfiling", "Man", "Stop vision frame profiling");
  BIND_METHOD(Man::stopProfiling);

  functionName("visionHack", "Man", "H4ck v1SI0n 2 pI3ce5");
  BIND_METHOD(Man::visionHack);


#ifdef DEBUG_MAN_INITIALIZATION
  printf("  Opening proxies\n");
#endif
  try {
    log = ALLoggerProxy::getInstance();
    log->setVerbosity("warning");
  }catch (ALError &e) {
    std::cerr << "Could not create a proxy to ALLogger module" << std::endl;
  }

  camera = NULL;
  lem = NULL;

#ifdef USE_VISION
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
    lem = new ALProxy(lem_name);
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

#endif // USE_VISION



#ifdef DEBUG_MAN_INITIALIZATION
  printf("  DONE!\n");
#endif
}

void
Man::closeModule() {
#ifdef USE_VISION
  if (camera != NULL) {
    try {
      camera->callVoid("unregister", lem_name);
      delete camera;
      camera = NULL;
    }catch (ALError &e) {
      log->error("Man", "Could not call the inregister method of the NaoCam "
                "module");
      camera = NULL;
    }
  }

  delete lem;
  lem = NULL;
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
  //else
  //  comm.getStart()->await();

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
  getStart()->signal();

  frame_counter = 0;
#ifdef USE_VISION
  visionHack();
#endif

  while (isRunning()) {

#ifdef USE_VISION
    // Wait for signal
    //image_sig->await();
    // wait for and retrieve the latest image
    waitForImage();
#else
    // simulate vision frame rate
    SleepMs(500);
#endif // USE_VISION

    // Break out of loop if thread should exit
    if (!isRunning())
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
  //motion.getStop()->await();
  comm.stop();
  //comm.getStop()->await();

#ifdef DEBUG_MAN_THREADING
  cout << "  run :: Signalling stop" << endl;
#endif

  // Signal stop event
  getStop()->signal();
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

void
Man::processFrame ()
{
#ifdef USE_VISION
  //  This is called from Python right now
  vision.copyImage(sensors.getImage());
#endif
  PROF_EXIT(&profiler, P_GETIMAGE);

  PROF_ENTER(&profiler, P_FINAL);
#ifdef USE_VISION
  vision.notifyImage();
#endif

#ifdef DEBUG_BALL_DETECTION
  if (vision.ball->getWidth() > 0)
    printf("We see the ball!\n");
  else
    printf("No ball in this frame\n");
#endif

  // run Python behaviors
  noggin.runStep();

  PROF_EXIT(&profiler, P_FINAL);
  PROF_NFRAME(&profiler);

  PROF_ENTER(&profiler, P_GETIMAGE);

}


////////////////////////////////////////////
//                                        //
//  Library or runtime entry definitions  //
//                                        //
////////////////////////////////////////////


#ifndef MAN_IS_REMOTE
// Non-remote module
//   builds a shared library to be loaded at naoqi initialization

#  ifdef _WIN32
#    define ALCALL __declspec(dllexport)
#  else
#    define ALCALL
#  endif

#  ifdef __cplusplus
extern "C" {
#  endif

ALCALL int
_createModule (ALBroker *pBroker)
{

  // init broker with the main broker instance 
  // from the parent executable
  ALBroker::setInstance(pBroker);
      
  // create modules instance. This will register automatically to the broker
  lMan = new Man();
  // start Man in a new thread, so as to run the libraries main functions
  lMan->go();

  return 0;
}

ALCALL int
_closeModule ()
{
  // Delete module instance.  Will unregister automatically.
  if (lMan != NULL) {
    lMan->stop();
    delete lMan;
    lMan = NULL;
  }

  return 0;
}

#  ifdef __cplusplus
}
#  endif



#else
// MAN_IS_REMOTE defined
// module is a remote module, so built as an executable binary


void
_terminationHandler (int signum)
{
  if (signum == SIGINT) {
    // no direct exit, main thread will exit when finished
    cout << "Exiting Man." << endl;
    lMan->stop();
    delete lMan;
  }
  else
    // fault, exit immediately
    ::exit(1);
}

int
usage (const char *name)
{
  cout << "USAGE: " << name << endl
       << "\t-h \t\t: Display this help" << endl
       << "\t-b <ip> \t: Binding address of the server.  Default is 127.0.0.1" << endl
       << "\t-p <port> \t: Binding port of the server.  Default is 9559" << endl
       << "\t-pip <ip> \t: Address of the parent broker.  Default is 127.0.0.1" << endl
       << "\t-pport <ip> \t: Port of the parent broker.  Default is 9559" << endl;

  return 0;
}

int
main (int argc, char **argv)
{
  int  i = 1;
  std::string brokerName = "man";
  std::string brokerIP = "";
  int brokerPort = 0 ;
  // Default parent broker IP
  std::string parentBrokerIP = "127.0.0.1";
  // Default parent broker port
  int parentBrokerPort = kBrokerPort;
  
  // checking options
  while( i < argc ) {
    if ( argv[i][0] != '-' ) return usage( argv[0] );
    else if ( std::string( argv[i] ) == "-b" )        brokerIP          = std::string( argv[++i] );
    else if ( std::string( argv[i] ) == "-p" )        brokerPort        = atoi( argv[++i] );
    else if ( std::string( argv[i] ) == "-pip" )      parentBrokerIP    = std::string( argv[++i] );
    else if ( std::string( argv[i] ) == "-pport" )    parentBrokerPort  = atoi( argv[++i] );
    else if ( std::string( argv[i] ) == "-h" )        return usage( argv[0] );
    i++;
  }

  // If server port is not set
  if ( !brokerPort )
    brokerPort = FindFreePort( brokerIP );     

  std::cout << "Try to connect to parent Broker at ip :" << parentBrokerIP
            << " and port : " << parentBrokerPort << std::endl;
  //std::cout << "Start the server bind on this ip :  " << brokerIP
  //          << " and port : " << brokerPort << std::endl;

  // Starting Broker
  AL::ALBroker* broker = AL::ALBroker::getInstance(  );
  // init the broker with its ip and port, and the ip and port of a parent broker, if exist
  broker->init( brokerName, brokerIP, brokerPort, parentBrokerIP, parentBrokerPort );

# ifndef _WIN32
  struct sigaction new_action;
  // Set up the structure to specify the new action.
  new_action.sa_handler = _terminationHandler;
  sigemptyset( &new_action.sa_mask );
  new_action.sa_flags = 0;

  sigaction( SIGINT, &new_action, NULL );
#endif

  // Init Man. Module is automatically registered to the broker.
  lMan = new Man();
  //lMan->go();
  // run the man processes in the current thread
  lMan->run();

  if (lMan != NULL)
    delete lMan;

  // successful exit
  ::exit(0);
}

#endif // MAN_IS_REMOTE

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

  modifySysPath();
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


