#pragma once

#include <boost/python.hpp>
#include <boost/python/refcount.hpp>


#include <structmember.h>

template <typename T, size_t n>
struct array_to_python
{
   static PyObject *convert(T p[n])
   {
      array_wrap *aw = PyObject_New(array_wrap, &array_wrapType);
      aw->p = p;

      return PyObject_Init((PyObject *)aw, &array_wrapType);
   }

   static int ready()
   {
      return PyType_Ready(&array_wrapType);
   }

private:
   struct array_wrap {
      PyObject_HEAD

      /* pointer to array */
      T *p;
   };

   static PyMappingMethods array_wrap_mapping;
   static PyTypeObject array_wrapType;

   static void
   array_wrap_dealloc(array_wrap *self);

   static PyObject *
   array_wrap_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

   static int
   array_wrap_init(array_wrap *self, PyObject *args, PyObject *kwds);

   static PyObject *
   array_wrap___getitem__(array_wrap* self, PyObject *item);

   static Py_ssize_t
   array_wrap___len__(array_wrap* self);
};

#include "array_conv.tcc"
