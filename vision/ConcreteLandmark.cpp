#include "ConcreteLandmark.h"

ConcreteLandmark::ConcreteLandmark(const double _fieldX,
                                   const double _fieldY) :
  fieldX(_fieldX), fieldY(_fieldY) {
}

ConcreteLandmark::ConcreteLandmark(const ConcreteLandmark& other) :
  fieldX(other.fieldX), fieldY(other.fieldY) {

}

ConcreteLandmark::~ConcreteLandmark() {}
