
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

#include <boost/static_assert.hpp>
#include <boost/assign/std/vector.hpp>
using namespace boost::assign;

#include "Sensors.h"
#include <structmember.h>

#include "Kinematics.h"
#include "corpusconfig.h"
#include <iostream>
#include <fstream>

using namespace Kinematics;
using namespace std;

// Static reference to Python sensors module

static PyObject *_sensors_module = NULL;

// static base image array, so we don't crash on image access if the setImage()
// method is never called
static unsigned char global_image[IMAGE_BYTE_SIZE];



static void PySensors_update(PySensors *self)
{
    int i;
    Sensors *sensors = self->_sensors;
    std::vector<float> values = sensors->getBodyAngles();
    for (i = 0; i < (int)values.size(); i++) {
        if (i < PyList_Size(self->angles))
            PyList_SET_ITEM(self->angles, i, PyFloat_FromDouble(values[i]*TO_DEG));
        else
            PyList_Append(self->angles, PyFloat_FromDouble(values[i]*TO_DEG));
    }

    values = sensors->getBodyAngleErrors();
    for (i = 0; i < (int)values.size(); i++) {
        if (i < PyList_Size(self->errors))
            PyList_SET_ITEM(self->errors, i, PyFloat_FromDouble(values[i]*TO_DEG));
        else
            PyList_Append(self->errors, PyFloat_FromDouble(values[i]*TO_DEG));
    }

    // HACK! FSRs are no longer stored in vectors. There are structs that hold
    // the values. In order to not break functionality, I decided to convert them
    // to the vector format for the python hookup.
    values.clear();
    const FSR leftFootFSR(sensors->getLeftFootFSR());
    const FSR rightFootFSR(sensors->getRightFootFSR());
    values += leftFootFSR.frontLeft, leftFootFSR.frontRight,
        leftFootFSR.rearLeft, leftFootFSR.rearRight,
        rightFootFSR.frontLeft, rightFootFSR.frontRight,
        rightFootFSR.rearLeft, rightFootFSR.rearRight;

    for (i = 0; i < (int)values.size(); i++) {
        if (i < PyList_Size(self->fsr))
            PyList_SET_ITEM(self->fsr, i, PyFloat_FromDouble(values[i]));
        else
            PyList_Append(self->fsr, PyFloat_FromDouble(values[i]));
    }

    // Gyros should eventually by converted to DEG
    values.clear();
    const Inertial inertial(sensors->getInertial());
    values += inertial.accX, inertial.accY, inertial.accZ,
        inertial.gyrX, inertial.gyrY,
        inertial.angleX, inertial.angleY;

    for (i = 0; i < (int)values.size(); i++) {
        if (i < PyList_Size(self->inertial))
            PyList_SET_ITEM(self->inertial, i, PyFloat_FromDouble(values[i]));
        else
            PyList_Append(self->inertial, PyFloat_FromDouble(values[i]));
    }

    const float dist = sensors->getUltraSound();

    Py_XDECREF(self->sonarLeft);
    self->sonarLeft = PyFloat_FromDouble(dist);
    Py_XDECREF(self->sonarRight);
    self->sonarRight = PyFloat_FromDouble(dist);
}

