
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

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>
#include "_synchromodule.h"

using namespace std;
using namespace boost;

/**
Allocate a new PySynchro object.
**/
PyObject* PySynchro_new (PyTypeObject* type, PyObject* args,
    PyObject* kwds)
{
    PyObject* self = type->tp_alloc(type, 0);
    PySynchro* synchro = reinterpret_cast<PySynchro*>(self);

    if (self != NULL) {
        synchro->_synchro = shared_ptr<Synchro> (new Synchro());
        if (synchro->_synchro == NULL) {
            Py_DECREF(self);
            PyErr_SetFromErrno(PyExc_SystemError);
            return NULL;
        }
    }

    return self;
}

/**
Initialize a PySynchro object.
**/
int PySynchro_init (PyObject* self, PyObject* args,
    PyObject* kwds)
{
    //PySynchro* synchro = reinterpret_cast<PySynchro*>(self); // unused
    return 0;
}

/**
Deallocate memory for a deleted PySynchro object.
**/
void PySynchro_dealloc (PyObject* self)
{
    PySynchro* synchro = reinterpret_cast<PySynchro*>(self);
    synchro->_synchro.reset();
    self->ob_type->tp_free(self);
}

/**
Create a new PySynchro object from a C++ Synchro object, from C++.
**/
PyObject* PySynchro_new (shared_ptr<Synchro> _synchro)
{
    PyObject* self = PySynchroType.tp_alloc(&PySynchroType, 0);
    PySynchro* synchro = reinterpret_cast<PySynchro*>(self);

    if (self != NULL) {
        synchro->_synchro = _synchro;
    }

    return self;
}

/**
Returns a list of events available for synchronization.
**/
PyObject* PySynchro_available (PyObject* self, PyObject* args)
{
    PySynchro* synchro = reinterpret_cast<PySynchro*>(self);
    PyObject* result = NULL;
    Py_BEGIN_ALLOW_THREADS;

    const map<string, shared_ptr<Event> > events =
        synchro->_synchro->available();
    result = PyDict_New();

    for (map<string, shared_ptr<Event> >::const_iterator itr = events.begin();
            itr != events.end(); itr++) {
        PyObject* event = PyEvent_new(itr->second);
        if (event != NULL) {
            PyDict_SetItem(result, reinterpret_cast<PyEvent*>(event)->name,
                           event);
            Py_DECREF(event);
        }else {
            Py_DECREF(result);
            result = NULL;
            break;
        }
    }

    Py_END_ALLOW_THREADS;
    return result;
}

/**
Wait, deferring processing time to other threads, until the
given event is signalled.

If the event has been signalled previously and hast not yet
been caught, this method will return immediately and clear the
signal.
**/
PyObject* PySynchro_await (PyObject* self, PyObject* args)
{
    //PySynchro* synchro = reinterpret_cast<PySynchro*>(self); // unused
    PyObject* result = NULL;
    Py_BEGIN_ALLOW_THREADS;

    if (!PyObject_TypeCheck(args, &PyEventType)) {
        PyErr_Format(PyExc_TypeError,
                     "await() expected an Event object, got '%s'",
                     args->ob_type->tp_name);
    }else{
        reinterpret_cast<PyEvent*>(args)->_event->await();
        Py_INCREF(Py_None);
        result = Py_None;
    }

    Py_END_ALLOW_THREADS;
    return result;
}

/**
Register a new event with the synchronizer.

Returns an Event object, which is also retrievable via available()
and the Event's name.  The event object itself should be passed to
any of the Synchronizer methods that act on Events, not the event's
name.
**/
PyObject* PySynchro_create (PyObject* self, PyObject* args)
{
    PySynchro* synchro = reinterpret_cast<PySynchro*>(self);
    PyObject* result = NULL;
    Py_BEGIN_ALLOW_THREADS;

    char *name;
    if (PyArg_ParseTuple(args, "s", &name)) {
        shared_ptr<Event> event = synchro->_synchro->create(name);
        if (event != NULL)
            result = PyEvent_new(event);
        else
            PyErr_SetFromErrno(PyExc_SystemError);
    }

    Py_END_ALLOW_THREADS;
    return result;
}

/**
Return a boolean indicating whether the given event has
occurred.

Returns True only if the event has been signalled after the
last call to poll() or await().
**/
PyObject* PySynchro_poll (PyObject* self, PyObject* args)
{
    //PySynchro* synchro = reinterpret_cast<PySynchro*>(self); // unused
    PyObject* result = NULL;
    Py_BEGIN_ALLOW_THREADS;

    if (!PyObject_TypeCheck(args, &PyEventType)) {
        PyErr_Format(PyExc_TypeError,
                     "await() expected an Event object, got '%s'",
                     args->ob_type->tp_name);
    }else{
        result = PyBool_FromLong(
                reinterpret_cast<PyEvent*>(args)->_event->poll());
    }

    Py_END_ALLOW_THREADS;
    return result;
}

