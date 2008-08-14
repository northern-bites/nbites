
#ifndef PyVision_h
#define PyVision_h

#include <Python.h>
#include <structmember.h>

#include "Common.h"
#include "Vision.h"

#if ROBOT(NAO)
#  define MODULE_HEAD "nao"
#  define MODULE_INIT(tail) (initnao ## tail)
#elif ROBOT(AIBO)
#  define MODULE_HEAD "aibo"
#  define MODULE_INIT(tail) (initaibo ## tail)
#else
#  error "Undefined robot type"
#endif



/*
  Courtesy of MySQL - http://lists.mysql.com/commits/13963

  Auxillary macro which can be used to get offsets of members in
  class for non-POD types (we can't use use offsetof() macro in such
  cases according to the current version of C++ standard, but probably
  it is still OK to assume that all instances of the class has the
  same offsets for the same members in many cases).
*/

#define offsetof_in_object(OBJECT, MEMBER) \
          ((size_t) ((char *)&(OBJECT.MEMBER) - (char*)&OBJECT))



//
// PyPose definitions
//


typedef struct PyPose_t {
  PyObject_HEAD
  Pose *pose;

  // Horizon y coordinates
  PyObject *leftHorizonY, *rightHorizonY;
  // Slope of the horizon line
  PyObject *horizonSlope;
  // Height of body in space
  //PyObject *bodyCenterHeight;
  // Angle of head to body
  //PyObject *panAngle;
} PyPose;

// C++ - accessible interface
extern PyObject *PyPose_new    (Pose *p);
extern void      PyPose_update (PyPose *p);
// backend methods
extern PyObject *PyPose_new    (PyTypeObject *type, PyObject *args,
                                PyObject *kwds);
extern void      PyPose_dealloc(PyPose *p);
// Python - accessible interface
extern PyObject *PyPose_update (PyObject *self, PyObject *args);

// Method list
static PyMethodDef PyPose_methods[] = {

  {"update", (PyCFunction)PyPose_update, METH_NOARGS,
    "Update all the built Python objects to reflect the current state of the "
    "backend C++ objects.  Recurses down the variable references to update "
    "any attributes that are also wrapped C++ vision objects."},

  /* Sentinel */
  { NULL }
};

// Member list
static PyMemberDef PyPose_members[] = {

  {"leftHorizonY", T_OBJECT_EX, offsetof(PyPose, leftHorizonY), READONLY,
    "Left horizon y coordinate"},
  {"rightHorizonY", T_OBJECT_EX, offsetof(PyPose, rightHorizonY), READONLY,
    "Right horizon y coordinate"},
  {"horizonSlope", T_OBJECT_EX, offsetof(PyPose, horizonSlope), READONLY,
    "Slope of the horizon line"},
  //{"bodyCenterHeight", T_OBJECT_EX, offsetof(PyPose, bodyCenterHeight),
  //  READONLY, "Height of center of body in space"},
  //{"panAngle", T_OBJECT_EX, offsetof(PyPose, panAngle), READONLY,
  //   "Angle of the head to body"},

  /* Sentinel */
  { NULL }
};

// PyPose type definition
static PyTypeObject PyPoseType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "vision.Pose",             /*tp_name*/
    sizeof(PyPose),            /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyPose_dealloc,/*tp_dealloc*/
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
    "Pose object",             /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PyPose_methods,            /* tp_methods */
    PyPose_members,            /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    PyPose_new,                /* tp_new */
};



//
// PyFieldLines definitions
//


typedef struct PyFieldLines_t {
  PyObject_HEAD
  FieldLines *fl;

  PyObject *numLines;
  PyObject *lines;
  vector<PyObject*> raw_corners;

  PyObject *numCorners;
  PyObject *corners;
  vector<PyObject*> raw_lines;

} PyFieldLines;


/*
  Courtesy of MySQL - http://lists.mysql.com/commits/13963

  Since PyFieldLines is non-POD type offsetof() macro can't be used
  with it according to the current version of C++ standard (attempt to do
  this will produce warnings). So we use auxillary object to get offsets
  of PyFieldLines's members (we assume that all objects of this
  type have same offsets of its members).

  This is temporary hack which should be removed if we ever get around to it.
*/

