#include "potentialfieldmodule.h"

static char *ModuleName = "potentialfield";
static PyObject *PotentialFieldError;

typedef struct PotentialFieldObject_t {
  PyObject_HEAD
  PotentialField *potentialField;
} PotentialFieldObject;

inline PyObject *PotentialField_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
inline int PotentialField_init(PotentialFieldObject *self, PyObject *args);
inline void PotentialField_dealloc(PotentialFieldObject *self);
inline PyObject *PotentialField_movementVectorAt(PotentialFieldObject *self, PyObject *args);
inline PyObject *PotentialField_addFieldCharges(PotentialFieldObject *self, PyObject *args);
inline PyObject *PotentialField_removeCharge(PotentialFieldObject *self, PyObject *args);
inline PyObject *PotentialField_addPointCharge(PotentialFieldObject *self, PyObject *args);
inline PyObject *PotentialField_addLineCharge(PotentialFieldObject *self, PyObject *args);
inline PyObject *PotentialField_addSegmentCharge(PotentialFieldObject *self, PyObject *args);
inline PyObject *PotentialField_heightAt(PotentialFieldObject *self, PyObject *args);
inline PyObject *PotentialField_movePointCharge(PotentialFieldObject *self, PyObject *args);
inline PyObject *PotentialField_moveLineCharge(PotentialFieldObject *self, PyObject *args);
inline PyObject *PotentialField_moveSegmentCharge(PotentialFieldObject *self, PyObject *args);

static PyMethodDef PotentialField_methods[] = {
  /*
   * Potential Field methods
   */
  {"movementVectorAt", (PyCFunction)PotentialField_movementVectorAt, METH_VARARGS,
   "Return a vector pointing in direction of steepest descent"},
  {"addFieldCharges", (PyCFunction)PotentialField_addFieldCharges, METH_VARARGS,
   "Add all the regular charges that are by default on the field"},
  {"removeCharge", (PyCFunction)PotentialField_removeCharge, METH_VARARGS,
   "Remove a charge from the Potential Field"},
  {"addPointCharge", (PyCFunction)PotentialField_addPointCharge, METH_VARARGS,
   "Add a single point charge to the Potential Field"},
  {"addLineCharge", (PyCFunction)PotentialField_addLineCharge, METH_VARARGS,
   "Add a single line charge to the Potential Field"},
  {"addSegmentCharge", (PyCFunction)PotentialField_addSegmentCharge, METH_VARARGS,
   "Add a single segment charge to the Potential Field"},
  {"heightAt", (PyCFunction)PotentialField_heightAt, METH_VARARGS,
    "Return the calculated field strength (height) at the given location."},
  {"movePointCharge", (PyCFunction)PotentialField_movePointCharge, METH_VARARGS,
   "Move a point charge that already exists to a different location"},
  {"moveLineCharge", (PyCFunction)PotentialField_moveLineCharge, METH_VARARGS,
   "Move a line charge that already exists to a different location"},
  {"moveSegmentCharge", (PyCFunction)PotentialField_moveSegmentCharge, METH_VARARGS,
   "Move a segment charge that already exists to a different location"},
  /*
   * Generic
   */
  {NULL, NULL, 0, NULL} // Sentinel
};

static PyTypeObject PotentialFieldType = {
  PyObject_HEAD_INIT(NULL)
  0,                         /*ob_size*/
  "potentialfield.PotentialField", /*tp_name*/
  sizeof(PotentialFieldObject), /*tp_basicsize*/
  0,                         /*tp_itemsize*/
  (destructor)PotentialField_dealloc,/*tp_dealloc*/
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
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_CHECKTYPES, /*tp_flags*/
  "",                        /* tp_doc */
  0,                         /* tp_traverse */
  0,                         /* tp_clear */
  0,                         /* tp_richcompare */
  0,                         /* tp_weaklistoffset */
  0,                         /* tp_iter */
  0,                         /* tp_iternext */
  PotentialField_methods,    /* tp_methods */
  0,                         /* tp_members */
  0,                         /* tp_getset */
  0,                         /* tp_base */
  0,                         /* tp_dict */
  0,                         /* tp_descr_get */
  0,                         /* tp_descr_set */
  0,                         /* tp_dictoffset */
  (initproc)PotentialField_init,/* tp_init */
  0,                         /* tp_alloc */
  PotentialField_new,        /* tp_new */
  0,                         /* tp_free */
  0,                         /* tp_is_gc */
  0,                         /* tp_bases */
  0,                         /* tp_mro */
  0,                         /* tp_cache */
  0,                         /* tp_subclasses */
  0,                         /* tp_weaklist */
  0,                         /* tp_del */
};

