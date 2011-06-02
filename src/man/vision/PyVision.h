#ifndef PyVision_h_DEFINED
#define PyVision_h_DEFINED

#include <boost/shared_ptr.hpp>

#include "Vision.h"

void c_init_vision();

void set_vision_pointer (boost::shared_ptr<Vision> vision_ptr);

#endif
<<<<<<< HEAD
=======
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
    VisualBall *ball;
    PyObject *centerX;
    PyObject *centerY;
    PyObject *width;
    PyObject *height;
    PyObject *focDist;
    PyObject *dist;
    PyObject *bearing;
    PyObject *elevation;
    PyObject *confidence;
    PyObject *angleX;
    PyObject *angleY;
} PyBall;

// C++ - accessible interface
extern PyObject *PyBall_new    (VisualBall *b);
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
    {"angleX", T_OBJECT_EX, offsetof(PyBall, angleX), READONLY,
     "Ball angle X value"},
    {"angleY", T_OBJECT_EX, offsetof(PyBall, angleY), READONLY,
     "Ball angle Y value"},

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
    VisualFieldObject *object;
    PyObject *centerX;
    PyObject *centerY;
    PyObject *width;
    PyObject *height;
    PyObject *focDist;
    PyObject *dist;
    PyObject *bearing;
    PyObject *certainty;
    PyObject *distCertainty;
    PyObject *angleX;
    PyObject *angleY;
} PyFieldObject;

// C++ - accessible inteface
extern PyObject *PyFieldObject_new    (VisualFieldObject *o);
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
    {"angleX", T_OBJECT_EX, offsetof(PyFieldObject, angleX), READONLY,
     "FieldObject angle X value"},
    {"angleY", T_OBJECT_EX, offsetof(PyFieldObject, angleY), READONLY,
     "FieldObject angle Y value"},

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

    // Blue goal left and right posts
    PyObject *bgrp, *bglp;
    // Yellow goal left and right posts
    PyObject *ygrp, *yglp;

    // Goal crossbars
    PyObject *bgCrossbar, *ygCrossbar;

    // Robot recognition
    PyObject *red1, *red2, *red3, *navy1, *navy2, *navy3;
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


//
// PyBackstop definitions
//


typedef struct PyCrossbar_t {
    PyObject_HEAD // Our stuff is below
    VisualCrossbar *crossbar;
    PyObject *x;
    PyObject *y;
    PyObject *centerX;
    PyObject *centerY;
    PyObject *angleX;
    PyObject *angleY;
    PyObject *width;
    PyObject *height;
    PyObject *focDist;
    PyObject *dist;
    PyObject *bearing;
    PyObject *elevation;
    PyObject *leftOpening;
    PyObject *rightOpening;
    PyObject *shoot;

} PyCrossbar;

// C++ - accessible interface
extern PyObject *PyCrossbar_new    (VisualCrossbar *b);
extern void      PyCrossbar_update (PyCrossbar *b);
// backend methods
extern PyObject *PyCrossbar_new    (PyTypeObject *type, PyObject *args,
                                    PyObject *kwds);
extern void      PyCrossbar_dealloc(PyCrossbar *b);

// Method list
static PyMethodDef PyCrossbar_methods[] = {

    {"update", (PyCFunction)PyCrossbar_update, METH_NOARGS,
     "Update all the built Python objects to reflect the current state of the "
     "backend C++ objects.  Recurses down the variable references to update "
     "any attributes that are also wrapped C++ vision objects."},

    /* Sentinal */
    { NULL }
};

// Member list
static PyMemberDef PyCrossbar_members[] = {
    {"x", T_OBJECT_EX, offsetof(PyCrossbar, x), READONLY,
     "Crossbar screen X coordinate"},
    {"y", T_OBJECT_EX, offsetof(PyCrossbar, y), READONLY,
     "Crossbar screen Y coordinate"},
    {"centerX", T_OBJECT_EX, offsetof(PyCrossbar, centerX), READONLY,
     "Crossbar center X coordinate"},
    {"centerY", T_OBJECT_EX, offsetof(PyCrossbar, centerY), READONLY,
     "Crossbar center Y coordinate"},
    {"angleX", T_OBJECT_EX, offsetof(PyCrossbar, angleX), READONLY,
     "Crossbar angleX"},
    {"angleY", T_OBJECT_EX, offsetof(PyCrossbar, angleY), READONLY,
     "Crossbar angleY"},
    {"width", T_OBJECT_EX, offsetof(PyCrossbar, width), READONLY,
     "Crossbar width"},
    {"height", T_OBJECT_EX, offsetof(PyCrossbar, height), READONLY,
     "Crossbar height"},
    {"focDist", T_OBJECT_EX, offsetof(PyCrossbar, focDist), READONLY,
     "Crossbar focal distance"},
    {"dist", T_OBJECT_EX, offsetof(PyCrossbar, dist), READONLY,
     "Crossbar linear distance"},
    {"bearing", T_OBJECT_EX, offsetof(PyCrossbar, bearing), READONLY,
     "Crossbar bearing to body"},
    {"elevation", T_OBJECT_EX, offsetof(PyCrossbar, elevation), READONLY,
     "Crossbar elevation"},
    {"leftOpening", T_OBJECT_EX, offsetof(PyCrossbar, leftOpening), READONLY,
     "Crossbar left opening"},
    {"rightOpening", T_OBJECT_EX, offsetof(PyCrossbar, rightOpening), READONLY,
     "Crossbar right opening"},
    {"shoot", T_OBJECT_EX, offsetof(PyCrossbar, shoot), READONLY,
     "Crossbar shot available"},

    /* Sentinal */
    { NULL }
};

