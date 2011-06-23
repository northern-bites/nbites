
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
//For gaitcommands

#define BOOST_PYTHON_MAX_ARITY 28

#include <Python.h>
#include <boost/python.hpp>
#include <boost/python/object.hpp>
#include <boost/python/module.hpp>
#include <boost/python/args.hpp>
#include <boost/shared_ptr.hpp>
using namespace std;
using namespace boost::python;

#include "BodyJointCommand.h"
#include "HeadJointCommand.h"
#include "SetHeadCommand.h"
#include "CoordHeadCommand.h"
#include "WalkCommand.h"
#include "Gait.h"
#include "MotionInterface.h"
#include "Kinematics.h"
#include "StepCommand.h"
using namespace Kinematics;

static MotionInterface* interface_reference = 0;

class PyHeadJointCommand {
public:
    typedef boost::shared_ptr<PyHeadJointCommand> ptr;

    PyHeadJointCommand(float time, tuple joints,
		       tuple stiffness, int interpolationType) {

        vector<float> * jointsVector = new vector<float>;
        vector<float> * head_stiffness = new vector<float>;
        // Head joint commands are sent in as degree values and we deal with
        // radians in the motion engine. They get converted here.
        for (unsigned int i=0; i < Kinematics::HEAD_JOINTS; i++)
            jointsVector->push_back(extract<float>(joints[i]) * TO_RAD);

	for (unsigned int i=0; i < Kinematics::HEAD_JOINTS; i++)
	    head_stiffness->push_back(extract<float>(stiffness[i]));

        command =
	    HeadJointCommand::ptr (
		new HeadJointCommand(time, jointsVector, head_stiffness,
				     static_cast<InterpolationType>(interpolationType)) 
		);
    }

    HeadJointCommand::ptr getCommand() const { return command; }

private:
    HeadJointCommand::ptr command;
};

class PyGaitCommand {
public:
    PyGaitCommand(const tuple &_stance_config,
                  const tuple &_step_config,
                  const tuple &_zmp_config,
                  const tuple &_joint_hack_config,
                  const tuple &_sensor_config,
                  const tuple &_stiffness_config,
                  const tuple &_odo_config,
                  const tuple &_arm_config) {

        float stance[WP::LEN_STANCE_CONFIG];
        fillArray<float,WP::LEN_STANCE_CONFIG>
            (stance,_stance_config,WP::STANCE_CONVERSION);

        fillArray<float,WP::LEN_STEP_CONFIG>
	    (step,_step_config,WP::STEP_CONVERSION);

        float zmp[WP::LEN_ZMP_CONFIG];
        fillArray<float,WP::LEN_ZMP_CONFIG>
            (zmp,_zmp_config,WP::ZMP_CONVERSION);

        float hack[WP::LEN_HACK_CONFIG];
        fillArray<float,WP::LEN_HACK_CONFIG>
            (hack,_joint_hack_config,WP::HACK_CONVERSION);

        float sensor[WP::LEN_SENSOR_CONFIG];
        fillArray<float,WP::LEN_SENSOR_CONFIG>
            (sensor,_sensor_config,WP::SENSOR_CONVERSION);

        float stiffness[WP::LEN_STIFF_CONFIG];
        fillArray<float,WP::LEN_STIFF_CONFIG>
            (stiffness,_stiffness_config,WP::STIFF_CONVERSION);

        float odo[WP::LEN_ODO_CONFIG];
        fillArray<float,WP::LEN_ODO_CONFIG>
            (odo,_odo_config,WP::ODO_CONVERSION);

        float arm[WP::LEN_ARM_CONFIG];
        fillArray<float,WP::LEN_ARM_CONFIG>
            (arm,_arm_config,WP::ARM_CONVERSION);

        command = Gait::ptr (
	    new Gait(stance,step,
		     zmp,
		     hack,
		     sensor,
		     stiffness,
		     odo,
		     arm));
    }

    Gait::ptr getCommand() const { return command; }