inline void PotentialField_dealloc(PotentialFieldObject *self)
{
  delete self->potentialField;
  self->ob_type->tp_free((PyObject*)self);
}

inline PyObject
*PotentialField_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  PotentialFieldObject *self;

  self = (PotentialFieldObject *)type->tp_alloc(type, 0);
  if (self) {
    self->potentialField = NULL;
  }
  return (PyObject *)self;
}

inline int PotentialField_init(PotentialFieldObject *self, PyObject *args)
{
  if (PyTuple_Size(args) == 0) {
    /*o = PyTuple_GetItem(args, 0);
    if (PyObject_TypeCheck(o, &PotentialFieldType)) {
      self->potentialField = new PotentialField();
      return self->potentialField == NULL ? -1 : 0;*/
    self->potentialField = new PotentialField();
    return 0;
  }
  else {
    char err_msg[100];
    sprintf(err_msg, "__init__() got %i argument, expected none",
	    PyTuple_Size(args));
    PyErr_SetString(PyExc_TypeError, err_msg);
    return -1;
  }
  return -1;
}

inline PyObject *PotentialField_movementVectorAt(PotentialFieldObject *self, PyObject *args)
{
  float x, y;
  
  if (PyArg_ParseTuple(args, "ff:movementVectorAt", &x, &y)) {
    Vector2D vec;
    bool x_eq, y_eq;
    int x_eq2, y_eq2;
    self->potentialField->movementVectorAt(&vec, &x_eq, &y_eq, x, y);
    if (x_eq) x_eq2 = 1;
    else      x_eq2 = 0;
    if (y_eq) y_eq2 = 1;
    else      y_eq2 = 0;
    return Py_BuildValue("ffii", vec.x, vec.y, x_eq2, y_eq2);
  }
  else {
    PyErr_SetString(PotentialFieldError, "PotentialField::movementVectorAt needs exactly 1 argument");
    return NULL;
  }
}


inline PyObject *PotentialField_addFieldCharges(PotentialFieldObject *self, PyObject *args) {
  self->potentialField->addFieldCharges();

  // need to return a tuple of all the charge ids
  Py_INCREF(Py_None);
  return Py_None;
}

inline PyObject *PotentialField_removeCharge(PotentialFieldObject *self, PyObject *args) {
  int charge_id;

  if (PyArg_ParseTuple(args, "i:removeCharge", &charge_id)) {
    self->potentialField->removeCharge(charge_id);
  }
  
  Py_INCREF(Py_None);
  return Py_None;
}

inline PyObject *PotentialField_addPointCharge(PotentialFieldObject *self, PyObject *args) {
  float attraction, spread, x, y;
  int affects_goalbox; //should be bool, but don't know how to do it...

  unsigned int charge_id;

  if (PyTuple_Size(args) == 4) {
    if (PyArg_ParseTuple(args, "ffff:addPointCharge", &attraction, &spread,
		       &x, &y)) {
      affects_goalbox = true;
      charge_id = self->potentialField->addPointCharge(attraction, spread, x, y,
						       affects_goalbox);
    return Py_BuildValue("i", charge_id);
  }
  }
  else if (PyTuple_Size(args) == 5) {
    if (PyArg_ParseTuple(args, "ffffi:addPointCharge", &attraction, &spread,
		       &x, &y, &affects_goalbox)) {
      charge_id = self->potentialField->addPointCharge(attraction, spread, x, y,
						       affects_goalbox);
      return Py_BuildValue("i", charge_id);
    }
  }

  else {
    PyErr_SetString(PotentialFieldError, "PotentialField::addPointCharge needs 4 or 5 arguments");
  }
  return NULL;
}