static PyFieldLines dummy_fieldlines;

// C++ - accessible interface
extern PyObject *PyFieldLines_new    (FieldLines *fl);
extern void      PyFieldLines_update (PyFieldLines *fl);
// backend methods
extern PyObject *PyFieldLines_new    (PyTypeObject *type, PyObject *args,
                                      PyObject *kwds);
extern void      PyFieldLines_dealloc(PyFieldLines *self);
// Python - accessible interface
extern PyObject *PyFieldLines_update (PyObject *self, PyObject *args);

// Method list
static PyMethodDef PyFieldLines_methods[] = {

  {"update", (PyCFunction)PyFieldLines_update, METH_NOARGS,
    "Update all the built Python objects to reflect the current state of the "
    "backend C++ objects.  Recurses down the variable references to update "
    "any attributes that are also wrapped C++ vision objects."},

  /* Sentinel */
  { NULL }
};

// Member list
static PyMemberDef PyFieldLines_members[] = {

  {"numLines", T_OBJECT_EX, offsetof_in_object(dummy_fieldlines, numLines),
    READONLY,
    "The number of lines detected in the current image"},
  {"lines", T_OBJECT_EX, offsetof_in_object(dummy_fieldlines, lines),
    READONLY,
    "List of lines in the image.  Note, the list contains references to\n"
    "MAX_FIELD_LINES Line objects.  Only the first `numLines' lines\n"
    "reflect accurate updated values of lines currently seen."},
  {"numCorners", T_OBJECT_EX, offsetof_in_object(dummy_fieldlines, numCorners),
    READONLY,
   "The Number of corners detected in the current image"},
  {"corners", T_OBJECT_EX, offsetof_in_object(dummy_fieldlines, corners),
    READONLY,
   "List of corners in the image."},

  /* Sentinel */
  { NULL }
};

// PyFieldLines type definition
static PyTypeObject PyFieldLinesType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "vision.FieldLines",       /*tp_name*/
    sizeof(PyFieldLines),      /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyFieldLines_dealloc,/*tp_dealloc*/
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
    "FieldLines object",       /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PyFieldLines_methods,      /* tp_methods */
    PyFieldLines_members,      /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    PyFieldLines_new,          /* tp_new */
};



//
// PyVisualCorner definitions
//


typedef struct PyVisualCorner_t {
  PyObject_HEAD
  PyFieldLines *fl;

  // The index of this VisualCorner in FieldLines.getCorners()
  int i;
  // Visual x and y coordinates (not included for now)
  // PyObject *x, *y;
  // Distance and bearing objects
  PyObject *dist, *bearing;
  // Certainty objects (not included for now)
  //PyObject *distCert, *idCert;
  // List of possible ConcreteCorner's
  PyObject *possibilities;

} PyVisualCorner;

// C++ - accessible interface
extern PyObject *PyVisualCorner_new    (PyFieldLines *fl, int i);
extern PyObject *PyVisualCorner_new    (PyFieldLines *fl, int i,
                                        const VisualCorner &corner);
//jf- extern void      PyVisualCorner_update (PyVisualCorner *self);
extern void      PyVisualCorner_update (PyVisualCorner *self,
                                        const VisualCorner &corner);
// backend methods
extern PyObject *PyVisualCorner_new    (PyTypeObject *type, PyObject *args,
                                        PyObject *kwds);
extern void      PyVisualCorner_dealloc(PyVisualCorner *self);
// Python - accessible interface
//jf- extern PyObject *PyVisualCorner_update (PyObject *self, PyObject *args);

// Method list
static PyMethodDef PyVisualCorner_methods[] = {

  /*jf-
  {"update", (PyCFunction)PyVisualCorner_update, METH_NOARGS,
    "Update all the built Python objects to reflect the current state of the "
    "backend C++ objects.  Recurses down the variable references to update "
    "any attributes that are also wrapped C++ vision objects."},
    */

  /* Sentinel */
  { NULL }
};

