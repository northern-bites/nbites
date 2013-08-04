#include "NBVector.h"
using namespace NBMath;

ufvector4 NBMath::vector4D(float x, float y, float z, float w) {
    ufvector4 p = boost::numeric::ublas::zero_vector <float> (4);
    p(0) = x;
    p(1) = y;
    p(2) = z;
    p(3) = w;
    return p;
}
