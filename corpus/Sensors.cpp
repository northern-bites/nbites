#include "Sensors.h"
#include <structmember.h>

#include "Kinematics.h"
#include "corpusconfig.h"

using namespace Kinematics;
using namespace std;

// Static reference to Python sensors module

static PyObject *_sensors_module = NULL;

// static base image array, so we don't crash on image access if the setImage()
// method is never called
static unsigned char global_image[IMAGE_BYTE_SIZE];



static void
PySensors_update(PySensors *self)
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

#if ROBOT(NAO)
  values = sensors->getFSR();
  for (i = 0; i < (int)values.size(); i++) {
    if (i < PyList_Size(self->fsr))
      PyList_SET_ITEM(self->fsr, i, PyFloat_FromDouble(values[i]));
    else
      PyList_Append(self->fsr, PyFloat_FromDouble(values[i]));
  }

  values = sensors->getInertial(); //Gyros should eventually be converted to DEG
  for (i = 0; i < (int)values.size(); i++) {
    if (i < PyList_Size(self->inertial))
      PyList_SET_ITEM(self->inertial, i, PyFloat_FromDouble(values[i]));
    else
      PyList_Append(self->inertial, PyFloat_FromDouble(values[i]));
  }

  values = sensors->getSonar();
  Py_XDECREF(self->sonarLeft);
  self->sonarLeft = PyFloat_FromDouble(values[0]);
  Py_XDECREF(self->sonarRight);
  self->sonarRight = PyFloat_FromDouble(values[1]);
#endif
}

static PyObject *
PySensors_update (PyObject *self, PyObject *)
{
  PySensors_update((PySensors *)self);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
PySensors_setImage (PyObject *self, PyObject *args)
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


static PyMethodDef PySensors_methods[] = {

  {"update", (PyCFunction)PySensors_update, METH_NOARGS,
    "Update all the built Python objects to reflect the current state of the "
    "backend C++ objects.  Recurses down the variable references to update "
    "any attributes that are also wrapped C++ vision objects."},

  {"setImage", (PyCFunction)PySensors_setImage, METH_VARARGS,
    "Set the image data for the robot.  Copies raw bytes data from the Python "
    "string into the static global_image array, then sets the image pointer "
    "in the Sensors object."},

  { NULL } /* Sentinel */
};

static PyMemberDef PySensors_members[] = {
  
  {"angles", T_OBJECT_EX, offsetof(PySensors, angles), READONLY,
    "Body angles values."},
  {"errors", T_OBJECT_EX, offsetof(PySensors, errors), READONLY,
    "Body angles error values."},
#if ROBOT(NAO)
  {"fsr", T_OBJECT_EX, offsetof(PySensors, fsr), READONLY,
    "Force sensitive resitor values."},
  {"inertial", T_OBJECT_EX, offsetof(PySensors, inertial), READONLY,
    "Inertial sensor values."},
  {"sonarLeft", T_OBJECT_EX, offsetof(PySensors, sonarLeft), READONLY,
    "Left sonar distance sensor value."},
  {"sonarRight", T_OBJECT_EX, offsetof(PySensors, sonarRight), READONLY,
    "Right sonar distance sensor value."},
#endif

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

static PyObject *
PySensors_new (Sensors *sensors)
{
  PySensors *self;

  self = (PySensors *)PySensorsType.tp_alloc(&PySensorsType, 0);
  if (self != NULL) {
    self->_sensors = sensors;

    self->angles = PyList_New(NUM_ACTUATORS);
    self->errors = PyList_New(NUM_ACTUATORS);
#if ROBOT(NAO)
    self->fsr = PyList_New(AL_NUMBER_OF_FSR);
    self->inertial = PyList_New(NUM_INERTIAL_SENSORS);
#endif

    if (self->angles == NULL || self->errors == NULL 
#if ROBOT(NAO)
        || self->fsr == NULL || self->inertial == NULL) {
#else
        ) {
#endif
      PySensors_dealloc(reinterpret_cast<PyObject*>(self));
      self = NULL;
    }else
      PySensors_update(self);
  }

  return (PyObject *)self;
}

static PyObject*
PySensors_new (PyTypeObject* type, PyObject* args, PyObject* kwds)
{
  return type->tp_alloc(type, 0);
}

static int
PySensors_init (PySensors *self, PyObject *args, PyObject *kwds)
{
  static char* keywords[] = {"other"};
  PyObject *other;

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!", keywords, 
        &PySensorsType, &other))
    return -1;

  self->_sensors = reinterpret_cast<PySensors*>(other)->_sensors;

  self->angles = PyList_New(NUM_ACTUATORS);
  self->errors = PyList_New(NUM_ACTUATORS);
#if ROBOT(NAO)
  self->fsr = PyList_New(AL_NUMBER_OF_FSR);
  self->inertial = PyList_New(NUM_INERTIAL_SENSORS);
#endif

  if (self->angles == NULL || self->errors == NULL 
#if ROBOT(NAO)
    || self->fsr == NULL || self->inertial == NULL) {
#else
      ) {
#endif
    return -1;
  }else
    PySensors_update(self);

  return 0;
}

