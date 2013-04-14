#include "NBMatrixMath.h"
using namespace NBMath;
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
const boost::numeric::ublas::matrix<float>
NBMath::invert2by2(boost::numeric::ublas::matrix<float> m)
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

const boost::numeric::ublas::vector<float>
NBMath::solve(boost::numeric::ublas::matrix<float> A,
              const boost::numeric::ublas::vector<float> &b) {

    boost::numeric::ublas::vector<float> result(A.size2());
    try {
    boost::numeric::ublas::permutation_matrix<> P(A.size1());
    int singularRow = lu_factorize(A, P);
    if (singularRow != 0) {
        // TODO: This case needs to be dealt with
        std::cout << "the system had no solution!" << std::endl;
    }
    result.assign(b);
    lu_substitute(A, P, result);
    } catch (std::exception &e) {
        std::cout << "This is a shitty error and someone should track it down and fix it"
                << "but right now we need not to segfault - Mexico 2012"
                << e.what() << std::endl;
    }
    return result;
}

const boost::numeric::ublas::matrix<float>
NBMath::solve(boost::numeric::ublas::matrix<float> A,
              const boost::numeric::ublas::matrix<float> &B) {
    boost::numeric::ublas::matrix<float> result(A.size2(), B.size1());
    try {
    boost::numeric::ublas::permutation_matrix<> P(A.size1());
    int singularRow = lu_factorize(A, P);
    if (singularRow != 0) {
        // TODO: This case needs to be dealt with
        std::cout << "the system had no solution!" << std::endl;
    }

    result.assign(B);
    lu_substitute(A, P, result);
    } catch (std::exception &e) {
        std::cout << "This is a shitty error and someone should track it down and fix it"
                << "but right now we need not to segfault - Mexico 2012"
                << e.what() << std::endl;
    }
    return result;
}

// Solve the linear system Ax=b for the vector x.
// NOTE: This method is hard coded to work for 3x3 matrices and 3-vectors.
//       We can get superior performance this way.

const ufvector3 NBMath::solve(ufmatrix3 &A,
                              const ufvector3 &b) {

    ufvector3 result(A.size2());
    try {
    boost::numeric::ublas::permutation_matrix
        <unsigned int, boost::numeric::ublas::bounded_array<unsigned int, 9> >
        P(A.size1());
    int singularRow = lu_factorize(A, P);
    if (singularRow != 0) {
        // TODO: This case needs to be dealt with
        std::cout << "the system had no solution" << std::endl;
    }
    result.assign(b);
    lu_substitute(A, P, result);
    } catch (std::exception &e) {
        std::cout << "This is a shitty error and someone should track it down and fix it"
                << "but right now we need not to segfault - Mexico 2012"
                << e.what() << std::endl;
    }
    return result;
}

const float NBMath::length(ufvector4 &v) {

    return sqrtf(v(0) * v(0) + v(1) * v(1) + v(2) * v(2));
}
