#include <Python.h>
#include <time.h>
#include <sys/time.h>
#include <map>
#include <boost/shared_ptr.hpp>

#include "PyVision.h"
#if ROBOT(NAO)
#include "visionconfig.h"
#endif

using namespace std;

using boost::shared_ptr;

static map<const ConcreteCorner*, PyObject*> py_concrete_corners;

#if 0
//
// PyRobotAccess definitions
//

// C++ - accessible interface
extern PyObject *
PyRobotAccess_new (RobotAccess *r)
{
    PyRobotAccess *self;

    self = (PyRobotAccess *)PyRobotAccessType.tp_alloc(&PyRobotAccessType, 0);
    if (self != NULL) {
        self->robot = r;

        self->joints = PyTuple_New(NUM_ACTUATORS);
        self->motionState = PyTuple_New(NUM_MOTION_STATES);
        self->sensors = PyTuple_New(NUM_SENSORS);

        if (self->joints == NULL || self->motionState == NULL ||
            self->sensors == NULL) {
            PyRobotAccess_dealloc(self);
            return NULL;
        }

        PyObject *tmp;
        for (int i = 0; i < NUM_ACTUATORS; i++) {
            tmp = PyFloat_FromDouble(r->joints[i]);
            if (tmp == NULL)
                break;
            PyTuple_SET_ITEM(self->joints, i, tmp);
        }

        if (tmp == NULL) {
            PyRobotAccess_dealloc(self);
            return NULL;
        }

        for (int i = 0; i < NUM_MOTION_STATES; i++) {
            tmp = PyFloat_FromDouble(r->motionState[i]);
            if (tmp == NULL)
                break;
            PyTuple_SET_ITEM(self->motionState, i, tmp);
        }

        if (tmp == NULL) {
            PyRobotAccess_dealloc(self);
            return NULL;
        }

        for (int i = 0; i < NUM_SENSORS; i++) {
            tmp = PyFloat_FromDouble(r->sensors[i]);
            if (tmp == NULL)
                break;
            PyTuple_SET_ITEM(self->sensors, i, tmp);
        }

        if (tmp == NULL) {
            PyRobotAccess_dealloc(self);
            return NULL;
        }
    }

    return (PyObject *)self;
}

extern void
PyRobotAccess_update (PyRobotAccess *self)
{
    PyObject *tmp;
    for (int i = 0; i < NUM_ACTUATORS; i++) {
        tmp = PyTuple_GET_ITEM(self->joints, i);
        PyTuple_SET_ITEM(self->joints, i, PyFloat_FromDouble(r->joints[i]));
        Py_XDECREF(tmp);
    }

    for (int i = 0; i < NUM_ACTUATORS; i++) {
        tmp = PyTuple_GET_ITEM(self->motionState, i);
        PyTuple_SET_ITEM(self->motionState, i,
                         PyFloat_FromDouble(r->motionState[i]));
        Py_XDECREF(tmp);
    }

    for (int i = 0; i < NUM_ACTUATORS; i++) {
        tmp = PyTuple_GET_ITEM(self->sensors, i);
        PyTuple_SET_ITEM(self->sensors, i, PyFloat_FromDouble(r->sensors[i]));
        Py_XDECREF(tmp);
    }
}

// backend methods
extern PyObject *
PyRobotAccess_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(PyExc_RuntimeError, "Cannot initialize a Python "
                    "RobotAccess from Python.  Need a C++ RobotAccess.");
    return NULL;
}

extern void
PyRobotAccess_dealloc (PyRobotAccess *self)
{
    Py_XDECREF(self->joints);
    Py_XDECREF(self->motionState);
    Py_XDECREF(self->sensors);

    self->ob_type->tp_free((PyObject *)self);
}

// Python - accessible interface
extern PyObject *
PyRobotAccess_update (PyObject *self, PyObject *args)
{
    PyRobotAccess((PyRobotAccess *)self);

    Py_INCREF(Py_None);
    return Py_None;
}
#endif



//
// PyPose definitions
//


// C++ - accessible interface
extern PyObject *
PyPose_new (NaoPose *p)
{
    PyPose *self;

    self = (PyPose *)PyPoseType.tp_alloc(&PyPoseType, 0);
    if (self != NULL) {
        self->pose = p;

        self->leftHorizonY = PyInt_FromLong(p->getLeftHorizonY());
        self->rightHorizonY = PyInt_FromLong(p->getRightHorizonY());
        self->horizonSlope = PyFloat_FromDouble(p->getHorizonSlope());
        self->bodyCenterHeight = PyFloat_FromDouble(p->getBodyCenterHeight());
        //self->panAngle = PyFloat_FromDouble(p->getPan());
		self->cameraInWorldFrameZ =
			PyFloat_FromDouble(p->getFocalPointInWorldFrameZ());

        if (self->leftHorizonY == NULL || self->rightHorizonY == NULL ||
            self->horizonSlope == NULL //|| self->bodyCenterHeight == NULL ||
            //self->panAngle == NULL
            ) {
            PyPose_dealloc(self);
            self = NULL;
        }
    }

    return (PyObject *)self;
}

extern void
PyPose_update (PyPose *self)
{
    Py_XDECREF(self->leftHorizonY);
    self->leftHorizonY = PyInt_FromLong(self->pose->getLeftHorizonY());

    Py_XDECREF(self->rightHorizonY);
    self->rightHorizonY = PyInt_FromLong(self->pose->getRightHorizonY());

    Py_XDECREF(self->horizonSlope);
    self->horizonSlope = PyFloat_FromDouble(self->pose->getHorizonSlope());

	Py_XDECREF(self->cameraInWorldFrameZ);
	self->cameraInWorldFrameZ =
		PyFloat_FromDouble(self->pose->getFocalPointInWorldFrameZ());

    Py_XDECREF(self->bodyCenterHeight);
    self->bodyCenterHeight = PyFloat_FromDouble(
        self->pose->getBodyCenterHeight());

    //Py_XDECREF(self->panAngle);
    //self->panAngle = PyFloat_FromDouble(self->pose->getPan());
}


// backend methods
extern PyObject *
PyPose_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(PyExc_RuntimeError, "Cannot initialize a Python "
                    "Pose from Python.  Need a C++ Pose.");
    return NULL;
}

