#ifndef PoseMatrix_h_DEFINED
#define PoseMatrix_h_DEFINED
using namespace std;

#include <math.h>
#include "Structs.h"

class PoseMatrix {
 private:
  double a1,a2,a3;
  double b1,b2,b3;
  double c1,c2,c3;
  double d1,d2,d3;
  double sin_angle,cos_angle;
  double data_[16];
 public:  
  PoseMatrix();
  virtual ~PoseMatrix() {}

  // main methods
  void rotateY(double angle);
  void rotateX(double angle);
  void rotateZ(double angle);
  void translate(double x, double y, double z);
  point3 <float> transform(point3 <float> coord);

  // helper methods
  void clear();
  inline double get(int index) { return data_[index]; }
  inline double getX() { return data_[3]; }
  inline double getY() { return data_[7]; }
  inline double getZ() { return data_[11]; }

};

#endif
