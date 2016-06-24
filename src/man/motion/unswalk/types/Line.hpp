#pragma once

#include <Eigen/Eigen>
#include "types/Point.hpp"

/**
 * Note that 
 *    Point  <=> Matrx<int  , 2, 1>
 *    PointF <=> Matrx<float, 2, 1>
 *
 * Templated typedef's are not supported by C++
 */

template <typename T>
struct __Line_Generic
{
   Eigen::Matrix<T, 2, 1> p, v;

   __Line_Generic(Eigen::Matrix<T, 2, 1> p, Eigen::Matrix<T, 2, 1> v);

   bool intersect(const __Line_Generic<T> &l,
                  Eigen::Matrix<T, 2, 1> &intersection) const;

   T distToPoint(Eigen::Matrix<T, 2, 1> p) const;

   template <typename cast_T>
   __Line_Generic<cast_T> cast() const;

   template<class Archive>
   void serialize(Archive &ar, const unsigned int version);
};

typedef __Line_Generic<int>   Line;
typedef __Line_Generic<float> LineF;

#include "types/Line.tcc"

