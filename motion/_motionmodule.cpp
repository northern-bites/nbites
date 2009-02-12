

// System headers
//
#include <iostream>
#include <vector>
#include <Python.h>
// Project headers
//
#include "motionconfig.h"
#include "Kinematics.h"
#include "MotionCore.h"
#include "MotionInterface.h"
#include "_motionmodule.h"

using namespace std;
using namespace AL;


// Static reference to the C MotionCore
static MotionInterface* interface_reference = NULL;

// Module initialization method (Python interface, shared library)
//
PyMODINIT_FUNC
init_motion (void)
{
  // we need a reference to the motion core, set before import/init
#ifdef USE_PYMOTION_CXX_BACKEND
  if (interface_reference == NULL) {
    PyErr_SetString(PyExc_SystemError,
                    "C++ _motion backend not ready for import");
    return;
  }
#else
  cerr << "****************************************************" << endl;
  cerr << "** WARNING - USE_PYMOTION_CXX_BACKEND not defined **" << endl;
  cerr << "**   Python motion commands will have no effect   **" << endl;
  cerr << "****************************************************" << endl;
#endif

  // initialize all defined Python types (class wrappers)
  if (PyType_Ready(&PyMotionInterfaceType) < 0  ||
      PyType_Ready(&PyBodyJointCommandType) < 0 ||
      PyType_Ready(&PyHeadJointCommandType) < 0 ||
      PyType_Ready(&PyHeadScanCommandType) < 0  ||
      PyType_Ready(&PyWalkCommandType) < 0      ||
      PyType_Ready(&PyWalkParametersType) < 0 //  ||
      //PyType_Ready(&PyCallbackActionType) < 0 ||
      )
    // Python will set the error
    return;

  // initialize the module
  PyObject *m = Py_InitModule3("_motion", NULL,
      "The _motion C++ backend extension module.");
  if (m == NULL)
    // Python will set the error
    return;

  // Add Python types to module
  Py_INCREF(&PyMotionInterfaceType);
  PyModule_AddObject(m, "MotionInterface",
      reinterpret_cast<PyObject*>(&PyMotionInterfaceType));
  
  Py_INCREF(&PyBodyJointCommandType);
  PyModule_AddObject(m, "BodyJointCommand",
      reinterpret_cast<PyObject*>(&PyBodyJointCommandType));

  Py_INCREF(&PyHeadJointCommandType);
  PyModule_AddObject(m, "HeadJointCommand",
      reinterpret_cast<PyObject*>(&PyHeadJointCommandType));

  Py_INCREF(&PyHeadScanCommandType);
  PyModule_AddObject(m, "HeadScanCommand",
      reinterpret_cast<PyObject*>(&PyHeadScanCommandType));

  Py_INCREF(&PyWalkCommandType);
  PyModule_AddObject(m, "WalkCommand",
      reinterpret_cast<PyObject*>(&PyWalkCommandType));

  Py_INCREF(&PyWalkParametersType);
  PyModule_AddObject(m, "WalkParameters",
      reinterpret_cast<PyObject*>(&PyWalkParametersType));
}

// Module initialization method (Python interface, C extension)
//
void
c_init_motion (void)
{
  // for C++ extension, initialize the interpreter
  if (!Py_IsInitialized())
    Py_Initialize();

  // we need a reference to the motion core, set before import/init
#ifdef USE_PYMOTION_CXX_BACKEND
  if (interface_reference == NULL) {
    std::cerr <<
      "ERROR - could not initialize C++/Python motion interface" << std::endl;
    std::cerr << "C++ _motion backend not ready for import" << std::endl;
    return;
  }
#else
  cerr << "****************************************************" << endl;
  cerr << "** WARNING - USE_PYMOTION_CXX_BACKEND not defined **" << endl;
  cerr << "**   Python motion commands will have no effect   **" << endl;
  cerr << "****************************************************" << endl;
#endif

  // initialize all defined Python types (class wrappers)
  if (PyType_Ready(&PyMotionInterfaceType) != 0 ||
      PyType_Ready(&PyBodyJointCommandType) != 0 ||
      PyType_Ready(&PyHeadJointCommandType) != 0 ||
      PyType_Ready(&PyHeadScanCommandType) != 0 ||
      PyType_Ready(&PyWalkCommandType) != 0 // ||
      //PyType_Ready(&PyCallbackActionType) != 0 ||
      ) {
    std::cerr <<
      "ERROR - could not initialize C++/Python motion interface" << std::endl;
    if (PyErr_Occurred())
      PyErr_Print();
    else
      std::cerr << "Uknown error occured during type readying" << std::endl;
    return;
  }

  // initialize the module
  PyObject *m = Py_InitModule3("_motion", NULL,
      "The _motion C++ backend extension module.");
  if (m == NULL) {
    std::cerr <<
      "ERROR - could not initialize C++/Python motion interface" << std::endl;
    if (PyErr_Occurred())
      PyErr_Print();
    else
      std::cerr << "Unknown error occured during module init" << std::endl;
    return;
  }

  // Add Python types to module
  Py_INCREF(&PyMotionInterfaceType);
  PyModule_AddObject(m, "MotionInterface",
      reinterpret_cast<PyObject*>(&PyMotionInterfaceType));
  
  Py_INCREF(&PyBodyJointCommandType);
  PyModule_AddObject(m, "BodyJointCommand",
      reinterpret_cast<PyObject*>(&PyBodyJointCommandType));

  Py_INCREF(&PyHeadJointCommandType);
  PyModule_AddObject(m, "HeadJointCommand",
      reinterpret_cast<PyObject*>(&PyHeadJointCommandType));

  Py_INCREF(&PyHeadScanCommandType);
  PyModule_AddObject(m, "HeadScanCommand",
      reinterpret_cast<PyObject*>(&PyHeadScanCommandType));

  Py_INCREF(&PyWalkCommandType);
  PyModule_AddObject(m, "WalkCommand",
      reinterpret_cast<PyObject*>(&PyWalkCommandType));

}

// C++ backend insertion (must be set before import)
//    steals a reference to the supplied MotionInterface
//    can only be called once (subsequent calls ignored)
void
set_motion_interface (MotionInterface *_interface)
{
  //core_reference = _core;
  interface_reference = _interface;
}


//
// PyMotionInterface definitions
//

static PyMotionInterface*
PyMotionInterface_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
#ifdef USE_PYMOTION_CXX_BACKEND
  if (interface_reference == NULL) {
    PyErr_SetString(PyExc_SystemError, "C backend has been lost");
    return NULL;
  }
