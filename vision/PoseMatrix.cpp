/* PoseMatrix.cc

   Matrix Class that instatiantes one 4x4 matrix. Which handles:
   - rotations in x, y, z axis
   - translations in x,y,z directions
   - transformation of a vector
   
   and that's pretty much it
 */

// class header
#include "PoseMatrix.h"

// class constructor
PoseMatrix::PoseMatrix() {
  clear();
}

// called typically at the beginning of every frame
// turns matrix into identity matrix
void PoseMatrix::clear() {
  for (int i = 0; i < 16; i++) { data_[i] = 0.0; }
  data_[0] = data_[5] = data_[10] = data_[15] = 1.0;
}

// rotates matrix in y-axis around a certain angle
// y axis on the aibo is a plane parallel to the ground
// positive is from the center of the body towards the camera 
// negative is from the center of the body towards the butt
void PoseMatrix::rotateY(double angle) {
    
  // stores locally as vector variables for easy calculation
  a1 = data_[0]; b1 = data_[1]; c1 = data_[2];  d1 = data_[3];
  a2 = data_[4]; b2 = data_[5]; c2 = data_[6];  d2 = data_[7];
  a3 = data_[8]; b3 = data_[9]; c3 = data_[10]; d3 = data_[11];

  // calculates sin/cos of angle only once for speed
  sin_angle = sin(angle);
  cos_angle = cos(angle);
   
  // does rotation
  data_[0] = a1*cos_angle - c1*sin_angle;
  data_[1] = b1;
  data_[2] = a1*sin_angle + c1*cos_angle;
  data_[3] = d1;
  data_[4] = a2*cos_angle - c2*sin_angle;
  data_[5] = b2;
  data_[6] = a2*sin_angle + c2*cos_angle;
  data_[7] = d2;
  data_[8] = a3*cos_angle - c3*sin_angle;
  data_[9] = b3;
  data_[10] = a3*sin_angle + c3*cos_angle;
  data_[11] = d3;
  data_[12] = 0;
  data_[13] = 0;
  data_[14] = 0;
  data_[15] = 1;
}

// rotates matrix in x-axis around a certain angle
// x axis on the aibo is a plane parallel to the ground
// standing behind the robot, 
// positive is to the right of the robot, negative to the left
void PoseMatrix::rotateX(double angle) {
    
  a1 = data_[0]; b1 = data_[1]; c1 = data_[2];  d1 = data_[3];
  a2 = data_[4]; b2 = data_[5]; c2 = data_[6];  d2 = data_[7];
  a3 = data_[8]; b3 = data_[9]; c3 = data_[10]; d3 = data_[11];

  sin_angle = sin(angle);
  cos_angle = cos(angle);
   
  data_[0] = a1;
  data_[1] = b1*cos_angle + c1*sin_angle;
  data_[2] = -b1*sin_angle + c1*cos_angle;
  data_[3] = d1;
  data_[4] = a2;
  data_[5] = b2*cos_angle + c2*sin_angle;
  data_[6] = -b2*sin_angle + c2*cos_angle;
  data_[7] = d2;
  data_[8] = a3;
  data_[9] = b3*cos_angle + c3*sin_angle;
  data_[10] = -b3*sin_angle + c3*cos_angle;
  data_[11] = d3;
  data_[12] = 0;
  data_[13] = 0;
  data_[14] = 0;
  data_[15] = 1;
}

// rotates matrix in z-axis around a certain angle
// z axis on the aibo is a plane parallel to the ground
// positive is from the center of the body pointing up towards t3h sky
// negative is from the center of the body heading down to the ground
void PoseMatrix::rotateZ(double angle) {
    
  a1 = data_[0]; b1 = data_[1]; c1 = data_[2];  d1 = data_[3];
  a2 = data_[4]; b2 = data_[5]; c2 = data_[6];  d2 = data_[7];
  a3 = data_[8]; b3 = data_[9]; c3 = data_[10]; d3 = data_[11];

  sin_angle = sin(angle);
  cos_angle = cos(angle);
   
  data_[0] = a1*cos_angle + b1*sin_angle;
  data_[1] = -a1*sin_angle + b1*cos_angle;
  data_[2] = c1;
  data_[3] = d1;
  data_[4] = a2*cos_angle + b2*sin_angle;
  data_[5] = -a2*sin_angle + b2*cos_angle;
  data_[6] = c2;
  data_[7] = d2;
  data_[8] = a3*cos_angle + b3*sin_angle;
  data_[9] = -a3*sin_angle + b3*cos_angle;
  data_[10] = c3;
  data_[11] = d3;
  data_[12] = 0;
  data_[13] = 0;
  data_[14] = 0;
  data_[15] = 1;
}

// translates matrix by a x,y,z value
// basically this means that it moves the x,y,z point that the matrix keeps 
// track of by a constant value
void PoseMatrix::translate(double x, double y, double z) {
    
  a1 = data_[0]; b1 = data_[1]; c1 = data_[2];  d1 = data_[3];
  a2 = data_[4]; b2 = data_[5]; c2 = data_[6];  d2 = data_[7];
  a3 = data_[8]; b3 = data_[9]; c3 = data_[10]; d3 = data_[11];

  data_[0] = a1;
  data_[1] = b1;
  data_[2] = c1;
  data_[3] = a1*x + b1*y + c1*z + d1;
  data_[4] = a2;
  data_[5] = b2;
  data_[6] = c2;
  data_[7] = a2*x + b2*y + c2*z + d2;
  data_[8] = a3;
  data_[9] = b3;
  data_[10] = c3;
  data_[11] = a3*x + b3*y + c3*z + d3;
  data_[12] = 0;
  data_[13] = 0;
  data_[14] = 0;
  data_[15] = 1;
}

// transforms a coordinate in the image coordinate frame to the body coordinate
// frame
point3 <float> PoseMatrix::transform(point3 <float> coord) {
     
  a1 = data_[0]; b1 = data_[1]; c1 = data_[2];  d1 = data_[3];
  a2 = data_[4]; b2 = data_[5]; c2 = data_[6];  d2 = data_[7];
  a3 = data_[8]; b3 = data_[9]; c3 = data_[10]; d3 = data_[11];
    
  point3 <float> transformed_coords;
    
  transformed_coords.x = a1*coord.x + b1*coord.y + c1*coord.z + d1;
  transformed_coords.y = a2*coord.x + b2*coord.y + c2*coord.z + d2;
  transformed_coords.z = a3*coord.x + b3*coord.y + c3*coord.z + d3;

  return transformed_coords;
}  
