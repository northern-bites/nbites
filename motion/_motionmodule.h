
#ifndef __motionmodule_h_DEFINED
#define __motionmodule_h_DEFINED

// System headers
//
#include <Python.h>
#include <vector>
// Project headers
//
#include "MotionCore.h"
#include "MotionInterface.h"


// Module initialization method (Python interface)
//
PyMODINIT_FUNC init_motion(void);
void           c_init_motion(void);
// C++ backend insertion (must be set before import)
extern void    set_motion_interface(MotionInterface *interface);


//
// PyMotionInterface type
//

// Declare the C struct
typedef struct PyMotionInterface_t
{
  PyObject_HEAD
  // hard reference to the C core
  MotionInterface *_interface;
} PyMotionInterface;
// C method declarations
static PyMotionInterface* PyMotionInterface_new(PyTypeObject *type,
    PyObject* args, PyObject *kwds);
static void PyMotionInterface_dealloc(PyMotionInterface* self);
static PyObject* PyMotionInterface_isWalkActive(PyMotionInterface* self,
    PyObject*args);
static PyObject* PyMotionInterface_enqueue(PyMotionInterface* self,
    PyObject* args);
static PyObject* PyMotionInterface_getHeadSpeed(PyMotionInterface* self,
    PyObject* args);
static PyObject* PyMotionInterface_getBalanceMode(PyMotionInterface* self,
    PyObject* args);
static PyObject* PyMotionInterface_getSupportMode(PyMotionInterface* self,
    PyObject* args);
static PyObject* PyMotionInterface_postGotoCom(PyMotionInterface* self,
    PyObject* args);
static PyObject* PyMotionInterface_postGotoTorsoOrientation(
    PyMotionInterface* self, PyObject* args);
static PyObject* PyMotionInterface_setBalanceMode(PyMotionInterface* self,
    PyObject* args);
static PyObject* PyMotionInterface_setBodyStiffness(PyMotionInterface* self,
    PyObject* args);
static PyObject* PyMotionInterface_setHead(PyMotionInterface* self,
    PyObject* args);
static PyObject* PyMotionInterface_setNextWalkCommand(PyMotionInterface* self,
    PyObject* args);
static PyObject* PyMotionInterface_setSupportMode(PyMotionInterface* self,
    PyObject* args);
static PyObject* PyMotionInterface_setWalkConfig(PyMotionInterface* self,
    PyObject* args);
static PyObject* PyMotionInterface_setWalkArmsConfig(PyMotionInterface* self,
    PyObject* args);
static PyObject* PyMotionInterface_setWalkExtraConfig(PyMotionInterface* self,
    PyObject* args);
static PyObject* PyMotionInterface_setWalkParameters(PyMotionInterface* self,
    PyObject* args);
static PyObject* PyMotionInterface_stopBodyMoves(PyMotionInterface* self,
    PyObject* args);
static PyObject* PyMotionInterface_stopHeadMoves(PyMotionInterface* self,
    PyObject* args);
