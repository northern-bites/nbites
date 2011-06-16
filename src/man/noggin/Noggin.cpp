#include <Python.h>
#include <exception>
#include <boost/shared_ptr.hpp>

#include "Noggin.h"
#include "nogginconfig.h"
#include "PyLoc.h"
#include "EKFStructs.h"
#include <cstdlib>
#include "MMLocEKF.h"
#include "LocEKF.h"

#include "PySensors.h"
#include "PyRoboGuardian.h"
#include "PyMotion.h"
#include "PyLights.h"
#include "PySpeech.h"

//#define DEBUG_POST_OBSERVATIONS
//#define DEBUG_CORNER_OBSERVATIONS
//#define DEBUG_CROSS_OBSERVATIONS
//#define DEBUG_BALL_OBSERVATIONS
//#define DEBUG_TEAMMATE_BALL_OBSERVATIONS
#define USE_TEAMMATE_BALL_REPORTS
#define RUN_LOCALIZATION
#define USE_LOC_CORNERS
//#define DEBUG_CC_DETECTION_SAVE_FRAMES
static const float MAX_CORNER_DISTANCE = 150.0f;
static const float MAX_CROSS_DISTANCE = 150.0f;
using namespace std;
using namespace boost;

#ifdef LOG_LOCALIZATION
fstream outputFile;
#include <ctime>
#endif

#ifdef DEBUG_CC_DETECTION_SAVE_FRAMES
#include "ConcreteCorner.h"
#endif

const char * BRAIN_MODULE = "man.noggin.Brain";
const int TEAMMATE_FRAMES_OFF_THRESH = 5;
Noggin::Noggin (shared_ptr<Profiler> p, shared_ptr<Vision> v,
                shared_ptr<Comm> c, shared_ptr<RoboGuardian> rbg,
                shared_ptr<Sensors> _sensors, MotionInterface * _minterface)
    : profiler(p),
      vision(v),
      comm(c),
      gc(c->getGC()),
      sensors(_sensors),
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
    comm->add_to_module();

    // Initialize PyVision module
    c_init_vision();

    // Initialize localization stuff
    initializeLocalization();

}

void Noggin::initializeLocalization()
{
#   ifdef DEBUG_NOGGIN_INITIALIZATION
    printf("Initializing localization modules\n");
#   endif

    // Initialize the localization modules
#ifdef USE_MM_LOC_EKF
    loc = shared_ptr<LocSystem>(new MMLocEKF());
#else
    loc = shared_ptr<LocSystem>(new LocEKF());
#endif

    ballEKF = shared_ptr<BallEKF>(new BallEKF());

    // Setup the python localization wrappers
    set_loc_reference(loc);
    set_ballEKF_reference(ballEKF);
    c_init_localization();

    // Set the comm localization access pointers
    comm->setLocalizationAccess(loc, ballEKF);

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

#   ifdef USE_NOGGIN_AUTO_HALT
    // don't bother doing anything if there's a Python error and we
    // haven't reloaded
    if (error_state)
        return;
#   endif

    //Check button pushes for game controller signals
    processGCButtonClicks();

    PROF_ENTER(profiler, P_PYTHON);

#   ifdef RUN_LOCALIZATION
    // Update localization information
    PROF_ENTER(profiler, P_LOC);
    updateLocalization();
    PROF_EXIT(profiler, P_LOC);
#   endif //RUN_LOCALIZATION


    // Call main run() method of Brain
    PROF_ENTER(profiler, P_PYRUN);
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
    PROF_EXIT(profiler, P_PYRUN);

    PROF_EXIT(profiler, P_PYTHON);
}