// PyCrossbar type definition
static PyTypeObject PyCrossbarType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "vision.Crossbar",             /*tp_name*/
    sizeof(PyCrossbar),            /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyCrossbar_dealloc,/*tp_dealloc*/
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
    "Crossbar object",             /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PyCrossbar_methods,            /* tp_methods */
    PyCrossbar_members,            /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    PyCrossbar_new,                /* tp_new */
};

//
// PyVisualRobot definitions
//


typedef struct PyVisualRobot_t {
    PyObject_HEAD // Our stuff is below
    VisualRobot *robot;
    PyObject *x;
    PyObject *y;
    PyObject *centerX;
    PyObject *centerY;
    PyObject *angleX;
    PyObject *angleY;
    PyObject *width;
    PyObject *height;
    PyObject *focDist;
    PyObject *dist;
    PyObject *bearing;
    PyObject *elevation;

} PyVisualRobot;

// C++ - accessible interface
extern PyObject *PyVisualRobot_new    (VisualRobot *b);
extern void      PyVisualRobot_update (PyVisualRobot *b);
// backend methods
extern PyObject *PyVisualRobot_new    (PyTypeObject *type, PyObject *args,
                                       PyObject *kwds);
extern void      PyVisualRobot_dealloc(PyVisualRobot *b);

// Method list
static PyMethodDef PyVisualRobot_methods[] = {

    {"update", (PyCFunction)PyVisualRobot_update, METH_NOARGS,
     "Update all the built Python objects to reflect the current state of the "
     "backend C++ objects.  Recurses down the variable references to update "
     "any attributes that are also wrapped C++ vision objects."},

    /* Sentinal */
    { NULL }
};

// Member list
static PyMemberDef PyVisualRobot_members[] = {
    {"x", T_OBJECT_EX, offsetof(PyVisualRobot, x), READONLY,
     "VisualRobot screen X coordinate"},
    {"y", T_OBJECT_EX, offsetof(PyVisualRobot, y), READONLY,
     "VisualRobot screen Y coordinate"},
    {"centerX", T_OBJECT_EX, offsetof(PyVisualRobot, centerX), READONLY,
     "VisualRobot center X coordinate"},
    {"centerY", T_OBJECT_EX, offsetof(PyVisualRobot, centerY), READONLY,
     "VisualRobot center Y coordinate"},
    {"angleX", T_OBJECT_EX, offsetof(PyVisualRobot, angleX), READONLY,
     "VisualRobot angleX"},
    {"angleY", T_OBJECT_EX, offsetof(PyVisualRobot, angleY), READONLY,
     "VisualRobot angleY"},
    {"width", T_OBJECT_EX, offsetof(PyVisualRobot, width), READONLY,
     "VisualRobot width"},
    {"height", T_OBJECT_EX, offsetof(PyVisualRobot, height), READONLY,
     "VisualRobot height"},
    {"focDist", T_OBJECT_EX, offsetof(PyVisualRobot, focDist), READONLY,
     "VisualRobot focal distance"},
    {"dist", T_OBJECT_EX, offsetof(PyVisualRobot, dist), READONLY,
     "VisualRobot linear distance"},
    {"bearing", T_OBJECT_EX, offsetof(PyVisualRobot, bearing), READONLY,
     "VisualRobot bearing to body"},
    {"elevation", T_OBJECT_EX, offsetof(PyVisualRobot, elevation), READONLY,
     "VisualRobot elevation"},
    /* Sentinal */
    { NULL }
};

// PyVisualRobot type definition
static PyTypeObject PyVisualRobotType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "vision.VisualRobot",             /*tp_name*/
    sizeof(PyVisualRobot),            /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyVisualRobot_dealloc,/*tp_dealloc*/
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
    "VisualRobot object",             /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PyVisualRobot_methods,            /* tp_methods */
    PyVisualRobot_members,            /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    PyVisualRobot_new,                /* tp_new */
};
>>>>>>> master