extern void
PyPose_dealloc (PyPose *self)
{
    Py_XDECREF(self->leftHorizonY);
    Py_XDECREF(self->rightHorizonY);
    Py_XDECREF(self->horizonSlope);
    Py_XDECREF(self->bodyCenterHeight);
	Py_XDECREF(self->cameraInWorldFrameZ);
//Py_XDECREF(self->panAngle);

    self->ob_type->tp_free((PyObject *)self);
}

// Python - accessible interface
extern PyObject *
PyPose_update (PyObject *self, PyObject *args)
{
    PyPose_update((PyPose *)self);

    Py_INCREF(Py_None);
    return Py_None;
}


//
// PyVisualCorner definitions
//


// C++ - accessible interface
/*jf-
  extern PyObject *
  PyVisualCorner_new (PyFieldLines *fl, int i)
  {
  return PyVisualCorner_new(fl, i, fl->fl->getCorners()->at(i));
  }
*/

extern PyObject *
PyVisualCorner_new (PyFieldLines *fl, int i, const VisualCorner &corner)
{
    PyVisualCorner *self;

    self = (PyVisualCorner *)PyVisualCornerType.tp_alloc(&PyVisualCornerType, 0);
    if (self != NULL) {
        self->fl = fl;
        self->i = i;

        list<const ConcreteCorner*> possibilities = corner.getPossibleCorners();

        self->dist = PyFloat_FromDouble(corner.getDistance());
        self->bearing = PyFloat_FromDouble(corner.getBearingDeg());

        self->possibilities = PyList_New(possibilities.size());
        if (self->possibilities != NULL) {
            int c_i = 0;
            for (list<const ConcreteCorner*>::const_iterator c =
                     possibilities.begin(); c != possibilities.end(); c_i++, c++) {
                Py_INCREF(py_concrete_corners[*c]);
                PyList_SetItem(self->possibilities, c_i, py_concrete_corners[*c]);
            }
        }

        if (self->dist == NULL || self->bearing == NULL ||
            self->possibilities == NULL) {
            PyVisualCorner_dealloc(self);
            self = NULL;
        }
    }

    return (PyObject *)self;
}

/*jf-
  extern void
  PyVisualCorner_update (PyVisualCorner *self)
  {
  if (self->fl->fl->getCorners()->size() > self->i)
  PyVisualCorner_update(self, self->fl->fl->getCorners()->at(self->i));
  }
*/

extern void
PyVisualCorner_update (PyVisualCorner *self, const VisualCorner &corner)
{
    Py_XDECREF(self->dist);
    self->dist = PyFloat_FromDouble(corner.getDistance());

    Py_XDECREF(self->bearing);
    self->bearing = PyFloat_FromDouble(corner.getBearingDeg());

    list<const ConcreteCorner*> possibilities = corner.getPossibleCorners();
    if (self->possibilities == NULL)
        self->possibilities = PyList_New(possibilities.size());

    if (self->possibilities != NULL) {
        int c_i = 0;
        for (list<const ConcreteCorner*>::const_iterator c =
                 possibilities.begin(); c != possibilities.end(); c_i++, c++) {
            Py_INCREF(py_concrete_corners[*c]);
            if (c_i < PyList_Size(self->possibilities))
                PyList_SetItem(self->possibilities, c_i, py_concrete_corners[*c]);
            else
                PyList_Append(self->possibilities, py_concrete_corners[*c]);
        }
        if (c_i >= PyList_Size(self->possibilities))
            PySequence_DelSlice(self->possibilities, c_i,
                                PyList_Size(self->possibilities));
    }
}

// backend methods
extern PyObject *
PyVisualCorner_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(PyExc_RuntimeError, "Cannot initialize a Python "
                    "Corner from Python.  Need a C++ Corner.");
    return NULL;
}

extern void
PyVisualCorner_dealloc (PyVisualCorner *self)
{
    Py_XDECREF(self->dist);
    Py_XDECREF(self->bearing);
    Py_XDECREF(self->possibilities);

    self->ob_type->tp_free((PyObject*)self);
}

// Python - accessible interface
/*jf-
  extern PyObject *
  PyVisualCorner_update (PyObject *self, PyObject *args)
  {
  PyVisualCorner *c = (PyVisualCorner*) self;
  if (c->fl->fl->getCorners()->size() > c->i) {
  PyVisualCorner_update(c, c->fl->fl->getCorners()->at(c->i));
  Py_INCREF(Py_None);
  return Py_None;

  }else {
  PyErr_SetString(PyExc_ValueError, "Supplied VisualCorner (index) is no "
  "longer available in the FieldLines object");
  return NULL;
  }
  }
*/


//
// PyConcreteCorner definitions
//


// C++ - accessible interface
extern PyObject *
PyConcreteCorner_new (const ConcreteCorner *corner)
{
    PyConcreteCorner *self;

    self = (PyConcreteCorner *)PyConcreteCornerType.tp_alloc(
        &PyConcreteCornerType, 0);
    if (self != NULL) {

        self->id = PyInt_FromLong(corner->getID());
        self->fieldX = PyFloat_FromDouble(corner->getFieldX());
        self->fieldY = PyFloat_FromDouble(corner->getFieldY());

        if (self->id == NULL || self->fieldX == NULL || self->fieldY == NULL) {
            PyConcreteCorner_dealloc(self);
            self = NULL;
        }
    }

    return (PyObject *)self;
}

// backend methods
extern PyObject *
PyConcreteCorner_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(PyExc_RuntimeError, "Cannot initialize a Python "
                    "Corner from Python.  Need a C++ Corner.");
    return NULL;
}

extern void
PyConcreteCorner_dealloc (PyConcreteCorner *self)
{
    Py_XDECREF(self->id);
    Py_XDECREF(self->fieldX);
    Py_XDECREF(self->fieldY);

    self->ob_type->tp_free((PyObject*)self);
}



//
// PyVisualLine definitions
//


// C++ - accessible interface
extern PyObject *
PyVisualLine_new (PyFieldLines *fl, int i)
{
    return PyVisualLine_new(fl, i, fl->fl->getLines()->at(i));
}