static PyObject* PySensors_update (PyObject *self, PyObject *)
{
    PySensors_update((PySensors *)self);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* PySensors_setImage (PyObject *self, PyObject *args)
{
    PyObject *result = NULL;
    const char *s;
    int len;

    if (PyArg_ParseTuple(args, "s#:setImage", &s, &len)) {
        if (len != IMAGE_BYTE_SIZE) {
            PyErr_Format(PyExc_ValueError, "setImage() expected a string of length "
                         "exactly %i, got %i", IMAGE_BYTE_SIZE, len);
        }else {
            memcpy(&global_image[0], s, IMAGE_BYTE_SIZE);
            ((PySensors *)self)->_sensors->setImage(&global_image[0]);

            Py_INCREF(Py_None);
            result = Py_None;
        }
    }

    return result;
}

static PyObject* PySensors_saveFrame(PyObject *self, PyObject *)
{
    ((PySensors *)self)->_sensors->saveFrame();
    return Py_None;
}


static PyMethodDef PySensors_methods[] = {

    {"update", (PyCFunction)PySensors_update, METH_NOARGS,
     "Update all the built Python objects to reflect the current state of the "
     "backend C++ objects.  Recurses down the variable references to update "
     "any attributes that are also wrapped C++ vision objects."},

    {"setImage", (PyCFunction)PySensors_setImage, METH_VARARGS,
     "Set the image data for the robot.  Copies raw bytes data from the Python "
     "string into the static global_image array, then sets the image pointer "
     "in the Sensors object."},

    {"saveFrame", (PyCFunction)PySensors_saveFrame, METH_NOARGS,
     "Save an image frame with associated sensor data."},

    { NULL } /* Sentinel */
};

static PyMemberDef PySensors_members[] = {

    {"angles", T_OBJECT_EX, offsetof(PySensors, angles), READONLY,
     "Body angles values."},
    {"errors", T_OBJECT_EX, offsetof(PySensors, errors), READONLY,
     "Body angles error values."},
    {"fsr", T_OBJECT_EX, offsetof(PySensors, fsr), READONLY,
     "Force sensitive resitor values."},
    {"inertial", T_OBJECT_EX, offsetof(PySensors, inertial), READONLY,
     "Inertial sensor values."},
    {"sonarLeft", T_OBJECT_EX, offsetof(PySensors, sonarLeft), READONLY,
     "Left sonar distance sensor value."},
    {"sonarRight", T_OBJECT_EX, offsetof(PySensors, sonarRight), READONLY,
     "Right sonar distance sensor value."},

    { NULL } /* Sentinel */
};

// forward declarations
static PyObject* PySensors_new (PyTypeObject* type, PyObject* args,
                                PyObject* kwds);
static int PySensors_init (PySensors* self, PyObject* args, PyObject* kwds);
static void PySensors_dealloc (PyObject* self);

static PyTypeObject PySensorsType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "_sensors.Sensors",        /*tp_name*/
    sizeof(PySensors),         /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PySensors_dealloc, /*tp_dealloc*/
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
    "Sensors object",          /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PySensors_methods,         /* tp_methods */
    PySensors_members,         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PySensors_init,  /* tp_init */
    0,                         /* tp_alloc */
    (newfunc)PySensors_new     /* tp_new */
};

static PyObject* PySensors_new (Sensors *sensors)
{
    PySensors *self;

    self = (PySensors *)PySensorsType.tp_alloc(&PySensorsType, 0);
    if (self != NULL) {
        self->_sensors = sensors;

        self->angles = PyList_New(NUM_ACTUATORS);
        self->errors = PyList_New(NUM_ACTUATORS);
        self->fsr = PyList_New(AL_NUMBER_OF_FSR);
        self->inertial = PyList_New(NUM_INERTIAL_SENSORS);

        if (self->angles == NULL || self->errors == NULL
            || self->fsr == NULL || self->inertial == NULL) {
            PySensors_dealloc(reinterpret_cast<PyObject*>(self));
            self = NULL;
        }else
             PySensors_update(self);
    }

    return (PyObject *)self;
}

static PyObject* PySensors_new (PyTypeObject* type, PyObject* args,
                                PyObject* kwds)
{
    return type->tp_alloc(type, 0);
}

static int PySensors_init (PySensors *self, PyObject *args, PyObject *kwds)
{
    static char* keywords[] = {"other"};
    PyObject *other;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!", keywords,
                                     &PySensorsType, &other))
        return -1;

    self->_sensors = reinterpret_cast<PySensors*>(other)->_sensors;

    self->angles = PyList_New(NUM_ACTUATORS);
    self->errors = PyList_New(NUM_ACTUATORS);
    self->fsr = PyList_New(AL_NUMBER_OF_FSR);
    self->inertial = PyList_New(NUM_INERTIAL_SENSORS);

    if (self->angles == NULL || self->errors == NULL
        || self->fsr == NULL || self->inertial == NULL) {
        return -1;
    }else
         PySensors_update(self);

    return 0;
}

static void PySensors_dealloc (PyObject* self)
{
    self->ob_type->tp_free(self);
}


//
// Python sensors module definitions
//

static PyMethodDef module_methods[] = { {NULL} };

