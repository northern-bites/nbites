
#ifndef _Pose_h_DEFINED
#define _Pose_h_DEFINED

#include <math.h>
#include <vector>
#include "VisionDef.h"
#include "PoseMatrix.h"
#include "matrix.h"
#include <stdio.h>
#include <string.h>
using namespace std;

// CONSTANTS

struct mDHParam{
  float xRot; //xRot
  float xTrans; //xTrans
  float zRot; //zRot
  float zTrans; //zTrans

};

// Various Body Constants ALL IN MILLIMETERS

static const float NECK_LENGTH = 80.0f;
static const float CAMERA_YAW_Y_LENGTH = 81.06f;
static const float CAMERA_YAW_Z_LENGTH = 14.6f;
static const float NECK_BASE_TO_CENTER_Y = 67.5f;
static const float NECK_BASE_TO_CENTER_Z = 19.5f;
static const float SHOULDER_TO_ELBOW_LENGTH = 69.5f;
static const float ELBOW_TO_ELBOW_X = 4.7f;
static const float ELBOW_TO_ELBOW_Y = -9.0f;
static const float REAR_LEG_LENGTH = 76.5f;// 79.4;--UTexas value
static const float REAR_LEG_TO_CENTER_BODY_Y = 65.0f;
static const float REAR_LEG_TO_CENTER_BODY_X = 62.5f;
static const float NECK_TILT2_TO_CAMERA_Y = 81.0f;
static const float NECK_TILT2_TO_CAMERA_Z = -14.6f;
static const float NECK_TILT_TO_TILT2 = 80.0f;
static const float SHOULDER_TO_NECK_TILT_Y = 2.5f;
static const float SHOULDER_TO_NECK_TILT_Z = 19.5f;
// Horizon-Related Constants
static const float MM_TO_PIX_X_PLANE = 0.0171014f;
static const float MM_TO_PIX_Y_PLANE = 0.0171195f;
static const float HORIZON_LEFT_X_MM = -1.76999f;
static const float HORIZON_RIGHT_X_MM = 1.76999f;
static const float FOCAL_LENGTH_PIX = 192.0f;
static const float FOCAL_LENGTH_MM = 3.27f;
static const float IMAGE_CENTER_X = 103.5f;
static const float IMAGE_CENTER_Y = 79.5f;
static const float HALF_FOV_X_MM = 1.77f;
static const float HALF_FOV_Y_MM = 1.361f;
static const float IMAGE_MM_WIDTH = 2.0f*tan(FOV_X/2.0f)*FOCAL_LENGTH_MM;
static const float IMAGE_MM_HEIGHT = 2.0f*tan(FOV_Y/2.0f)*FOCAL_LENGTH_MM;
// e.g. 3 mm * mm_to_pix = 176 pixels
static const float MM_TO_PIX_X = IMAGE_WIDTH/IMAGE_MM_WIDTH;
static const float MM_TO_PIX_Y = IMAGE_HEIGHT/IMAGE_MM_HEIGHT;

// 3d origin constant
static const point3 <float> ZERO_COORD(0.0f,0.0f,0.0f);
// a null estimate struct. zero dist,bearing,elevation (check dist always), x, y
static const estimate NULL_ESTIMATE = {0.0f,0.0f,0.0f,0.0f,0.0f};
// defines the left and right horizontal points in mm and in xyz space
static const point3 <float> HORIZON_LEFT_3D ( HORIZON_LEFT_X_MM, FOCAL_LENGTH_MM, 0.0);
static const point3 <float> HORIZON_RIGHT_3D (HORIZON_RIGHT_X_MM, FOCAL_LENGTH_MM, 0.0 );
static const float LINE_HEIGHT = 0.0f;

// Pose class. Constructor in Pose.cc
class Pose {
 public:
  //Axis constants
  enum Axis{
    X_AXIS = 0,
    Y_AXIS,
    Z_AXIS,
    W_AXIS
  };