#endif

  PyMotionInterface *self = 
    reinterpret_cast<PyMotionInterface*>(type->tp_alloc(type, 0));
#ifdef USE_PYMOTION_CXX_BACKEND
  if (self != NULL)
    self->_interface = interface_reference; 
#endif

  return self;
}

static void
PyMotionInterface_dealloc (PyMotionInterface *self)
{
  // deallocate the Python core
  self->ob_type->tp_free(reinterpret_cast<PyObject*>(self));
}

static PyObject*
PyMotionInterface_isWalkActive (PyMotionInterface *self, PyObject *args)
{
  if (self->_interface->isWalkActive()) {
    Py_RETURN_TRUE;
  }else {
    Py_RETURN_FALSE;
  }
}

static PyObject*
PyMotionInterface_enqueue (PyMotionInterface *self, PyObject *args)
{
  if (PyTuple_Size(args) != 1) {
    PyErr_Format(PyExc_TypeError,
        "enqueue() takes exactly one argument (%i given)",
        PyTuple_Size(args));
    return NULL;
  }


  // Check the type and enqueue a new copy of the appropriate command type
  PyObject *cmd = PyTuple_GET_ITEM(args, 0);
  if (PyObject_TypeCheck(cmd, &PyBodyJointCommandType)) {
#ifdef USE_PYMOTION_CXX_BACKEND
    self->_interface->enqueue(new BodyJointCommand(
        *reinterpret_cast<PyBodyJointCommand*>(cmd)->_cmd
        ));
#endif
  
  } else if (PyObject_TypeCheck(cmd, &PyHeadJointCommandType)) {
#ifdef USE_PYMOTION_CXX_BACKEND
    self->_interface->enqueue(new HeadJointCommand(
        *reinterpret_cast<PyHeadJointCommand*>(cmd)->_cmd
        ));
#endif

  } else if (PyObject_TypeCheck(cmd, &PyHeadScanCommandType)) {
#ifdef USE_PYMOTION_CXX_BACKEND
    self->_interface->enqueue(new HeadScanCommand(
        *reinterpret_cast<PyHeadScanCommand*>(cmd)->_cmd
        ));
#endif

  } else {
    PyErr_Format(PyExc_TypeError,
        "a BodyJointCommand, HeadJointCommand, or HeadScanCommand is "
        "required (%s given)", cmd->ob_type->tp_name);
    return NULL;
  }

  Py_RETURN_NONE;
}

static PyObject*
PyMotionInterface_getHeadSpeed(PyMotionInterface *self,	PyObject *args)
{
  float headSpeed;
#ifdef USE_PYMOTION_CXX_BACKEND
  headSpeed = self->_interface->getHeadSpeed();
#endif

  return PyFloat_FromDouble(headSpeed);
}

static PyObject*
PyMotionInterface_getBalanceMode (PyMotionInterface *self, PyObject *args)
{
  int mode = static_cast<int>(ALMotionProxy::BALANCE_MODE_OFF);

#ifdef USE_PYMOTION_CXX_BACKEND
  mode = self->_interface->getBalanceMode();
#endif

  return PyInt_FromLong(mode);
}

static PyObject*
PyMotionInterface_getSupportMode (PyMotionInterface *self, PyObject *args)
{
  int mode = static_cast<int>(ALMotionProxy::SUPPORT_MODE_LEFT);

#ifdef USE_PYMOTION_CXX_BACKEND
  mode = self->_interface->getSupportMode();
#endif

  return PyInt_FromLong(mode);
}

static PyObject*
PyMotionInterface_postGotoCom (PyMotionInterface* self, PyObject* args)
{
  float pX, pY, pZ, pTime;
  int pType = static_cast<int>(Kinematics::INTERPOLATION_LINEAR);

  if (!PyArg_ParseTuple(args, "ffff|i:postGotoCom", &pX, &pY, &pZ, &pTime,
        &pType))
    return NULL;

  if (pType != Kinematics::INTERPOLATION_LINEAR &&
      pType != Kinematics::INTERPOLATION_SMOOTH) {
    PyErr_Format(PyExc_ValueError,
        "postGotoCom() expected and integer interpolation type argument of "
        "%i or %i (%i given)", Kinematics::INTERPOLATION_LINEAR,
        Kinematics::INTERPOLATION_SMOOTH, pType);
    return NULL;
  }

#ifdef USE_PYMOTION_CXX_BACKEND
  return PyInt_FromLong(self->_interface->postGotoCom(
      pX, pY, pZ, pTime, static_cast<Kinematics::InterpolationType>(pType)
    ));
#else
  return PyInt_FromLong(0);
#endif
}

static PyObject*
PyMotionInterface_postGotoTorsoOrientation (PyMotionInterface* self,
                                            PyObject* args)
{
  float pX, pY, pTime;
  int pType = static_cast<int>(Kinematics::INTERPOLATION_LINEAR);

  if (!PyArg_ParseTuple(args, "fff|i:postGotoTorsoOrientation", &pX, &pY, 
        &pTime, &pType))
    return NULL;

  if (pType != Kinematics::INTERPOLATION_LINEAR &&
      pType != Kinematics::INTERPOLATION_SMOOTH) {
    PyErr_Format(PyExc_ValueError,
        "postGotoTorsoOrientation() expected and integer interpolation type "
        "argument of %i or %i (%i given)", Kinematics::INTERPOLATION_LINEAR,
        Kinematics::INTERPOLATION_SMOOTH, pType);
    return NULL;
  }

#ifdef USE_PYMOTION_CXX_BACKEND
  return PyInt_FromLong(self->_interface->postGotoTorsoOrientation(
    pX, pY, pTime, static_cast<Kinematics::InterpolationType>(pType)
    ));
#else
  return PyInt_FromLong(0);
#endif
}
      
static PyObject*
PyMotionInterface_setBalanceMode (PyMotionInterface *self, PyObject *args)
{
  int mode;

  if (!PyInt_Check(args)) {
    PyErr_Format(PyExc_TypeError,
        "setBalanceMode() expects a single integer argument (%s given)",
        args->ob_type->tp_name);
    return NULL;
  }

  mode = PyInt_AsLong(args);
  if (mode < ALMotionProxy::BALANCE_MODE_OFF ||
      mode > ALMotionProxy::BALANCE_MODE_COM_CONTROL) {
    PyErr_Format(PyExc_ValueError,
        "setBalanceMode() expects an integer from %i to %i (%i given)",
        ALMotionProxy::BALANCE_MODE_OFF,
        ALMotionProxy::BALANCE_MODE_COM_CONTROL, mode);
    return NULL;
  }

#ifdef USE_PYMOTION_CXX_BACKEND
  self->_interface->setBalanceMode(mode);
#endif

  Py_RETURN_NONE;
}