PyMODINIT_FUNC init_sensors (void)
{
    if (!Py_IsInitialized())
        Py_Initialize();

    if (PyType_Ready(&PySensorsType) == -1) {
        fprintf(stderr, "Error creating Sensors Python class type\n");
        if (PyErr_Occurred())
            PyErr_Print();
        else
            fprintf(stderr, "But no error available!\n");
        return;
    }

    _sensors_module = Py_InitModule3("_sensors", module_methods,
                                     "Container module for Sensors proxy class to C++");

    if (_sensors_module == NULL) {
        fprintf(stderr, "Error initializing Sensors Python module\n");
        return;
    }

    Py_INCREF(&PySensorsType);
    PyModule_AddObject(_sensors_module, "Sensors",
                       reinterpret_cast<PyObject*>(&PySensorsType));

#ifdef USE_PYSENSORS_FAKE_BACKEND
    PyObject *psensors = PySensors_new(new Sensors());
    PyModule_AddObject(_sensors_module, "inst", psensors);
#endif
}

int c_init_sensors (void)
{
    if (!Py_IsInitialized())
        Py_Initialize();

    if (PyType_Ready(&PySensorsType) == -1) {
        fprintf(stderr, "Error creating Sensors Python class type\n");
        if (PyErr_Occurred())
            PyErr_Print();
        else
            fprintf(stderr, "But no error available!\n");
        return -1;
    }

    _sensors_module = Py_InitModule3("_sensors", module_methods,
                                     "Container module for Sensors proxy class to C++");

    if (_sensors_module == NULL) {
        fprintf(stderr, "Error initializing Sensors Python module\n");
        return -1;
    }

    Py_INCREF(&PySensorsType);
    PyModule_AddObject(_sensors_module, "Sensors",
                       reinterpret_cast<PyObject*>(&PySensorsType));

#ifdef USE_PYSENSORS_FAKE_BACKEND
    PyObject *psensors = PySensors_new(new Sensors());
    PyModule_AddObject(_sensors_module, "inst", psensors);
#endif
    return 0;
}


//
// C++ Sensors class methods
//

Sensors::Sensors ()
    : bodyAngles(NUM_ACTUATORS), visionBodyAngles(NUM_ACTUATORS),
      motionBodyAngles(NUM_ACTUATORS),
      bodyAnglesError(NUM_ACTUATORS),
      leftFootFSR(0.0f, 0.0f, 0.0f, 0.0f),
      rightFootFSR(leftFootFSR),
      leftFootBumper(0.0f, 0.0f),
      rightFootBumper(0.0f, 0.0f),
      inertial(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f),
      unfilteredInertial(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f),
      ultraSoundDistance(0.0f), ultraSoundMode(LL),
      chestButton(0.0f),
      image(&global_image[0]), pySensors(NULL)
{
    pthread_mutex_init(&angles_mutex, NULL);
    pthread_mutex_init(&vision_angles_mutex, NULL);
    pthread_mutex_init(&motion_angles_mutex, NULL);
    pthread_mutex_init(&temperatures_mutex, NULL);
    pthread_mutex_init(&errors_mutex, NULL);
    pthread_mutex_init(&fsr_mutex, NULL);
    pthread_mutex_init(&button_mutex, NULL);
    pthread_mutex_init(&inertial_mutex, NULL);
    pthread_mutex_init(&unfiltered_inertial_mutex, NULL);
    pthread_mutex_init(&ultra_sound_mutex, NULL);
    pthread_mutex_init(&battery_mutex, NULL);
#ifdef USE_SENSORS_IMAGE_LOCKING
    pthread_mutex_init(&image_mutex, NULL);
#endif

    //BREAKS NAOQI1.0
//#ifndef NAOQI1
    add_to_module();
//#endif
}

Sensors::~Sensors ()
{
    pthread_mutex_destroy(&angles_mutex);
    pthread_mutex_destroy(&vision_angles_mutex);
    pthread_mutex_destroy(&motion_angles_mutex);
    pthread_mutex_destroy(&temperatures_mutex);
    pthread_mutex_destroy(&errors_mutex);
    pthread_mutex_destroy(&fsr_mutex);
    pthread_mutex_destroy(&button_mutex);
    pthread_mutex_destroy(&inertial_mutex);
    pthread_mutex_destroy(&unfiltered_inertial_mutex);
    pthread_mutex_destroy(&ultra_sound_mutex);
    pthread_mutex_destroy(&battery_mutex);
#ifdef USE_SENSORS_IMAGE_LOCKING
    pthread_mutex_destroy(&image_mutex);
#endif
}

