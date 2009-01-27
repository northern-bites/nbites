#ifndef PoseMatrix_h_DEFINED
#define PoseMatrix_h_DEFINED

#include <math.h>
#include "Structs.h"

class PoseMatrix {
 private:
  float a1,a2,a3;
  float b1,b2,b3;
  float c1,c2,c3;
  float d1,d2,d3;
  float sin_angle,cos_angle;
  float data_[16];
 public:  
  PoseMatrix();
  virtual ~PoseMatrix() {}

  // main methods
  void rotateY(float angle);
  void rotateX(float angle);
  void rotateZ(float angle);
  void translate(float x, float y, float z);
  point3 <float> transform(point3 <float> coord);

  // helper methods
  void clear();
  inline float get(int index) { return data_[index]; }
  inline float getX() { return data_[3]; }
  inline float getY() { return data_[7]; }
  inline float getZ() { return data_[11]; }

};

#endif
