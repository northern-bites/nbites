#include <Python.h>
#include <boost/python.hpp>
#include <boost/python/object.hpp>
#include <boost/python/module.hpp>
#include <boost/python/args.hpp>
using namespace std;
using namespace boost::python;

#include "BodyJointCommand.h"
#include "HeadJointCommand.h"
#include "WalkCommand.h"
#include "MotionInterface.h"


static MotionInterface* interface_reference = 0;

class PyHeadJointCommand {
public:
    PyHeadJointCommand(float time, tuple joints, int interpolationType) {
        vector<float> * jointsVector = new vector<float>;

        // Head joint commands are sent in as degree values and we deal with
        // radians in the motion engine. They get converted here.
        for (unsigned int i=0; i < Kinematics::HEAD_JOINTS; i++)
            jointsVector->push_back(extract<float>(joints[i]) * TO_RAD);

        command = new HeadJointCommand(time, jointsVector,
                      static_cast<InterpolationType>(interpolationType));
    }

    HeadJointCommand* getCommand() const { return command; }

private:
    HeadJointCommand *command;
};

class PyGaitCommand {
public:
    /**
     * The constructor takes a list of headJointCommands and whether they
     * should be executed forever in a sequence
     */
    PyGaitCommand( const float _bh,
                   const float _hox, const float _dur,
                   const float _dblSupFrac, const float _stepHeight,
                   const float _footLengthX, const float _dblInactivePerc,
                   const float _lSwHRAdd,const float _rSwHRAdd,
                   const float _lZMPSwOffY,const float _rZMPSwOffY){
        command = new GaitCommand(0.02,_bh, //HACK
                                  _hox, _dur,
                                  _dblSupFrac, _stepHeight,
                                  _footLengthX, _dblInactivePerc,
                                  _lSwHRAdd,_rSwHRAdd,
                                  _lZMPSwOffY,_rZMPSwOffY);
    }

    GaitCommand* getCommand() const { return command; }

private:
    GaitCommand *command;
};


class PyWalkCommand {
public:
    PyWalkCommand(float x_cms, float m_cms, float theta_degs) {
        //All python units should be in CM and DEG per second
        //C++ is in mm and rads, so we need to convert
        command = new WalkCommand(x_cms*CM_TO_MM,
                                  m_cms*CM_TO_MM,
                                  theta_degs*TO_RAD);
    }

    WalkCommand* getCommand() const { return command; }

private:
    WalkCommand *command;
};


class PyBodyJointCommand {
public:
    PyBodyJointCommand(float time,
                       tuple larmJoints, tuple llegJoints,
                       tuple rlegJoints, tuple rarmJoints,
                       int interpolationType) {
        vector<float> * larm = new vector<float>;
        vector<float> * lleg = new vector<float>;
        vector<float> * rleg = new vector<float>;
        vector<float> * rarm = new vector<float>;

        // The joints come in in degrees. Convert them to radians here
        for (unsigned int i=0; i < Kinematics::ARM_JOINTS; i++)
            larm->push_back(extract<float>(larmJoints[i]) * TO_RAD);

        for (unsigned int i=0; i < Kinematics::LEG_JOINTS; i++)
            lleg->push_back(extract<float>(llegJoints[i]) * TO_RAD);

        for (unsigned int i=0; i < Kinematics::LEG_JOINTS; i++)
            rleg->push_back(extract<float>(rlegJoints[i]) * TO_RAD);

        for (unsigned int i=0; i < Kinematics::ARM_JOINTS; i++)
            rarm->push_back(extract<float>(rarmJoints[i]) * TO_RAD);

        command = new BodyJointCommand(time, larm, lleg, rleg, rarm,
                      static_cast<InterpolationType>(interpolationType));
    }

    BodyJointCommand* getCommand() const { return command; }

private:
    BodyJointCommand *command;
};


class PyMotionInterface {
public:
    PyMotionInterface() {
        motionInterface = interface_reference;
    }

    void enqueue(const PyHeadJointCommand *command) {
        motionInterface->enqueue(command->getCommand());
    }
    void enqueue(const PyBodyJointCommand *command) {
        motionInterface->enqueue(command->getCommand());
    }

    void setNextWalkCommand(const PyWalkCommand *command) {
        motionInterface->setNextWalkCommand(command->getCommand());
    }

    void setGait(const PyGaitCommand *command) {
        motionInterface->setGait(command->getCommand());
    }

    bool isWalkActive() {
        return motionInterface->isWalkActive();
    }

    void stopBodyMoves() {
        motionInterface->stopBodyMoves();
    }

    void stopHeadMoves() {
        motionInterface->stopHeadMoves();
    }

private:
    MotionInterface *motionInterface;
};

/**
 * If you want to expose an overloaded function to python, you have to create
 * pointers to each method which has a different signature and then add each
 * of these separately to the module.
 */
void (PyMotionInterface::*enq1)(const PyHeadJointCommand*) =
    &PyMotionInterface::enqueue;
void (PyMotionInterface::*enq2)(const PyBodyJointCommand*) =
    &PyMotionInterface::enqueue;


BOOST_PYTHON_MODULE(_motion)
{
    class_<PyHeadJointCommand>("HeadJointCommand",
                               init<float, tuple, int>(
 "A container for a head joint command passed to the motion engine"))
        ;
    class_<PyGaitCommand>("GaitCommand",
                          init<
                          float, float, float, float,
                          float, float, float, float,
                          float, float, float>(
"A container for setting the walking gait of the robot. "
"(bodyHeight,hipOffsetX,stepDuration,doubleSupportFraction,stepHeight,"
"footLengthX,dblSupInactivePercentage,leftSwingHipRollAddition,"
"rightSwingHipRollAddition,leftZMPSwingOffsetY,rightZMPSwingOffsetY"))
        ;
    class_<PyBodyJointCommand>("BodyJointCommand",
                               init<float, tuple, tuple, tuple, tuple, int>(
 "A container for a body joint command passed to the motion engine"))
        ;
    class_<PyWalkCommand>("WalkCommand",
                          init<float, float, float>(args("x","y","theta"),
 "A container for a walk command. Holds an x, y and theta which represents a"
 " walk vector"))
        ;

    class_<PyMotionInterface>("MotionInterface")
        .def("enqueue", enq1)
        .def("enqueue", enq2)
        .def("setNextWalkCommand", &PyMotionInterface::setNextWalkCommand)
        .def("setGait", &PyMotionInterface::setGait)
        .def("isWalkActive", &PyMotionInterface::isWalkActive)
        .def("stopBodyMoves", &PyMotionInterface::stopBodyMoves)
        .def("stopHeadMoves", &PyMotionInterface::stopHeadMoves)
        ;
}


void c_init_motion () {
    if (!Py_IsInitialized())
        Py_Initialize();

    try {
        init_motion();
    } catch (error_already_set) {
        PyErr_Print();
    }

}

void set_motion_interface (MotionInterface *_interface)
{
  interface_reference = _interface;
}