static PyObject*
PyMotionInterface_setBodyStiffness(PyMotionInterface* self, PyObject* args)
{
  float x, y;

  if (!PyArg_ParseTuple(args, "ff|setBodyStiffness", &x, &y))
    return NULL;

  self->_interface->setBodyStiffness(x, y);
  Py_RETURN_NONE;
}

static PyObject*
PyMotionInterface_setHead (PyMotionInterface *self, PyObject *args)
{
  float time, yaw, pitch;
  int type;

  if (!PyArg_ParseTuple(args, "fffi:setHead", &time, &yaw, &pitch, &type))
    return NULL;

  if (type != 0 && type != 1) {
    PyErr_Format(PyExc_ValueError,
        "setHead() expects an integer Kinematics::InterpolationType "
        "of 0 or 1 for the 'type' argument (%i given)", type);
    return NULL;
  }

#ifdef USE_PYMOTION_CXX_BACKEND
  self->_interface->setHead(time, yaw*TO_RAD, pitch*TO_RAD,
      static_cast<Kinematics::InterpolationType>(type));
#endif
  Py_RETURN_NONE;
}

static PyObject*
PyMotionInterface_setNextWalkCommand (PyMotionInterface *self, PyObject *args)
{
  if (PyTuple_Size(args) != 1) {
    PyErr_Format(PyExc_TypeError,
        "setNextWalkCommand() takes exactly one argument (%i given)",
        PyTuple_Size(args));
    return NULL;
  }

  // Check the command WalkType and create a new WalkCommand
  // of the appropriate sub-class
  PyObject *o = PyTuple_GET_ITEM(args, 0);
  if (PyObject_TypeCheck(o, &PyWalkCommandType)) {
    PyWalkCommand *cmd = reinterpret_cast<PyWalkCommand*>(o);
/** HACK - breaking passing of WalkCommands!
    WalkCommand *_cmd;
    switch (cmd->type) {
      case WALK_STRAIGHT:
        _cmd = new WalkStraight(*static_cast<WalkStraight*>(cmd->_cmd));
        break;
      case WALK_SIDEWAYS:
        _cmd = new WalkSideways(*static_cast<WalkSideways*>(cmd->_cmd));
        break;
      case WALK_TURN:
        _cmd = new WalkTurn(*static_cast<WalkTurn*>(cmd->_cmd));
        break;
      case WALK_ARC:
        _cmd = new WalkArc(*static_cast<WalkArc*>(cmd->_cmd));
        break;
      default:
        PyErr_Format(PyExc_ValueError, "Unimplemented command type %i", 
            cmd->type);
        return NULL;
    }
    // set walk command in the motion interface
#ifdef USE_PYMOTION_CXX_BACKEND
    self->_interface->setNextWalkCommand(_cmd);
#endif
*/
  }else {
    PyErr_Format(PyExc_TypeError, "a WalkCommand is required (%s given)",
        o->ob_type->tp_name);
    return NULL;
  }
  Py_RETURN_NONE;
}

static PyObject*
PyMotionInterface_setSupportMode (PyMotionInterface *self, PyObject *args)
{
  int mode;

  if (!PyInt_Check(args)) {
    PyErr_Format(PyExc_TypeError,
        "setSupportMode() expects a single integer argument (%s given)",
        args->ob_type->tp_name);
    return NULL;
  }

  mode = PyInt_AsLong(args);
  if (mode < ALMotionProxy::SUPPORT_MODE_LEFT ||
      mode > ALMotionProxy::SUPPORT_MODE_NONE) {
    PyErr_Format(PyExc_ValueError,
        "setSupportMode() expects an integer from %i to %i (%i given)",
        ALMotionProxy::SUPPORT_MODE_LEFT, ALMotionProxy::SUPPORT_MODE_NONE,
        mode);
    return NULL;
  }

#ifdef USE_PYMOTION_CXX_BACKEND
  self->_interface->setSupportMode(mode);
#endif

  Py_RETURN_NONE;
}

static PyObject*
PyMotionInterface_setWalkConfig (PyMotionInterface *self, PyObject *args)
{
  float pMaxStepLength, pMaxStepHeight, pMaxStepSide, pMaxStepTurn;
  float pZmpOffsetX, pZmpOffsetY;

  if (!PyArg_ParseTuple(args, "ffffff:setWalkConfig", &pMaxStepLength,
        &pMaxStepHeight, &pMaxStepSide, &pMaxStepTurn, &pZmpOffsetX,
        &pZmpOffsetY))
    return NULL;

#ifdef USE_PYMOTION_CXX_BACKEND
  self->_interface->setWalkConfig(pMaxStepLength, pMaxStepHeight, pMaxStepSide, pMaxStepTurn,
      pZmpOffsetX, pZmpOffsetY);
#endif
  Py_RETURN_NONE;
}

static PyObject*
PyMotionInterface_setWalkArmsConfig (PyMotionInterface *self, PyObject *args)
{
  float pShoulderMedian, pShoulderAmplitude, pElbowMedian, pElbowAmplitude;

  if (!PyArg_ParseTuple(args, "ffff:setWalkArmsConfig", &pShoulderMedian,
        &pShoulderAmplitude, &pElbowMedian, &pElbowAmplitude))
    return NULL;

#ifdef USE_PYMOTION_CXX_BACKEND
  self->_interface->setWalkArmsConfig(pShoulderMedian, pShoulderAmplitude,
      pElbowMedian, pElbowAmplitude);
#endif
  Py_RETURN_NONE;
}

static PyObject*
PyMotionInterface_setWalkExtraConfig (PyMotionInterface *self, PyObject *args)
{
  float pLHipRollBacklashCompensator, pRHipRollBacklashCompensator;
  float pHipHeight, pTorsoYOrientation;

  if (!PyArg_ParseTuple(args, "ffff:setWalkExtraConfig", 
        &pLHipRollBacklashCompensator, &pRHipRollBacklashCompensator,
        &pHipHeight, &pTorsoYOrientation))
    return NULL;

#ifdef USE_PYMOTION_CXX_BACKEND
  self->_interface->setWalkExtraConfig(pLHipRollBacklashCompensator,
      pRHipRollBacklashCompensator, pHipHeight, pTorsoYOrientation);
#endif
  Py_RETURN_NONE;
}

