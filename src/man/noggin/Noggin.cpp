#include <Python.h>
#include <cstdlib>
#include <exception>
#include <boost/shared_ptr.hpp>

#include "Noggin.h"
#include "PyLoc.h"

#include "EKFStructs.h"
#include <cstdlib>
#include "NaoPaths.h"

#include "Comm.h"
#include "GameData.h"

#include "PyComm.h"
#include "PyLoc.h"
#include "PySensors.h"
#include "PyRoboGuardian.h"
#include "PyMotion.h"
#include "PyLights.h"
#include "PySpeech.h"
#include "PyObjects.h"
#include "PyGoalie.h"

//#define DEBUG_POST_OBSERVATIONS
//#define DEBUG_CORNER_OBSERVATIONS
//#define DEBUG_CROSS_OBSERVATIONS
//#define DEBUG_BALL_OBSERVATIONS
//#define DEBUG_TEAMMATE_BALL_OBSERVATIONS
#define USE_TEAMMATE_BALL_REPORTS
#define RUN_LOCALIZATION
#define USE_LOC_CORNERS
#define DEBUG_NOGGIN_INITIALIZATION

static const float MAX_CORNER_DISTANCE = 400.0f;
static const float MAX_CROSS_DISTANCE = 200.0f;
static const unsigned int NUM_PYTHON_RESTARTS_MAX = 3;
using namespace std;
using namespace boost;

using namespace man::memory;
using namespace man::memory::log;

#ifdef LOG_LOCALIZATION
fstream outputFile;
#include <ctime>
#endif

const char * BRAIN_MODULE = "man.noggin.Brain";
const int TEAMMATE_FRAMES_OFF_THRESH = 5;
Noggin::Noggin (boost::shared_ptr<Vision> v,
                boost::shared_ptr<Comm> c, boost::shared_ptr<RoboGuardian> rbg,
                boost::shared_ptr<Sensors> _sensors, boost::shared_ptr<LoggingBoard> loggingBoard,
                MotionInterface * _minterface, man::memory::Memory::ptr memory
                )
    : vision(v),
      comm(c),
      sensors(_sensors),
      guard(rbg),
      loggingBoard(loggingBoard),
      memory(memory),
      chestButton(rbg->getButton(CHEST_BUTTON)),
      leftFootButton(rbg->getButton(LEFT_FOOT_BUTTON)),
      rightFootButton(rbg->getButton(RIGHT_FOOT_BUTTON)),
      error_state(false), brain_module(NULL), brain_instance(NULL),
      motion_interface(_minterface),registeredGCReset(false), ballFramesOff(0),
      do_reload(0)
{
#   ifdef DEBUG_NOGGIN_INITIALIZATION
    printf("Noggin::initializing\n");
#   endif

    // Initialize the interpreter and C python extensions
    initializePython();

    // import noggin.Brain and instantiate a Brain reference
    import_modules();

#   ifdef DEBUG_NOGGIN_INITIALIZATION
    printf("  Retrieving Brain.Brain instance\n");
#   endif

    // Instantiate a Brain instance
    getBrainInstance();

#   ifdef DEBUG_NOGGIN_INITIALIZATION
    printf("  DONE!\n");
#   endif
}

Noggin::~Noggin ()
{
    cout << "Noggin destructor" << endl;
    Py_XDECREF(brain_instance);
    Py_XDECREF(brain_module);
#   ifdef LOG_LOC
    stopLocLog();
#   endif
}

void Noggin::initializePython()
{
#   ifdef DEBUG_NOGGIN_INITIALIZATION
    printf("  Initializing interpreter and extension modules\n");
#   endif

    // Initialize localization stuff
    initializeLocalization();

    Py_Initialize();

    modifySysPath();

    brain_module = NULL;

    // Initialize low-level modules
    c_init_sensors();
    //init_leds();
    c_init_lights();
    c_init_speech();
    c_init_roboguardian();
    c_init_motion();
    c_init_comm();
    c_init_logging();

    c_init_vision();
    c_init_noggin_constants();
    c_init_objects();
    c_init_goalie();
}

void Noggin::initializeLocalization()
{
#   ifdef DEBUG_NOGGIN_INITIALIZATION
    printf("  Initializing localization modules\n");
#   endif

    locMotionSystem = shared_ptr<MotionSystem>(new MotionSystem());
    locVisionSystem = shared_ptr<VisionSystem>(new VisionSystem(vision));

    loc = shared_ptr<LocSystem>(new PF::ParticleFilter(locMotionSystem,
                                                       locVisionSystem,
                                                       memory->get<MLocalization>()));

    ballEKF = boost::shared_ptr<BallEKF>(new BallEKF());

    // Setup the python localization wrappers
    set_loc_reference(loc);
    set_ballEKF_reference(ballEKF);
    c_init_localization();

#   ifdef LOG_LOCALIZATION
    startLocLog();
#   endif
}

