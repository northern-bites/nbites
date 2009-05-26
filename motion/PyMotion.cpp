
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

#define BOOST_PYTHON_MAX_ARITY 19

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
#include "WalkCommand.h"
#include "StiffnessCommand.h"
#include "MotionInterface.h"
#include "Kinematics.h"
using namespace Kinematics;

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
                   const float _hox, const float _yao, const float _dur,
                   const float _dblSupFrac, const float _stepHeight,
                   const float _footLengthX, const float _dblInactivePerc,
                   const float _lSwHRAdd,const float _rSwHRAdd,
                   const float _lZMPSwOffY,const float _rZMPSwOffY,
                   const float _tZMPOffY, const float _sZMPOffY,
                   const float maxx, const float maxy, const float maxtheta,
                   const float sensorFeedback)

        : command(new GaitCommand(0.02,_bh*CM_TO_MM, //HACK
                                  _hox*CM_TO_MM, _yao*TO_RAD, _dur,
                                  _dblSupFrac, _stepHeight*CM_TO_MM,
                                  _footLengthX*CM_TO_MM, _dblInactivePerc,
                                  _lSwHRAdd*TO_RAD,_rSwHRAdd*TO_RAD,
                                  _lZMPSwOffY*CM_TO_MM,_rZMPSwOffY*CM_TO_MM,
                                  // Note: no conversion for these
                                  _tZMPOffY, _sZMPOffY,
                                  maxx*CM_TO_MM,maxy*CM_TO_MM,maxtheta*TO_RAD,
                                  sensorFeedback))
        {
        }

    boost::shared_ptr<GaitCommand> getCommand() const { return command; }

private:
    boost::shared_ptr<GaitCommand> command;
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


class PyStiffnessCommand {
public:
    PyStiffnessCommand(){
        command =  boost::shared_ptr<StiffnessCommand>(new StiffnessCommand());
    }
    PyStiffnessCommand(const float bodyStiffness){
        command =  boost::shared_ptr<StiffnessCommand>
            (new StiffnessCommand(bodyStiffness));
    }
    PyStiffnessCommand(const int chainID, const float chainStiffness){
        command = boost::shared_ptr<StiffnessCommand>
            (new StiffnessCommand((ChainID)chainID,chainStiffness));
    }
    void setChainStiffness(const int chainID, const float chainStiffness){
        command->setChainStiffness((ChainID) chainID, chainStiffness );
    }
    boost::shared_ptr<StiffnessCommand>  getCommand() const {return command;}
private:
    boost::shared_ptr<StiffnessCommand> command;
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

	// Single chain command
	PyBodyJointCommand(float time,
					   int chainID, tuple chainJoints,
					   int interpolationType) {
        vector<float> * chain = new vector<float>;

        // The joints come in in degrees. Convert them to radians here
        for (unsigned int i=0; i < chain_lengths[chainID] ; i++)
            chain->push_back(extract<float>(chainJoints[i]) * TO_RAD);

        command = new BodyJointCommand(time, static_cast<ChainID>(chainID),
									   chain, static_cast<InterpolationType>(interpolationType));
	}


    BodyJointCommand* getCommand() const { return command; }

private:
    BodyJointCommand *command;
};


class PySetHeadCommand {
public:
    PySetHeadCommand(const float yaw, const float pitch) {
        command = new SetHeadCommand(yaw * TO_RAD, pitch * TO_RAD);
    }

	PySetHeadCommand(const float yaw, const float pitch,
					 const float maxYawSpeed, const float maxPitchSpeed) {
        command = new SetHeadCommand(yaw * TO_RAD, pitch * TO_RAD,
									 maxYawSpeed * TO_RAD, maxPitchSpeed * TO_RAD);
    }

    SetHeadCommand* getCommand() const { return command; }
private:
    SetHeadCommand *command;
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
    void setHead(const PySetHeadCommand *command) {
        motionInterface->setHead(command->getCommand());
    }
    void sendStiffness(const PyStiffnessCommand *command){
        motionInterface->sendStiffness(command->getCommand());
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
                          float, float, float, float,
                          float, float, float, float,
                          float, float>(
"A container for setting the walking gait of the robot. "
"(bodyHeight,hipOffsetX,XAngleOffset,stepDuration,doubleSupportFraction,"
"stepHeight,"
"footLengthX,dblSupInactivePercentage,leftSwingHipRollAddition,"
"rightSwingHipRollAddition,leftZMPSwingOffsetY,rightZMPSwingOffsetY,"
"turnZMPOffsetY, strafeZMPOffsetY, sensorFeedback"))
        ;
    class_<PyBodyJointCommand>("BodyJointCommand",
                               init<float, tuple, tuple, tuple, tuple, int>(
								   "A container for a body joint command passed to the motion engine"))
		.def(init<float, int, tuple, int>( // Single chain command
				 args("time","chainID", "joints","interpolation"),
				 "A container for a body joint command passed to the motion engine"))
		;
    class_<PySetHeadCommand>("SetHeadCommand",
                             init<float, float>(args("yaw", "pitch"),
 "A container for a set head command. Holds yaw and pitch angles in degrees."))
		.def(init<float,float,float,float>(args("yaw","pitch",
												"maxYawSpeed","maxPitchSpeed")))
        ;
    class_<PyWalkCommand>("WalkCommand",
                          init<float, float, float>(args("x","y","theta"),
 "A container for a walk command. Holds an x, y and theta which represents a"
 " walk vector"))
        ;
    class_<PyStiffnessCommand>("StiffnessCommand",
                               init<float>(args("bodyStiffness"),
"A container for a stiffness comamnd. Allows setting stiffness per chain or "
"for the whole body"))
        .def(init<int,float>(args("chainID","chainStiffness")))
        .def(init<>())
        .def("setChainStiffness", &PyStiffnessCommand::setChainStiffness)
        ;
    class_<PyMotionInterface>("MotionInterface")
        .def("enqueue", enq1)
        .def("enqueue", enq2)
        .def("setNextWalkCommand", &PyMotionInterface::setNextWalkCommand)
        .def("setGait", &PyMotionInterface::setGait)
        .def("setHead",&PyMotionInterface::setHead)
        .def("sendStiffness",&PyMotionInterface::sendStiffness)
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

