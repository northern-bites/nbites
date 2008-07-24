#ifndef ConcreteLandmark_h_defined__
#define ConcreteLandmark_h_defined__

#include <string>
using namespace std;

class ConcreteLandmark {
public:
  ConcreteLandmark(const double _fieldX, const double _fieldY);
  ConcreteLandmark(const ConcreteLandmark& other);
  virtual ~ConcreteLandmark();
  
  virtual const string toString() const = 0;
  
  const double getFieldX() const { return fieldX; }
  const double getFieldY() const { return fieldY; }

private:
  //  point <const double> fieldLocation;
  const double fieldX, fieldY;
};

#endif