static PyObject*
PyMotionInterface_setWalkParameters (PyMotionInterface *self, PyObject *args)
{
  if (!PyObject_TypeCheck(args, &PyWalkParametersType)) {
    PyErr_Format(PyExc_TypeError,
        "setWalkParameters() expected a WalkParameters instance (%s given)",
        args->ob_type->tp_name);
    return NULL;
  }

#ifdef USE_PYMOTION_CXX_BACKEND
  self->_interface->setWalkParameters(
      *reinterpret_cast<PyWalkParameters*>(args)->_params);
#endif
  Py_RETURN_NONE;
}

static PyObject*
PyMotionInterface_stopBodyMoves (PyMotionInterface *self, PyObject *args)
{
#ifdef USE_PYMOTION_CXX_BACKEND
  self->_interface->stopBodyMoves();
#endif
  Py_RETURN_NONE;
}

static PyObject*
PyMotionInterface_stopHeadMoves (PyMotionInterface *self, PyObject *args)
{
#ifdef USE_PYMOTION_CXX_BACKEND
  self->_interface->stopHeadMoves();
#endif
  Py_RETURN_NONE;
}



//
// Python sequence -> std::vector helper function
//

static vector<float>*
PySequence_AsFloatVector (PyObject *seq)
{
  PyObject *fast = NULL, *o;
  vector<float> *vec = NULL;

  if (seq != NULL &&
      (fast = PySequence_Fast(seq, "a sequence of floats is expected")) != NULL
     ) {
    // convert float values
    vec = new vector<float>();
    for (int i = 0; i < PySequence_Fast_GET_SIZE(fast); i++) {
      o = PySequence_Fast_GET_ITEM(fast, i);
      if (PyNumber_Check(o))
        vec->push_back((float)PyFloat_AsDouble(o));
      else {
        delete vec;
        vec = NULL;
        break;
      }
    }
  }

  if (fast != NULL)
    Py_DECREF(fast);

  return vec;
}

static vector<float>*
PySequence_AsFloatVector (PyObject *seq, int size)
{
  vector<float> *v = PySequence_AsFloatVector(seq);
  if (v != NULL && v->size() != static_cast<unsigned int>(size)) {
    PyErr_Format(PyExc_TypeError, "expected a sequence of %i float (%i given)",
        size, v->size());
    delete v;
    return NULL;
  }
  return v;
}

static vector<float>*
PyDegJointSequence_AsRadFloatVector (PyObject *seq)
{
  PyObject *fast = NULL, *o;
  vector<float> *vec = NULL;

  if (seq != NULL &&
      (fast = PySequence_Fast(seq, "a sequence of floats is expected")) != NULL
     ) {
    // convert float values
    vec = new vector<float>();
    for (int i = 0; i < PySequence_Fast_GET_SIZE(fast); i++) {
      o = PySequence_Fast_GET_ITEM(fast, i);
      if (PyNumber_Check(o))
        vec->push_back(static_cast<float>(PyFloat_AsDouble(o))*TO_RAD);
      else {
        delete vec;
        vec = NULL;
        break;
      }
    }
  }

  if (fast != NULL)
    Py_DECREF(fast);

  return vec;
}



//
// PyBodyJointCommand definitions
//