// Python method definitions
static PyMethodDef PyMotionInterface_methods[] = {

  {"isWalkActive",
    reinterpret_cast<PyCFunction>(PyMotionInterface_isWalkActive),
    METH_NOARGS,
    "isWalkActive() -> Bool.  Is the walk active."},
  {"enqueue",
    reinterpret_cast<PyCFunction>(PyMotionInterface_enqueue),
    METH_VARARGS,
    "enqueue(cmd) -> None.  Enqueue a BodyJointCommand, HeadJointCommand,\n"
    "or a HeadScanCommand through the MotionInterface."},
  {"getHeadSpeed", (PyCFunction)PyMotionInterface_getHeadSpeed, METH_NOARGS,
    "getHeadSpeed() -> float. Get the current head speed in rad/s."},
  {"getBalanceMode",
    reinterpret_cast<PyCFunction>(PyMotionInterface_getBalanceMode),
    METH_NOARGS,
    "getBalanceMode() -> int.  Retrieve the current balance mode."},
  {"getSupportMode",
    reinterpret_cast<PyCFunction>(PyMotionInterface_getSupportMode),
    METH_NOARGS,
    "getSupportMode() -> int.  Retrieve the current support mode."},
  {"setBalanceMode",
    reinterpret_cast<PyCFunction>(PyMotionInterface_setBalanceMode),
    METH_O,
    "setBalanceMode(mode) -> None.  Set the current balance mode."},
  {"setHead",
    reinterpret_cast<PyCFunction>(PyMotionInterface_setHead),
    METH_VARARGS,
    "setHead(time, yaw, pitch, type) -> None.  Clear the current head\n"
    "queue and move the head to the given position"},
  {"setNextWalkCommand",
    reinterpret_cast<PyCFunction>(PyMotionInterface_setNextWalkCommand),
    METH_VARARGS,
    "setNextWalkCommand(cmd) -> None.  Set the direction of travel via a\n"
    "WalkCommand through the MotionInterface."},
  {"setSupportMode",
    reinterpret_cast<PyCFunction>(PyMotionInterface_setSupportMode),
    METH_O,
    "setSupportMode(mode) -> None.  Set the current balance mode."},
  {"postGotoTorsoOrientation",
    reinterpret_cast<PyCFunction>(PyMotionInterface_postGotoTorsoOrientation),
   METH_VARARGS,
    "Set torso orientation."},

  {"setWalkConfig",
    reinterpret_cast<PyCFunction>(PyMotionInterface_setWalkConfig),
    METH_VARARGS,
    "setWalkConfig(pMaxStepLength, pMaxStepHeight, pMaxStepSide,\n"
    "              pMaxStepTurn, pZmpOffsetX, pZmpOffsetY)\n"
    "  -> None.\n\n"
    "Set the configuration parameters for the walk engine"},
  {"setWalkArmsConfig",
    reinterpret_cast<PyCFunction>(PyMotionInterface_setWalkArmsConfig),
    METH_VARARGS,
    "setWalkArmsConfig(pShoulderMedian, pShoulderAmplitude, pElbowMedian,\n"
    "                  pElbowAmplitude) -> None.\n\n"
    "Set the arm configuration parametets for the walk engine"},
  {"setWalkExtraConfig",
    reinterpret_cast<PyCFunction>(PyMotionInterface_setWalkExtraConfig),
    METH_VARARGS,
    "setWalkExtraConfig(pLHipRollBacklashCompensator,\n"
    "                   pRHipRollBacklashCompensator,\n"
    "                   pHipHeight, pTorsoYOrientation) -> None.\n\n"
    "Set the extra configuration parameters for the walk engine"},
  {"setWalkParameters",
    reinterpret_cast<PyCFunction>(PyMotionInterface_setWalkParameters),
    METH_O,
    "setWalkParaters(params) -> None.  Set all the walk parameters/configs at "
    "once"},
  {"stopBodyMoves",
    reinterpret_cast<PyCFunction>(PyMotionInterface_stopBodyMoves),
    METH_NOARGS,
    "stopBodyMoves() -> None.  Stop all current body movement."},
  {"stopHeadMoves",
    reinterpret_cast<PyCFunction>(PyMotionInterface_stopHeadMoves),
    METH_NOARGS,
    "stopBodyMoves() -> None.  Stop all current head movement."},

  /* Sentinel */
  {NULL}
};
// Python type definition
static PyTypeObject PyMotionInterfaceType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "_motion.PyMotionInterface", /*tp_name*/
    sizeof(PyMotionInterface), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyMotionInterface_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PyMotionInterface objects", /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PyMotionInterface_methods, /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    (newfunc)PyMotionInterface_new, /* tp_new */
};


//
// PyBodyJointCommand type
//

// Declare the C structure
typedef struct PyBodyJointCommand_t
{
  PyObject_HEAD
  // Python reference to the core
  PyObject *pycore;
  // hard reference to C command class
  BodyJointCommand *_cmd;
} PyBodyJointCommand;
// C method declarations
static PyBodyJointCommand* PyBodyJointCommand_new (PyTypeObject *type,
    PyObject* args, PyObject *kwds);