bool Noggin::import_modules ()
{
    // Load Brain module
    //
    if (brain_module == NULL) {
        // Import brain module
#       ifdef  DEBUG_NOGGIN_INITIALIZATION
        printf("  Importing noggin.Brain\n");
#       endif
        brain_module = PyImport_ImportModule(BRAIN_MODULE);
    }

    if (brain_module == NULL) {
        // error, couldn't import noggin.Brain
        fprintf(stderr, "Error importing noggin.Brain module\n");
        if (PyErr_Occurred())
            PyErr_Print();
        else
            fprintf(stderr, "  No Python exception information available\n");
        return false;
    }

    return true;
}

void Noggin::reload_hard ()
{
    printf("Reloading Noggin Python interpreter\n");
    // finalize and reinitialize the Python interpreter
    Py_Finalize();
    // load C extension modules
    initializePython();
    // import noggin.Brain and instantiate a Brain reference
    import_modules();
    // Instantiate a Brain instance
    getBrainInstance();
}

void Noggin::reload_brain ()
{
    if (brain_module == NULL)
        if (!import_modules())
            return;

    // reload Brain module
    PyImport_ReloadModule(brain_module);
    // Instantiate a Brain instance
    getBrainInstance();
}

void Noggin::reload_modules(std::string modules)
{
    if (brain_module == NULL)
        if (!import_modules())
            return;

    /* Not currently implemented */

    // reload Brain module
    PyImport_ReloadModule(brain_module);
    // Instantiate a Brain instance
    getBrainInstance();
}

void Noggin::getBrainInstance ()
{
    if (brain_module == NULL)
        if (!import_modules())
            return;

    // drop old reference
    Py_XDECREF(brain_instance);
    // Grab instantiate and hold a reference to a new noggin.Brain.Brain()
    PyObject *dict = PyModule_GetDict(brain_module);
    PyObject *brain_class = PyDict_GetItemString(dict, "Brain");
    if (brain_class != NULL)
        brain_instance = PyObject_CallObject(brain_class, NULL);
    else
        brain_instance = NULL;

    if (brain_instance == NULL) {
        fprintf(stderr, "Error accessing noggin.Brain.Brain\n");
        if (PyErr_Occurred())
            PyErr_Print();
        else
            fprintf(stderr, "  No error available\n");
    }

    // Successfully reloaded
    error_state = (brain_instance == NULL);
}

void Noggin::runStep ()
{
    static const int RELOAD_PYTHON_HARD = 6;
    //Check to see if we should reload python or not:
    if (chestButton->peekNumClicks() == RELOAD_PYTHON_HARD) {
        chestButton->getAndClearNumClicks();
        reload_hard();
    }

    static unsigned int num_crashed = 0;
    if (error_state && num_crashed < NUM_PYTHON_RESTARTS_MAX) {
        this->reload_hard();
        error_state = false;
        num_crashed++;
    }

    //Check button pushes for game controller signals
    processButtonClicks();

    PROF_ENTER(P_PYTHON);

#   ifdef RUN_LOCALIZATION
    // Update localization information
    PROF_ENTER(P_LOC);
    updateLocalization();
    PROF_EXIT(P_LOC);
#   endif //RUN_LOCALIZATION


    // Call main run() method of Brain
    PROF_ENTER(P_PYRUN);
    if (brain_instance != NULL) {
        PyObject *result = PyObject_CallMethod(brain_instance, "run", NULL);
        if (result == NULL) {
            // set Noggin in error state
            error_state = true;
            // report error
            fprintf(stderr, "Error occurred in noggin.Brain.run() method\n");
            if (PyErr_Occurred()) {
                PyErr_Print();
            } else {
                fprintf(stderr,
                        "  No Python exception information available\n");
            }
        } else {
            Py_DECREF(result);
        }
    }
    PROF_EXIT(P_PYRUN);

    updateComm();

    PROF_EXIT(P_PYTHON);
}

void Noggin::updateComm()
{
    comm->setLocData(0, loc->getXEst(), loc->getYEst(), loc->getHEst(),
                     loc->getXUncert(), loc->getYUncert(), loc->getHUncert());
    comm->setBallData(0, (float)vision->ball->isOn(),
                      ballEKF->getDistance(), ballEKF->getBearingDeg(),
                      0, 0);
}

