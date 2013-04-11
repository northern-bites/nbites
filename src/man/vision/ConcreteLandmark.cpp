#include "ConcreteLandmark.h"

namespace man {
namespace vision {

ConcreteLandmark::ConcreteLandmark(const float& _fieldX,
                                   const float& _fieldY) :
    fieldX(_fieldX), fieldY(_fieldY)
{
}
ConcreteLandmark::ConcreteLandmark(const ConcreteLandmark& other) :
    fieldX(other.fieldX), fieldY(other.fieldY)
{
}

ConcreteLandmark::~ConcreteLandmark() {}


}
}
