#pragma once

#include <boost/python.hpp>
#include <boost/python/refcount.hpp>

#include <types/Point.hpp>

struct Point_to_python
{
   static PyObject *convert(const Point &p)
   {
      return
         boost::python::incref(
            boost::python::make_tuple(p.x(), p.y()).ptr()
         );
   }
};

struct PointF_to_python
{
   static PyObject *convert(const PointF &p)
   {
      return
         boost::python::incref(
            boost::python::make_tuple(p.x(), p.y()).ptr()
         );
   }
};