    template <class T, const unsigned int size> void
    fillArray( T target[size], tuple t, const T convert_units[size]){
        for(unsigned int i = 0; i < size; i++){
            target[i] = extract<T>(t[i])*convert_units[i];
        }
    }

    float getStepValue(int i) {
	if (i < WP::LEN_STEP_CONFIG) {
	    return step[i];
	}
	return 0;
    }

private:
    Gait::ptr command;
    float step[WP::LEN_STEP_CONFIG];
};

class PyWalkCommand {
public:
    PyWalkCommand(float x_cms, float m_cms, float theta_degs) {
        //All python units should be in CM and DEG per second
        //C++ is in mm and rads, so we need to convert
        command = WalkCommand::ptr(
	    new WalkCommand(x_cms*CM_TO_MM,
			    m_cms*CM_TO_MM,
			    theta_degs*TO_RAD)
	    );
    }

    WalkCommand::ptr getCommand() const { return command; }

private:
    WalkCommand::ptr command;
};

class PyStepCommand {
public:
    PyStepCommand(float x_cms, float m_cms, float theta_degs, int numStep) {
        //All python units should be in CM and DEG per second
        //C++ is in mm and rads, so we need to convert
        command =
            StepCommand::ptr(new StepCommand(x_cms*CM_TO_MM,
					     m_cms*CM_TO_MM,
					     theta_degs*TO_RAD,
					     numStep));
    }

    StepCommand::ptr getCommand() const { return command; }

private:
    StepCommand::ptr command;
};

class PyBodyJointCommand {
public:
    typedef boost::shared_ptr<PyBodyJointCommand> ptr;

    PyBodyJointCommand(float time,
                       tuple larmJoints, tuple llegJoints,
                       tuple rlegJoints, tuple rarmJoints,
		       tuple stiffness,
                       int interpolationType) {
        vector<float> * larm = new vector<float>;
        vector<float> * lleg = new vector<float>;
        vector<float> * rleg = new vector<float>;
        vector<float> * rarm = new vector<float>;
	vector<float> * body_stiffness = new vector<float>;

        // The joints come in in degrees. Convert them to radians here
        for (unsigned int i=0; i < Kinematics::ARM_JOINTS; i++)
            larm->push_back(extract<float>(larmJoints[i]) * TO_RAD);

        for (unsigned int i=0; i < Kinematics::LEG_JOINTS; i++)
            lleg->push_back(extract<float>(llegJoints[i]) * TO_RAD);

        for (unsigned int i=0; i < Kinematics::LEG_JOINTS; i++)
            rleg->push_back(extract<float>(rlegJoints[i]) * TO_RAD);

        for (unsigned int i=0; i < Kinematics::ARM_JOINTS; i++)
            rarm->push_back(extract<float>(rarmJoints[i]) * TO_RAD);

	for (unsigned int i=0; i < Kinematics::NUM_JOINTS; i++)
	    body_stiffness->push_back(extract<float>(stiffness[i]));

        command = BodyJointCommand::ptr (
	    new BodyJointCommand(time, larm, lleg, rleg, rarm,
				 body_stiffness,
				 static_cast<InterpolationType>(interpolationType))
	    );
    }

    // Single chain command
    PyBodyJointCommand(float time,
		       int chainID, tuple chainJoints,
		       tuple stiffness,
		       int interpolationType) {
        vector<float> * chain = new vector<float>;
	vector<float> * body_stiffness = new vector<float>;

        // The joints come in in degrees. Convert them to radians here
        for (unsigned int i=0; i < chain_lengths[chainID] ; i++)
            chain->push_back(extract<float>(chainJoints[i]) * TO_RAD);

	for (unsigned int i=0; i < Kinematics::NUM_BODY_JOINTS; i++)
	    body_stiffness->push_back(extract<float>(stiffness[i]));

        command = BodyJointCommand::ptr (
	    new BodyJointCommand(time, static_cast<ChainID>(chainID),
				 chain, body_stiffness,
				 static_cast<InterpolationType>(interpolationType))
	    );
    }