// Member list
static PyMemberDef PyVisualCorner_members[] = {

  {"dist", T_OBJECT_EX, offsetof(PyVisualCorner, dist), READONLY,
    "Distance to the center of the corner"},
  {"bearing", T_OBJECT_EX, offsetof(PyVisualCorner, bearing), READONLY,
    "Bearing to the center of the corner"},
  {"possibilities", T_OBJECT_EX, offsetof(PyVisualCorner, possibilities),
    READONLY,
    "List of possible ConcreteCorner objects that this VisualCorner could "
    "represent."},

  /* Sentinel */
  { NULL }
};

// PyVisualCorner type definition
static PyTypeObject PyVisualCornerType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "vision.VisualCorner",     /*tp_name*/
    sizeof(PyVisualCorner),    /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyVisualCorner_dealloc,/*tp_dealloc*/
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
    "VisualCorner object",     /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PyVisualCorner_methods,    /* tp_methods */
    PyVisualCorner_members,    /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    PyVisualCorner_new,        /* tp_new */
};



//
// PyConcreteCorner definitions
//


typedef struct PyConcreteCorner_t {
  PyObject_HEAD
  PyFieldLines *fl;

  // The id of this corner, out of all corners
  PyObject *id;
  // Concrete x and y coordinates (localization-wise)
  PyObject *fieldX, *fieldY;

} PyConcreteCorner;

// C++ - accessible interface
extern PyObject *PyConcreteCorner_new    (const ConcreteCorner *corner);
// backend methods
extern PyObject *PyConcreteCorner_new    (PyTypeObject *type, PyObject *args,
                                          PyObject *kwds);
extern void      PyConcreteCorner_dealloc(PyConcreteCorner *self);

// Member list
static PyMemberDef PyConcreteCorner_members[] = {

  {"id", T_OBJECT_EX, offsetof(PyConcreteCorner, id), READONLY,
    "The unique ID of this ConcreteCorner."},
  {"fieldX", T_OBJECT_EX, offsetof(PyConcreteCorner, fieldX), READONLY,
    "Field x coordinate of the location of this ConcreteCorner."},
  {"fieldY", T_OBJECT_EX, offsetof(PyConcreteCorner, fieldY), READONLY,
    "Field y coordinate of the location of this ConcreteCorner."},

  /* Sentinel */
  { NULL }
};

// PyConcreteCorner type definition
static PyTypeObject PyConcreteCornerType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "vision.ConcreteCorner",   /*tp_name*/
    sizeof(PyConcreteCorner),  /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyConcreteCorner_dealloc,/*tp_dealloc*/
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
    "ConcreteCorner object",   /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    0,                         /* tp_methods */
    PyConcreteCorner_members,  /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    PyConcreteCorner_new,      /* tp_new */
};



//
// PyVisualLine definitions
//


typedef struct PyVisualLine_t {
  PyObject_HEAD
  PyFieldLines *fl;

  // The index of this VisualLine in FieldLines.getLines()
  int i;
  // X and Y coordinates
  PyObject *x1, *y1, *x2, *y2;
  // Line slope and length
  PyObject *slope, *length;

} PyVisualLine;

// C++ - accessible interface
extern PyObject *PyVisualLine_new    (PyFieldLines *fl, int i);
extern PyObject *PyVisualLine_new    (PyFieldLines *fl, int i,
                                      const VisualLine &line);
//jf- extern void      PyVisualLine_update (PyVisualLine *self);
extern void      PyVisualLine_update (PyVisualLine *self,
                                      const VisualLine &line);
// backend methods
extern PyObject *PyVisualLine_new    (PyTypeObject *type, PyObject *args,
                                      PyObject *kwds);
extern void      PyVisualLine_dealloc(PyVisualLine *self);
// Python - accessible interface
/*jf-
extern PyObject *PyVisualLine_update (PyObject *self, PyObject *args);
*/

// Method list
static PyMethodDef PyVisualLine_methods[] = {

  /*jf-
  {"update", (PyCFunction)PyVisualLine_update, METH_NOARGS,
    "Update all the built Python objects to reflect the current state of the "
    "backend C++ objects.  Recurses down the variable references to update "
    "any attributes that are also wrapped C++ vision objects."},
    */

  /* Sentinel */
  { NULL }
};