static PyObject* PyBodyJointCommand_conflicts (PyBodyJointCommand* self,
    PyObject* args);
static void PyBodyJointCommand_dealloc (PyBodyJointCommand* self);
// Python method definition
static PyMethodDef PyBodyJointCommand_methods[] = {
  {"conflicts",
    reinterpret_cast<PyCFunction>(PyBodyJointCommand_conflicts),
    METH_VARARGS,
    "Determine if two BodyJointCommands conflict with each other (if\n"
    "they attempt to set the same joints)"},
  {NULL} // Sentinel
};
// Python type definition
static PyTypeObject PyBodyJointCommandType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "_motion.PyBodyJointCommand", /*tp_name*/
    sizeof(PyBodyJointCommand), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyBodyJointCommand_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PyBodyJointCommand objects",  /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PyBodyJointCommand_methods, /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    (newfunc)PyBodyJointCommand_new, /* tp_new */
};



//
// PyHeadJointCommand type
//

// Declare the C structure
typedef struct PyHeadJointCommand_t
{
  PyObject_HEAD
  // Python reference to the core
  PyObject *pycore;
  // hard reference to C command class
  HeadJointCommand *_cmd;
} PyHeadJointCommand;
// C method declarations
static PyHeadJointCommand* PyHeadJointCommand_new (PyTypeObject *type,
    PyObject* args, PyObject *kwds);
static PyObject* PyHeadJointCommand_conflicts (PyHeadJointCommand* self,
    PyObject* args);
static void PyHeadJointCommand_dealloc (PyHeadJointCommand* self);
// Python method definition
static PyMethodDef PyHeadJointCommand_methods[] = {
  {NULL} // Sentinel
};
// Python type definition
static PyTypeObject PyHeadJointCommandType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "_motion.PyHeadJointCommand", /*tp_name*/
    sizeof(PyHeadJointCommand), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyHeadJointCommand_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PyHeadJointCommand objects",  /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PyHeadJointCommand_methods, /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    (newfunc)PyHeadJointCommand_new, /* tp_new */
};



//
// PyHeadScanCommand type
//

// Declare the C structure
typedef struct PyHeadScanCommand_t
{
  PyObject_HEAD
  // Python reference to the core
  PyObject *pycore;
  // hard reference to C command class
  HeadScanCommand *_cmd;
} PyHeadScanCommand;
// C method declarations
static PyHeadScanCommand* PyHeadScanCommand_new (PyTypeObject *type,
    PyObject* args, PyObject *kwds);
static PyObject* PyHeadScanCommand_conflicts (PyHeadScanCommand* self,
    PyObject* args);
static void PyHeadScanCommand_dealloc (PyHeadScanCommand* self);
// Python method definition
static PyMethodDef PyHeadScanCommand_methods[] = {
  {NULL} // Sentinel
};
// Python type definition
static PyTypeObject PyHeadScanCommandType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "_motion.PyHeadScanCommand", /*tp_name*/
    sizeof(PyHeadScanCommand), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyHeadScanCommand_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PyHeadScanCommand objects",  /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PyHeadScanCommand_methods, /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    (newfunc)PyHeadScanCommand_new, /* tp_new */
};



//
// PyWalkCommand type
//

enum WalkType {
  WALK_STRAIGHT = 0,
  WALK_SIDEWAYS,
  WALK_TURN,
  WALK_ARC,
  WALK_PYTHON,
};

static const char* WalkType_Names[] = {
  "WALK_STRAIGHT",
  "WALK_SIDEWAYS",
  "WALK_TURN",
  "WALK_ARC",
  "WALK_PYTHON",
};

// Declare the C structure
typedef struct PyWalkCommand_t
{
  PyObject_HEAD
  // Python reference to the core
  PyObject *pycore;
  // indicator for actual WalkCommand sub-class type
  WalkType type;
  // hard reference to C command class
  WalkCommand *_cmd;
} PyWalkCommand;
// C method declarations
static PyWalkCommand* PyWalkCommand_new (PyTypeObject *type,
    PyObject* args, PyObject *kwds);
