
#include <vector>
#include "altypes.h"
#include "_ledsmodule.h"
#include "corpusconfig.h"

using namespace AL;

static const string LED_NAMES[] = {
  "ChestBoard/Led/Red/Actuator/Value",
  "ChestBoard/Led/Green/Actuator/Value",
  "ChestBoard/Led/Blue/Actuator/Value",
  "LFoot/Led/Red/Actuator/Value",
  "LFoot/Led/Green/Actuator/Value",
  "LFoot/Led/Blue/Actuator/Value",
  "RFoot/Led/Red/Actuator/Value",
  "RFoot/Led/Green/Actuator/Value",
  "RFoot/Led/Blue/Actuator/Value",
  "Ears/Led/Left/0Deg/Actuator/Value",
  "Ears/Led/Left/36Deg/Actuator/Value",
  "Ears/Led/Left/72Deg/Actuator/Value",
  "Ears/Led/Left/108Deg/Actuator/Value",
  "Ears/Led/Left/144Deg/Actuator/Value",
  "Ears/Led/Left/180Deg/Actuator/Value",
  "Ears/Led/Left/216Deg/Actuator/Value",
  "Ears/Led/Left/252Deg/Actuator/Value",
  "Ears/Led/Left/288Deg/Actuator/Value",
  "Ears/Led/Left/324Deg/Actuator/Value",
  "Ears/Led/Right/0Deg/Actuator/Value",
  "Ears/Led/Right/36Deg/Actuator/Value",
  "Ears/Led/Right/72Deg/Actuator/Value",
  "Ears/Led/Right/108Deg/Actuator/Value",
  "Ears/Led/Right/144Deg/Actuator/Value",
  "Ears/Led/Right/180Deg/Actuator/Value",
  "Ears/Led/Right/216Deg/Actuator/Value",
  "Ears/Led/Right/252Deg/Actuator/Value",
  "Ears/Led/Right/288Deg/Actuator/Value",
  "Ears/Led/Right/324Deg/Actuator/Value",
  "Face/Led/Red/Left/0Deg/Actuator/Value",
  "Face/Led/Green/Left/0Deg/Actuator/Value",
  "Face/Led/Blue/Left/0Deg/Actuator/Value",
  "Face/Led/Red/Left/45Deg/Actuator/Value",
  "Face/Led/Green/Left/45Deg/Actuator/Value",
  "Face/Led/Blue/Left/45Deg/Actuator/Value",
  "Face/Led/Red/Left/90Deg/Actuator/Value",
  "Face/Led/Green/Left/90Deg/Actuator/Value",
  "Face/Led/Blue/Left/90Deg/Actuator/Value",
  "Face/Led/Red/Left/135Deg/Actuator/Value",
  "Face/Led/Green/Left/135Deg/Actuator/Value",
  "Face/Led/Blue/Left/135Deg/Actuator/Value",
  "Face/Led/Red/Left/180Deg/Actuator/Value",
  "Face/Led/Green/Left/180Deg/Actuator/Value",
  "Face/Led/Blue/Left/180Deg/Actuator/Value",
  "Face/Led/Red/Left/225Deg/Actuator/Value",
  "Face/Led/Green/Left/225Deg/Actuator/Value",
  "Face/Led/Blue/Left/225Deg/Actuator/Value",
  "Face/Led/Red/Left/270Deg/Actuator/Value",
  "Face/Led/Green/Left/270Deg/Actuator/Value",
  "Face/Led/Blue/Left/270Deg/Actuator/Value",
  "Face/Led/Red/Left/315Deg/Actuator/Value",
  "Face/Led/Green/Left/315Deg/Actuator/Value",
  "Face/Led/Blue/Left/315Deg/Actuator/Value",
  "Face/Led/Red/Right/0Deg/Actuator/Value",
  "Face/Led/Green/Right/0Deg/Actuator/Value",
  "Face/Led/Blue/Right/0Deg/Actuator/Value",
  "Face/Led/Red/Right/45Deg/Actuator/Value",
  "Face/Led/Green/Right/45Deg/Actuator/Value",
  "Face/Led/Blue/Right/45Deg/Actuator/Value",
  "Face/Led/Red/Right/90Deg/Actuator/Value",
  "Face/Led/Green/Right/90Deg/Actuator/Value",
  "Face/Led/Blue/Right/90Deg/Actuator/Value",
  "Face/Led/Red/Right/135Deg/Actuator/Value",
  "Face/Led/Green/Right/135Deg/Actuator/Value",
  "Face/Led/Blue/Right/135Deg/Actuator/Value",
  "Face/Led/Red/Right/180Deg/Actuator/Value",
  "Face/Led/Green/Right/180Deg/Actuator/Value",
  "Face/Led/Blue/Right/180Deg/Actuator/Value",
  "Face/Led/Red/Right/225Deg/Actuator/Value",
  "Face/Led/Green/Right/225Deg/Actuator/Value",
  "Face/Led/Blue/Right/225Deg/Actuator/Value",
  "Face/Led/Red/Right/270Deg/Actuator/Value",
  "Face/Led/Green/Right/270Deg/Actuator/Value",
  "Face/Led/Blue/Right/270Deg/Actuator/Value",
  "Face/Led/Red/Right/315Deg/Actuator/Value",
  "Face/Led/Green/Right/315Deg/Actuator/Value",
  "Face/Led/Blue/Right/315Deg/Actuator/Value"
};
static const int NUM_LEDS = sizeof(LED_NAMES) / sizeof(string);