const vector<float> Sensors::getBodyAngles () const
{
    pthread_mutex_lock (&angles_mutex);

    vector<float> vec(bodyAngles);

    pthread_mutex_unlock (&angles_mutex);

    return vec;
}

const vector<float> Sensors::getVisionBodyAngles() const
{
    pthread_mutex_lock (&vision_angles_mutex);

    vector<float> vec(visionBodyAngles);

    pthread_mutex_unlock (&vision_angles_mutex);

    return vec;
}


const vector<float> Sensors::getMotionBodyAngles() const
{
    pthread_mutex_lock (&motion_angles_mutex);

    vector<float> vec(motionBodyAngles);

    pthread_mutex_unlock (&motion_angles_mutex);

    return vec;
}

const vector<float> Sensors::getBodyTemperatures() const
{
    pthread_mutex_lock (&temperatures_mutex);

    vector<float> vec(bodyTemperatures);

    pthread_mutex_unlock (&temperatures_mutex);

    return vec;
}

const float Sensors::getBodyAngle(const int index) const {
    pthread_mutex_lock (&angles_mutex);

    const float angle = bodyAngles[index];

    pthread_mutex_unlock (&angles_mutex);

    return angle;
}

const vector<float> Sensors::getBodyAngleErrors () const
{
    pthread_mutex_lock (&errors_mutex);

    vector<float> vec(bodyAnglesError);

    pthread_mutex_unlock (&errors_mutex);

    return vec;
}

const float Sensors::getBodyAngleError (int index) const
{
	pthread_mutex_lock (&errors_mutex);

	const float angleError = bodyAnglesError[index];

	pthread_mutex_unlock (&errors_mutex);

	return angleError;
}

const FSR Sensors::getLeftFootFSR () const
{
    pthread_mutex_lock (&fsr_mutex);

    const FSR left(leftFootFSR);

    pthread_mutex_unlock (&fsr_mutex);

    return left;
}

const FSR Sensors::getRightFootFSR () const
{
    pthread_mutex_lock (&fsr_mutex);

    const FSR right(rightFootFSR);

    pthread_mutex_unlock (&fsr_mutex);

    return right;
}

const FootBumper Sensors::getLeftFootBumper() const
{
    pthread_mutex_lock (&button_mutex);

    const FootBumper bumper = leftFootBumper;

    pthread_mutex_unlock (&button_mutex);

    return bumper;
}

const FootBumper Sensors::getRightFootBumper() const
{
    pthread_mutex_lock (&button_mutex);

    const FootBumper bumper = rightFootBumper;

    pthread_mutex_unlock (&button_mutex);

    return bumper;
}

const Inertial Sensors::getInertial () const
{
    pthread_mutex_lock (&inertial_mutex);

    const Inertial inert(inertial);

    pthread_mutex_unlock (&inertial_mutex);

    return inert;
}

const Inertial Sensors::getUnfilteredInertial () const
{
    pthread_mutex_lock (&unfiltered_inertial_mutex);

    const Inertial inert(unfilteredInertial);

    pthread_mutex_unlock (&unfiltered_inertial_mutex);

    return inert;
}

const float Sensors::getUltraSound () const
{
    pthread_mutex_lock (&ultra_sound_mutex);

    float dist = ultraSoundDistance;

    pthread_mutex_unlock (&ultra_sound_mutex);

    return dist;
}

const UltraSoundMode Sensors::getUltraSoundMode () const
{
    pthread_mutex_lock (&ultra_sound_mutex);

    UltraSoundMode mode = ultraSoundMode;

    pthread_mutex_unlock (&ultra_sound_mutex);

    return mode;
}

const float Sensors::getChestButton () const
{
    pthread_mutex_lock (&button_mutex);

    float button = chestButton;

    pthread_mutex_unlock (&button_mutex);

    return button;
}

const float Sensors::getBatteryCharge () const
{
    pthread_mutex_lock (&battery_mutex);

    const float charge = batteryCharge;

    pthread_mutex_unlock (&battery_mutex);

    return charge;
}
const float Sensors::getBatteryCurrent () const
{
    pthread_mutex_lock (&battery_mutex);

    const float current = batteryCurrent;

    pthread_mutex_unlock (&battery_mutex);

    return current;
}