static int PyWalkCommand_init (PyWalkCommand* self, PyObject* args,
    PyObject *kwds);
static void PyWalkCommand_dealloc (PyWalkCommand* self);
// Python method definition
static PyMethodDef PyWalkCommand_methods[] = {
  {NULL} // Sentinel
};
// Python type definition
static PyTypeObject PyWalkCommandType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "_motion.PyWalkCommand", /*tp_name*/
    sizeof(PyWalkCommand),   /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyWalkCommand_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PyWalkCommand objects", /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PyWalkCommand_methods,   /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyWalkCommand_init, /* tp_init */
    0,                         /* tp_alloc */
    (newfunc)PyWalkCommand_new, /* tp_new */
};



//
// PyWalkParameters type
//

// Declare the C structure
typedef struct PyWalkParameters_t
{
  PyObject_HEAD
  // hard reference to C command class
  WalkParameters *_params;
} PyWalkParameters;
// C method declarations
extern PyObject* PyWalkParameters_new (PyTypeObject* type, PyObject* args,
    PyObject* kwds);
extern int PyWalkParameters_init (PyWalkParameters* self, PyObject* args,
    PyObject *kwds);
extern void PyWalkParameters_dealloc (PyObject* self);
// Python method definition
static PyMethodDef PyWalkParameters_methods[] = {
  {NULL} // Sentinel
};
// Python type definition
static PyTypeObject PyWalkParametersType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "_motion.PyWalkParameters", /*tp_name*/
    sizeof(PyWalkParameters), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyWalkParameters_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PyWalkParameters objects",  /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PyWalkParameters_methods, /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyWalkParameters_init, /* tp_init */
    0,                         /* tp_alloc */
    (newfunc)PyWalkParameters_new, /* tp_new */
};


/*
//
// PyCallbackAction type
//

// Declare the C structure
typedef struct PyCallbackAction_t
{
  PyObject_HEAD
  // Python reference to the core
  PyObject *pycore;
  // hard reference to C command class
  CallbackAction *_cmd;
} PyCallbackAction;
// C method declarations
static PyCallbackAction* PyCallbackAction_new (PyTypeObject *type,
    PyObject* args, PyObject *kwds);
static void      PyCallbackAction_dealloc (PyCallbackAction* self);
// Python method definition
static PyMethodDef PyCallbackAction_methods[] = {
  {NULL} // Sentinel
};
// Python type definition
static PyTypeObject PyCallbackActionType = {
    PyObject_HEAD_INIT(NULL)
    0,                         //ob_size//
    "_motion.PyCallbackAction",//tp_name//
    sizeof(PyCallbackAction),  //tp_basicsize//
    0,                         //tp_itemsize//
    (destructor)PyCallbackAction_dealloc, //tp_dealloc//
    0,                         //tp_print//
    0,                         //tp_getattr//
    0,                         //tp_setattr//
    0,                         //tp_compare//
    0,                         //tp_repr//
    0,                         //tp_as_number//
    0,                         //tp_as_sequence//
    0,                         //tp_as_mapping//
    0,                         //tp_hash //
    0,                         //tp_call//
    0,                         //tp_str//
    0,                         //tp_getattro//
    0,                         //tp_setattro//
    0,                         //tp_as_buffer//
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, //tp_flags//
    "PyCallbackAction objects",// tp_doc //
    0,                         // tp_traverse //
    0,                         // tp_clear //
    0,                         // tp_richcompare //
    0,                         // tp_weaklistoffset //
    0,                         // tp_iter //
    0,                         // tp_iternext //
    PyCallbackAction_methods,  // tp_methods //
    0,                         // tp_members //
    0,                         // tp_getset //
    0,                         // tp_base //
    0,                         // tp_dict //
    0,                         // tp_descr_get //
    0,                         // tp_descr_set //
    0,                         // tp_dictoffset //
    0,                         // tp_init //
    0,                         // tp_alloc //
    (newfunc)PyCallbackAction_new, // tp_new //
};

*/

#endif // _motionmodule_h_DEFINED
