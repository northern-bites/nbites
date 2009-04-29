#ifndef NBMatrixMath_h
#define NBMatrixMath_h

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/lu.hpp>              // for lu_factorize

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
    typedef boost::numeric::ublas::vector<
        float, boost::numeric::ublas::bounded_array<float,3> > ufvector3;
    typedef boost::numeric::ublas::matrix<
        float,
        boost::numeric::ublas::row_major,
        boost::numeric::ublas::bounded_array<float,3> > ufrowVector3;

    typedef boost::numeric::ublas::matrix<
        float,
        boost::numeric::ublas::row_major,
        boost::numeric::ublas::bounded_array<float,16> > ufmatrix4;
    typedef boost::numeric::ublas::vector<
        float, boost::numeric::ublas::bounded_array<float,4> > ufvector4;


/**
   Invert a two by two matrix easily
   given:
   [a b
   c d]
   return:
   (1/(ad - bc)) [ d -b
   -c  a]
   @param m the 2 by 2 matrix to invert.
   @return the inversion of m
*/
    static const boost::numeric::ublas::matrix<float>
    invert2by2(boost::numeric::ublas::matrix<float> m)
    {
        float det = 1.0f / ( m(0,0) * m(1,1) - m(0,1) * m(1,0));
        float tmp = m(0,0);
        m(0,0) = m(1,1);
        m(1,1) = tmp;
        tmp = -m(0,1);
        m(0,1) = -m(1,0);
        m(1,0) = tmp;
        return det * m;
    }

    static const boost::numeric::ublas::vector<float>
    solve(boost::numeric::ublas::matrix<float> A,
          const boost::numeric::ublas::vector<float> &b) {
        boost::numeric::ublas::permutation_matrix<float> P(A.size1());
        int singularRow = lu_factorize(A, P);
        if (singularRow != 0) {
            // TODO: This case needs to be dealt with
            throw "the system had no solution";
        }
        boost::numeric::ublas::vector<float> result(A.size2());
        result.assign(b);
        lu_substitute(A, P, result);
        return result;
    }

    static const boost::numeric::ublas::matrix<float>
    solve(boost::numeric::ublas::matrix<float> A,
          const boost::numeric::ublas::matrix<float> &B) {
        boost::numeric::ublas::permutation_matrix<float> P(A.size1());
        int singularRow = lu_factorize(A, P);
        if (singularRow != 0) {
            // TODO: This case needs to be dealt with
            throw "the system had no solution";
        }
        boost::numeric::ublas::matrix<float> result(A.size2(), B.size1());
        result.assign(B);
        lu_substitute(A, P, result);
        return result;
    }


// Solve the linear system Ax=b for the vector x.
// NOTE: This method is hard coded to work for 3x3 matrices and 3-vectors.
//       We can get superior performance this way.
    static const ufvector3 solve(ufmatrix3 &A,
                                 const ufvector3 &b) {
        boost::numeric::ublas::permutation_matrix
            <float, boost::numeric::ublas::bounded_array<float, 9> >
            P(A.size1());
        int singularRow = lu_factorize(A, P);
        if (singularRow != 0) {
            // TODO: This case needs to be dealt with
            throw "the system had no solution";
        }
        ufvector3 result(A.size2());
        result.assign(b);
        lu_substitute(A, P, result);
        return result;
    }
}

#endif