void Noggin::updateLocalization()
{
    // Self Localization
    MotionModel odometery = motion_interface->getOdometryUpdate();

    // Build the observations from vision data
    vector<Observation> observations;
    // FieldObjects

    VisualFieldObject fo;
    fo = *vision->bgrp;

    if(fo.getDistance() > 0 && fo.getDistanceCertainty() != BOTH_UNSURE) {
        Observation seen(fo);
        observations.push_back(seen);
#       ifdef DEBUG_POST_OBSERVATIONS
        cout << "Saw bgrp at distance " << fo.getDistance()
             << " and bearing " << seen.getVisBearing() << endl;
#       endif
    }

    fo = *vision->bglp;
    if(fo.getDistance() > 0 && fo.getDistanceCertainty() != BOTH_UNSURE) {
        Observation seen(fo);
        observations.push_back(seen);
#       ifdef DEBUG_POST_OBSERVATIONS
        cout << "Saw bglp at distance " << fo.getDistance()
             << " and bearing " << seen.getVisBearing() << endl;
#       endif
    }

    fo = *vision->ygrp;
    if(fo.getDistance() > 0 && fo.getDistanceCertainty() != BOTH_UNSURE) {
        Observation seen(fo);
        observations.push_back(seen);
#       ifdef DEBUG_POST_OBSERVATIONS
        cout << "Saw ygrp at distance " << fo.getDistance()
             << " and bearing " << seen.getVisBearing() << endl;
#       endif
    }

    fo = *vision->yglp;
    if(fo.getDistance() > 0 && fo.getDistanceCertainty() != BOTH_UNSURE) {
        Observation seen(fo);
        observations.push_back(seen);
#       ifdef DEBUG_POST_OBSERVATIONS
        cout << "Saw yglp at distance " << fo.getDistance()
             << " and bearing " << seen.getVisBearing() << endl;
#       endif
    }

    // Corners
#   ifdef USE_LOC_CORNERS
    const list<VisualCorner> * corners = vision->fieldLines->getCorners();
    list <VisualCorner>::const_iterator i;
    for ( i = corners->begin(); i != corners->end(); ++i) {
        if (i->getDistance() < MAX_CORNER_DISTANCE) {
            Observation seen(*i);
            observations.push_back(seen);
#           ifdef DEBUG_CORNER_OBSERVATIONS
            cout << "Saw corner "
                 << ConcreteCorner::cornerIDToString(i->getID())
                 << " at distance "
                 << seen.getVisDistance() << " and bearing "
                 << seen.getVisBearing() << endl;
#           endif
#           ifdef DEBUG_CC_DETECTION_SAVE_FRAMES
            if (i->getShape() == CIRCLE) {
	      cout<< "saw cc" <<endl;
                sensors->saveFrame();
            }
#           endif
        }
    }
#   endif

    // Field Cross
    if (vision->cross->getDistance() > 0 &&
        vision->cross->getDistance() < MAX_CROSS_DISTANCE) {
        Observation seen(*vision->cross);
        observations.push_back(seen);
#       ifdef DEBUG_CROSS_OBSERVATIONS
        cout << "Saw cross "
             << vision->cross->getID()
             << " at distance " << vision->cross->getDistance()
             << " and bearing " << vision->cross->getBearing() << endl;
        //sensors->saveFrame();
#       endif
    }

    // Lines
	// const vector< shared_ptr<VisualLine> > * lines = vision->fieldLines->getLines();
    // vector <shared_ptr<VisualLine> >::const_iterator j;
    // for ( j = lines->begin(); j != lines->end(); ++j) {
	// 	if ( !(*j)->getCCLine() &&
	// 		 (*j)->getPossibleLines().size() < ConcreteLine::NUM_LINES) {
	// 		Observation seen(**j);
	// 		observations.push_back(seen);
	// 	}
    // }

    // Process the information
    PROF_ENTER(profiler, P_MCL);
    loc->updateLocalization(odometery, observations);
    PROF_EXIT(profiler, P_MCL);

    // Ball Tracking
    if (vision->ball->getDistance() > 0.0) {
        ballFramesOff = 0;
#   ifdef DEBUG_BALL_OBSERVATIONS
        cout << "Ball seen at distance " << vision->ball->getDistance()
             << " and bearing " << vision->ball->getBearing() << endl;
        //sensors->saveFrame();
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
        // If it's off for more then the threshold, then try and use mate data
        TeammateBallMeasurement n;
#       ifdef USE_TEAMMATE_BALL_REPORTS
        n = comm->getTeammateBallReport();
        if (!(n.ballX == 0.0 && n.ballY == 0.0) &&
            !(gc->gameState() == STATE_INITIAL ||
              gc->gameState() == STATE_FINISHED)) {
            m.distance = hypotf(loc->getXEst() - n.ballX,
                               loc->getYEst() - n.ballY);
            m.bearing = subPIAngle(atan2(n.ballY - loc->getYEst(),
                                         n.ballX - loc->getXEst()) -
                                   loc->getHEst());
            m.distanceSD = vision->ball->ballDistanceToSD(m.distance);
            m.bearingSD =  vision->ball->ballBearingToSD(m.bearing);
#           ifdef DEBUG_TEAMMATE_BALL_OBSERVATIONS
            cout << setprecision(4)
                 << "Using teammate ball report of (" << m.distance << ", "
                 << m.bearing << ")" << "\tReported x,y : "
                 << "(" << n.ballX << ", " << n.ballY << ")" << endl;
            cout << *ballEKF << endl;
#           endif
        }
#       endif
    }

    ballEKF->updateModel(m, loc->getCurrentEstimate());
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
void Noggin::processGCButtonClicks(){
    static const int ADVANCE_STATES_CLICKS  = 1;
    static const int SWITCH_TEAM_CLICKS  = 1;
    static const int SWITCH_KICKOFF_CLICKS  = 1;
    static const int REVERT_TO_INITIAL_CLICKS = 4;
    //cout << "In noggin chest clicks are " << chestButton->peekNumClicks() <<endl;

    if(chestButton->peekNumClicks() ==  ADVANCE_STATES_CLICKS){
        gc->advanceButtonClickState();
        chestButton->getAndClearNumClicks();
#       ifdef DEBUG_NOGGIN_GC
        cout << "Button pushing advanced GC to state : " << gc->gameState() <<endl;
#       endif
    }

    //Only toggle colors and kickoff when you are in initial
    if(gc->gameState() == STATE_INITIAL){
        if(leftFootButton->peekNumClicks() ==  SWITCH_TEAM_CLICKS){
            gc->toggleTeamColor();
            leftFootButton->getAndClearNumClicks();
#           ifdef DEBUG_NOGGIN_GC
            cout << "Button pushing switched GC to color : ";
            if(gc->color() == TEAM_BLUE)
                cout << "BLUE" <<endl;
            else
                cout << "RED" <<endl;

#           endif
        }

        if(rightFootButton->peekNumClicks() ==  SWITCH_KICKOFF_CLICKS){
            gc->toggleKickoff();
            rightFootButton->getAndClearNumClicks();
#ifdef DEBUG_NOGGIN_GC
            cout << " Button pushing switched GC to kickoff state : ";
            if(gc->kickOffTeam() == gc->color())
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
           gc->setGameState(STATE_INITIAL);
    }

}

void Noggin::modifySysPath ()
{
    // Enter the current working directory into the python module path
    //
#if ROBOT(NAO)
#  ifdef WEBOTS_BACKEND
     const string test = std::string(getenv("WEBOTS_HOME")) +
         std::string("/projects/contests") +
         std::string("/robotstadium/controllers/nao_team_1/lib");
     const char *cwd = test.c_str();
#  else //WEBOTS
#    if defined OFFLINE || defined STRAIGHT
       const char *cwd = "/usr/local/nao-1.6/modules/lib";
#    else
       const char *cwd = "/home/nao/naoqi/lib/naoqi";
#    endif
#  endif
#else//ROBOT(NAO)
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

#ifdef WEBOTS_BACKEND
    string s  = "./lib/man/noggin/" + string(buf) + ".loc";
#else
    string s  = "/home/nao/naoqi/log/" + string(buf) + ".loc";
#endif
    cout << "Started localization log at " << s << endl;
    outputFile.open(s.c_str(), ios::out);
    outputFile << (int)gc->color() << " " << (int)gc->player() << endl;
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