/**
Signal occurance of the event.

If any threads are waiting for the Event, a single thread will
resume from wait, immediately clearing the signal.  If no threads
await, the signal will persist until the Event is polled or
awaited.
**/
PyObject* PySynchro_signal (PyObject* self, PyObject* args)
{
    //PySynchro* synchro = reinterpret_cast<PySynchro*>(self); // unused
    Py_BEGIN_ALLOW_THREADS;

    if (!PyObject_TypeCheck(args, &PyEventType)) {
        PyErr_Format(PyExc_TypeError,
                     "signal() expected an Event object, got '%s'",
                     args->ob_type->tp_name);
    }else{
        reinterpret_cast<PyEvent*>(args)->_event->signal();
    }

    Py_END_ALLOW_THREADS;
    Py_RETURN_NONE;
}

/**
Allocate a new PyEvent object.
**/
PyObject* PyEvent_new (PyTypeObject* type, PyObject* args,
    PyObject* kwds)
{
    PyErr_SetString(PyExc_SystemError,
            "Cannot create Event object from Python");
    return NULL;
}

/**
Initialize a PyEvent object.
**/
int PyEvent_init (PyObject* self, PyObject* args,
    PyObject* kwds)
{
    //PyEvent* event = reinterpret_cast<PyEvent*>(self); // unused
    return 0;
}

/**
Deallocate memory for a deleted PyEvent object.
**/
void PyEvent_dealloc (PyObject* self)
{
    PyEvent* event = reinterpret_cast<PyEvent*>(self);
    event->_event.reset();
    self->ob_type->tp_free(self);
}

/**
Create a new PyEvent object from a C++ Event object, from C++.
**/
PyObject* PyEvent_new (shared_ptr<Event> _event)
{
    PyObject* self = PyEventType.tp_alloc(&PyEventType, 0);
    PyEvent* event = reinterpret_cast<PyEvent*>(self);

    if (self != NULL) {
        event->_event = _event;
        event->name = PyString_FromStringAndSize(_event->name.data(),
                                                 _event->name.size());
    }

    return self;
}

/**
Wait, deferring processing time to other threads, until the
given event is signalled.

If the event has been signalled previously and hast not yet
been caught, this method will return immediately and clear the
signal.
**/
PyObject* PyEvent_await (PyObject* self, PyObject* args)
{
    PyEvent* event = reinterpret_cast<PyEvent*>(self);
    Py_BEGIN_ALLOW_THREADS;

    event->_event->await();

    Py_END_ALLOW_THREADS;
    Py_RETURN_NONE;
}

/**
Return a boolean indicating whether the given event has
occurred.

Returns True only if the event has been signalled after the
last call to poll() or await().
**/
PyObject* PyEvent_poll (PyObject* self, PyObject* args)
{
    PyEvent* event = reinterpret_cast<PyEvent*>(self);
    PyObject* result = NULL;
    Py_BEGIN_ALLOW_THREADS;

    result = PyBool_FromLong(event->_event->poll());

    Py_END_ALLOW_THREADS;
    return result;
}

/**
Signal occurance of the event.

If any threads are waiting for the Event, a single thread will
resume from wait, immediately clearing the signal.  If no threads
await, the signal will persist until the Event is polled or
awaited.
**/
PyObject* PyEvent_signal (PyObject* self, PyObject* args)
{
    PyEvent* event = reinterpret_cast<PyEvent*>(self);
    Py_BEGIN_ALLOW_THREADS;

    event->_event->signal();

    Py_END_ALLOW_THREADS;
    Py_RETURN_NONE;
}

/**
Module-level methods for _synchro
**/
static PyMethodDef _synchro_methods[] = {

    {NULL} // Sentinel
};

/**
Initialize the _synchro module as a dynamic library.
**/
PyMODINIT_FUNC init_synchro (void)
{
    // Initialize the type objects
    if (PyType_Ready(&PySynchroType) < 0)
        return; // error handled by Python
    if (PyType_Ready(&PyEventType) < 0)
        return; // error handled by Python

    // Initialize module
    PyObject *module = Py_InitModule3("_synchro",
        _synchro_methods,
        "Python wrapped access to synchro");
    if (module == NULL)
        return; // error handled by Python

    // Add type object references to the module
    Py_INCREF(&PySynchroType);
    PyModule_AddObject(module, "Synchro",
        reinterpret_cast<PyObject*>(&PySynchroType));
    Py_INCREF(&PyEventType);
    PyModule_AddObject(module, "Event",
        reinterpret_cast<PyObject*>(&PyEventType));

}

/**
Initialize the _synchro module from C++ as a backend extension.
**/
int c_init_synchro (void)
{
    // Initialize the type objects
    if (PyType_Ready(&PySynchroType) < 0)
        return -1;
    if (PyType_Ready(&PyEventType) < 0)
        return -1;

    // Initialize module
    PyObject *module = Py_InitModule3("_synchro",
        _synchro_methods,
        "Python wrapped access to synchro");
    if (module == NULL)
        return -1;

    // Add type object references to the module
    Py_INCREF(&PySynchroType);
    PyModule_AddObject(module, "Synchro",
        reinterpret_cast<PyObject*>(&PySynchroType));
    Py_INCREF(&PyEventType);
    PyModule_AddObject(module, "Event",
        reinterpret_cast<PyObject*>(&PyEventType));

    return 0;
}