// Member list
static PyMemberDef PyVisualLine_members[] = {

  {"x1", T_OBJECT_EX, offsetof(PyVisualLine, x1), READONLY,
    "First x coordinate"},
  {"y1", T_OBJECT_EX, offsetof(PyVisualLine, y1), READONLY,
    "First y coordinate"},
  {"x2", T_OBJECT_EX, offsetof(PyVisualLine, x2), READONLY,
    "Second x coordinate"},
  {"y2", T_OBJECT_EX, offsetof(PyVisualLine, y2), READONLY,
    "Second x coordinate"},
  {"slope", T_OBJECT_EX, offsetof(PyVisualLine, slope), READONLY,
    "Line slope"},
  {"length", T_OBJECT_EX, offsetof(PyVisualLine, length), READONLY,
    "Line length"},

  /* Sentinel */
  { NULL }
};

// PyVisualLine type definition
static PyTypeObject PyVisualLineType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "vision.Line",             /*tp_name*/
    sizeof(PyVisualLine),      /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyVisualLine_dealloc,/*tp_dealloc*/
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
    "Line object",             /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PyVisualLine_methods,      /* tp_methods */
    PyVisualLine_members,      /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    PyVisualLine_new,          /* tp_new */
};



//
// PyTheshold definitions
//


typedef struct PyThreshold_t {
  PyObject_HEAD
  Threshold *thresh;
  PyObject *width;
  PyObject *height;
#if ROBOT(NAO)
  PyObject *thresholded;
#endif
} PyThreshold;

// C++ - accessible interface
extern PyObject *PyThreshold_new    (Threshold *t);
extern void      PyThreshold_update (PyThreshold *t);
// backend methods
extern PyObject *PyThreshold_new    (PyTypeObject *type, PyObject *args,
                                     PyObject *kwds);
extern void      PyThreshold_dealloc(PyThreshold *t);
// Python-accessible interface
extern PyObject *PyThreshold_visionLoop       (PyObject *self, PyObject *args);
extern PyObject *PyThreshold_thresholdAndRuns (PyObject *self, PyObject *args);
extern PyObject *PyThreshold_objectRecognition(PyObject *self, PyObject *args);

// Method list
static PyMethodDef PyThreshold_methods[] = {

  {"visionLoop", (PyCFunction)PyThreshold_visionLoop, METH_NOARGS,
    "Run the entire thresholding loop"},
  {"thresholdAndRuns", (PyCFunction)PyThreshold_thresholdAndRuns, METH_NOARGS,
    "Threshold the image and do runs"},
  {"objectRecognition", (PyCFunction)PyThreshold_objectRecognition, 
    METH_NOARGS, "Perform object recognition"},
  {"update", (PyCFunction)PyThreshold_update, METH_NOARGS,
    "Update all the built Python objects to reflect the current state of the "
    "backend C++ objects.  Recurses down the variable references to update "
    "any attributes that are also wrapped C++ vision objects."},

  /* Sentinel */
  { NULL }
};

// Member list
static PyMemberDef PyThreshold_members[] = {

  {"width", T_OBJECT_EX, offsetof(PyThreshold, width), READONLY,
    "Image width"},
  {"height", T_OBJECT_EX, offsetof(PyThreshold, height), READONLY,
    "Image height"},
#if ROBOT(NAO)
  {"thresholded", T_OBJECT_EX, offsetof(PyThreshold, thresholded), READONLY,
    "Thresholded image"},
#endif

  /* Sentinel */
  { NULL }
};

// PyThreshold type definition
static PyTypeObject PyThresholdType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "vision.Threshold",        /*tp_name*/
    sizeof(PyThreshold),       /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyThreshold_dealloc,/*tp_dealloc*/
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
    "Threshold object",        /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PyThreshold_methods,       /* tp_methods */
    PyThreshold_members,       /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    PyThreshold_new,           /* tp_new */
};



//
// PyBall definitions
//