    BodyJointCommand::ptr getCommand() const { return command; }

private:
    BodyJointCommand::ptr command;
};

class PySetHeadCommand {
public:
    PySetHeadCommand(const float yaw, const float pitch) {
        command = SetHeadCommand::ptr (
	    new SetHeadCommand(yaw * TO_RAD, pitch * TO_RAD)
	    );
    }

    PySetHeadCommand(const float yaw, const float pitch,
		     const float maxYawSpeed, const float maxPitchSpeed) {
        command = SetHeadCommand::ptr(
	    new SetHeadCommand(yaw * TO_RAD, pitch * TO_RAD,
			       maxYawSpeed * TO_RAD, maxPitchSpeed * TO_RAD)
	    );
    }

    SetHeadCommand::ptr getCommand() const { return command; }
private:
    SetHeadCommand::ptr command;
};

class PyCoordHeadCommand {
public:
    PyCoordHeadCommand( const float _x, const float _y, const float _z) {
	command = CoordHeadCommand::ptr (
	    new CoordHeadCommand(_x,_y,_z)
	    );
    }
    PyCoordHeadCommand( const float _x, const float _y, const float _z,
                        const float maxYawSpeed,
                        const float maxPitchSpeed ) {
	command = CoordHeadCommand::ptr(
	    new CoordHeadCommand( _x, _y, _z,
				  maxYawSpeed, maxPitchSpeed )
	    );
    }

    CoordHeadCommand::ptr getCommand() const { return command; }
private:
    CoordHeadCommand::ptr command;
};

class PyFreezeCommand{
public:
    //Later, one could add more specific stiffness options
    PyFreezeCommand()
        :command(FreezeCommand::ptr(new FreezeCommand())){}
    const FreezeCommand::ptr getCommand() const{return command;}
private:
    FreezeCommand::ptr command;
};

class PyUnfreezeCommand{
public:
    //Later, one could add more specific stiffness options
    PyUnfreezeCommand(float stiffness)
        :command(UnfreezeCommand::ptr(new UnfreezeCommand())){}
    const UnfreezeCommand::ptr getCommand()const{return command;}
private:
    UnfreezeCommand::ptr command;
};

/* All the Py<>Command pointers are ref-counted by Python so they don't need to
   to be shared_ptrs here */
class PyMotionInterface {
public:
    PyMotionInterface() {
        motionInterface = interface_reference;
    }

    void enqueue(const PyHeadJointCommand* command) {
        motionInterface->enqueue(command->getCommand());
    }
    void enqueue(const PyBodyJointCommand* command) {
        motionInterface->enqueue(command->getCommand());
    }

    void setNextWalkCommand(const PyWalkCommand *command) {
        motionInterface->setNextWalkCommand(command->getCommand());
    }
    void sendStepCommand(const PyStepCommand *command) {
        motionInterface->sendStepCommand(command->getCommand());
    }
    void setGait(const PyGaitCommand *command) {
        motionInterface->setGait(command->getCommand());
    }
    void setHead(const PySetHeadCommand *command) {
        motionInterface->setHead(command->getCommand());
    }
    void coordHead(const PyCoordHeadCommand *command) {
        motionInterface->coordHead(command->getCommand());
    }
    void sendFreezeCommand(const PyFreezeCommand *command){
        motionInterface->sendFreezeCommand(command->getCommand());
    }
    void sendFreezeCommand(const PyUnfreezeCommand *command){
        motionInterface->sendFreezeCommand(command->getCommand());
    }
    bool isWalkActive() {
        return motionInterface->isWalkActive();
    }

    bool isBodyActive() {
	return motionInterface->isBodyActive();
    }

    void resetWalkProvider(){
        motionInterface->resetWalkProvider();
    }

    void resetScriptedProvider(){
        motionInterface->resetScriptedProvider();
    }