const vector<float> Sensors::getAllSensors () const
{
    //All sensors sans unfiltered Inertials and Temperatures
    //and the chest button preses
    pthread_mutex_lock (&fsr_mutex);
    pthread_mutex_lock (&button_mutex);
    pthread_mutex_lock (&inertial_mutex);
    pthread_mutex_lock (&ultra_sound_mutex);

    vector<float> allSensors;

    // write the FSR values
    allSensors += leftFootFSR.frontLeft, leftFootFSR.frontRight,
        leftFootFSR.rearLeft, leftFootFSR.rearRight,
        rightFootFSR.frontLeft, rightFootFSR.frontRight,
        rightFootFSR.rearLeft, rightFootFSR.rearRight;

    // write the foot bumper values
    allSensors += static_cast<float>(leftFootBumper.left),
        static_cast<float>(leftFootBumper.right),
        static_cast<float>(rightFootBumper.left),
        static_cast<float>(rightFootBumper.right);

    // write the accelerometers + gyros + filtered angleX and angleY
    allSensors += inertial.accX, inertial.accY, inertial.accZ,
        inertial.gyrX, inertial.gyrY,
        inertial.angleX, inertial.angleY;

    // write the ultrasound values
    allSensors += ultraSoundDistance;
    allSensors += static_cast<float>(ultraSoundMode);

    pthread_mutex_unlock (&fsr_mutex);
    pthread_mutex_unlock (&button_mutex);
    pthread_mutex_unlock (&inertial_mutex);
    pthread_mutex_unlock (&ultra_sound_mutex);

    return allSensors;
}

void Sensors::setBodyAngles (const vector<float>& v)
{
    pthread_mutex_lock (&angles_mutex);

    bodyAngles = v;
    /*
      cout << "Body angles in sensors";
      for (int i = 0 ; i < 22; i++){
      cout <<  bodyAngles[i] << " ";

      }
      cout << endl;
    */
    pthread_mutex_unlock (&angles_mutex);
}

void Sensors::setVisionBodyAngles (const vector<float>& v)
{
    pthread_mutex_lock (&vision_angles_mutex);

    visionBodyAngles = v;

    pthread_mutex_unlock (&vision_angles_mutex);
}

void Sensors::setMotionBodyAngles (const vector<float>& v)
{
    pthread_mutex_lock (&motion_angles_mutex);

    motionBodyAngles = v;

    pthread_mutex_unlock (&motion_angles_mutex);
}

void Sensors::setBodyAngleErrors (const vector<float>& v)
{
    pthread_mutex_lock (&errors_mutex);

    bodyAnglesError = v;

    pthread_mutex_unlock (&errors_mutex);
}


void Sensors::setBodyTemperatures (const vector<float>& v)
{
    pthread_mutex_lock (&temperatures_mutex);

    bodyTemperatures = v;

    pthread_mutex_unlock (&temperatures_mutex);
}

void Sensors::setLeftFootFSR(const float frontLeft, const float frontRight,
                             const float rearLeft, const float rearRight)
{
    pthread_mutex_lock (&fsr_mutex);

    leftFootFSR = FSR(frontLeft, frontRight, rearLeft, rearRight);

    pthread_mutex_unlock (&fsr_mutex);
}

void Sensors::setRightFootFSR(const float frontLeft, const float frontRight,
                              const float rearLeft, const float rearRight)
{
    pthread_mutex_lock (&fsr_mutex);

    rightFootFSR = FSR(frontLeft, frontRight, rearLeft, rearRight);

    pthread_mutex_unlock (&fsr_mutex);
}

void Sensors::setFSR(const FSR &_leftFootFSR, const FSR &_rightFootFSR)
{
    pthread_mutex_lock (&fsr_mutex);

    leftFootFSR = _leftFootFSR;
    rightFootFSR = _rightFootFSR;

    pthread_mutex_unlock (&fsr_mutex);
}

void Sensors::setLeftFootBumper(const float left, const float right)
{
    pthread_mutex_lock (&button_mutex);

    leftFootBumper = FootBumper(left, right);

    pthread_mutex_unlock (&button_mutex);
}

void Sensors::setLeftFootBumper(const FootBumper& bumper)
{
    pthread_mutex_lock (&button_mutex);

    leftFootBumper = bumper;

    pthread_mutex_unlock (&button_mutex);
}

void Sensors::setRightFootBumper(const float left, const float right)
{
    pthread_mutex_lock (&button_mutex);

    rightFootBumper = FootBumper(left, right);

    pthread_mutex_unlock (&button_mutex);
}