static void
PySensors_dealloc (PyObject* self)
{
  self->ob_type->tp_free(self);
}


//
// Python sensors module definitions
//

static PyMethodDef module_methods[] = { {NULL} };

PyMODINIT_FUNC
init_sensors (void)
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

int
c_init_sensors (void)
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
    bodyAnglesError(NUM_ACTUATORS),
#if ROBOT(NAO)
    fsr(AL_NUMBER_OF_FSR), inertial(NUM_INERTIAL_SENSORS),
#endif
    image(&global_image[0]), pySensors(NULL)
{
  pthread_mutex_init(&angles_mutex, NULL);
  pthread_mutex_init(&vision_angles_mutex, NULL);
  pthread_mutex_init(&errors_mutex, NULL);
#if ROBOT(NAO)
  pthread_mutex_init(&fsr_mutex, NULL);
  pthread_mutex_init(&inertial_mutex, NULL);
  pthread_mutex_init(&sonar_mutex, NULL);
#endif
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
  pthread_mutex_destroy(&errors_mutex);
#if ROBOT(NAO)
  pthread_mutex_destroy(&fsr_mutex);
  pthread_mutex_destroy(&inertial_mutex);
  pthread_mutex_destroy(&sonar_mutex);
#endif
#ifdef USE_SENSORS_IMAGE_LOCKING
  pthread_mutex_destroy(&image_mutex);
#endif
}

vector<float>
Sensors::getBodyAngles ()
{
  pthread_mutex_lock (&angles_mutex);

  vector<float> vec(bodyAngles);

  pthread_mutex_unlock (&angles_mutex);

  return vec;
}

vector<float>
Sensors::getVisionBodyAngles()
{
  pthread_mutex_lock (&vision_angles_mutex);

  vector<float> vec(visionBodyAngles);

  pthread_mutex_unlock (&vision_angles_mutex);

  return vec;
}

const float
Sensors::getBodyAngle(const int index) {
  pthread_mutex_lock (&angles_mutex);

  const float angle = bodyAngles[index];

  pthread_mutex_unlock (&angles_mutex);

  return angle;
}

vector<float>
Sensors::getBodyAngleErrors ()
{
  pthread_mutex_lock (&errors_mutex);

  vector<float> vec(bodyAnglesError);

  pthread_mutex_unlock (&errors_mutex);

  return vec;
}

void
Sensors::setBodyAngles (vector<float>& v)
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

void
Sensors::setVisionBodyAngles (vector<float>& v)
{
  pthread_mutex_lock (&vision_angles_mutex);

  visionBodyAngles = v;

  pthread_mutex_unlock (&vision_angles_mutex);
}

void
Sensors::setBodyAngleErrors (vector<float>& v)
{
  pthread_mutex_lock (&errors_mutex);

  bodyAnglesError = v;

  pthread_mutex_unlock (&errors_mutex);
}

#if ROBOT(NAO)

vector<float>
Sensors::getFSR ()
{
  pthread_mutex_lock (&fsr_mutex);

  vector<float> vec(fsr);

  pthread_mutex_unlock (&fsr_mutex);

  return vec;
}

vector<float>
Sensors::getInertial ()
{
  pthread_mutex_lock (&inertial_mutex);

  vector<float> vec(inertial);

  pthread_mutex_unlock (&inertial_mutex);

  return vec;
}

vector<float>
Sensors::getSonar ()
{
  pthread_mutex_lock (&sonar_mutex);

  vector<float> vec(2);
  vec.push_back(sonarLeft);
  vec.push_back(sonarRight);

  pthread_mutex_unlock (&sonar_mutex);

  return vec;
}

void
Sensors::setFSR (vector<float>& v)
{
  pthread_mutex_lock (&fsr_mutex);

  fsr = v;

  pthread_mutex_unlock (&fsr_mutex);
}

void
Sensors::setInertial (vector<float>& v)
{
  pthread_mutex_lock (&inertial_mutex);

  inertial = v;

  pthread_mutex_unlock (&inertial_mutex);
}

void
Sensors::setSonar (float l, float r)
{
  pthread_mutex_lock (&sonar_mutex);

  sonarLeft = l;
  sonarRight = r;

  pthread_mutex_unlock (&sonar_mutex);
}

#endif /* ROBOT(NAO) */

void
Sensors::lockImage()
{
#ifdef USE_SENSORS_IMAGE_LOCKING
  pthread_mutex_lock (&image_mutex);
#endif
}

void
Sensors::releaseImage()
{
#ifdef USE_SENSORS_IMAGE_LOCKING
  pthread_mutex_unlock (&image_mutex);
#endif
}

void
Sensors::updatePython() {
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
void
Sensors::updateVisionAngles() {
  pthread_mutex_lock (&angles_mutex);
  pthread_mutex_lock (&vision_angles_mutex);

  visionBodyAngles = bodyAngles;

  pthread_mutex_unlock (&angles_mutex);
  pthread_mutex_unlock (&vision_angles_mutex);
}

const unsigned char*
Sensors::getImage ()
{
  return image;
}

void
Sensors::setImage (const unsigned char *img)
{
  image = img;
}

void
Sensors::add_to_module ()
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