static PyBodyJointCommand*
PyBodyJointCommand_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  PyBodyJointCommand *self = 
    reinterpret_cast<PyBodyJointCommand*>(type->tp_alloc(type, 0));
  self->_cmd = NULL;

  if (self != NULL) {
    int argc = PyTuple_Size(args);
    int kwdc = 0;
    if (kwds != NULL)
      kwdc = PyDict_Size(kwds);
    int size = argc + kwdc;

    if (kwdc > 0 && (PyDict_GetItemString(kwds, "larm") != NULL ||
                     PyDict_GetItemString(kwds, "lleg") != NULL ||
                     PyDict_GetItemString(kwds, "rleg") != NULL ||
                     PyDict_GetItemString(kwds, "rarm") != NULL))
        // if individual joint lists are passed, assume the call is using
        // the 6 argument call format.  Missing values will be NULL.
        size = 6;


    if (size != 1 && size != 3 && size != 4 && size != 6) {
      PyErr_Format(PyExc_TypeError,
          "__init__() expects 1, 3, 4, or 6 normal or keyword arguments "
          "(%i given)", size);
      PyBodyJointCommand_dealloc(self);
      return NULL;
    }

    PyObject *other, *ptime, *pjoints, *pchain,
             *plarm, *plleg, *prleg, *prarm, *ptype;
    other = ptime = pjoints = pchain = ptype = NULL;
    plarm = plleg = prleg = prarm = NULL;

    float time;
    ChainID chainID = LARM_CHAIN;
    vector<float> *joints, *larm, *lleg, *rleg, *rarm;
    joints = larm = lleg = rleg = rarm = NULL;
    Kinematics::InterpolationType type;

    // special case for copy constructor
    //
    if (size == 1) {
      if (argc > 0)
        other = PyTuple_GET_ITEM(args, 0);
      else if (kwds != NULL)
        other = PyDict_GetItemString(kwds, "other");

      if (other == NULL ||
          !PyObject_TypeCheck(other, &PyBodyJointCommandType)) {
        PyErr_SetString(PyExc_TypeError,
            "__init__() as a copy constructor (1 argument) requires a "
            "BodyJointCommand argument");
        PyBodyJointCommand_dealloc(self);
        return NULL;
      }else {
        self->_cmd = new BodyJointCommand(
            *reinterpret_cast<PyBodyJointCommand*>(other)->_cmd
            );
        return self;
      }
    }


    // Read arguments into PyObject variables
    //
    int i = 0;
    // motion time
    if (argc > i)
      ptime = PyTuple_GET_ITEM(args, i++);
    else if (kwds != NULL)
      ptime = PyDict_GetItemString(kwds, "time");

    switch (size) {
      case 3:
        // body joints vector
        if (argc > i)
          pjoints = PyTuple_GET_ITEM(args, i++);
        else if (kwds != NULL)
          pjoints = PyDict_GetItemString(kwds, "joints");
        break;

      case 4:
        // chain id
        if (argc > i)
          pchain = PyTuple_GET_ITEM(args, i++);
        else if (kwds != NULL)
          pchain = PyDict_GetItemString(kwds, "chain");

        // chain joint vector
        if (argc > i)
          pjoints = PyTuple_GET_ITEM(args, i++);
        else if (kwds != NULL)
          pjoints = PyDict_GetItemString(kwds, "joints");
        break;

      case 6:
        // all chain joint vectors
        if (argc > i)
          plarm = PyTuple_GET_ITEM(args, i++);
        else if (kwds != NULL)
          if ((plarm = PyDict_GetItemString(kwds, "larm")) == NULL)
            plarm = Py_None;

        if (argc > i)
          plleg = PyTuple_GET_ITEM(args, i++);
        else if (kwds != NULL)
          if ((plleg = PyDict_GetItemString(kwds, "lleg")) == NULL)
            plleg = Py_None;

        if (argc > i)
          prleg = PyTuple_GET_ITEM(args, i++);
        else if (kwds != NULL)
          if ((prleg = PyDict_GetItemString(kwds, "rleg")) == NULL)
            prleg = Py_None;

        if (argc > i)
          prarm = PyTuple_GET_ITEM(args, i++);
        else if (kwds != NULL)
          if ((prarm = PyDict_GetItemString(kwds, "rarm")) == NULL)
            prarm = Py_None;
        break;
    }

    // interpolation type
    if (argc > i)
      ptype = PyTuple_GET_ITEM(args, i++);
    else if (kwds != NULL)
      ptype = PyDict_GetItemString(kwds, "type");


    // Ensure all required arguments are supplied, and convert
    // them to their C++ values
    //
    
    // motion time
    if (ptime == NULL || !PyNumber_Check(ptime)) {
      PyErr_SetString(PyExc_TypeError,
          "__init__() requires a float time as the first argument, or in the "
          "keyword list as 'time', when not a copy constructor");
      PyBodyJointCommand_dealloc(self);
      return NULL;
    }else
      time = (float)PyFloat_AsDouble(ptime);

    switch (size) {
      case 3:
        // body joint vector
        joints = PyDegJointSequence_AsRadFloatVector(pjoints);
        if (joints == NULL) {
          PyErr_SetString(PyExc_TypeError,
              "or in the keyword list as 'joints', when 3 arguments are "
              "supplied");
          PyBodyJointCommand_dealloc(self);
          return NULL;
        }
        if (joints->size() != NUM_JOINTS) {
          PyErr_Format(PyExc_TypeError,
              "__init__() joints argument must contain %i floats (%i given)",
              NUM_JOINTS, joints->size());
          PyBodyJointCommand_dealloc(self);
          return NULL;
        }
        break;

      case 4:
        // chain id
        if (pchain == NULL || !PyInt_Check(pchain)) {
          PyErr_SetString(PyExc_TypeError,
              "__init__() requires an integer ChainID as the second "
              "argument, or in the keyword list as 'chain', when 4 arguments "
              "are supplied");
          PyBodyJointCommand_dealloc(self);
          return NULL;
        }else {
          int _chain = PyInt_AsLong(pchain);
          if (_chain < LARM_CHAIN || _chain > RARM_CHAIN) {
            PyErr_SetString(PyExc_ValueError,
                "ChainID argument to __init__() is not a valid chain id for "
		"BodyJointCommands"
                );
            PyBodyJointCommand_dealloc(self);
            return NULL;
          }else
            chainID = (ChainID)_chain;
        }

        // chain joint vector
        joints = PyDegJointSequence_AsRadFloatVector(pjoints);
        if (joints == NULL) {
          PyErr_SetString(PyExc_TypeError,
              "__init__() requires a list of floats as the third argument, "
              "or in the keyword list as 'joints', when 4 arguments are "
              "supplied");
          PyBodyJointCommand_dealloc(self);
          return NULL;
        }
        if (joints->size() != chain_lengths[chainID]) {
          PyErr_Format(PyExc_TypeError,
              "__init__() joints argument must contain %i floats for chain %i "
              "(%i given)", chain_lengths[chainID], chainID, joints->size());
          PyBodyJointCommand_dealloc(self);
          return NULL;
        }
        break;

      case 6:
        // all chain joint vectors
        larm = PyDegJointSequence_AsRadFloatVector(plarm);
        lleg = PyDegJointSequence_AsRadFloatVector(plleg);
        rleg = PyDegJointSequence_AsRadFloatVector(prleg);
        rarm = PyDegJointSequence_AsRadFloatVector(prarm);

        if (larm == NULL && plarm != Py_None ||
            lleg == NULL && plleg != Py_None ||
            rleg == NULL && prleg != Py_None ||
            rarm == NULL && prarm != Py_None ||
            (plarm == Py_None && plleg == Py_None &&
             prleg == Py_None && prarm == Py_None   )) {
          PyErr_SetString(PyExc_TypeError,
              "__init__() expects four lists of floats as the 2nd-5th "
              "arguments, or in the keyword list as 'larm', 'lleg', 'rleg', "
              "and 'rarm', when 6 arguments are supplied.  At least one must "
              "be a list, all others may be None");

          if (larm != NULL) delete larm;
          if (lleg != NULL) delete lleg;
          if (rleg != NULL) delete rleg;
          if (rarm != NULL) delete rarm;
          PyBodyJointCommand_dealloc(self);
          return NULL;
        }
        if (larm != NULL && larm->size() != chain_lengths[LARM_CHAIN]) {
          PyErr_Format(PyExc_TypeError,
              "__init__() larm argument must contain %i floats (%i given)",
              chain_lengths[LARM_CHAIN], larm->size());
          PyBodyJointCommand_dealloc(self);
          return NULL;
        }
        if (lleg != NULL && lleg->size() != chain_lengths[LLEG_CHAIN]) {
          PyErr_Format(PyExc_TypeError,
              "__init__() lleg argument must contain %i floats (%i given)",
              chain_lengths[LLEG_CHAIN], lleg->size());
          PyBodyJointCommand_dealloc(self);
          return NULL;
        }
        if (rleg != NULL && rleg->size() != chain_lengths[RLEG_CHAIN]) {
          PyErr_Format(PyExc_TypeError,
              "__init__() rleg argument must contain %i floats (%i given)",
              chain_lengths[RLEG_CHAIN], rleg->size());
          PyBodyJointCommand_dealloc(self);
          return NULL;
        }
        if (rarm != NULL && rarm->size() != chain_lengths[RARM_CHAIN]) {
          PyErr_Format(PyExc_TypeError,
              "__init__() rarm argument must contain %i floats (%i given)",
              chain_lengths[RARM_CHAIN], rarm->size());
          PyBodyJointCommand_dealloc(self);
          return NULL;
        }
        break;
    }

    // interpolation type
    if (ptype == NULL || !PyInt_Check(ptype)) {
      PyErr_SetString(PyExc_TypeError,
          "__init__() expects an integer "
          "AL::Kinematics::InterpolationType as the last argument, or in "
          "the keyword list as 'type', when not a copy constructor");
      if (joints != NULL) delete joints;
      if (larm != NULL) delete larm;
      if (lleg != NULL) delete lleg;
      if (rleg != NULL) delete rleg;
      if (rarm != NULL) delete rarm;
      PyBodyJointCommand_dealloc(self);
      return NULL;
    }
    int _type = PyInt_AsLong(ptype);
    if (_type != 0 && _type != 1) {
      PyErr_Format(PyExc_ValueError,
          "AL::Kinematics::InterpolationType argument to __init__() is "
          "not a valid interpolation type ('%i' given)", _type
          );
      if (joints != NULL) delete joints;
      if (larm != NULL) delete larm;
      if (lleg != NULL) delete lleg;
      if (rleg != NULL) delete rleg;
      if (rarm != NULL) delete rarm;
      PyBodyJointCommand_dealloc(self);
      return NULL;
    }else
		type = (Kinematics::InterpolationType) _type;
    

    // Initialize the BodyJointCommand object
    //

    switch (size) {
      case 3:
        self->_cmd = new BodyJointCommand(time, joints, type);
        break;
      case 4:
        self->_cmd = new BodyJointCommand(time, chainID, joints, type);
        break;
      case 6:
        self->_cmd = new BodyJointCommand(time, larm, lleg, rleg, rarm, type);
        break;
    }
  }

  return self;
}