void Sensors::setRightFootBumper(const FootBumper& bumper)
{
    pthread_mutex_lock (&button_mutex);

    rightFootBumper = bumper;

    pthread_mutex_unlock (&button_mutex);
}

void Sensors::setInertial(const float accX, const float accY, const float accZ,
                          const float gyrX, const float gyrY,
                          const float angleX, const float angleY)
{
    pthread_mutex_lock (&inertial_mutex);

    inertial = Inertial(accX, accY, accZ, gyrX, gyrY, angleX, angleY);

    pthread_mutex_unlock (&inertial_mutex);
}

void Sensors::setInertial (const Inertial &v)
{
    pthread_mutex_lock (&inertial_mutex);

    inertial = v;

    pthread_mutex_unlock (&inertial_mutex);
}

void Sensors::setUnfilteredInertial(const float accX, const float accY, const float accZ,
                          const float gyrX, const float gyrY,
                          const float angleX, const float angleY)
{
    pthread_mutex_lock (&unfiltered_inertial_mutex);

    unfilteredInertial = Inertial(accX, accY, accZ, gyrX, gyrY, angleX, angleY);

    pthread_mutex_unlock (&unfiltered_inertial_mutex);
}

void Sensors::setUnfilteredInertial (const Inertial &v)
{
    pthread_mutex_lock (&unfiltered_inertial_mutex);

    unfilteredInertial = v;

    pthread_mutex_unlock (&unfiltered_inertial_mutex);
}

void Sensors::setUltraSound (const float dist)
{
    pthread_mutex_lock (&ultra_sound_mutex);

    ultraSoundDistance = dist;

    pthread_mutex_unlock (&ultra_sound_mutex);
}

void Sensors::setUltraSoundMode (const UltraSoundMode mode)
{
    pthread_mutex_lock (&ultra_sound_mutex);

    ultraSoundMode = mode;

    pthread_mutex_unlock (&ultra_sound_mutex);
}


/**
 * Sets the sensors which are updated on the motion frequency (every 20ms)
 */
void Sensors::setMotionSensors (const FSR &_leftFoot, const FSR &_rightFoot,
                                const float _chestButton,
                                const Inertial &_inertial,
                                const Inertial & _unfilteredInertial)
{
    pthread_mutex_lock(&button_mutex);
    pthread_mutex_lock(&fsr_mutex);
    pthread_mutex_lock(&inertial_mutex);
    pthread_mutex_lock(&unfiltered_inertial_mutex);

    leftFootFSR = _leftFoot;
    rightFootFSR = _rightFoot;
    chestButton = _chestButton;
    inertial = _inertial;
    unfilteredInertial = _unfilteredInertial;

    pthread_mutex_unlock(&unfiltered_inertial_mutex);
    pthread_mutex_unlock(&inertial_mutex);
    pthread_mutex_unlock(&fsr_mutex);
    pthread_mutex_unlock(&button_mutex);
}

/**
 * Sets the sensors which are updated on the vision frequency (every ?? ms)
 */
void Sensors::setVisionSensors (const FootBumper &_leftBumper,
                                const FootBumper &_rightBumper,
                                const float ultraSound,
                                const UltraSoundMode _mode,
                                const float bCharge, const float bCurrent)
{
    pthread_mutex_lock (&battery_mutex);
    pthread_mutex_lock (&button_mutex);
    pthread_mutex_lock(&ultra_sound_mutex);

    leftFootBumper = _leftBumper;
    rightFootBumper = _rightBumper;
    ultraSoundDistance = ultraSound;
    ultraSoundMode = _mode;
    batteryCharge = bCharge;
    batteryCurrent = bCurrent;

    pthread_mutex_unlock(&ultra_sound_mutex);
    pthread_mutex_unlock (&button_mutex);
    pthread_mutex_unlock (&battery_mutex);

}

