#include "ConcreteLandmark.h"

ConcreteLandmark::ConcreteLandmark(const float& _fieldX,
                                   const float& _fieldY) :
    fieldX(_fieldX), fieldY(_fieldY), fieldAngle(0)
{
}


ConcreteLandmark::ConcreteLandmark(const float& _fieldX,
                                   const float& _fieldY,
                                   const float& _fieldAngle) :
    fieldX(_fieldX), fieldY(_fieldY), fieldAngle(_fieldAngle)
{
}

ConcreteLandmark::ConcreteLandmark(const ConcreteLandmark& other) :
    fieldX(other.fieldX), fieldY(other.fieldY), fieldAngle(other.fieldAngle)
{
}

ConcreteLandmark::~ConcreteLandmark() {}
