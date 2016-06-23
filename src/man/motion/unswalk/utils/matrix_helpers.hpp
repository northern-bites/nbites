#pragma once

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>

/* Creates a matrix that Rotate vector about the z axis
 * Currently needed for our robot relative coordinate system
 */
template <typename T>
boost::numeric::ublas::matrix<T> rotateZMatrix(T theta);

/* Creates a translation matrix
 */
template <typename T>
boost::numeric::ublas::matrix<T> translateMatrix(T x, T y, T z);

/* Creates a projection matrix
 */
template <typename T>
boost::numeric::ublas::matrix<T> projectionMatrix(T ex, T ey,
                                                  T ez);


/* Creates a DH matrix used in Kinematics
 */
template <typename T>
boost::numeric::ublas::matrix<T> createDHMatrix(T a, T alpha,
                                                T d, T theta);

/* Function to invert matrix
 */
template <typename T>
bool invertMatrix(const boost::numeric::ublas::matrix<T>& input,
                  boost::numeric::ublas::matrix<T>& inverse);

/* Creates a 4,1 vector */
template <typename T>
inline boost::numeric::ublas::matrix<T>
vec4(T a, T b, T c, T d) {
   boost::numeric::ublas::matrix<T> m(4, 1);
   m(0, 0) = a;
   m(1, 0) = b;
   m(2, 0) = c;
   m(3, 0) = d;
   return m;
}

/* Creates a 4,1 vector from a 4,1 array */
template <typename T>
inline boost::numeric::ublas::matrix<T>
vec4(const T a[]) {
   boost::numeric::ublas::matrix<T> m(4, 1);
   m(0, 0) = a[0];
   m(1, 0) = a[1];
   m(2, 0) = a[2];
   m(3, 0) = a[3];
   return m;
}

#include "utils/matrix_helpers.tcc"
