#include <Python.h>
#include <boost/python.hpp>
#include <boost/python/object.hpp>
#include <boost/python/module.hpp>
#include <boost/python/args.hpp>
using namespace std;
using namespace boost::python;

#include "BodyJointCommand.h"
#include "HeadJointCommand.h"
#include "HeadScanCommand.h"
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


class PyHeadScanCommand {
public:
    /**
     * The constructor takes a list of headJointCommands and whether they
     * should be executed forever in a sequence
     */
    PyHeadScanCommand(boost::python::list headJointCommands,
                      bool forever = false) {
        vector<const HeadJointCommand*> *commands =
            new vector<const HeadJointCommand*>;

        unsigned int listLength =
            extract<unsigned int>(headJointCommands.attr("__len__")());
        for (unsigned int i = 0; i < listLength; i++) {
            PyHeadJointCommand cmd =
                extract<PyHeadJointCommand>(headJointCommands.pop(0));
            commands->push_back(cmd.getCommand());
        }

        command = new HeadScanCommand(commands, forever);
    }

    HeadScanCommand* getCommand() const { return command; }

private:
    HeadScanCommand *command;
};

class PyWalkCommand {
public:
    PyWalkCommand(float x_mms, float m_mms, float theta_rads) {
        command = new WalkCommand(x_mms, m_mms, theta_rads);
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
    void enqueue(const PyHeadScanCommand *command) {
        motionInterface->enqueue(command->getCommand());
    }
    void enqueue(const PyBodyJointCommand *command) {
        motionInterface->enqueue(command->getCommand());
    }

    void setNextWalkCommand(const PyWalkCommand *command) {
        motionInterface->setNextWalkCommand(command->getCommand());
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
void (PyMotionInterface::*enq2)(const PyHeadScanCommand*)  =
    &PyMotionInterface::enqueue;
void (PyMotionInterface::*enq3)(const PyBodyJointCommand*) =
    &PyMotionInterface::enqueue;


BOOST_PYTHON_MODULE(_motion)
{
    class_<PyHeadJointCommand>("HeadJointCommand",
                               init<float, tuple, int>(
 "A container for a head joint command passed to the motion engine"))
        ;
    class_<PyHeadScanCommand>("HeadScanCommand",
                              init<boost::python::list, optional<bool> >(
 "A container for a head scan command passed to the motion engine. It is"
 " composed of several head joint commands which are executed in succession."
 " An optional second parameter indicates whether the command should execute"
 " forever unless it is stopped manually"))
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
        .def("enqueue", enq3)
        .def("setNextWalkCommand", &PyMotionInterface::setNextWalkCommand)
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

