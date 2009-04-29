
#ifndef __ledsmodule_h_DEFINED
#define __ledsmodule_h_DEFINED

#include <albroker.h>
#include <alledsproxy.h>
#include <alptr.h>

#include <Python.h>

//
// _leds module initialization
//
void setLedsProxy(AL::ALPtr<AL::ALLedsProxy> led_proxy);

PyMODINIT_FUNC init_leds();

//
// PyLEDS definitions
//

typedef struct PyLEDs_t {
  PyObject_HEAD
  // Aldeberan proxy to the leds
  AL::ALPtr<AL::ALLedsProxy> proxy;
  vector<string> groups;
} PyLEDs;

// C++ - accessible interface
extern PyLEDs*   PyLEDs_new    ();
// backend methods
extern PyObject* PyLEDs_new    (PyTypeObject* type, PyObject* args,
                                PyObject* kwds);
extern void      PyLEDs_dealloc(PyObject* self);
// Python - accessible interface
extern PyObject* PyLEDs_createGroup(PyLEDs* self, PyObject* args);
extern PyObject* PyLEDs_off(PyLEDs* self, PyObject* args);
extern PyObject* PyLEDs_on (PyLEDs* self, PyObject* args);
extern PyObject* PyLEDs_set(PyLEDs* self, PyObject* args);
extern PyObject* PyLEDs_fadeRGB(PyLEDs* self, PyObject* args);

//extern PyObject* PyLEDs_spinGroup(PyLEDs* self, PyObject* args);
// Method list
static PyMethodDef PyLEDs_methods[] = {

  {"createGroup", reinterpret_cast<PyCFunction>(PyLEDs_createGroup),
    METH_VARARGS,
    "createGroup(name, leds) --> None. Group the given LEDS under a common"
    "group name.\n"
    "\n"
    "LED ids amay be integer ids or string names."},

  {"off", reinterpret_cast<PyCFunction>(PyLEDs_off), METH_O,
    "off(id) --> None. Switch the given LED or group to minimum intensity."
    "\n\n"
    "Accepts either an integer ID or string name of and LED or group \n"
    "(group id must be a string name of a group created via createGroup())"},

  {"on", reinterpret_cast<PyCFunction>(PyLEDs_on), METH_O,
    "on(id) --> None. Switch the given LED or group to maximum intensity."
    "\n\n"
    "Accepts either an integer ID or string name of and LED or group \n"
    "(group id must be a string name of a group created via createGroup())"},

  {"set", reinterpret_cast<PyCFunction>(PyLEDs_set), METH_VARARGS,
    "set(id) --> None. Set the given LED or group to the given intensity."
    "\n\n"
    "Accepts either an integer ID or string name of and LED or group \n"
    "(group id must be a string name of a group created via createGroup())"},
  {"fadeRGB", reinterpret_cast<PyCFunction>(PyLEDs_fadeRGB), METH_VARARGS,
    "fadeRGB(id,0xRRGGBB,seconds) --> None. Set the given LED or group to the "
    "given RGB Hex value over a certain number of seconds."
    "\n\n"
    "Accepts either an integer ID or string name of and LED or group \n"
    "(group id must be a string name of a group created via createGroup())"},
  //{"spinGroup", reinterpret_cast<PyCFunction>(PyLEDs_spinGroup), METH_VARARGS,
  //  "spinGroup(name, start=0, forwards=True, onTime=1.0,\n"
  //  "          fadeTime=.5, totalTime=4.0) --> None.\n"
  //  "  Spin a group of LEDs."},

  // Sentinel
  { NULL }
};

// PyLEDs type definition
static PyTypeObject PyLEDsType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "_leds.LEDs",               /*tp_name*/
    sizeof(PyLEDs),            /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyLEDs_dealloc,/*tp_dealloc*/
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
    "LEDs object",             /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PyLEDs_methods,            /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    (newfunc)PyLEDs_new,       /* tp_new */
};


#endif