inline PyObject *PotentialField_addLineCharge(PotentialFieldObject *self, PyObject *args) {
  float attraction, spread, x1, y1, x2, y2;
  int affects_goalbox; //should be bool, but don't know how to do it...

  unsigned int charge_id;

  if (PyTuple_Size(args) == 6) {
    if (PyArg_ParseTuple(args, "ffffff:addLineCharge", &attraction, &spread,
			 &x1, &y1, &x2, &y2)) {
      affects_goalbox = true;
      charge_id = self->potentialField->addLineCharge(attraction, spread,
						      x1, y1, x2, y2,
						      affects_goalbox);
    return Py_BuildValue("i", charge_id);
  }
  }
  else if (PyTuple_Size(args) == 7) {
    if (PyArg_ParseTuple(args, "ffffffi:addLineCharge", &attraction, &spread,
			 &x1, &y1, &x2, &y2, &affects_goalbox)) {
      charge_id = self->potentialField->addLineCharge(attraction, spread,
						      x1, y1, x2, y2,
						      affects_goalbox);
      return Py_BuildValue("i", charge_id);
    }
  }

  else {
    PyErr_SetString(PotentialFieldError, "PotentialField::addLineCharge needs 6 or 7 arguments");
  }
  return NULL;
}

inline PyObject *PotentialField_addSegmentCharge(PotentialFieldObject *self, PyObject *args) {
  float attraction, spread, x1, y1, x2, y2;
  int affects_goalbox; //should be bool, but don't know how to do it...

  unsigned int charge_id;

  if (PyTuple_Size(args) == 6) {
    if (PyArg_ParseTuple(args, "ffffff:addSegmentCharge", &attraction, &spread,
			 &x1, &y1, &x2, &y2)) {
      affects_goalbox = true;
      charge_id = self->potentialField->addSegmentCharge(attraction, spread,
							 x1, y1, x2, y2,
							 affects_goalbox);
    return Py_BuildValue("i", charge_id);
    }
  }
  else if (PyTuple_Size(args) == 7) {
    if (PyArg_ParseTuple(args, "ffffffi:addSegmentCharge", &attraction, &spread,
			 &x1, &y1, &x2, &y2, &affects_goalbox)) {
      charge_id = self->potentialField->addSegmentCharge(attraction, spread,
							 x1, y1, x2, y2,
							 affects_goalbox);
      return Py_BuildValue("i", charge_id);
    }
  }

  else {
    PyErr_SetString(PotentialFieldError, "PotentialField::addSegmentCharge needs 6 or 7 arguments");
  }
  return NULL;
}

inline PyObject *PotentialField_heightAt(PotentialFieldObject *self, PyObject *args) {
  float x, y;
  if (PyArg_ParseTuple(args, "ff:heightAt", &x, &y))
    return PyFloat_FromDouble(self->potentialField->heightAt(x, y));
  else
    return NULL;
}

inline PyObject *PotentialField_movePointCharge(PotentialFieldObject *self, PyObject *args) {
  unsigned int charge_id;
  float x, y;
  if (PyArg_ParseTuple(args, "iff", &charge_id, &x, &y)) {
    self->potentialField->movePointCharge(charge_id, x, y);
    Py_INCREF(Py_None);
    return Py_None;
  }
  else {
    PyErr_SetString(PotentialFieldError, "PotentialField::movePointCharge needs exactly 3 arguments");
    return NULL;
  }
}

inline PyObject *PotentialField_moveLineCharge(PotentialFieldObject *self, PyObject *args) {
  unsigned int charge_id;
  float x1, y1, x2, y2;
  if (PyArg_ParseTuple(args, "iffff", &charge_id, &x1, &y1, &x2, &y2)) {
    self->potentialField->moveLineCharge(charge_id, x1, y1, x2, y2);
    Py_INCREF(Py_None);
    return Py_None;
  }
  else {
    PyErr_SetString(PotentialFieldError, "moveLineCharge() needs exactly 3 arguments");
    return NULL;
  }
}