typedef struct PyBall_t {
  PyObject_HEAD
  Ball *ball;
  PyObject *centerX;
  PyObject *centerY;
  PyObject *width;
  PyObject *height;
  PyObject *focDist;
  PyObject *dist;
  PyObject *bearing;
  PyObject *elevation;
  PyObject *confidence;
} PyBall;

// C++ - accessible interface
extern PyObject *PyBall_new    (Ball *b);
extern void      PyBall_update (PyBall *b);
// backend methods
extern PyObject *PyBall_new    (PyTypeObject *type, PyObject *args,
                                PyObject *kwds);
extern void      PyBall_dealloc(PyBall *b);

// Method list
static PyMethodDef PyBall_methods[] = {

  {"update", (PyCFunction)PyBall_update, METH_NOARGS,
    "Update all the built Python objects to reflect the current state of the "
    "backend C++ objects.  Recurses down the variable references to update "
    "any attributes that are also wrapped C++ vision objects."},
  
  /* Sentinal */
  { NULL }
};

// Member list
static PyMemberDef PyBall_members[] = {

  {"centerX", T_OBJECT_EX, offsetof(PyBall, centerX), READONLY,
    "Ball center X coordinate"},
  {"centerY", T_OBJECT_EX, offsetof(PyBall, centerY), READONLY,
    "Ball center Y coordinate"},
  {"width", T_OBJECT_EX, offsetof(PyBall, width), READONLY,
    "Ball width"},
  {"height", T_OBJECT_EX, offsetof(PyBall, height), READONLY,
    "Ball height"},
  {"focDist", T_OBJECT_EX, offsetof(PyBall, focDist), READONLY,
    "Ball focal distance"},
  {"dist", T_OBJECT_EX, offsetof(PyBall, dist), READONLY,
    "Ball linear distance"},
  {"bearing", T_OBJECT_EX, offsetof(PyBall, bearing), READONLY,
    "Ball bearing to body"},
  {"elevation", T_OBJECT_EX, offsetof(PyBall, elevation), READONLY,
    "Ball elevation"},
  {"confidence", T_OBJECT_EX, offsetof(PyBall, confidence), READONLY,
    "Ball confidence (that it exists)"},

  /* Sentinal */
  { NULL }
};

// PyBall type definition
static PyTypeObject PyBallType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "vision.Ball",             /*tp_name*/
    sizeof(PyBall),            /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyBall_dealloc,/*tp_dealloc*/
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
    "Ball object",             /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PyBall_methods,            /* tp_methods */
    PyBall_members,            /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    PyBall_new,                /* tp_new */
};



//
// PyFieldObject definitions
//


typedef struct PyFieldObject_t {
  PyObject_HEAD
  FieldObjects *object;
  PyObject *centerX;
  PyObject *centerY;
  PyObject *width;
  PyObject *height;
  PyObject *focDist;
  PyObject *dist;
  PyObject *bearing;
  PyObject *certainty;
  PyObject *distCertainty;
} PyFieldObject;

// C++ - accessible inteface
extern PyObject *PyFieldObject_new    (FieldObjects *o);
extern void      PyFieldObject_update (PyFieldObject *o);
// backend methods
extern PyObject *PyFieldObject_new    (PyTypeObject *type, PyObject *args,
                                       PyObject *kwds);
extern void      PyFieldObject_dealloc(PyFieldObject *self);
// Python - accessible interface
extern PyObject *PyFieldObject_update (PyObject *self, PyObject *args);

// Method list
static PyMethodDef PyFieldObject_methods[] = {

  {"update", (PyCFunction)PyFieldObject_update, METH_NOARGS,
    "Update all the built Python objects to reflect the current state of the "
    "backend C++ objects.  Recurses down the variable references to update "
    "any attributes that are also wrapped C++ vision objects."},

  /* Sentinel */
  { NULL }
};

