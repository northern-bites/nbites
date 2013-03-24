#ifndef NBMatrixMath_h
#define NBMatrixMath_h

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/lu.hpp>              // for lu_factorize
#include "NBVector.h"

namespace NBMath {

/*
 * This saves me some nasty typing...
 * uBLAS by default uses std::vector as its underlying storage data
 * structure. By overriding the matrix type to use ublas::bounded_array
 * we're forcing uBLAS to use arrays as the storage type which allows all
 * variables to stay off the heap and on the stack. This offers a dramatic
 * increase in performance with the slight inconvenience that the static
 * array needs to be big enough to fit a n by n matrix. The biggest one I
 * handle is 3x3, so a bounded array of size 9 is sufficient.
 */
    typedef boost::numeric::ublas::matrix<
        float,
        boost::numeric::ublas::row_major,
        boost::numeric::ublas::bounded_array<float,9> > ufmatrix3;

    typedef boost::numeric::ublas::matrix<
        float,
        boost::numeric::ublas::row_major,
        boost::numeric::ublas::bounded_array<float,16> > ufmatrix4;

    typedef boost::numeric::ublas::matrix <float> ufmatrix;

    const boost::numeric::ublas::matrix<float>
    invert2by2(boost::numeric::ublas::matrix<float> m);

    const boost::numeric::ublas::vector<float>
    solve(boost::numeric::ublas::matrix<float> A,
          const boost::numeric::ublas::vector<float> &b);

    const boost::numeric::ublas::matrix<float>
    solve(boost::numeric::ublas::matrix<float> A,
          const boost::numeric::ublas::matrix<float> &B);


// Solve the linear system Ax=b for the vector x.
// NOTE: This method is hard coded to work for 3x3 matrices and 3-vectors.
//       We can get superior performance this way.
    const ufvector3 solve(ufmatrix3 &A,
                          const ufvector3 &b);

    const float length(ufvector4 &v);

    // Kalman filter needs to be able to transpose square matrices
}

#endif
