
//  WARNING, this file was automatically generated using the Northern
//  Bites' Python wrapper extension module generator.  Subsequent
//  form-completion was done by hand.


//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <Python.h>
#include <structmember.h>
#include <boost/shared_ptr.hpp>
#include "synchro.h"

//
// PySynchro definitions
//

typedef struct PySynchro_t
{
    PyObject_HEAD
    boost::shared_ptr<Synchro> _synchro;
} PySynchro;


// backend methods
extern PyObject* PySynchro_new (PyTypeObject* type, PyObject* args,
    PyObject* kwds);
extern int PySynchro_init (PyObject* self, PyObject* arg, PyObject* kwds);
extern void PySynchro_dealloc (PyObject* self);
// C++ - accessible interface
extern PyObject* PySynchro_new (boost::shared_ptr<Synchro> _synchro);
// Python - accesible interface
extern PyObject* PySynchro_available (PyObject* self, PyObject* args);
extern PyObject* PySynchro_await (PyObject* self, PyObject* args);
extern PyObject* PySynchro_create (PyObject* self, PyObject* args);
extern PyObject* PySynchro_poll (PyObject* self, PyObject* args);
extern PyObject* PySynchro_signal (PyObject* self, PyObject* args);

// backend method list
static PyMethodDef PySynchro_methods[] = {

    {"available", reinterpret_cast<PyCFunction>(PySynchro_available),
      METH_VARARGS,
      "Returns a list of events available for synchronization."},

    {"await", reinterpret_cast<PyCFunction>(PySynchro_await),
      METH_O,
      "\n        Wait, deferring processing time to other threads, until the\n        given event is signalled.\n\n        If the event has been signalled previously and hast not yet\n        been caught, this method will return immediately and clear the\n        signal.\n        "},

    {"create", reinterpret_cast<PyCFunction>(PySynchro_create),
      METH_VARARGS,
      "\n        Register a new event with the synchronizer.\n\n        Returns an Event object, which is also retrievable via available()\n        and the Event's name.  The event object itself should be passed to\n        any of the Synchronizer methods that act on Events, not the event's\n        name.\n        "},

    {"poll", reinterpret_cast<PyCFunction>(PySynchro_poll),
      METH_O,
      "\n        Return a boolean indicating whether the given event has\n        occurred.\n\n        Returns True only if the event has been signalled after the\n        last call to poll() or await().\n        "},

    {"signal", reinterpret_cast<PyCFunction>(PySynchro_signal),
      METH_O,
      "\n        Signal occurance of the event.\n\n        If any threads are waiting for the Event, a single thread will\n        resume from wait, immediately clearing the signal.  If no threads\n        await, the signal will persist until the Event is polled or\n        awaited.\n        "},

    {NULL} // Sentinel
};

// backend member list
static PyMemberDef PySynchro_members[] = {

    {NULL} // Sentinel
};

// PySynchro type definition
static PyTypeObject PySynchroType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "_synchro.Synchro",    /*tp_name*/
    sizeof(PySynchro),        /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PySynchro_dealloc,/*tp_dealloc*/
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
    "\n    A generic Synchronizer object.\n\n    Maintains a collection of Events (mutexs), which may be polled or\n    awaited.\n    ",                 /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PySynchro_methods,        /* tp_methods */
    PySynchro_members,        /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PySynchro_init, /* tp_init */
    0,                         /* tp_alloc */
    (newfunc)PySynchro_new,   /* tp_new */
};

//
// PyEvent definitions
//

typedef struct PyEvent_t
{
    PyObject_HEAD
    boost::shared_ptr<Event> _event;

    PyObject* name;
} PyEvent;


// backend methods
extern PyObject* PyEvent_new (PyTypeObject* type, PyObject* args,
    PyObject* kwds);
extern int PyEvent_init (PyObject* self, PyObject* arg, PyObject* kwds);
extern void PyEvent_dealloc (PyObject* self);
// C++ - accessible interface
extern PyObject* PyEvent_new (boost::shared_ptr<Event> _event);
// Python - accesible interface
extern PyObject* PyEvent_await (PyObject* self, PyObject* args);
extern PyObject* PyEvent_poll (PyObject* self, PyObject* args);
extern PyObject* PyEvent_signal (PyObject* self, PyObject* args);

// backend method list
static PyMethodDef PyEvent_methods[] = {

    {"await", reinterpret_cast<PyCFunction>(PyEvent_await),
      METH_VARARGS,
      "\n        Wait, deferring processing time to other threads, until the\n        given event is signalled.\n\n        If the event has been signalled previously and hast not yet\n        been caught, this method will return immediately and clear the\n        signal.\n        "},

    {"poll", reinterpret_cast<PyCFunction>(PyEvent_poll),
      METH_VARARGS,
      "\n        Return a boolean indicating whether the given event has\n        occurred.\n\n        Returns True only if the event has been signalled after the\n        last call to poll() or await().\n        "},

    {"signal", reinterpret_cast<PyCFunction>(PyEvent_signal),
      METH_VARARGS,
      "\n        Signal occurance of the event.\n\n        If any threads are waiting for the Event, a single thread will\n        resume from wait, immediately clearing the signal.  If no threads\n        await, the signal will persist until the Event is polled or\n        awaited.\n        "},

    {NULL} // Sentinel
};

// backend member list
static PyMemberDef PyEvent_members[] = {

    {"name", T_OBJECT_EX, offsetof(PyEvent, name), READONLY,
     "the name of this event"},

    {NULL} // Sentinel
};

// PyEvent type definition
static PyTypeObject PyEventType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "_synchro.Event",    /*tp_name*/
    sizeof(PyEvent),        /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PyEvent_dealloc,/*tp_dealloc*/
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
    "\n    An abstract Event, recieved from a Synchronizer.\n\n    Used to pass and hold reference to a specific event.  A string name is\n    associated with the event, but the object itself is used in the backend\n    map.\n    ",                 /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PyEvent_methods,        /* tp_methods */
    PyEvent_members,        /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyEvent_init, /* tp_init */
    0,                         /* tp_alloc */
    (newfunc)PyEvent_new,   /* tp_new */
};