void Sensors::setAllSensors (vector<float> sensorValues) {
    //All sensors sans unfiltered Inertials and Temperatures
    //and the chest button preses
    pthread_mutex_lock (&fsr_mutex);
    pthread_mutex_lock (&button_mutex);
    pthread_mutex_lock (&inertial_mutex);
    pthread_mutex_lock (&ultra_sound_mutex);

    // we have to be EXTRA careful about this order. If someone can think of
    // a better way to assign these so that it's checked at compile time
    // please do!
    leftFootFSR = FSR(sensorValues[0], sensorValues[1],
                      sensorValues[2], sensorValues[3]);
    rightFootFSR = FSR(sensorValues[4], sensorValues[5],
                       sensorValues[6], sensorValues[7]);

    leftFootBumper = FootBumper(sensorValues[8], sensorValues[9]);
    rightFootBumper = FootBumper(sensorValues[10], sensorValues[11]);

    inertial = Inertial(sensorValues[12], sensorValues[13], sensorValues[14],
                        sensorValues[15], sensorValues[16], // gyros
                        sensorValues[17], sensorValues[18]); // angleX/angleY

    ultraSoundDistance = sensorValues[19];
    // ugh... can't cast float to an enum, so cast to int and then to the enum.
    ultraSoundMode = static_cast<UltraSoundMode>(
        static_cast<int>(sensorValues[20]));

    pthread_mutex_unlock (&fsr_mutex);
    pthread_mutex_unlock (&button_mutex);
    pthread_mutex_unlock (&inertial_mutex);
    pthread_mutex_unlock (&ultra_sound_mutex);
}


void Sensors::lockImage()
{
#ifdef USE_SENSORS_IMAGE_LOCKING
    pthread_mutex_lock (&image_mutex);
#endif
}

void Sensors::releaseImage()
{
#ifdef USE_SENSORS_IMAGE_LOCKING
    pthread_mutex_unlock (&image_mutex);
#endif
}

void Sensors::updatePython() {
    if (pySensors != NULL) {
        PySensors_update(pySensors);
    }else{
        cout<< "py sensors none!!" << endl;
    }
}


/**
 * Beware!!! I am toying with the possibility that a possible deadlock condition
 * exists here. If we somehow are able to lock body_angles, but unable to lock
 * vision_body_angles because someone else is using it, but that someone else
 * is also waiting on the body_angles mutex, we get a beautiful deadlock. I don't
 * think anyone else requires both mutexes locked at the same time. Beware
 * nonetheless.
 */
void Sensors::updateVisionAngles() {
    pthread_mutex_lock (&angles_mutex);
    pthread_mutex_lock (&vision_angles_mutex);

    visionBodyAngles = bodyAngles;

    pthread_mutex_unlock (&vision_angles_mutex);
    pthread_mutex_unlock (&angles_mutex);
}

const unsigned char* Sensors::getImage ()
{
    return image;
}

void Sensors::setImage (const unsigned char *img)
{
    image = img;
}

void Sensors::add_to_module ()
{
    if (_sensors_module == NULL){
        if(c_init_sensors() != 0) {
            cerr << "import _sensors failed to import in C++ backend" << endl;
            PyErr_Print();
        }
    }
    if (_sensors_module != NULL) {
        if (pySensors != NULL)
            Py_DECREF(reinterpret_cast<PySensors*>(pySensors));

        pySensors = reinterpret_cast<PySensors*>(PySensors_new(this));
        PyModule_AddObject(_sensors_module, "inst",
                           reinterpret_cast<PyObject*>(pySensors));
    }else{
        cout << "sensors modules is null" << endl;
    }
}


void Sensors::saveFrame()
{
    static int saved_frames = 0;
    int MAX_FRAMES = 150;
    if (saved_frames > MAX_FRAMES)
        return;

    string EXT(".NFRM");
    string BASE("/");
    int NUMBER = saved_frames;
    string FOLDER("/home/root/frames");
    stringstream FRAME_PATH;

    FRAME_PATH << FOLDER << BASE << NUMBER << EXT;
    fstream fout(FRAME_PATH.str().c_str(), fstream::out);

    // Retrive joints
    vector<float> joints = getVisionBodyAngles();

    // Lock and write imag1e
    lockImage();
    fout.write(reinterpret_cast<const char*>(getImage()),
               IMAGE_BYTE_SIZE);
    releaseImage();

    // Write joints
    for (vector<float>::const_iterator i = joints.begin(); i < joints.end();
         i++) {
        fout << *i << " ";
    }

    // Write sensors
    vector<float> sensor_data = getAllSensors();
    for (vector<float>::const_iterator i = sensor_data.begin();
         i != sensor_data.end(); i++) {
        fout << *i << " ";
    }

    fout.close();
    cout << "Saved frame #" << saved_frames++ << endl;
}