static PyObject*
PyBodyJointCommand_conflicts (PyBodyJointCommand *self, PyObject *args)
{
  PyObject *other;

  if (PyTuple_Size(args) != 1) {
    PyErr_Format(PyExc_TypeError,
        "conflicts() takes exactly one argument (%i given)",
        PyTuple_Size(args));
    return NULL;
  }

  other = PyTuple_GET_ITEM(args, 0);
  if (!PyObject_TypeCheck(other, &PyBodyJointCommandType)) {
    PyErr_Format(PyExc_TypeError,
        "conflicts() expects a BodyJointCommand object (%s given)",
        other->ob_type->tp_name);
    return NULL;
  }

  if (self->_cmd->conflicts(
        *reinterpret_cast<PyBodyJointCommand*>(other)->_cmd) )
    Py_RETURN_TRUE;
  else
    Py_RETURN_FALSE;
}

static void
PyBodyJointCommand_dealloc (PyBodyJointCommand *self)
{
  if (self->_cmd != NULL)
    delete self->_cmd;
  self->ob_type->tp_free(reinterpret_cast<PyObject*>(self));
}



//
// PyHeadJointCommand definitions
//

static PyHeadJointCommand*
PyHeadJointCommand_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  PyHeadJointCommand *self =
    reinterpret_cast<PyHeadJointCommand*>(type->tp_alloc(type, 0));
  self->_cmd = NULL;

  if (self != NULL) {
    int argc = PyTuple_Size(args);
    int kwdc = 0;
    if (kwds != NULL)
      PyDict_Size(kwds);
    int size = argc + kwdc;

    if (size != 1 && size != 3) {
      PyErr_Format(PyExc_TypeError,
          "__init__() expects 1 or 3 normal or keyword arguments (%i given)",
          size);
      PyHeadJointCommand_dealloc(self);
      return NULL;
    }

    PyObject *other;
    PyObject *ptime, *pjoints, *ptype;
    ptime = pjoints = ptype = NULL;
    float time;
    vector<float> *joints;
	Kinematics::InterpolationType type;

    // special case for copy constructor
    //
    if (size == 1) {
      if (argc > 0)
        other = PyTuple_GET_ITEM(args, 0);
      else
        other = PyDict_GetItemString(kwds, "other");

      if (other == NULL ||
          !PyObject_TypeCheck(other, &PyHeadJointCommandType)) {
        PyErr_SetString(PyExc_TypeError,
            "__init__() as a copy constructor (1 argument) requires a "
            "HeadJointCommand argument");
        PyHeadJointCommand_dealloc(self);
        return NULL;
      }else {
        self->_cmd = new HeadJointCommand(
            *reinterpret_cast<PyHeadJointCommand*>(other)->_cmd
            );
        return self;
      }
    }


    // Read arguments into PyObject variables
    //
    int i = 0;
    // motion time
    if (argc > i)
      ptime = PyTuple_GET_ITEM(args, i++);
    else
      ptime = PyDict_GetItemString(kwds, "time");
    // head joints vector
    if (argc > i)
      pjoints = PyTuple_GET_ITEM(args, i++);
    else
      pjoints = PyDict_GetItemString(kwds, "joints");
    // interpolation type
    if (argc > i)
      ptype = PyTuple_GET_ITEM(args, i++);
    else
      ptype = PyDict_GetItemString(kwds, "type");


    // Ensure all required arguments are supplied, and convert
    // them to their C++ values
    //
    
    // motion time
    if (ptime == NULL || !PyNumber_Check(ptime)) {
      PyErr_SetString(PyExc_TypeError,
          "__init__() requires a float time as the first argument, or in the "
          "keyword list as 'time', when 3 arguments are supplied");
      PyHeadJointCommand_dealloc(self);
      return NULL;
    }else
      time = (float)PyFloat_AsDouble(ptime);

    // body joint vector
    joints = PyDegJointSequence_AsRadFloatVector(pjoints);
    if (joints == NULL) {
      PyErr_SetString(PyExc_TypeError,
          "__init__() requires a list of floats as the second argument, "
          "or in the keyword list as 'joints', when 3 arguments are "
          "supplied");
      PyHeadJointCommand_dealloc(self);
      return NULL;
    }

    // interpolation type
    if (ptype == NULL || !PyInt_Check(ptype)) {
      PyErr_SetString(PyExc_TypeError,
          "__init__() expects an integer "
          "AL::Kinematics::InterpolationType as the last argument, or in "
          "the keyword list as 'type', when not a copy constructor");
      if (joints != NULL) delete joints;
      PyHeadJointCommand_dealloc(self);
      return NULL;
    }
    int _type = PyInt_AsLong(ptype);
    if (_type != 0 && _type != 1) {
      PyErr_SetString(PyExc_ValueError,
          "AL::Kinematics::InterpolationType argument to __init__() is "
          "not a valid interpolation type"
          );
      if (joints != NULL) delete joints;
      PyHeadJointCommand_dealloc(self);
      return NULL;
    }else
      type = (Kinematics::InterpolationType) _type;


    // Initialize the HeadJointCommand object
    //
    self->_cmd = new HeadJointCommand(time, joints, type);
  }

  return self;
}