// Attribute list
static PyMemberDef PyFieldObject_members[] = {

  {"centerX", T_OBJECT_EX, offsetof(PyFieldObject, centerX), READONLY,
    "Object center X coordinate"},
  {"centerY", T_OBJECT_EX, offsetof(PyFieldObject, centerY), READONLY,
    "Object center Y coordinate"},
  {"width", T_OBJECT_EX, offsetof(PyFieldObject, width), READONLY,
    "Object width"},
  {"height", T_OBJECT_EX, offsetof(PyFieldObject, height), READONLY,
    "Object height"},
  {"focDist", T_OBJECT_EX, offsetof(PyFieldObject, focDist), READONLY,
    "Object focal distance"},
  {"dist", T_OBJECT_EX, offsetof(PyFieldObject, dist), READONLY,
    "Object linear distance"},
  {"bearing", T_OBJECT_EX, offsetof(PyFieldObject, bearing), READONLY,
    "Object bearing to body"},
  {"certainty", T_OBJECT_EX, offsetof(PyFieldObject, certainty), READONLY,
    "Object certainty (that it exists)"},
  {"distCertainty", T_OBJECT_EX, offsetof(PyFieldObject, distCertainty),
    READONLY, "Object distance certainty"},

  /* Sentinal */
  { NULL }
};

// PyFieldObject type definition
static PyTypeObject PyFieldObjectType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "vision.FieldObject",      /*tp_name*/
    sizeof(PyFieldObject),     /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyFieldObject_dealloc, /*tp_dealloc*/
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
    "FieldObject object",      /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PyFieldObject_methods,     /* tp_methods */
    PyFieldObject_members,     /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    PyFieldObject_new,         /* tp_new */
};



//
// PyVision definitions
//


typedef struct PyVision_t {
  PyObject_HEAD
  Vision *vision;
  PyObject *width;
  PyObject *height;

  // Pose object
  PyObject *pose;
  // FieldLines object
  PyObject *fieldLines;
  // Threshold object
  PyObject *thresh;

  // Blue-yellow and yellow-blue post
  PyObject *by, *yb;
  // Blue goal left and right posts
  PyObject *bgrp, *bglp;
  // Yellow goal left and right posts
  PyObject *ygrp, *yglp;

  // Goal crossbars
  PyObject *bgCrossbar, *ygCrossbar;

  // Robot recognition
  PyObject *red1, *red2, *navy1, *navy2;
  // Orange ball
  PyObject *ball;

} PyVision;

// C++ - accessible interface
extern PyObject *PyVision_new      (Vision *v);
extern void      PyVision_update   (PyVision *self);
// backend methods, 
extern PyObject *PyVision_new      (PyTypeObject *type, PyObject *args, 
                                    PyObject *kwds);
extern void      PyVision_dealloc  (PyVision *self);

// Python - accessible interface
extern PyObject *PyVision_copyImage  (PyObject *self, PyObject *args);
extern PyObject *PyVision_notifyImage(PyObject *self, PyObject *args);
extern PyObject *PyVision_update     (PyObject *self, PyObject *args);
extern PyObject *PyVision_getColorTablePath(PyObject *self, PyObject *args);
extern PyObject *PyVision_setColorTablePath(PyObject *self, PyObject *args);
extern PyObject *PyVision_startProfiling(PyObject *self, PyObject *args);
extern PyObject *PyVision_stopProfiling(PyObject *self, PyObject *args);

// Method list
static PyMethodDef PyVision_methods[] = {

  {"copyImage", (PyCFunction)PyVision_copyImage, METH_VARARGS, 
    "copyImage(s) --> None.  Copy the contents of s into the extern image "
    "buffer"},
  {"notifyImage", (PyCFunction)PyVision_notifyImage, METH_VARARGS,
    "notifyImage([s]) --> None.  \n\n"
    "If argument s is passed, run the vision processing methods using the "
    "pointer to s's internal char array.  If s is not provided, uses the "
    "extern buffer."},
  {"getColorTablePath", (PyCFunction)PyVision_getColorTablePath, METH_NOARGS,
    "Get the path to the color table that is used."},
  {"setColorTablePath", (PyCFunction)PyVision_setColorTablePath, METH_VARARGS,
    "Set the path to the color table that should be used."},
  {"startProfiling", (PyCFunction)PyVision_startProfiling, METH_VARARGS,
    "startProfiling(nframes) --> None.  Start profiling for the next nframes\n"
    "frames."},
  {"stopProfiling", (PyCFunction)PyVision_stopProfiling, METH_NOARGS,
    "stopProfiling() --> None.  Stop profiling, if still running, and print\n"
    "profiling results."},
  {"update", (PyCFunction)PyVision_update, METH_NOARGS,
    "Update all the built Python objects to reflect the current state of the "
    "backend C++ objects.  Recurses down the variable references to update "
    "any attributes that are also wrapped C++ vision objects."},

  /* Sentinel */
  { NULL }
};