inline PyObject *PotentialField_moveSegmentCharge(PotentialFieldObject *self, PyObject *args) {
  unsigned int charge_id;
  float x1, y1, x2, y2;
  if (PyArg_ParseTuple(args, "iffff", &charge_id, &x1, &y1, &x2, &y2)) {
    self->potentialField->moveSegmentCharge(charge_id, x1, y1, x2, y2);
    Py_INCREF(Py_None);
    return Py_None;
  }
  else {
    PyErr_SetString(PotentialFieldError,"PotentialField::moveSegmentCharge needs exactly 3 arguments");
    return NULL;
  }
}

static PyMethodDef module_methods[] = {
  /*
   * Generic
   */
  {NULL, NULL, 0, NULL} // Sentinel
};

PyMODINIT_FUNC
initpotentialfield(void) 
{
    if (!Py_IsInitialized())
      Py_Initialize();

    PyObject* m;
    m = Py_InitModule3(ModuleName, module_methods,
                       "Module that creates an extension type.");

    PotentialFieldError = PyErr_NewException("potential field.error", NULL, NULL);
    Py_INCREF(PotentialFieldError);
    PyModule_AddObject(m, "error", PotentialFieldError);
    
    PotentialFieldType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&PotentialFieldType) < 0)
        return;

    Py_INCREF(&PotentialFieldType);
    PyModule_AddObject(m, "PotentialField", (PyObject *)&PotentialFieldType);

    // Spread constants
    PyModule_AddObject(m, "DISTANCE_NEUTRAL", PyFloat_FromDouble(DISTANCE_NEUTRAL ));
    PyModule_AddObject(m, "WHOLE_FIELD", PyFloat_FromDouble(WHOLE_FIELD ));
    PyModule_AddObject(m, "HALF_FIELD", PyFloat_FromDouble(HALF_FIELD));
    PyModule_AddObject(m, "REGION", PyFloat_FromDouble(REGION));
    PyModule_AddObject(m, "SMALL_REGION", PyFloat_FromDouble(SMALL_REGION));
    PyModule_AddObject(m, "VERY_SMALL_REGION", PyFloat_FromDouble(VERY_SMALL_REGION));
    PyModule_AddObject(m, "UTTERLY_SMALL_REGION", PyFloat_FromDouble(UTTERLY_SMALL_REGION));
    PyModule_AddObject(m, "LINE_SPREAD", PyFloat_FromDouble(LINE_SPREAD));
    PyModule_AddObject(m, "POINT_SPREAD", PyFloat_FromDouble(POINT_SPREAD));

    // Attraction constants
    PyModule_AddObject(m, "ATTRACTION", PyFloat_FromDouble(ATTRACTION));
    PyModule_AddObject(m, "REPULSION", PyFloat_FromDouble(REPULSION));
    PyModule_AddObject(m, "VERY_STRONG", PyFloat_FromDouble(VERY_STRONG));
    PyModule_AddObject(m, "STRONG", PyFloat_FromDouble(STRONG));
    PyModule_AddObject(m, "MEDIUM", PyFloat_FromDouble(MEDIUM_));
    PyModule_AddObject(m, "WEAK", PyFloat_FromDouble(WEAK));
    PyModule_AddObject(m, "VERY_WEAK", PyFloat_FromDouble(VERY_WEAK));

    // Standard field setup
    PyModule_AddObject(m, "SIDELINE_REPULSION", PyFloat_FromDouble(SIDELINE_REPULSION));
    PyModule_AddObject(m, "SIDELINE_SPREAD", PyFloat_FromDouble(SIDELINE_SPREAD));
    PyModule_AddObject(m, "MY_GOALBOX_REPULSION", PyFloat_FromDouble(MY_GOALBOX_REPULSION));
    PyModule_AddObject(m, "MY_GOALBOX_POINT_SPREAD", PyFloat_FromDouble(MY_GOALBOX_POINT_SPREAD));
    PyModule_AddObject(m, "MY_GOALBOX_LINE_SPREAD", PyFloat_FromDouble(MY_GOALBOX_LINE_SPREAD));

    PyModule_AddObject(m, "SIDELINE_OFFSET", PyFloat_FromDouble(SIDELINE_OFFSET));

    PyModule_AddObject(m, "EQUILIBRIUM_RANGE", PyFloat_FromDouble(EQUILIBRIUM_RANGE));
}