static PyObject*
PyHeadJointCommand_conflicts (PyHeadJointCommand *self, PyObject *args)
{
  Py_RETURN_NONE;
}

static void
PyHeadJointCommand_dealloc (PyHeadJointCommand *self)
{
  if (self->_cmd != NULL)
    delete self->_cmd;
  self->ob_type->tp_free(reinterpret_cast<PyObject*>(self));
}



//
// PyHeadScanCommand definitions
//

static PyHeadScanCommand*
PyHeadScanCommand_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  PyHeadScanCommand *self = 
    reinterpret_cast<PyHeadScanCommand*>(type->tp_alloc(type, 0));
  self->_cmd = NULL;

  if (self != NULL) {
    int argc = PyTuple_Size(args);
    int kwdc = 0;
    if (kwds != NULL)
      PyDict_Size(kwds);
    int size = argc + kwdc;

    if (size != 1 && size != 2) {
      PyErr_Format(PyExc_TypeError,
          "__init__() expects 1 or 2 normal or keyword arguments (%i given)",
          size);
      PyHeadScanCommand_dealloc(self);
      return NULL;
    }

    PyObject *other, *pcommands, *pforever;
    other = pcommands = pforever = NULL;
    vector<const HeadJointCommand*> *commands = NULL;
    bool forever;

    // Read arguments into PyObject variables
    //
    int i = 0;
    // copy constructor
    if (i <= argc)
      other = PyTuple_GET_ITEM(args, i++);
    else
      other = PyDict_GetItemString(kwds, "other");

    if (other != NULL && !PyObject_TypeCheck(other, &PyBodyJointCommandType)) {
      pcommands = other;
      other = NULL;
    }else if (other != NULL) {
      if (size > 1) {
        PyErr_Format(PyExc_TypeError,
            "__init__() expects only one HeadScanCommand argument, or in the "
            "keyword list as 'other' when acting as copy constructor "
            "(%s given)", other->ob_type->tp_name);
        PyHeadScanCommand_dealloc(self);
        return NULL;
      }
    }

    // HeadJointCommand list
    if (other == NULL && pcommands == NULL) {
      if (i <= argc)
        pcommands = PyTuple_GET_ITEM(args, i++);
      else
        pcommands = PyDict_GetItemString(kwds, "commands");
    }

    // forever boolean
    if (other == NULL) {
      if (i <= argc)
        pforever = PyTuple_GET_ITEM(args, i++);
      else
        pforever = PyDict_GetItemString(kwds, "forever");
    }

    // Ensure all required arguments are supplied, and convert
    // them to their C++ values
    //
    // copy constructor
    if (other != NULL) {
      self->_cmd = new HeadScanCommand(
          *reinterpret_cast<PyHeadScanCommand*>(other)->_cmd
          );
      return self;
    }
    // HeadJointCommand list
    if (pcommands != NULL && PySequence_Check(pcommands)) {
      PyObject *fast = PySequence_Fast(pcommands,
          "a sequence of HeadScanCommands is expected");
      if (fast != NULL) {
        PyObject *o;
        commands = new vector<const HeadJointCommand*>();
        for (int i = 0; i < PySequence_Fast_GET_SIZE(fast); i++) {
          o = PySequence_Fast_GET_ITEM(fast, i);
          if (PyObject_TypeCheck(o, &PyHeadJointCommandType))
            commands->push_back(new HeadJointCommand(
                  *reinterpret_cast<PyHeadJointCommand*>(o)->_cmd
                  ));
          else {
            delete commands;
            commands = NULL;
            break;
          }
        }
      }
    }
    if (commands == NULL) {
      PyErr_SetString(PyExc_TypeError,
          "__init__() expects a list of HeadJointCommands as the first "
          "argument, or in the keyword list as 'commands'");
      PyHeadScanCommand_dealloc(self);
      return NULL;
    }
    // forever boolean
    if (pforever != NULL && !PyBool_Check(pforever)) {
      PyErr_Format(PyExc_TypeError,
          "__init__() expects the second argument, or the keyword argument "
          "'forever', to be a boolean (%s given)", pforever->ob_type->tp_name);
      PyHeadScanCommand_dealloc(self);
      return NULL;
    }else
      forever = (pforever == Py_True);

    // Initialize the HeadJointCommand object
    //
    if (pforever == NULL)
      self->_cmd = new HeadScanCommand(commands);
    else
      self->_cmd = new HeadScanCommand(commands, forever);
  }

  return self;
}

static PyObject*
PyHeadScanCommand_conflicts (PyHeadScanCommand *self, PyObject *args)
{
  Py_RETURN_NONE;
}

static void
PyHeadScanCommand_dealloc (PyHeadScanCommand *self)
{
  if (self->_cmd != NULL)
    delete self->_cmd;
  self->ob_type->tp_free(reinterpret_cast<PyObject*>(self));
}



//
// PyWalkCommand definitions
//

static PyWalkCommand*
PyWalkCommand_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  PyWalkCommand *self =
    reinterpret_cast<PyWalkCommand*>(type->tp_alloc(type, 0));
  self->_cmd = NULL;

  return self;
}