    bool isHeadActive(){
        return motionInterface->isHeadActive();
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

void (PyMotionInterface::*frz1)(const PyFreezeCommand*) =
    &PyMotionInterface::sendFreezeCommand;
void (PyMotionInterface::*frz2)(const PyUnfreezeCommand*) =
    &PyMotionInterface::sendFreezeCommand;


BOOST_PYTHON_MODULE(_motion)
{
    class_<PyHeadJointCommand>("HeadJointCommand",
                               init<float, tuple, tuple, int>(
				   "A container for a head joint command passed to the motion engine"))
        ;
    class_<PyGaitCommand>("GaitCommand",
			  init<
			  tuple, tuple, tuple, tuple,
			  tuple, tuple, tuple, tuple>
			  (args("stance,step,zmp,joint_hack,sensor,"
				"stiffness,odo,arms"),
			   "Parameterization of the"
			   " walk engine"))
        .def("getStepValue", &PyGaitCommand::getStepValue)
	;
    class_<PyBodyJointCommand>("BodyJointCommand",
                               init<float, tuple, tuple, tuple,
			       tuple, tuple, int>(
				   "A container for a body joint command passed to the motion engine"))
	.def(init<float, int, tuple, tuple, int>( // Single chain command
		 args("time","chainID", "joints","body_stiffness","interpolation"),
		 "A container for a body joint command passed to the motion engine"))
	;
    class_<PySetHeadCommand>("SetHeadCommand",
                             init<float, float>(args("yaw", "pitch"),
						"A container for a set head command. Holds yaw and pitch angles in degrees."))
	.def(init<float,float,float,float>(args("yaw","pitch",
						"maxYawSpeed","maxPitchSpeed")))
        ;

    class_<PyCoordHeadCommand>("CoordHeadCommand",
			       init<float, float, float>
			       (args("relX", "relY", "relZ"),
				"A container for a coord head command."))
	.def(init<float, float, float, float, float> (
		 args( "relX", "relY", "relZ", "maxYawSpeed", "maxPitchSpeed")))
	;

    class_<PyWalkCommand>("WalkCommand",
                          init<float, float, float>(args("x","y","theta"),
						    "A container for a walk command. Holds an x, y and theta which represents a"
						    " walk vector"))
        ;
    class_<PyStepCommand>("StepCommand",
                          init<float, float, float, int>(args("x","y","theta",
                                                              "numSteps"),
							 "A container for a step command. Holds an x, y and theta which represents a"
							 " walk vector, in addition to the number of desired steps."))
        ;

    class_<PyFreezeCommand>("FreezeCommand",
			    init<>("A container for a "
                                   "command to freeze the robot"))
        ;

    class_<PyUnfreezeCommand>("UnfreezeCommand",
                              init<float>(args("stiffness"),
                                          "A container for a command to "
                                          "UNfreeze the robot"))

        ;
    class_<PyMotionInterface>("MotionInterface")
        .def("enqueue", enq1)
        .def("enqueue", enq2)
        .def("setNextWalkCommand", &PyMotionInterface::setNextWalkCommand)
        .def("sendStepCommand", &PyMotionInterface::sendStepCommand)
        .def("setGait", &PyMotionInterface::setGait)
        .def("setHead", &PyMotionInterface::setHead)
	.def("coordHead", &PyMotionInterface::coordHead)
        .def("sendFreezeCommand",frz1)
        .def("sendFreezeCommand",frz2)
        .def("isWalkActive", &PyMotionInterface::isWalkActive)
        .def("isHeadActive", &PyMotionInterface::isHeadActive)
	.def("isBodyActive", &PyMotionInterface::isBodyActive)
        .def("stopBodyMoves", &PyMotionInterface::stopBodyMoves)
        .def("stopHeadMoves", &PyMotionInterface::stopHeadMoves)
        .def("resetWalk", &PyMotionInterface::resetWalkProvider)
        .def("resetScripted", &PyMotionInterface::resetScriptedProvider)
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

