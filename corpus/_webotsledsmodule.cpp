//JUNK FILE



#include <vector>
#include "_webotsledsmodule.h"
#include "corpusconfig.h"


//
// _leds module initialization
//

static PyMethodDef _leds_methods[] = {
  // Sentinel
  { NULL }
};

PyMODINIT_FUNC
init_leds ()
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
  }

  return self;
}

void
PyLEDs_dealloc (PyObject* self)
{
  self->ob_type->tp_free(self);
}

PyObject*
PyLEDs_dummy (PyLEDs *self, PyObject *args)
{

  Py_RETURN_NONE;

}
