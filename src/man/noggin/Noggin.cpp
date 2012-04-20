#include <Python.h>
#include <exception>
#include <boost/shared_ptr.hpp>

#include "Noggin.h"
#include "nogginconfig.h"
#include "PyLoc.h"
#include "EKFStructs.h"
#include <cstdlib>
#include "MultiLocEKF.h"
#include "NaoPaths.h"

#include "PySensors.h"
#include "PyRoboGuardian.h"
#include "PyMotion.h"
#include "PyLights.h"
#include "PySpeech.h"
#include "PyObjects.h"

//#define DEBUG_POST_OBSERVATIONS
//#define DEBUG_CORNER_OBSERVATIONS
//#define DEBUG_CROSS_OBSERVATIONS
//#define DEBUG_BALL_OBSERVATIONS
//#define DEBUG_TEAMMATE_BALL_OBSERVATIONS
#define USE_TEAMMATE_BALL_REPORTS
#define RUN_LOCALIZATION
#define USE_LOC_CORNERS
static const float MAX_CORNER_DISTANCE = 150.0f;
static const float MAX_CROSS_DISTANCE = 150.0f;
static const unsigned int NUM_PYTHON_RESTARTS_MAX = 3;
using namespace std;
using namespace boost;

using namespace man::memory::log;

#ifdef LOG_LOCALIZATION
fstream outputFile;
#include <ctime>
#endif

const char * BRAIN_MODULE = "man.noggin.Brain";
const int TEAMMATE_FRAMES_OFF_THRESH = 5;
Noggin::Noggin (shared_ptr<Vision> v,
                shared_ptr<Comm> c, shared_ptr<RoboGuardian> rbg,
                shared_ptr<Sensors> _sensors, shared_ptr<LoggingBoard> loggingBoard,
                MotionInterface * _minterface
                )
    : vision(v),
      comm(c),
      gc(c->getGC()),
      sensors(_sensors),
      loggingBoard(loggingBoard),
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
    comm->add_to_module();

    // Initialize PyVision module
    c_init_vision();

    // Initlialize PyConstants module
    c_init_noggin_constants();

    // Initialize PyLocation module
    c_init_objects();
}

void Noggin::initializeLocalization()
{
#   ifdef DEBUG_NOGGIN_INITIALIZATION
    printf("Initializing localization modules\n");
#   endif

    // Initialize the localization module
    loc = shared_ptr<LocSystem>(new MultiLocEKF());

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
    static unsigned int num_crashed = 0;
    if (error_state && num_crashed < NUM_PYTHON_RESTARTS_MAX) {
        this->reload_hard();
        error_state = false;
        num_crashed++;
    }
#   endif

    //Check button pushes for game controller signals
    processGCButtonClicks();

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

    PROF_EXIT(P_PYTHON);
}