static const string LED_GROUPS[] = {
  "AllLeds",
  "AllLedsBlue",
  "AllLedsGreen",
  "AllLedsRed",
  "ChestLeds",
  "EarLeds",
  "FaceLeds",
  "FeetLeds",
  "LeftEarLeds",
  "LeftEarLedsBack",
  "LeftEarLedsEven",
  "LeftEarLedsFront",
  "LeftEarLedsOdd",
  "LeftFaceLeds",
  "LeftFaceLedsBlue",
  "LeftFaceLedsGreen",
  "LeftFaceLedsRed",
  "LeftFootLeds",
  "RightEarLeds",
  "RightEarLedsBack",
  "RightEarLedsEven",
  "RightEarLedsFront",
  "RightEarLedsOdd",
  "RightFaceLeds",
  "RightFaceLedsBlue",
  "RightFaceLedsGreen",
  "RightFaceLedsRed",
  "RightFootLeds"
};
static const int NUM_LED_GROUPS = sizeof(LED_GROUPS) / sizeof(string);

//
// _leds module initialization
//

static PyMethodDef _leds_methods[] = {
  // Sentinel
  { NULL }
};

PyMODINIT_FUNC
init_leds (void)
{
  // Initialize LEDs type object
  if (PyType_Ready(&PyLEDsType) < 0)
    return; // error handled by Python

  // Initialize module
  PyObject *module = Py_InitModule3("_leds",
      _leds_methods,
      "Python wrapped access to the Aldebaran LEDs proxy");
  if (module == NULL)
    return; // error handled by Python

  // Add LEDs type reference to the module
  Py_INCREF(&PyLEDsType);
  PyModule_AddObject(module, "LEDs", reinterpret_cast<PyObject*>(&PyLEDsType));
}


//
// PyLEDS definitions
//

PyLEDs*
PyLEDs_new ()
{
  return reinterpret_cast<PyLEDs*>(PyLEDs_new(&PyLEDsType, NULL, NULL));
}

PyObject*
PyLEDs_new (PyTypeObject* type, PyObject* args, PyObject* kwds)
{
  PyObject *self = type->tp_alloc(type, 0);

  if (self != NULL) {
    PyLEDs* leds = reinterpret_cast<PyLEDs*>(self);
    try {
#ifdef USE_PY_LEDS_CXX_BACKEND
      leds->proxy = new ALProxy("ALLeds");
#endif

    }catch (ALError &e) {
      PyErr_Format(PyExc_SystemError,
          "Error occured initializing module proxy.\n  %s",
          e.toString(VerbosityFull).c_str()
          );
      PyLEDs_dealloc(self);
      self = NULL;
    }

    leds->groups = vector<string>();
  }

  return self;
}

void
PyLEDs_dealloc (PyObject* self)
{
  self->ob_type->tp_free(self);
}

const string*
get_led_or_group (PyLEDs *self,  int id)
{
  if (id < 0)
    return NULL;
  else if (id < NUM_LEDS)
    return &LED_NAMES[id];

  id -= NUM_LEDS;
  if (id < NUM_LED_GROUPS)
    return &LED_GROUPS[id];

  id -= NUM_LED_GROUPS;
  if (id < (signed int)self->groups.size())
    return &self->groups[id];

  return NULL;
}