extern PyObject *
PyVisualLine_new (PyFieldLines *fl, int i, shared_ptr<VisualLine> line)
{
    PyVisualLine *self;

    self = (PyVisualLine *)PyVisualLineType.tp_alloc(&PyVisualLineType, 0);
    if (self != NULL) {
        self->fl = fl;
        self->i = i;

        self->x1 = PyInt_FromLong(line->getStartpoint().x);
        self->y1 = PyInt_FromLong(line->getStartpoint().y);
        self->x2 = PyInt_FromLong(line->getEndpoint().x);
        self->y2 = PyInt_FromLong(line->getEndpoint().y);
        self->slope = PyFloat_FromDouble(line->getSlope());
        self->length = PyFloat_FromDouble(line->getLength());

        if (self->x1 == NULL || self->y1 == NULL || self->x2 == NULL ||
            self->y2 == NULL || self->slope == NULL || self->length == NULL) {
            PyVisualLine_dealloc(self);
            self = NULL;
        }
    }

    return (PyObject *)self;
}

extern void
PyVisualLine_update (PyVisualLine *self)
{
    if (self->fl->fl->getLines()->size() > self->i)
        PyVisualLine_update(self, self->fl->fl->getLines()->at(self->i));
}

extern void
PyVisualLine_update (PyVisualLine *self, shared_ptr<VisualLine> line)
{
    Py_XDECREF(self->x1);
    self->x1 = PyInt_FromLong(line->getStartpoint().x);

    Py_XDECREF(self->y1);
    self->y1 = PyInt_FromLong(line->getStartpoint().y);

    Py_XDECREF(self->x2);
    self->x2 = PyInt_FromLong(line->getEndpoint().x);

    Py_XDECREF(self->y2);
    self->y2 = PyInt_FromLong(line->getEndpoint().y);

    Py_XDECREF(self->slope);
    self->slope = PyFloat_FromDouble(line->getSlope());

    Py_XDECREF(self->length);
    self->length = PyFloat_FromDouble(line->getLength());
}

// backend methods
extern PyObject *
PyVisualLine_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(PyExc_RuntimeError, "Cannot initialize a Python "
                    "Line from Python.  Need a C++ Line.");
    return NULL;
}

extern void
PyVisualLine_dealloc (PyVisualLine *self)
{
    Py_XDECREF(self->x1);
    Py_XDECREF(self->y1);
    Py_XDECREF(self->x2);
    Py_XDECREF(self->y2);
    Py_XDECREF(self->slope);
    Py_XDECREF(self->length);

    self->ob_type->tp_free((PyObject*)self);
}

// Python - accessible interface
extern PyObject *
PyVisualLine_update (PyObject *self, PyObject *args)
{
    PyVisualLine *l = (PyVisualLine*)self;
    if (l->fl->fl->getLines()->size() > l->i) {
        PyVisualLine_update(l);
        Py_INCREF(Py_None);
        return Py_None;
    }else {
        PyErr_SetString(PyExc_ValueError, "Supplied VisualLine (index) is no "
                        "longer available in the FieldLines object");
        return NULL;
    }
}



//
// PyFieldLines definitions
//


// C++ - accessible interface
extern PyObject *
PyFieldLines_new (shared_ptr<FieldLines> fl)
{
    PyFieldLines *self;

    self = (PyFieldLines *)PyFieldLinesType.tp_alloc(&PyFieldLinesType, 0);
    if (self != NULL) {
        self->fl = fl;

        const list<VisualCorner> *corners = fl->getCorners();
		const vector< shared_ptr<VisualLine> > *lines = fl->getLines();

        // Corners
        self->numCorners = PyInt_FromLong(corners->size());
        unsigned int i = 0;
        for (list<VisualCorner>::const_iterator c = corners->begin();
             c != corners->end(); c++,i++) {
            PyObject *o = PyVisualCorner_new(self, i, *c);
            if (o != NULL)
                self->raw_corners.push_back(o);
            else
                break;
        }
        if (self->raw_corners.size() == corners->size()) {
            self->corners = PyList_New(corners->size());
            if (self->corners != NULL) {
                for (i = 0; i < self->raw_corners.size(); i++) {
                    Py_INCREF(self->raw_corners[i]);
                    PyList_SET_ITEM(self->corners, i, self->raw_corners[i]);
                }
            }
        }

        // Lines
        self->numLines = PyInt_FromLong(lines->size());
        for (unsigned int i = 0; i < lines->size(); i++) {
            PyObject *l = PyVisualLine_new(self, i, lines->at(i));
            if (l != NULL)
                self->raw_lines.push_back(l);
            else
                break;
        }
        if (self->raw_lines.size() == lines->size()) {
            self->lines = PyList_New(lines->size());
            if (self->lines != NULL) {
                for (unsigned int i = 0; i < self->raw_lines.size(); i++) {
                    Py_INCREF(self->raw_lines[i]);
                    PyList_SET_ITEM(self->lines, i, self->raw_lines[i]);
                }
            }
        }

        if (self->numCorners == NULL || self->numLines == NULL ||
            self->corners == NULL || self->lines == NULL) {
            PyFieldLines_dealloc(self);
            self = NULL;
        }
    }

    return (PyObject *)self;
}

extern void
PyFieldLines_update (PyFieldLines *self)
{
    const list<VisualCorner> *corners = self->fl->getCorners();
    const vector< shared_ptr<VisualLine> > *lines = self->fl->getLines();

    Py_XDECREF(self->numCorners);
    self->numCorners = PyInt_FromLong(corners->size());
    Py_XDECREF(self->numLines);
    self->numLines = PyInt_FromLong(lines->size());

    // Update all the corners, adding new ones if necessary
    unsigned int i = 0;
    for (list<VisualCorner>::const_iterator c = corners->begin();
         c != corners->end(); i++, c++) {
        if (i >= self->raw_corners.size()) {
            // add a new VisualCorner
            PyObject *o = PyVisualCorner_new(self, i, *c);
            if (o == NULL)
                break;
            self->raw_corners.push_back(o);
            Py_INCREF(o);
            PyList_Append(self->corners, o);
        }else
            // update the visual corner
            PyVisualCorner_update((PyVisualCorner*)self->raw_corners[i], *c);
    }

    // Update all the lines, adding new ones if necessary
    for (i = 0; i < lines->size(); i++) {
        if (i >= self->raw_lines.size()) {
            // add a new VisualLine
            PyObject *l = PyVisualLine_new(self, i, lines->at(i));
            if (l == NULL)
                break;
            self->raw_lines.push_back(l);
            Py_INCREF(l);
            PyList_Append(self->lines, l);
        }else
            // update the visual line
            PyVisualLine_update((PyVisualLine*)self->raw_lines[i], lines->at(i));
    }
}