static int
PyWalkCommand_init (PyWalkCommand *self, PyObject *args, PyObject *kwds)
{
  //old
  PyObject *ptype, *parg1, *parg2, *psamples;
  ptype = parg1 = parg2 = psamples = NULL;

  int type, samples = 100; //HACK
  float dist, angle, radius;
  dist = angle = radius = 0;


  PyObject *x_vel_mms, *y_vel_mms, *theta_vel_mms;
  //new
  float x, y, theta;

  int argc = PyTuple_Size(args);
  int kwdc = kwds != NULL ? PyTuple_Size(kwds) : 0;
  int i = 0;

  // Parse type argument
  //
  if (kwdc > 0)
    // try to get out of keywords
    ptype = PyDict_GetItemString(kwds, "type");
  if (ptype == NULL && i < argc)
    // try to get out of arguments
    ptype = PyTuple_GET_ITEM(args, i++);
  if (ptype == NULL || !PyInt_Check(ptype)) {
    // error, no type given
    if (ptype == NULL)
      PyErr_Format(PyExc_TypeError,
          "__init__() expects at least two arguments (%i given)",
          argc);
    else
      PyErr_Format(PyExc_TypeError,
          "__init__() expects an integer type argument (%s given)",
          ptype->ob_type->tp_name);
    return -1;
  }


  // Parse remaining arguments
  //
/* HACK -- need to parse x,y,theta now
  switch (static_cast<WalkType>(type)) {
    case WALK_STRAIGHT:
    case WALK_SIDEWAYS:
      if (kwdc > 0)
        parg1 = PyDict_GetItemString(kwds, "dist");
      if (parg1 == NULL && i < argc)
        parg1 = PyTuple_GET_ITEM(args, i++);
      if (parg1 == NULL) {
        PyErr_Format(PyExc_TypeError,
            "__init__(), when given a type of %i, expects a float 'dist' "
            "argument", type);
        return -1;
      }else if (!PyNumber_Check(parg1)) {
        PyErr_Format(PyExc_TypeError,
            "'dist' argument to __init__() must be a float (%s given)",
            parg1->ob_type->tp_name);
        return -1;
      }

      dist = static_cast<float>(PyFloat_AsDouble(parg1));
      break;
    case WALK_TURN:
      if (kwdc > 0)
        parg1 = PyDict_GetItemString(kwds, "angle");
      if (parg1 == NULL && i < argc)
        parg1 = PyTuple_GET_ITEM(args, i++);
      if (parg1 == NULL) {
        PyErr_Format(PyExc_TypeError,
            "__init__(), when given a type of %i, expects a float 'angle' "
            "argument", type);
        return -1;
      }else if (!PyNumber_Check(parg1)) {
        PyErr_Format(PyExc_TypeError,
            "'angle' argument to __init__() must be a float (%s given)",
            parg1->ob_type->tp_name);
        return -1;
      }

      angle = static_cast<float>(PyFloat_AsDouble(parg1));
      break;
    case WALK_ARC:
      if (kwdc > 0) {
        parg1 = PyDict_GetItemString(kwds, "angle");
        parg2 = PyDict_GetItemString(kwds, "radius");
      }
      if (parg1 == NULL && i < argc)
        parg1 = PyTuple_GET_ITEM(args, i++);
      if (parg2 == NULL && i < argc)
        parg2 = PyTuple_GET_ITEM(args, i++);

      if (parg1 == NULL) {
        PyErr_Format(PyExc_TypeError,
            "__init__(), when given a type of %i, expects a float 'angle' "
            "argument", type);
        return -1;
      }else if (!PyNumber_Check(parg1)) {
        PyErr_Format(PyExc_TypeError,
            "'angle' argument to __init__() must be a float (%s given)",
            parg1->ob_type->tp_name);
        return -1;
      }

      if (parg2 == NULL) {
        PyErr_Format(PyExc_TypeError,
            "__init__(), when given a type of %i, expects a float 'radius' "
            "argument", type);
        return -1;
      }else if (!PyNumber_Check(parg2)) {
        PyErr_Format(PyExc_TypeError,
            "'radius' argument to __init__() must be a float (%s given)",
            parg2->ob_type->tp_name);
        return -1;
      }

      angle = static_cast<float>(PyFloat_AsDouble(parg1));
      radius = static_cast<float>(PyFloat_AsDouble(parg2));
      break;
    default:
      PyErr_Format(PyExc_ValueError, "Unimplemented command type %i", type);
      return -1; // could this value be a bug? I don't know. Keep an eye on it
  }
  // number of samples
  if (kwdc > 0)
    psamples = PyDict_GetItemString(kwds, "samples");
  if (psamples == NULL && i < argc)
    psamples = PyTuple_GET_ITEM(args, i++);
  if (psamples != NULL) {
    if (PyInt_Check(psamples))
      samples = PyInt_AsLong(psamples);
    else {
      PyErr_Format(PyExc_TypeError,
          "__init__() 'samples' argument must be an int (%s given)",
          psamples->ob_type->tp_name);
      return -1;
    }
  }
*/

  self->_cmd = new WalkCommand(x,y,theta);
/*More hackery
  // Initialize WalkCommand Object
  //
  switch (static_cast<WalkType>(type)) {
    case WALK_STRAIGHT:
      self->_cmd = new WalkStraight(dist, samples);
      break;
    case WALK_SIDEWAYS:
      self->_cmd = new WalkSideways(dist, samples);
      break;
    case WALK_TURN:
      self->_cmd = new WalkTurn(angle*TO_RAD, samples);
      break;
    case WALK_ARC:
      self->_cmd = new WalkArc(angle*TO_RAD, radius, samples);
      break;
    default:
      PyErr_Format(PyExc_ValueError, "Unimplemented command type %i", type);
      return -1;
  }
*/
  return 0;
}

static void
PyWalkCommand_dealloc (PyWalkCommand *self)
{
  if (self->_cmd != NULL)
    delete self->_cmd;
  self->ob_type->tp_free(reinterpret_cast<PyObject*>(self));
}



//
// PyWalkParameters definitions
//

PyObject*
PyWalkParameters_new (PyTypeObject* type, PyObject* args, PyObject* kwds)
{
  PyObject *self = type->tp_alloc(type, 0);
  if (self != NULL)
    reinterpret_cast<PyWalkParameters*>(self)->_params = NULL;
  return self;
}

int
PyWalkParameters_init (PyWalkParameters* self, PyObject* args, PyObject* kwds)
{
  static char* keywords[] = {"walk", "arms", "extra"};
  PyObject *pwalk, *parms, *pextra;
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "OOO:init", keywords,
        &pwalk, &parms, &pextra))
    return -1;

  vector<float> *walk = PySequence_AsFloatVector(pwalk, 6);
  vector<float> *arms = PySequence_AsFloatVector(parms, 4);
  vector<float> *extra = PySequence_AsFloatVector(pextra, 4);

  if (walk == NULL || arms == NULL || extra == NULL) {
    PyErr_SetString(PyExc_TypeError,
        "init() expects three sequences of floats, lengths 6, 4, and 4");
    return -1;
  }

  self->_params = new WalkParameters(*walk, *arms, *extra);
  return 0;
}

void
PyWalkParameters_dealloc (PyObject* self)
{
  if (reinterpret_cast<PyWalkParameters*>(self)->_params != NULL)
    delete reinterpret_cast<PyWalkParameters*>(self)->_params;
  self->ob_type->tp_free(self);
}