// Attribute list
static PyMemberDef PyVision_members[] = {

  // Direct attribute
  {"width", T_OBJECT_EX, offsetof(PyVision, width), READONLY,
    "Image width"},
  {"height", T_OBJECT_EX, offsetof(PyVision, height), READONLY,
    "Image height"},

  // Class reference attributes
  {"by", T_OBJECT_EX, offsetof(PyVision, by), READONLY,
    "Blue-yellow post"},
  {"yb", T_OBJECT_EX, offsetof(PyVision, yb), READONLY,
    "Yellow-blue post"},

  {"bgrp", T_OBJECT_EX, offsetof(PyVision, bgrp), READONLY,
    "Blue goal right post"},
  {"bglp", T_OBJECT_EX, offsetof(PyVision, bglp), READONLY,
    "Blue goal left post"},
  {"ygrp", T_OBJECT_EX, offsetof(PyVision, ygrp), READONLY,
    "Yellow goal right post"},
  {"yglp", T_OBJECT_EX, offsetof(PyVision, yglp), READONLY,
    "Yellow goal left post"},
  {"bgCrossbar", T_OBJECT_EX, offsetof(PyVision, bgCrossbar), READONLY,
    "Blue goal crossbar"},
  {"ygCrossbar", T_OBJECT_EX, offsetof(PyVision, ygCrossbar), READONLY,
    "Yellow goal crossbar"},


  // Robot recognition
  {"red1", T_OBJECT_EX, offsetof(PyVision, red1), READONLY,
    "First red robot"},
  {"red2", T_OBJECT_EX, offsetof(PyVision, red2), READONLY,
    "Second red robot"},
  {"navy1", T_OBJECT_EX, offsetof(PyVision, navy1), READONLY,
    "First navy robot"},
  {"navy2", T_OBJECT_EX, offsetof(PyVision, navy2), READONLY,
    "Second navy robot"},

  // Orange ball
  {"ball", T_OBJECT_EX, offsetof(PyVision, ball), READONLY,
    "Orange ball"},

  {"thresh", T_OBJECT_EX, offsetof(PyVision, thresh), READONLY,
    "Threshold class.  Access to thresholded image, and control method to "
    "run thresholding processing."},

  {"fieldLines", T_OBJECT_EX, offsetof(PyVision, fieldLines), READONLY,
    "FieldLines class.  Access to lines processed from image."},

  {"pose", T_OBJECT_EX, offsetof(PyVision, pose), READONLY,
    "Pose class.  Access to horizon line and body height calculations"},

  /* Sentinel */
  { NULL }
};

// PyVision type definition
static PyTypeObject PyVisionType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "vision.Vision",           /*tp_name*/
    sizeof(PyVision),          /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyVision_dealloc, /*tp_dealloc*/
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
    "Vision object",           /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PyVision_methods,          /* tp_methods */
    PyVision_members,          /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    PyVision_new,                 /* tp_new */
};



//
// 'vision' module definitions
//

extern void vision_addToModule(PyObject *v, const char *name);

static PyObject* vision_createNew(PyObject *self, PyObject *args);

static PyMethodDef vision_methods[] = {
  {"createNew", (PyCFunction)vision_createNew, METH_NOARGS,
    "Create a new bare Vision object and wrap it.  Causes a memory leak, for "
    "now, on Python object deallocation (C++ Vision isn't deallocated)"},
  /* Sentinel */
  { NULL }
};

PyMODINIT_FUNC MODULE_INIT(vision) (void);


#endif /* PyVision_h */