void Noggin::updateLocalization()
{
    // Self Localization
    MotionModel odometery = motion_interface->getOdometryUpdate();

    // Build the observations from vision data
    vector<PointObservation> pt_observations;
    vector<CornerObservation> corner_observations;

    // Get team of the robot for localization.
    uint8 teamColor = (*gc->getMyTeam()).teamColor;

    // FieldObjects

    VisualFieldObject fo;
    // We should now never see a blue goal post. 
    // fo = *vision->bgrp;

    // if(fo.getDistance() > 0 && fo.getDistanceCertainty() != BOTH_UNSURE) {
    //     PointObservation seen(fo);
    //     pt_observations.push_back(seen);
    // }

    // fo = *vision->bglp;
    // if(fo.getDistance() > 0 && fo.getDistanceCertainty() != BOTH_UNSURE) {
    //     PointObservation seen(fo);
    //     pt_observations.push_back(seen);
    // }

    // If the robot is on the opposing side, the CLOSER goal posts
    // are the opposing ("blue") goal posts. Otherwise, the closer
    // posts are their own posts.
    bool bluePost = false;
    fo = *vision->ygrp;
    if(fo.getDistance() > 0 && fo.getDistanceCertainty() != BOTH_UNSURE) 
    {
      if(loc->isOnOpposingSide())
      {
	if(fo.getDistance() < (FIELD_WHITE_WIDTH * 0.5f))
	{
	  const std::list<const ConcreteFieldObject *> * possibleFieldObjects = 
	    (teamColor == TEAM_RED) ?
	    &ConcreteFieldObject::blueGoalRightPostList
	    : &ConcreteFieldObject::yellowGoalRightPostList;

	  fo.setPossibleFieldObjects(possibleFieldObjects);
	  std::cout << "See opposing yellow right post (" << fo.toString() << ") at distance "
		    << fo.getDistance() << " (offender.)" << std::endl;
	  bluePost = true;
	}
	else
	{
	  std::cout << "See own yellow right post (" << fo.getID() << ") at distance "
		    << fo.getDistance() << " (offender.)" << std::endl;
	}
      }
      else
      {
	if(fo.getDistance() > (FIELD_WHITE_WIDTH * 0.5f))
	{
	  const std::list<const ConcreteFieldObject *> * possibleFieldObjects = 
	    (teamColor == TEAM_RED) ?
	    &ConcreteFieldObject::blueGoalRightPostList
	    : &ConcreteFieldObject::yellowGoalRightPostList;

	  fo.setPossibleFieldObjects(possibleFieldObjects);
	  std::cout << "See opposing yellow right post (" << fo.getID() << ") at distance "
		    << fo.getDistance() << " (defender.)" << std::endl;
	  bluePost = true;
      	}
	else
	{
	  std::cout << "See own yellow right post (" << fo.getID() << ") at distance "
		    << fo.getDistance() << " (defender.)" << std::endl;
	}
      }
      PointObservation seen(fo);
      pt_observations.push_back(seen);
    }

    fo = *vision->yglp;
    if(fo.getDistance() > 0 && fo.getDistanceCertainty() != BOTH_UNSURE) 
    {
      if(loc->isOnOpposingSide())
      {
	if(fo.getDistance() < (0.5f * FIELD_WHITE_WIDTH))
	{
	  const std::list<const ConcreteFieldObject *> * possibleFieldObjects = 
	    &ConcreteFieldObject::blueGoalLeftPostList;
	  fo.setPossibleFieldObjects(possibleFieldObjects);
	  std::cout << "See opposing yellow left post (" << fo.getID() << ") at distance "
		    << fo.getDistance() << " (offender.)" << std::endl;
	  bluePost = true;
	}
	else
	{
	  std::cout << "See own yellow left post (" << fo.getID() << ") at distance "
		    << fo.getDistance() << " (offender.)" << std::endl;
	}
      }
      else
      {
	if(fo.getDistance() > (0.5f * FIELD_WHITE_WIDTH))
	{
	  const std::list<const ConcreteFieldObject *> * possibleFieldObjects = 
	    &ConcreteFieldObject::blueGoalLeftPostList;
	  fo.setPossibleFieldObjects(possibleFieldObjects);
	  std::cout << "See opposing yellow left post (" << fo.getID() << ") at distance "
		    << fo.getDistance() << " (defender.)" << std::endl;	
	  bluePost = true;
	}
	else
	{
	  std::cout << "See own yellow left post (" << fo.getID() << ") at distance "
		    << fo.getDistance() << " (defender.)" << std::endl;
	}
      }
        PointObservation seen(fo);
        pt_observations.push_back(seen);
    }

    if(pt_observations.size() > 0)
    {
      std::cout << "Looking at " << (bluePost ? "BLUE " : "YELLOW ") << " posts." << std::endl;
    }

    vector<PointObservation>::iterator obsIter;
    for(obsIter = pt_observations.begin(); obsIter != pt_observations.end(); ++obsIter)
    {
      std::cout << "Spotted post: " << *obsIter << std::endl;
      std::cout << "Possibilities: " << std::endl;
      std::vector<PointLandmark> p = (*obsIter).getPossibilities();
      std::vector<PointLandmark>::iterator goalIter = p.begin();
      for(; goalIter != p.end(); ++goalIter)
	std::cout << *goalIter << std::endl;
    }

    // Field Cross
    if (vision->cross->getDistance() > 0 &&
        vision->cross->getDistance() < MAX_CROSS_DISTANCE) {

        PointObservation seen(*vision->cross);
        pt_observations.push_back(seen);
#       ifdef DEBUG_CROSS_OBSERVATIONS
        cout << "Saw cross " << pt_observations.back() << endl;
#       endif
    }

    // Corners
#   ifdef USE_LOC_CORNERS
    const list<VisualCorner> * corners = vision->fieldLines->getCorners();
    list <VisualCorner>::const_iterator i;
    for ( i = corners->begin(); i != corners->end(); ++i) {
        if (i->getDistance() < MAX_CORNER_DISTANCE) {
            CornerObservation seen(*i);
            corner_observations.push_back(seen);

#           ifdef DEBUG_CORNER_OBSERVATIONS
            cout << "Saw corner "
                 << ConcreteCorner::cornerIDToString(i->getID())
                 << " at distance "
                 << seen.getVisDistance() << " and bearing "
                 << seen.getVisBearing() << endl;
#           endif
        }
    }
#   endif

    // Process the information
    PROF_ENTER(P_MCL);
    loc->updateLocalization(odometery, pt_observations, corner_observations);
    PROF_EXIT(P_MCL);

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

    ballEKF->updateModel(odometery, m, loc->getCurrentEstimate());

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
#  ifdef WEBOTS_BACKEND
     const string test = std::string(getenv("WEBOTS_HOME")) +
         std::string("/projects/contests") +
         std::string("/robotstadium/controllers/nao_team_1/lib");
     const char *cwd = test.c_str();
#  else //WEBOTS
#    if defined OFFLINE || defined STRAIGHT
       string dir1 = NBITES_DIR"/build/qtool";
       string dir2 = NBITES_DIR"/build/qtool/man";
       const char* cwd = "";
#    else
       const char *cwd = "/home/nao/nbites/lib";
#    endif
#  endif

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

#ifdef WEBOTS_BACKEND
    string s  = "./lib/man/noggin/" + string(buf) + ".loc";
#else
    string s  = "/home/nao/nbites/log/" + string(buf) + ".loc";
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