PyObject*
PyLEDs_createGroup (PyLEDs *self, PyObject *args)
{
  char *name;
  PyObject *seq;

  if (!PyArg_ParseTuple(args, "sO:createGroup", &name, &seq))
    return NULL;

  PyObject *fast = PySequence_Fast(seq,
      "createGroup() expected a sequence of string or integer ids of leds "
      "as the second argument");
  if (fast == NULL)
    return NULL;

  // convert sequence of ints or strings to a string vector
  vector<string> leds(PySequence_Fast_GET_SIZE(fast), string());
  PyObject *o;
  for (int i = 0; i < PySequence_Fast_GET_SIZE(fast); i++) {
    o = PySequence_Fast_GET_ITEM(fast, i);
    if (PyInt_Check(o)) {
      int id = PyInt_AsLong(o);
      if (id > 0 && id < NUM_LEDS)
        leds[i] = LED_NAMES[i];
      else {
        PyErr_Format(PyExc_ValueError, "invalid led id given (%i)", id);
        Py_DECREF(fast);
        return NULL;
      }
    }else if (PyString_Check(o))
      leds[i] = string(PyString_AsString(o));

    else {
      PyErr_Format(PyExc_TypeError,
        "createGroup() expected a sequence of string or integer ids of leds "
        "as the second argument (%s in the list)", o->ob_type->tp_name);
      Py_DECREF(fast);
      return NULL;
    }
  }

  // call proxy method with group anem and array of leds names
  try {
#ifdef USE_PY_LEDS_CXX_BACKEND
    self->proxy->callVoid("createGroup", string(name), ALValue(leds));
#endif
  }catch (ALError &e) {
    PyErr_Format(PyExc_SystemError,
        "createGroup() returned an ALError.\n  %s",
        e.toString(VerbosityFull).c_str()
        );
    return NULL;
  }
  self->groups.push_back(name);
  return PyInt_FromLong(self->groups.size() - 1);
}

PyObject*
PyLEDs_off (PyLEDs *self, PyObject *args)
{
  string tmp;
  const string *name = &tmp;

  if (PyInt_Check(args)) {
    int id = PyInt_AsLong(args);
    name = get_led_or_group(self, id);
    if (name != NULL) {
      PyErr_Format(PyExc_ValueError, "unknown integer reference id (%i given)", 
          id);
      return NULL;
    }
  }else if (PyString_Check(args))
    tmp = string(PyString_AsString(args));

  else {
    PyErr_Format(PyExc_TypeError, "expected integer or string (%s given)",
        args->ob_type->tp_name);
    return NULL;
  }

  try {
#ifdef USE_PY_LEDS_CXX_BACKEND
    self->proxy->callVoid("off", *name);
#endif
  }catch (ALError &e) {
    PyErr_Format(PyExc_SystemError,
        "Error occured initializing module proxy.\n  %s",
        e.toString(VerbosityFull).c_str()
        );
    return NULL;
  }
  Py_RETURN_NONE;
}

PyObject*
PyLEDs_on (PyLEDs *self, PyObject *args)
{
  string tmp;
  const string *name = &tmp;

  if (PyInt_Check(args)) {
    int id = PyInt_AsLong(args);
    name = get_led_or_group(self, id);
    if (name == NULL) {
      PyErr_Format(PyExc_ValueError, "unknown integer reference id (%i given)", 
          id);
      return NULL;
    }
  }else if (PyString_Check(args))
    tmp = string(PyString_AsString(args));

  else {
    PyErr_Format(PyExc_TypeError, "expected integer or string (%s given)",
        args->ob_type->tp_name);
    return NULL;
  }

  try {
#ifdef USE_PY_LEDS_CXX_BACKEND
    self->proxy->callVoid("on", *name);
#endif
  }catch (ALError &e) {
    PyErr_Format(PyExc_SystemError,
        "createGroup() returned an ALError.\n  %s",
        e.toString(VerbosityFull).c_str()
        );
    return NULL;
  }
  Py_RETURN_NONE;
}

PyObject*
PyLEDs_set (PyLEDs *self, PyObject *args)
{
  PyObject *arg2;
  float intensity;
  string tmp;
  const string *name = &tmp;

  if (!PyArg_ParseTuple(args, "Of:set", &arg2, &intensity))
    return NULL;

  if (PyInt_Check(arg2)) {
    int id = PyInt_AsLong(arg2);
    name = get_led_or_group(self, id);
    if (name == NULL) {
      PyErr_Format(PyExc_ValueError, "unknown integer reference id (%i given)", 
          id);
      return NULL;
    }
  }else if (PyString_Check(arg2))
    tmp = string(PyString_AsString(arg2));

  else {
    PyErr_Format(PyExc_TypeError, "expected integer or string (%s given)",
        arg2->ob_type->tp_name);
    return NULL;
  }

  try {
#ifdef USE_PY_LEDS_CXX_BACKEND
    self->proxy->callVoid("set", *name, intensity);
#endif
  }catch (ALError &e) {
    PyErr_Format(PyExc_SystemError,
        "createGroup() returned an ALError.\n  %s",
        e.toString(VerbosityFull).c_str()
        );
    return NULL;
  }
  Py_RETURN_NONE;
}