void Noggin::updateLocalization()
{
    const ::MotionModel odometry = motion_interface->getOdometryUpdate();

    updateRobotFallenState(guard->isRobotFallen());

    locMotionSystem->motionUpdate(odometry);

//    std::vector<PF::Observation> observations;
//    std::vector<Landmark> landmarks;
    float dist, theta;

    // Get team of the robot for localization.
    GameData gd = comm->getGameData();
    int teamColor = gd.myTeamColor();

    // Observe Corners.
//    float phi;
//    const list<VisualCorner> * corners = vision->fieldLines->getCorners();
//    list <VisualCorner>::const_iterator ci;
//    for(ci = corners->begin(); ci != corners->end(); ++ci)
//    {
//        if (ci->getDistance() < MAX_CORNER_DISTANCE)
//    {
//        landmarks = constructLandmarks<VisualCorner, ConcreteCorner>(*ci);
//        dist = ci->getDistance();
//        theta = ci->getBearing();
//        phi = ci->getPhysicalOrientation();
//        observations.push_back(PF::CornerObservation(landmarks, dist,
//                                                     theta, phi));
//        }
//        else{
//            std::cout << "We saw a corner REALLY far away: "
//                      << ci->getDistance()<< " centimeters away" <<std::endl
//                      << "They can't be more than" << MAX_CORNER_DISTANCE
//                      << " centimeters away." << std::endl;
//        }
//    }

    // Update the localiztion vision interface with Observations.
//    locVisionSystem->feedObservations(observations);

    // Now, run the particle filter.
    MotionModel u_t;
    std::vector<PointObservation> pt_z;
    std::vector<CornerObservation> c_z;
    loc->updateLocalization(u_t, pt_z, c_z);

    // HACK HACK HACK for testing particles
//    loc->resetLocTo(500,321,-.349);

    // END LOCALIZATION UPDATE //

    // Ball Tracking
    if (vision->ball->getDistance() > 0.0) {
        ballFramesOff = 0;
#   ifdef DEBUG_BALL_OBSERVATIONS
        cout << "Ball seen at distance " << vision->ball->getDistance()
             << " and bearing " << vision->ball->getBearing() << endl;
#   endif
    } else {
        ++ballFramesOff;
    }

    RangeBearingMeasurement m;
    if( ballFramesOff < TEAMMATE_FRAMES_OFF_THRESH) {
        // If it's less than the threshold then we either see a ball or report
        // no ball seen
        RangeBearingMeasurement k(vision->ball);
        m = k;
    } else {

//   @TODO: 9/21/2012: This has to be reworked as Comm no longer
//        provides a TeammateBallMeasurement object.
//        // If it's off for more then the threshold, then try and use mate data
//         TeammateBallMeasurement n;
// #       ifdef USE_TEAMMATE_BALL_REPORTS
//         n = comm->getTeammateBallReport();
//         if (!(n.ballX == 0.0 && n.ballY == 0.0) &&
//             !(comm->getGameData().currentState() == STATE_INITIAL ||
//               comm->getGameData().currentState() == STATE_FINISHED)) {

//             m.distance = hypotf(loc->getXEst() - n.ballX,
//                                loc->getYEst() - n.ballY);
//             m.bearing = subPIAngle(atan2(n.ballY - loc->getYEst(),
//                                          n.ballX - loc->getXEst()) -
//                                    loc->getHEst());

//             m.distanceSD = vision->ball->ballDistanceToSD(m.distance);
//             m.bearingSD =  vision->ball->ballBearingToSD(m.bearing);

// #           ifdef DEBUG_TEAMMATE_BALL_OBSERVATIONS
//             cout << setprecision(4)
//                  << "Using teammate ball report of (" << m.distance << ", "
//                  << m.bearing << ")" << "\tReported x,y : "
//                  << "(" << n.ballX << ", " << n.ballY << ")" << endl;
//             cout << *ballEKF << endl;
// #           endif

//         }
// #       endif
    }

    ballEKF->updateModel(odometry, m, loc->getCurrentEstimate());

#   ifdef LOG_LOCALIZATION
    if (loggingLoc) {
        // Print out odometry and ball readings
        outputFile << odometery.deltaF << " " << odometery.deltaL << " "
                   << odometery.deltaR << " " << m.distance
                   << " " << m.bearing;
        // Print out observation information
        for (unsigned int x = 0; x < observations.size(); ++x) {
            // Separate observations with a colon
            outputFile << ":";
            outputFile << observations[x].getID() << " "
                       << observations[x].getVisDistance() << " "
                       << observations[x].getVisBearing() << " "
                       << observations[x].getDistanceSD() << " "
                       << observations[x].getBearingSD();
            if( observations[x].isLine() ) {
                vector<LineLandmark> ps;
                ps = observations[x].getLinePossibilities();
                for (unsigned int u = 0; u < ps.size(); ++u) {
                    outputFile << " " << ps[u];
                }
            } else {
                vector<PointLandmark> ps;
                ps = observations[x].getPointPossibilities();
                for (unsigned int u = 0; u < ps.size(); ++u) {
                    outputFile << " " << ps[u];
                }
            }
        }
        outputFile << endl;
    }
#   endif

}