  enum mDHNames {
    ALPHA = 0,
    L,
    THETA,
    D
  };

 public:
  Pose();
  virtual ~Pose();

  // master method
  virtual void transform();

  // vision system called methods for distance/angle calculations
  virtual const estimate pixEstimate(const int pixel_x,const int pixel_y,
				     const float object_height);
  virtual const estimate bodyEstimate(const int x, const int y,
				      const float obj_dist);

  // GETTERS
  
  virtual const int getHorizonY(const int x) const; // in Pose.cc
  virtual const int getHorizonX(const int y) const; // in Pose.cc
  virtual const point <int> getLeftHorizon() const { return horizon_left_2d; }
  virtual const point <int> getRightHorizon() const { return horizon_right_2d; }
  virtual const int getLeftHorizonY() const { return horizon_left_2d.y; }
  virtual const int getRightHorizonY() const { return horizon_right_2d.y; }
  virtual const float getHorizonSlope() const { return horizon_slope; }
  virtual const float getPerpenSlope() const { return perpen_slope; }
  //const float getPan() const { return pan_angle;}

  // SETTERS
  void setHorizonLeft(int x, int y) {
    horizon_left_2d.x = x;
    horizon_left_2d.y = y;
  }
  void setHorizonRight(int x, int y) {
    horizon_right_2d.x = x;
    horizon_right_2d.y = y;
  }
  void setHorizonSlope(float _s) { horizon_slope = _s; }
  void setPerpenSlope(float _p) { perpen_slope = _p; }


 protected: // helper methods potentially needed by deriving classes

  //helper methods for dealing with matrices
  Matrix * origin4D();
  Matrix * identity4D();
  Matrix * point3D(float x, float y, float z);
  Matrix * point4D(float x, float y, float z, float w = 1.0);
  Matrix * rotation4D(int axis, float rotation );
  Matrix * translation4D(float dx, float dy, float dz );

  // once-per-frame calculation methods
  point3 <float> calcFocalPointInBodyFrame();
  float calcBodyCenterHeight();
  void calcImageHorizonLine(Matrix * camera_to_world);
  Matrix * intersectLineWithXYPlane(vector<Matrix *> * line);

  //general methods
  Matrix *calcChainTransforms(const int id, const vector<float>* chainAngles);

  // helper methods
  void printMatrix(PoseMatrix matrix);
  float get3dDist(point3 <float> coord1, point3 <float> coord2);
  float get2dDist(point <int> coord1, point <int> coord2);
  float get2dDist(float x1, float y1, float x2, float y2);
  float getHypotenuse(float x, float y);

 protected:
  // PoseMatrix instances
  Matrix * camera_to_world;
  PoseMatrix image_matrix;
  PoseMatrix left_leg_matrix, right_leg_matrix;

  // local joint angle variables
  float body_roll_angle, body_tilt_angle;
  float pan_angle, yaw_angle, neck_angle;
  float left_rear_shoulder_angle, left_rear_hip_angle, left_rear_knee_angle;
  float right_rear_shoulder_angle, right_rear_hip_angle, right_rear_knee_angle;


  //vector to hold mDH params
  vector <vector < Matrix * > * > baseTransforms;
  vector <vector < const mDHParam* > * > mDHParams;
  vector <vector < Matrix * > * > endTransforms;

  vector<Matrix * > cameraEdgePoints;

  // local holders of image width/height
  //int IMAGE_WIDTH, IMAGE_HEIGHT;

  // holds 3d position of focal point in body frame
  point3 <float> focal_point_body_frame;

  // holds body center height from ground, in mm
  float body_center_height;

  // horizon left / right x,y points in the image plane
  point <int> horizon_left_2d, horizon_right_2d;

  // slope of horizon line + slope of line perpendicular to it
  float horizon_slope, perpen_slope;
};

#endif // Pose_h_DEFINED