// backend methods
extern PyObject *
PyFieldLines_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(PyExc_RuntimeError, "Cannot initialize a Python "
                    "FieldLines from Python.  Need a C++ FieldLines.");
    return NULL;
}

extern void
PyFieldLines_dealloc (PyFieldLines *self)
{
    Py_XDECREF(self->numLines);
    Py_XDECREF(self->numCorners);
    Py_XDECREF(self->lines);
    Py_XDECREF(self->corners);

    self->ob_type->tp_free((PyObject*)self);
}

// Python - accessible interface
extern PyObject *
PyFieldLines_update (PyObject *self, PyObject *args)
{
    PyFieldLines_update((PyFieldLines *)self);

    Py_INCREF(Py_None);
    return Py_None;
}



//
// PyThreshold defiinitions
//


// C++ - accessible interface
extern PyObject *
PyThreshold_new (Threshold *t)
{
    PyThreshold *self;

    self = (PyThreshold *)PyThresholdType.tp_alloc(&PyThresholdType, 0);
    if (self != NULL) {
        self->thresh = t;

        // TODO - change to using actual image width
        self->width = PyInt_FromLong(IMAGE_WIDTH);
        self->height = PyInt_FromLong(IMAGE_HEIGHT);

#if 0//ROBOT(NAO)
        npy_intp dims[] = { IMAGE_HEIGHT, IMAGE_WIDTH };
        self->thresholded = PyArray_NewFromDescr(
            &PyArray_Type,
            PyArray_DescrFromType(PyArray_BYTE),
            2,
            &dims[0],
            NULL,
            &self->thresh->thresholded[0],
            NPY_CARRAY_RO,
            NULL
            );
#endif

        if (self->width == NULL || self->height == NULL) {
            PyThreshold_dealloc(self);
            self = NULL;
        }
    }

    return (PyObject *)self;
}

extern void
PyThreshold_update (PyThreshold *self)
{
    Py_XDECREF(self->width);
    self->width = PyInt_FromLong(IMAGE_WIDTH);

    Py_XDECREF(self->height);
    self->height = PyInt_FromLong(IMAGE_HEIGHT);
}

// backend methods
extern PyObject *
PyThreshold_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(PyExc_RuntimeError, "Cannot initialize a Python "
                    "Threshold from Python.  Need a C++ Threshold.");
    return NULL;
}

extern void
PyThreshold_dealloc (PyThreshold *self)
{
    Py_XDECREF(self->width);
    Py_XDECREF(self->height);

#if 0//ROBOT(NAO)
    Py_XDECREF(self->thresholded);
#endif

    self->ob_type->tp_free((PyObject*)self);
}

// Python - accessible interface
extern PyObject *
PyThreshold_visionLoop (PyObject *self, PyObject *args)
{
    ((PyThreshold *)self)->thresh->visionLoop();

    Py_INCREF(Py_None);
    return Py_None;
}

extern PyObject *
PyThreshold_thresholdAndRuns (PyObject *self, PyObject *args)
{
    ((PyThreshold *)self)->thresh->thresholdAndRuns();

    Py_INCREF(Py_None);
    return Py_None;
}

extern PyObject *
PyThreshold_objectRecognition (PyObject *self, PyObject *args)
{
    ((PyThreshold *)self)->thresh->objectRecognition();

    Py_INCREF(Py_None);
    return Py_None;
}

extern PyObject *
PyThreshold_update (PyObject *self, PyObject *args)
{
    PyThreshold_update((PyThreshold *)self);

    Py_INCREF(Py_None);
    return Py_None;

}



//
// PyBall definitions
//


// C++ - accessible interface
extern PyObject *
PyBall_new (VisualBall *b)
{
    PyBall *self;

    self = (PyBall *)PyBallType.tp_alloc(&PyBallType, 0);
    if (self != NULL) {
        self->ball = b;

        self->centerX = PyInt_FromLong(b->getCenterX());
        self->centerY = PyInt_FromLong(b->getCenterY());
        self->width = PyFloat_FromDouble(b->getWidth());
        self->height = PyFloat_FromDouble(b->getHeight());
        self->focDist = PyFloat_FromDouble(b->getFocDist());
        self->dist = PyFloat_FromDouble(b->getDistance());
        self->bearing = PyFloat_FromDouble(b->getBearingDeg());
        self->elevation = PyFloat_FromDouble(b->getElevationDeg());
        self->confidence = PyInt_FromLong(b->getConfidence());
        self->angleX = PyFloat_FromDouble(b->getAngleXDeg());
        self->angleY = PyFloat_FromDouble(b->getAngleYDeg());

        if (self->centerX == NULL || self->centerY == NULL ||
            self->width == NULL || self->height == NULL ||
            self->focDist == NULL || self->dist == NULL ||
            self->bearing == NULL || self->elevation == NULL ||
            self->confidence == NULL) {

            PyBall_dealloc(self);
            self = NULL;
        }
    }

    return (PyObject *)self;
}

extern void
PyBall_update (PyBall *self)
{
    Py_XDECREF(self->centerX);
    self->centerX = PyInt_FromLong(self->ball->getCenterX());

    Py_XDECREF(self->centerY);
    self->centerY = PyInt_FromLong(self->ball->getCenterY());

    Py_XDECREF(self->width);
    self->width = PyFloat_FromDouble(self->ball->getWidth());

    Py_XDECREF(self->height);
    self->height = PyFloat_FromDouble(self->ball->getHeight());

    Py_XDECREF(self->focDist);
    self->focDist = PyFloat_FromDouble(self->ball->getFocDist());

    Py_XDECREF(self->dist);
    self->dist = PyFloat_FromDouble(self->ball->getDistance());

    Py_XDECREF(self->bearing);
    self->bearing = PyFloat_FromDouble(self->ball->getBearingDeg());

    Py_XDECREF(self->elevation);
    self->elevation = PyFloat_FromDouble(self->ball->getElevationDeg());

    Py_XDECREF(self->confidence);
    self->confidence = PyInt_FromLong(self->ball->getConfidence());

    Py_XDECREF(self->angleX);
    self->angleX = PyFloat_FromDouble(self->ball->getAngleXDeg());

    Py_XDECREF(self->angleY);
    self->angleY = PyFloat_FromDouble(self->ball->getAngleYDeg());
}

