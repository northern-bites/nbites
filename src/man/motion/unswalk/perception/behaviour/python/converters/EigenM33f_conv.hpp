#pragma once

#include <boost/python.hpp>
#include <boost/python/refcount.hpp>

#include <Eigen/Eigen>

struct EigenM33f_to_python
{
   static PyObject* convert(const Eigen::Matrix<float, 3, 3> &m)
   {
      return
         boost::python::incref(
            boost::python::make_tuple(
               boost::python::make_tuple(m(0,0), m(1,0), m(2,0)),
               boost::python::make_tuple(m(0,1), m(1,1), m(2,1)),
               boost::python::make_tuple(m(0,2), m(1,2), m(2,2))
            ).ptr()
         );
   }
};