//#define DEBUG_NOGGIN_GC
void Noggin::processButtonClicks(){
    static const int ADVANCE_STATES_CLICKS  = 1;
    static const int SWITCH_TEAM_CLICKS  = 1;
    static const int SWITCH_KICKOFF_CLICKS  = 1;
    static const int REVERT_TO_INITIAL_CLICKS = 4;
    //cout << "In noggin chest clicks are " << chestButton->peekNumClicks() <<endl;

    GameData gd = comm->getGameData();

    if(chestButton->peekNumClicks() ==  ADVANCE_STATES_CLICKS){
        gd.advanceState();
        chestButton->getAndClearNumClicks();
#       ifdef DEBUG_NOGGIN_GC
        cout << "Button pushing advanced GC to state : " << gd.currentState() <<endl;
#       endif
    }

    //Only toggle colors and kickoff when you are in initial
    if(gd.currentState() == STATE_INITIAL){
        if(leftFootButton->peekNumClicks() ==  SWITCH_TEAM_CLICKS){
            gd.switchTeams();
            leftFootButton->getAndClearNumClicks();
#           ifdef DEBUG_NOGGIN_GC
            cout << "Button pushing switched GC to color : ";
            if(gd.myTeamColor() == TEAM_BLUE)
                cout << "BLUE" <<endl;
            else
                cout << "RED" <<endl;

#           endif
        }

        if(rightFootButton->peekNumClicks() ==  SWITCH_KICKOFF_CLICKS){
            gd.toggleKickoff();
            rightFootButton->getAndClearNumClicks();
#ifdef DEBUG_NOGGIN_GC
            cout << " Button pushing switched GC to kickoff state : ";
            if(gd.ourKickoff())
                cout << "ON KICKOFF" <<endl;
            else
                cout << "OFF KICKOFF" <<endl;
#endif
        }
    }
    if( chestButton->peekNumClicks() == REVERT_TO_INITIAL_CLICKS){
#ifdef DEBUG_NOGGIN_CC
           cout << "SENDING GC TO INITIAL DUE TO CHEST BUTTONS"
                <<endl;
#endif
           chestButton->getAndClearNumClicks();
           gd.initial();
    }

}

void Noggin::modifySysPath ()
{
    // Enter the current working directory into the python module path
    //
#if defined OFFLINE || defined STRAIGHT
       string dir1 = NBITES_DIR"/build/qtool";
       string dir2 = NBITES_DIR"/build/qtool/man";
       const char* cwd = "";
#else
       const char *cwd = "/home/nao/nbites/lib";
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
#ifdef OFFLINE
        PyList_Append(path, PyString_FromString(dir1.c_str()));
        PyList_Append(path, PyString_FromString(dir2.c_str()));
#else
        PyList_Append(path, PyString_FromString(cwd));
#endif
        Py_DECREF(sys_module);
    }
}

#ifdef LOG_LOCALIZATION
void Noggin::startLocLog()
{
    if (loggingLoc) {
        return;
    }
    loggingLoc = true;

    time_t systime;
    struct tm * locTime;
    char buf[80];
    time( &systime );
    locTime = localtime( &systime );
    strftime(buf, 80, "%Y-%m-%d-%H-%M-%S",locTime);

    string s  = "/home/nao/nbites/log/" + string(buf) + ".loc";

    cout << "Started localization log at " << s << endl;
    outputFile.open(s.c_str(), ios::out);
    outputFile << (int)comm->getGameData().myTeamColor() << " "
               << (int)comm->myPlayerNumber() << endl;
    outputFile << loc->getXEst() << " " << loc->getYEst() << " "
               << loc->getHEst() << " "
               << loc->getXUncert() << " " << loc->getYUncert() << " "
               << loc->getHUncert() << " "
               << ballEKF->getXEst() << " " << ballEKF->getYEst() << " "
               << ballEKF->getXUncert() << " " << ballEKF->getYUncert() << " "
               << ballEKF->getXVelocityEst() << " "
               << ballEKF->getYVelocityEst() << " "
               << ballEKF->getXVelocityUncert() << " "
               << ballEKF->getYVelocityUncert() << endl;
}

void Noggin::stopLocLog()
{
    outputFile.close();
    loggingLoc = false;
}
#endif
