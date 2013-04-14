#pragma once

#include <boost/numeric/ublas/vector.hpp>

namespace NBMath {

    typedef boost::numeric::ublas::vector<
        float, boost::numeric::ublas::bounded_array<float,3> > ufvector3;

    typedef boost::numeric::ublas::matrix<
        float,
        boost::numeric::ublas::row_major,
        boost::numeric::ublas::bounded_array<float,3> > ufrowVector3;

    typedef boost::numeric::ublas::vector<
            float, boost::numeric::ublas::bounded_array<float,4> > ufvector4;

    typedef boost::numeric::ublas::vector <float> ufvector;

}