// backend methods
extern PyObject *
PyBall_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(PyExc_RuntimeError, "Cannot initialize a Python "
                    "Ball from Python.  Need a C++ VisualBall.");
    return NULL;
}

extern void
PyBall_dealloc (PyBall *self)
{
    if (self == NULL)
        return;

    Py_XDECREF(self->centerX);
    Py_XDECREF(self->centerY);
    Py_XDECREF(self->width);
    Py_XDECREF(self->height);
    Py_XDECREF(self->focDist);
    Py_XDECREF(self->dist);
    Py_XDECREF(self->bearing);
    Py_XDECREF(self->elevation);
    Py_XDECREF(self->confidence);
    self->ob_type->tp_free((PyObject*)self);
}

// Python - accessible methods
extern PyObject *
PyBall_update (PyObject *self, PyObject *arg)
{
    PyBall_update((PyBall *)self);

    Py_INCREF(Py_None);
    return Py_None;
}



//
// PyFieldObject definitions
//


// C++ - accessible interface
extern PyObject *
PyFieldObject_new (VisualFieldObject *o)
{
    PyFieldObject *self;

    self = (PyFieldObject *)PyFieldObjectType.tp_alloc(&PyFieldObjectType, 0);
    if (self != NULL) {
        self->object = o;

        self->centerX = PyInt_FromLong(o->getCenterX());
        self->centerY = PyInt_FromLong(o->getCenterY());
        self->width = PyFloat_FromDouble(o->getWidth());
        self->height = PyFloat_FromDouble(o->getHeight());
        self->focDist = PyFloat_FromDouble(o->getFocDist());
        self->dist = PyFloat_FromDouble(o->getDistance());
        self->bearing = PyFloat_FromDouble(o->getBearingDeg());
        self->certainty = PyInt_FromLong(o->getIDCertainty());
        self->distCertainty = PyInt_FromLong(o->getDistanceCertainty());
        self->angleX = PyFloat_FromDouble(o->getAngleXDeg());
        self->angleY = PyFloat_FromDouble(o->getAngleYDeg());

        if (self->centerX == NULL || self->centerY == NULL ||
            self->width == NULL || self->height == NULL ||
            self->focDist == NULL || self->dist == NULL ||
            self->bearing == NULL || self->certainty == NULL ||
            self->distCertainty == NULL) {

            PyFieldObject_dealloc(self);
            self = NULL;
        }
    }

    return (PyObject *)self;
}

extern void
PyFieldObject_update (PyFieldObject *self)
{
    Py_XDECREF(self->centerX);
    self->centerX = PyInt_FromLong(self->object->getCenterX());

    Py_XDECREF(self->centerY);
    self->centerY = PyInt_FromLong(self->object->getCenterY());

    Py_XDECREF(self->width);
    self->width = PyFloat_FromDouble(self->object->getWidth());

    Py_XDECREF(self->height);
    self->height = PyFloat_FromDouble(self->object->getHeight());

    Py_XDECREF(self->focDist);
    self->focDist = PyFloat_FromDouble(self->object->getFocDist());

    Py_XDECREF(self->dist);
    self->dist = PyFloat_FromDouble(self->object->getDistance());

    Py_XDECREF(self->bearing);
    self->bearing = PyFloat_FromDouble(self->object->getBearingDeg());

    Py_XDECREF(self->certainty);
    self->certainty = PyInt_FromLong(self->object->getIDCertainty());

    Py_XDECREF(self->distCertainty);
    self->distCertainty = PyInt_FromLong(self->object->getDistanceCertainty());

    Py_XDECREF(self->angleX);
    self->angleX = PyFloat_FromDouble(self->object->getAngleXDeg());

    Py_XDECREF(self->angleY);
    self->angleY = PyFloat_FromDouble(self->object->getAngleYDeg());
}

// backend methods
extern PyObject *
PyFieldObject_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(PyExc_RuntimeError, "Cannot initialize a Python "
                    "FieldObject from Python.  Need a C++ FieldObject.");
    return NULL;
}

extern void
PyFieldObject_dealloc (PyFieldObject *self)
{
    if (self == NULL)
        return;

    Py_XDECREF(self->centerX);
    Py_XDECREF(self->centerY);
    Py_XDECREF(self->width);
    Py_XDECREF(self->height);
    Py_XDECREF(self->focDist);
    Py_XDECREF(self->dist);
    Py_XDECREF(self->bearing);
    Py_XDECREF(self->certainty);
    Py_XDECREF(self->distCertainty);
    self->ob_type->tp_free((PyObject*)self);
}

// Python - accessible interface
extern PyObject *
PyFieldObject_update (PyObject *self, PyObject *args)
{
    PyFieldObject_update((PyFieldObject *)self);

    Py_INCREF(Py_None);
    return Py_None;
}

//
// PyVision definitions
//

// C++ - accessible inteface
extern PyObject *
PyVision_new (Vision *v)
{
    PyVision *self;

    self = (PyVision *)PyVisionType.tp_alloc(&PyVisionType, 0);
    if (self != NULL) {
        self->vision = v;

        //self->width = PyInt_FromLong(v->getWidth());
        //self->height = PyInt_FromLong(v->getHeight());
        self->width = PyInt_FromLong(IMAGE_WIDTH);
        self->height = PyInt_FromLong(IMAGE_HEIGHT);

        self->bgrp = PyFieldObject_new(v->bgrp);
        self->bglp = PyFieldObject_new(v->bglp);
        self->ygrp = PyFieldObject_new(v->ygrp);
        self->yglp = PyFieldObject_new(v->yglp);

        self->bgCrossbar = PyCrossbar_new(v->bgCrossbar);
        self->ygCrossbar = PyCrossbar_new(v->ygCrossbar);


        self->red1 = PyVisualRobot_new(v->red1);
        self->red2 = PyVisualRobot_new(v->red2);
        self->navy1 = PyVisualRobot_new(v->navy1);
        self->navy2 = PyVisualRobot_new(v->navy2);

        self->ball = PyBall_new(v->ball);

        self->thresh = PyThreshold_new(v->thresh);
        self->fieldLines = PyFieldLines_new(v->fieldLines);
        self->pose = PyPose_new(v->pose.get());

        if (self->width == NULL      || self->height == NULL ||
            self->bgrp == NULL       || self->bglp == NULL   ||
            self->ygrp == NULL       || self->yglp == NULL   ||

            self->bgCrossbar == NULL || self->ygCrossbar == NULL ||

            self->red1 == NULL       || self->red2 == NULL   ||
            self->navy1 == NULL      || self->navy2 == NULL  ||
            self->ball == NULL       || self->thresh == NULL ||
            self->fieldLines == NULL || self->pose == NULL   ) {

            PyVision_dealloc(self);
            self = NULL;
        }
    }

    return (PyObject *)self;
}

extern void
PyVision_update (PyVision *self)
{
    //Py_XDECREF(self->width);
    //self->width = PyInt_FromLong(self->vision->getWidth());

    //Py_XDECREF(self->height);
    //self->height = PyInt_FromLong(self->vision->getHeight());

    PyFieldObject_update((PyFieldObject *)self->bgrp);
    PyFieldObject_update((PyFieldObject *)self->bglp);
    PyFieldObject_update((PyFieldObject *)self->ygrp);
    PyFieldObject_update((PyFieldObject *)self->yglp);

    PyCrossbar_update((PyCrossbar *)self->bgCrossbar);
    PyCrossbar_update((PyCrossbar *)self->ygCrossbar);

    PyVisualRobot_update((PyVisualRobot *)self->red1);
    PyVisualRobot_update((PyVisualRobot *)self->red2);
    PyVisualRobot_update((PyVisualRobot *)self->navy1);
    PyVisualRobot_update((PyVisualRobot *)self->navy2);
    PyBall_update((PyBall *)self->ball);

    PyThreshold_update((PyThreshold *)self->thresh);
    PyFieldLines_update((PyFieldLines *)self->fieldLines);
    PyPose_update((PyPose *)self->pose);
}

// backend methods
extern PyObject *
PyVision_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(PyExc_RuntimeError, "Cannot initialize a Python Vision "
                    "from Python.  Need a C++ Vision object.");
    return NULL;
}

extern void
PyVision_dealloc (PyVision* self)
{
    if (self == NULL)
        return;

    Py_XDECREF(self->width);
    Py_XDECREF(self->height);
    Py_XDECREF(self->bgrp);
    Py_XDECREF(self->bglp);
    Py_XDECREF(self->ygrp);
    Py_XDECREF(self->yglp);
    Py_XDECREF(self->red1);
    Py_XDECREF(self->red2);
    Py_XDECREF(self->navy1);
    Py_XDECREF(self->navy2);
    Py_XDECREF(self->ball);

    Py_XDECREF(self->thresh);
    Py_XDECREF(self->fieldLines);
    Py_XDECREF(self->pose);

    self->ob_type->tp_free((PyObject*)self);
}

// Python - accessible interface
extern PyObject *
PyVision_copyImage (PyObject *self, PyObject *args)
{
    //PyObject *result = NULL;
    char *s;
    int len;

    if (PyArg_ParseTuple(args, "s#:notifyImage()", &s, &len)){

        if (len < IMAGE_BYTE_SIZE) {
            PyErr_SetObject(PyExc_ValueError,
                            PyString_FromFormat("String argument is too short for use "
                                                "in this Vision's processing.  (length == %i, required == %i)",
                                                len, IMAGE_BYTE_SIZE));
            return NULL;
        }

        // call vision copyImage() method with underlying char array
        ((PyVision *)self)->vision->copyImage((byte *)s);

        Py_INCREF(Py_None);
        return Py_None;
    }else
        return NULL;
}

extern PyObject *
PyVision_notifyImage (PyObject *self, PyObject *args)
{
    Vision *v = ((PyVision *)self)->vision;

    int size = PyTuple_Size(args);

    if (size == 0) {
        v->notifyImage();

    }else if (size == 1) {
        PyObject *s = PyTuple_GetItem(args, 0);

        if (!PyString_Check(s)) {
            PyErr_SetString(PyExc_TypeError, "notifyImage() expects a String "
                            "argument");
            return NULL;
        }

        // TODO - change to using Vision.getImageSize()
        if (PyString_Size(s) < IMAGE_BYTE_SIZE) {
            PyErr_SetObject(PyExc_ValueError,
                            PyString_FromFormat("String argument is too short for use "
                                                "in this Vision's processing.  (length == %i, required == %i)",
                                                static_cast<int>(PyString_Size(s)), IMAGE_BYTE_SIZE));
            return NULL;
        }

        v->notifyImage((uint16_t *)PyString_AsString(s));

    }else {
        PyErr_SetObject(PyExc_TypeError,
                        PyString_FromFormat("notifyImage() takes 0 or 1 arguments (%i given",
                                            size));

        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

extern PyObject *
PyVision_getColorTablePath (PyObject *self, PyObject *args)
{
    return PyString_FromString(
        ((PyVision *)self)->vision->getColorTablePath().c_str());
}

extern PyObject *
PyVision_setColorTablePath (PyObject *self, PyObject *args)
{
    PyObject *result = NULL;
    char *s;

    if (PyArg_ParseTuple(args, "s:setColorTablePath", &s)) {

        ((PyVision *)self)->vision->setColorTablePath(s);

        Py_INCREF(Py_None);
        result = Py_None;
    }

    return result;
}

extern PyObject *
PyVision_startProfiling (PyObject *self, PyObject *args)
{
    PyObject *result = NULL;
    int nframes = -1;

    if (PyArg_ParseTuple(args, "|i:startProfiling", &nframes)) {
        ((PyVision*)self)->vision->profiler->reset();
        ((PyVision*)self)->vision->profiler->profileFrames(nframes);
        Py_INCREF(Py_None);
        result = Py_None;
    }

    return result;
}

extern PyObject *
PyVision_stopProfiling (PyObject *self, PyObject *args)
{
    ((PyVision*)self)->vision->profiler->profiling = false;
    ((PyVision*)self)->vision->profiler->printSummary();
    Py_INCREF(Py_None);
    return Py_None;
}


extern PyObject *
PyVision_update (PyObject *self, PyObject *args)
{
    PyVision_update((PyVision *)self);

    Py_INCREF(Py_None);
    return Py_None;
}

//
// 'vision' module methods
//

static PyObject *module = NULL;

void
vision_addToModule (PyObject *v, const char *name)
{
    if (module != NULL) {
        Py_INCREF(v);
        if (PyModule_AddObject(module, name, v) != 0) {
            fprintf(stderr, "Could not add PyVision object to module\n");
            if (PyErr_Occurred())
                PyErr_Print();
            else
                fprintf(stderr, "No Python exception information available\n");
        }
    }else {
        fprintf(stderr, "Could not add PyVision object to module\n");
        fprintf(stderr, "Module has not been successfully initialized\n");
    }
}

// This method is only used if USE_PYVISION_FAKE_BACKEND is defined.
// Its purpose is to allow you to test the PyVision link by instantiating a new
// vision object. This will never happen on robots. Vision is instantiated
// wherever we interface with the robot hardware API.
static PyObject*
vision_createNew (PyObject *self, PyObject *args)
{
    shared_ptr<Profiler> prof = shared_ptr<Profiler>(new Profiler(&micro_time));
    shared_ptr<Sensors> sensors = shared_ptr<Sensors>(new Sensors());
    shared_ptr<NaoPose> pose = shared_ptr<NaoPose>(new NaoPose(sensors));

    return PyVision_new(new Vision(pose, prof));
}


PyMODINIT_FUNC
MODULE_INIT(vision) (void)
{
#if 0// ROBOT(NAO)
    import_array();
#endif

    if (PyType_Ready(&PyVisionType) < 0 ||
        PyType_Ready(&PyFieldObjectType) < 0 ||
        PyType_Ready(&PyBallType) < 0 ||
        PyType_Ready(&PyThresholdType) < 0 ||
        PyType_Ready(&PyFieldLinesType) < 0 ||
        PyType_Ready(&PyVisualCornerType) < 0 ||
        PyType_Ready(&PyConcreteCornerType) < 0 ||
        PyType_Ready(&PyVisualLineType) < 0 ||
        PyType_Ready(&PyPoseType) < 0 ||
        PyType_Ready(&PyCrossbarType) < 0 ||
        PyType_Ready(&PyVisualRobotType) < 0)
        return;

    //
    // Initialize module
    //

    module = Py_InitModule3(MODULE_HEAD "vision", vision_methods,
                            "Python-wrapped C++ robot vision library");
    if (module == NULL)
        return;

    Py_INCREF(&PyVisualRobotType);
    PyModule_AddObject(module, "VisualRobot", (PyObject *)&PyVisualRobotType);

    Py_INCREF(&PyVisualRobotType);
    PyModule_AddObject(module, "VisualCrossbar", (PyObject *)&PyCrossbarType);

    Py_INCREF(&PyPoseType);
    PyModule_AddObject(module, "Pose", (PyObject *)&PyPoseType);

    Py_INCREF(&PyVisualCornerType);
    PyModule_AddObject(module, "VisualCorner", (PyObject *)&PyVisualCornerType);

    Py_INCREF(&PyConcreteCornerType);
    PyModule_AddObject(module, "ConcreteCorner",
                       (PyObject *)&PyConcreteCornerType);

    Py_INCREF(&PyVisualLineType);
    PyModule_AddObject(module, "VisualLine", (PyObject *)&PyVisualLineType);

    Py_INCREF(&PyFieldLinesType);
    PyModule_AddObject(module, "FieldLines", (PyObject *)&PyFieldLinesType);

    Py_INCREF(&PyThresholdType);
    PyModule_AddObject(module, "Threshold", (PyObject *)&PyThresholdType);

    Py_INCREF(&PyBallType);
    PyModule_AddObject(module, "Ball", (PyObject *)&PyBallType);

    Py_INCREF(&PyFieldObjectType);
    PyModule_AddObject(module, "FieldObject", (PyObject *)&PyFieldObjectType);

    Py_INCREF(&PyVisionType);
    PyModule_AddObject(module, "Vision", (PyObject *)&PyVisionType);

    //
    // Fill py_concrete_corners array
    //

    bool success = true;
    for (unsigned int i = 0; i < ConcreteCorner::NUM_CORNERS; i++) {
        const ConcreteCorner *corner = ConcreteCorner::concreteCorners()[i];
        // attempt to create the static corner references
        py_concrete_corners[corner] = PyConcreteCorner_new(corner);
        if (py_concrete_corners[corner] == NULL) {
            success = false;
            break;
        }
    }
    if (!success) {
        // something failed, so dereference those that succeeded, set error, return
        for (unsigned int i = 0; i < ConcreteCorner::NUM_CORNERS; i++) {
            const ConcreteCorner *corner = ConcreteCorner::concreteCorners()[i];
            if (py_concrete_corners[corner] != NULL) {
                Py_DECREF(py_concrete_corners[corner]);
                py_concrete_corners.erase(corner);
            }
        }
        PyErr_SetString(PyExc_SystemError, "Could not build static references to "
                        "the global ConcreteCorner list");
        return;
    }

#ifdef USE_PYVISION_FAKE_BACKEND
    vision_addToModule(vision_createNew(NULL, NULL), MODULE_HEAD);
#endif
}


//
// PyCrossbar definitions
//


// C++ - accessible interface
extern PyObject * PyCrossbar_new (VisualCrossbar *b)
{
    PyCrossbar *self;

    self = (PyCrossbar *)PyCrossbarType.tp_alloc(&PyCrossbarType, 0);
    if (self != NULL) {
        self->crossbar = b;

        self->x = PyInt_FromLong(b->getX());
        self->y = PyInt_FromLong(b->getY());
        self->centerX = PyInt_FromLong(b->getCenterX());
        self->centerY = PyInt_FromLong(b->getCenterY());
        self->angleX = PyFloat_FromDouble(b->getAngleXDeg());
        self->angleY = PyFloat_FromDouble(b->getAngleYDeg());
        self->width = PyFloat_FromDouble(b->getWidth());
        self->height = PyFloat_FromDouble(b->getHeight());
        self->focDist = PyFloat_FromDouble(b->getFocDist());
        self->dist = PyFloat_FromDouble(b->getDistance());
        self->bearing = PyFloat_FromDouble(b->getBearingDeg());
        self->elevation = PyFloat_FromDouble(b->getElevationDeg());
        self->leftOpening = PyFloat_FromDouble(b->getLeftOpening());
        self->rightOpening = PyFloat_FromDouble(b->getRightOpening());
        self->shoot = PyInt_FromLong(b->shotAvailable());

        if (self->centerX == NULL || self->centerY == NULL ||
            self->width == NULL || self->height == NULL ||
            self->focDist == NULL || self->dist == NULL ||
            self->bearing == NULL || self->elevation == NULL) {

            PyCrossbar_dealloc(self);
            self = NULL;
        }
    }

    return (PyObject *)self;
}

extern void PyCrossbar_update (PyCrossbar *self)
{
    Py_XDECREF(self->x);
    self->x = PyInt_FromLong(self->crossbar->getX());

    Py_XDECREF(self->y);
    self->y = PyInt_FromLong(self->crossbar->getY());

    Py_XDECREF(self->centerX);
    self->centerX = PyInt_FromLong(self->crossbar->getCenterX());

    Py_XDECREF(self->centerY);
    self->centerY = PyInt_FromLong(self->crossbar->getCenterY());

    Py_XDECREF(self->angleX);
    self->angleX = PyFloat_FromDouble(self->crossbar->getAngleXDeg());

    Py_XDECREF(self->angleX);
    self->angleY = PyFloat_FromDouble(self->crossbar->getAngleYDeg());

    Py_XDECREF(self->width);
    self->width = PyFloat_FromDouble(self->crossbar->getWidth());

    Py_XDECREF(self->height);
    self->height = PyFloat_FromDouble(self->crossbar->getHeight());

    Py_XDECREF(self->focDist);
    self->focDist = PyFloat_FromDouble(self->crossbar->getFocDist());

    Py_XDECREF(self->dist);
    self->dist = PyFloat_FromDouble(self->crossbar->getDistance());

    Py_XDECREF(self->bearing);
    self->bearing = PyFloat_FromDouble(self->crossbar->getBearingDeg());

    Py_XDECREF(self->elevation);
    self->elevation = PyFloat_FromDouble(self->crossbar->getElevationDeg());

    Py_XDECREF(self->leftOpening);
    self->leftOpening = PyInt_FromLong(self->crossbar->getLeftOpening());

    Py_XDECREF(self->rightOpening);
    self->rightOpening = PyInt_FromLong(self->crossbar->getRightOpening());

    Py_XDECREF(self->shoot);
    self->shoot = PyInt_FromLong(self->crossbar->shotAvailable());

}

// backend methods
extern PyObject *
PyCrossbar_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(PyExc_RuntimeError, "Cannot initialize a Python "
                    "Crossbar from Python.  Need a C++ VisualCrossbar.");
    return NULL;
}

extern void
PyCrossbar_dealloc (PyCrossbar *self)
{
    if (self == NULL)
        return;

    self->ob_type->tp_free((PyObject*)self);
}

// Python - accessible methods
extern PyObject *
PyCrossbar_update (PyObject *self, PyObject *arg)
{
    PyCrossbar_update((PyCrossbar *)self);
    Py_INCREF(Py_None);
    return Py_None;
}

//
// PyVisualRobot definitions
//


// C++ - accessible interface
extern PyObject * PyVisualRobot_new (VisualRobot *b)
{
    PyVisualRobot *self;

    self = (PyVisualRobot *)PyVisualRobotType.tp_alloc(&PyVisualRobotType, 0);
    if (self != NULL) {
        self->robot = b;

        self->x = PyInt_FromLong(b->getX());
        self->y = PyInt_FromLong(b->getY());
        self->centerX = PyInt_FromLong(b->getCenterX());
        self->centerY = PyInt_FromLong(b->getCenterY());
        self->angleX = PyFloat_FromDouble(b->getAngleXDeg());
        self->angleY = PyFloat_FromDouble(b->getAngleYDeg());
        self->width = PyFloat_FromDouble(b->getWidth());
        self->height = PyFloat_FromDouble(b->getHeight());
        self->focDist = PyFloat_FromDouble(b->getFocDist());
        self->dist = PyFloat_FromDouble(b->getDistance());
        self->bearing = PyFloat_FromDouble(b->getBearingDeg());
        self->elevation = PyFloat_FromDouble(b->getElevationDeg());

        if (self->centerX == NULL || self->centerY == NULL ||
            self->width == NULL || self->height == NULL ||
            self->focDist == NULL || self->dist == NULL ||
            self->bearing == NULL || self->elevation == NULL) {

            PyVisualRobot_dealloc(self);
            self = NULL;
        }
    }

    return (PyObject *)self;
}

extern void PyVisualRobot_update (PyVisualRobot *self)
{
    Py_XDECREF(self->x);
    self->x = PyInt_FromLong(self->robot->getX());

    Py_XDECREF(self->y);
    self->y = PyInt_FromLong(self->robot->getY());

    Py_XDECREF(self->centerX);
    self->centerX = PyInt_FromLong(self->robot->getCenterX());

    Py_XDECREF(self->centerY);
    self->centerY = PyInt_FromLong(self->robot->getCenterY());

    Py_XDECREF(self->angleX);
    self->angleX = PyFloat_FromDouble(self->robot->getAngleXDeg());

    Py_XDECREF(self->angleX);
    self->angleY = PyFloat_FromDouble(self->robot->getAngleYDeg());

    Py_XDECREF(self->width);
    self->width = PyFloat_FromDouble(self->robot->getWidth());

    Py_XDECREF(self->height);
    self->height = PyFloat_FromDouble(self->robot->getHeight());

    Py_XDECREF(self->focDist);
    self->focDist = PyFloat_FromDouble(self->robot->getFocDist());

    Py_XDECREF(self->dist);
    self->dist = PyFloat_FromDouble(self->robot->getDistance());

    Py_XDECREF(self->bearing);
    self->bearing = PyFloat_FromDouble(self->robot->getBearingDeg());

    Py_XDECREF(self->elevation);
    self->elevation = PyFloat_FromDouble(self->robot->getElevationDeg());
}

// backend methods
extern PyObject *
PyVisualRobot_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(PyExc_RuntimeError, "Cannot initialize a Python "
                    "VisualRobot from Python.  Need a C++ VisualVisualRobot.");
    return NULL;
}

extern void
PyVisualRobot_dealloc (PyVisualRobot *self)
{
    if (self == NULL)
        return;

    self->ob_type->tp_free((PyObject*)self);
}

// Python - accessible methods
extern PyObject *
PyVisualRobot_update (PyObject *self, PyObject *arg)
{
    PyVisualRobot_update((PyVisualRobot *)self);
    Py_INCREF(Py_None);
    return Py_None;
}
