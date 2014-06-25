/**
 * @file CameraMatrixBH.cpp
 *
 * Implementation of class CameraMatrixBH.
 */

#include "CameraMatrix.h"
#include "Tools/Boundary.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Math/Geometry.h"
#include "Representations/Infrastructure/CameraInfo.h"

CameraMatrixBH::CameraMatrixBH(const Pose3DBH& pose)
: Pose3DBH(pose),
  isValid(true) {}

void CameraMatrixBH::draw() const
{
  DECLARE_DEBUG_DRAWING("representation:CameraMatrixBH:ImageBH", "drawingOnImage"); // Shows the robot coordinate system
  DECLARE_DEBUG_DRAWING("representation:CameraMatrixBH:Field", "drawingOnField"); // Shows the robot coordinate system

  COMPLEX_DRAWING("representation:CameraMatrixBH:Field",
  {
    CameraInfoBH cameraInfo; // HACK!
    Vector2BH<int> pointOnField[6];
    // calculate the projection of the four image corners to the ground
    Geometry::calculatePointOnField(0, 0, *this, cameraInfo, pointOnField[0]);
    Geometry::calculatePointOnField(cameraInfo.width, 0, *this, cameraInfo, pointOnField[1]);
    Geometry::calculatePointOnField(cameraInfo.width, cameraInfo.height, *this, cameraInfo, pointOnField[2]);
    Geometry::calculatePointOnField(0, cameraInfo.height, *this, cameraInfo, pointOnField[3]);

    // calculate a line 15 pixels below the horizon in the image
    Geometry::Line horizon = Geometry::calculateHorizon(*this, cameraInfo);
    Geometry::Line lineBelowHorizon;
    Vector2BH<> vertLineDirection(-horizon.direction.y, horizon.direction.x);
    lineBelowHorizon.direction = horizon.direction;
    lineBelowHorizon.base = horizon.base;
    lineBelowHorizon.base += vertLineDirection * 15.0f;

    // calculate the projection to the ground of the intersection points of the line parallel to the horizon and the image borders
    Vector2BH<int> beginPoint;
    Vector2BH<int> endPoint;
    if(Geometry::getIntersectionPointsOfLineAndRectangle(
      Vector2BH<int>(0, 0), Vector2BH<int>(cameraInfo.width - 1, cameraInfo.height - 1), lineBelowHorizon, beginPoint, endPoint))
    {
      Geometry::calculatePointOnField(beginPoint.x, beginPoint.y, *this, cameraInfo, pointOnField[4]);
      Geometry::calculatePointOnField(endPoint.x, endPoint.y, *this, cameraInfo, pointOnField[5]);
      LINE("representation:CameraMatrixBH:Field", pointOnField[4].x, pointOnField[4].y, pointOnField[5].x, pointOnField[5].y, 30, Drawings::ps_solid, ColorClasses::yellow);
    }

    // determine the boundary of all the points that were projected to the ground
    Boundary<int> boundary(-10000, +10000);
    if(pointOnField[0].x != 0 || pointOnField[0].y != 0) {boundary.add(pointOnField[0]); CIRCLE("representation:CameraMatrixBH:Field", pointOnField[0].x, pointOnField[0].y, 100, 50, Drawings::ps_solid, ColorClasses::white, Drawings::bs_null, ColorClasses::white); }
    if(pointOnField[1].x != 0 || pointOnField[1].y != 0) {boundary.add(pointOnField[1]); CIRCLE("representation:CameraMatrixBH:Field", pointOnField[1].x, pointOnField[1].y, 100, 50, Drawings::ps_solid, ColorClasses::white, Drawings::bs_null, ColorClasses::white); }
    if(pointOnField[2].x != 0 || pointOnField[2].y != 0) {boundary.add(pointOnField[2]); CIRCLE("representation:CameraMatrixBH:Field", pointOnField[2].x, pointOnField[2].y, 100, 50, Drawings::ps_solid, ColorClasses::white, Drawings::bs_null, ColorClasses::white); }
    if(pointOnField[3].x != 0 || pointOnField[3].y != 0) {boundary.add(pointOnField[3]); CIRCLE("representation:CameraMatrixBH:Field", pointOnField[3].x, pointOnField[3].y, 100, 50, Drawings::ps_solid, ColorClasses::white, Drawings::bs_null, ColorClasses::white); }
    if(pointOnField[4].x != 0 || pointOnField[4].y != 0) {boundary.add(pointOnField[4]); CIRCLE("representation:CameraMatrixBH:Field", pointOnField[4].x, pointOnField[4].y, 100, 50, Drawings::ps_solid, ColorClasses::yellow, Drawings::bs_null, ColorClasses::yellow); }
    if(pointOnField[5].x != 0 || pointOnField[5].y != 0) {boundary.add(pointOnField[5]); CIRCLE("representation:CameraMatrixBH:Field", pointOnField[5].x, pointOnField[5].y, 100, 50, Drawings::ps_solid, ColorClasses::yellow, Drawings::bs_null, ColorClasses::yellow); }

    LINE("representation:CameraMatrixBH:Field", boundary.x.min, boundary.y.min, boundary.x.max, boundary.y.min, 30, Drawings::ps_solid, ColorRGBA(255, 0, 0));
    LINE("representation:CameraMatrixBH:Field", boundary.x.max, boundary.y.min, boundary.x.max, boundary.y.max, 30, Drawings::ps_solid, ColorClasses::yellow);
    LINE("representation:CameraMatrixBH:Field", boundary.x.max, boundary.y.max, boundary.x.min, boundary.y.max, 30, Drawings::ps_solid, ColorClasses::blue);
    LINE("representation:CameraMatrixBH:Field", boundary.x.min, boundary.y.max, boundary.x.min, boundary.y.min, 30, Drawings::ps_solid, ColorClasses::white);

    // fill the bounding rectangle with coordinate system lines (and reproject it to the image)
    int spacing = 100;
    for(int xx = boundary.x.min - boundary.x.min % spacing + spacing ; xx <= boundary.x.max; xx += spacing)
    {
      LINE("representation:CameraMatrixBH:Field", xx, boundary.y.min, xx, boundary.y.max, 5, Drawings::ps_solid, ColorClasses::white);
    }
    for(int yy = boundary.y.min - boundary.y.min % spacing + spacing ; yy <= boundary.y.max; yy += spacing)
    {
      LINE("representation:CameraMatrixBH:Field", boundary.x.min, yy, boundary.x.max, yy, 5, Drawings::ps_solid, ColorClasses::white);
    }
  });// end complex drawing

  COMPLEX_DRAWING("representation:CameraMatrixBH:ImageBH",
  {
    CameraInfoBH cameraInfo; // HACK!
    Vector2BH<int> pointOnField[6];
    // calculate the projection of the four image corners to the ground
    Geometry::calculatePointOnField(0, 0, *this, cameraInfo, pointOnField[0]);
    Geometry::calculatePointOnField(cameraInfo.width, 0, *this, cameraInfo, pointOnField[1]);
    Geometry::calculatePointOnField(cameraInfo.width, cameraInfo.height, *this, cameraInfo, pointOnField[2]);
    Geometry::calculatePointOnField(0, cameraInfo.height, *this, cameraInfo, pointOnField[3]);

    // calculate a line 15 pixels below the horizon in the image
    Geometry::Line horizon = Geometry::calculateHorizon(*this, cameraInfo);
    Geometry::Line lineBelowHorizon;
    Vector2BH<> vertLineDirection(-horizon.direction.y, horizon.direction.x);
    lineBelowHorizon.direction = horizon.direction;
    lineBelowHorizon.base = horizon.base;
    lineBelowHorizon.base += vertLineDirection * 15.0f;

    // calculate the projection to the ground of the intersection points of the line parallel to the horizon and the image borders
    Vector2BH<int> beginPoint;
    Vector2BH<int> endPoint;
    if(Geometry::getIntersectionPointsOfLineAndRectangle(
      Vector2BH<int>(0, 0), Vector2BH<int>(cameraInfo.width - 1, cameraInfo.height - 1), lineBelowHorizon, beginPoint, endPoint))
    {
      LINE("representation:CameraMatrixBH:ImageBH", beginPoint.x, beginPoint.y, endPoint.x, endPoint.y, 3, Drawings::ps_dash, ColorClasses::white);
      Geometry::calculatePointOnField(beginPoint.x, beginPoint.y, *this, cameraInfo, pointOnField[4]);
      Geometry::calculatePointOnField(endPoint.x, endPoint.y, *this, cameraInfo, pointOnField[5]);
    }

    // determine the boundary of all the points that were projected to the ground
    Boundary<int> boundary(-10000, +10000);
    if(pointOnField[0].x != 0 || pointOnField[0].y != 0) {boundary.add(pointOnField[0]); }
    if(pointOnField[1].x != 0 || pointOnField[1].y != 0) {boundary.add(pointOnField[1]); }
    if(pointOnField[2].x != 0 || pointOnField[2].y != 0) {boundary.add(pointOnField[2]); }
    if(pointOnField[3].x != 0 || pointOnField[3].y != 0) {boundary.add(pointOnField[3]); }
    if(pointOnField[4].x != 0 || pointOnField[4].y != 0) {boundary.add(pointOnField[4]); }
    if(pointOnField[5].x != 0 || pointOnField[5].y != 0) {boundary.add(pointOnField[5]); }

    // fill the bounding rectangle with coordinate system lines (and reproject it to the image)
    int spacing = 100;
    for(int xx = boundary.x.min - boundary.x.min % spacing + spacing ; xx <= boundary.x.max; xx += spacing)
    {
      Geometry::calculatePointInImage(Vector3BH<>((float) xx, (float) boundary.y.min, 0), *this, cameraInfo, beginPoint);
      Geometry::calculatePointInImage(Vector3BH<>((float) xx, (float) boundary.y.max, 0), *this, cameraInfo, endPoint);
      int lineWidth = 0;
      if(xx == 0) lineWidth = 3;
      LINE("representation:CameraMatrixBH:ImageBH",
           beginPoint.x, beginPoint.y,
           endPoint.x, endPoint.y,
           lineWidth, Drawings::ps_solid, ColorClasses::white);
    }
    for(int yy = boundary.y.min - boundary.y.min % spacing + spacing ; yy <= boundary.y.max; yy += spacing)
    {
      Geometry::calculatePointInImage(Vector3BH<>((float) boundary.x.min, (float) yy, 0), *this, cameraInfo, beginPoint);
      Geometry::calculatePointInImage(Vector3BH<>((float) boundary.x.max, (float) yy, 0), *this, cameraInfo, endPoint);
      int lineWidth = 0;
      if(yy == 0) lineWidth = 3;
      LINE("representation:CameraMatrixBH:ImageBH",
           beginPoint.x, beginPoint.y,
           endPoint.x, endPoint.y,
           lineWidth, Drawings::ps_solid, ColorClasses::white);
    }
  });// end complex drawing
}

void RobotCameraMatrixBH::draw() const
{
  DECLARE_DEBUG_DRAWING("representation:RobotCameraMatrixBH:ImageBH", "drawingOnImage"); // Shows the robot coordinate system
  DECLARE_DEBUG_DRAWING("representation:RobotCameraMatrixBH:Field", "drawingOnField"); // Shows the robot coordinate system

  COMPLEX_DRAWING("representation:RobotCameraMatrixBH:Field",
  {
    CameraInfoBH cameraInfo; // HACK!
    Vector2BH<int> pointOnField[6];
    // calculate the projection of the four image corners to the ground
    Geometry::calculatePointOnField(0, 0, *this, cameraInfo, pointOnField[0]);
    Geometry::calculatePointOnField(cameraInfo.width, 0, *this, cameraInfo, pointOnField[1]);
    Geometry::calculatePointOnField(cameraInfo.width, cameraInfo.height, *this, cameraInfo, pointOnField[2]);
    Geometry::calculatePointOnField(0, cameraInfo.height, *this, cameraInfo, pointOnField[3]);

    // calculate a line 15 pixels below the horizon in the image
    Geometry::Line horizon = Geometry::calculateHorizon(*this, cameraInfo);
    Geometry::Line lineBelowHorizon;
    Vector2BH<> vertLineDirection(-horizon.direction.y, horizon.direction.x);
    lineBelowHorizon.direction = horizon.direction;
    lineBelowHorizon.base = horizon.base;
    lineBelowHorizon.base += vertLineDirection * 15.0;

    // calculate the projection to the ground of the intersection points of the line parallel to the horizon and the image borders
    Vector2BH<int> beginPoint;
    Vector2BH<int> endPoint;
    if(Geometry::getIntersectionPointsOfLineAndRectangle(Vector2BH<int>(0, 0),
                                                         Vector2BH<int>(cameraInfo.width - 1,
                                                                      cameraInfo.height - 1),
                                                         lineBelowHorizon, beginPoint, endPoint))
    {
      Geometry::calculatePointOnField(beginPoint.x, beginPoint.y, *this, cameraInfo, pointOnField[4]);
      Geometry::calculatePointOnField(endPoint.x, endPoint.y, *this, cameraInfo, pointOnField[5]);
      LINE("representation:CameraMatrixBH:Field", pointOnField[4].x, pointOnField[4].y, pointOnField[5].x, pointOnField[5].y, 30, Drawings::ps_solid, ColorClasses::yellow);
    }

    // determine the boundary of all the points that were projected to the ground
    Boundary<int> boundary(-10000, +10000);
    if(pointOnField[0].x != 0 || pointOnField[0].y != 0) {boundary.add(pointOnField[0]); CIRCLE("representation:RobotCameraMatrixBH:Field", pointOnField[0].x, pointOnField[0].y, 100, 50, Drawings::ps_solid, ColorClasses::white, Drawings::bs_null, ColorClasses::white); }
    if(pointOnField[1].x != 0 || pointOnField[1].y != 0) {boundary.add(pointOnField[1]); CIRCLE("representation:RobotCameraMatrixBH:Field", pointOnField[1].x, pointOnField[1].y, 100, 50, Drawings::ps_solid, ColorClasses::white, Drawings::bs_null, ColorClasses::white); }
    if(pointOnField[2].x != 0 || pointOnField[2].y != 0) {boundary.add(pointOnField[2]); CIRCLE("representation:RobotCameraMatrixBH:Field", pointOnField[2].x, pointOnField[2].y, 100, 50, Drawings::ps_solid, ColorClasses::white, Drawings::bs_null, ColorClasses::white); }
    if(pointOnField[3].x != 0 || pointOnField[3].y != 0) {boundary.add(pointOnField[3]); CIRCLE("representation:RobotCameraMatrixBH:Field", pointOnField[3].x, pointOnField[3].y, 100, 50, Drawings::ps_solid, ColorClasses::white, Drawings::bs_null, ColorClasses::white); }
    if(pointOnField[4].x != 0 || pointOnField[4].y != 0) {boundary.add(pointOnField[4]); CIRCLE("representation:RobotCameraMatrixBH:Field", pointOnField[4].x, pointOnField[4].y, 100, 50, Drawings::ps_solid, ColorClasses::yellow, Drawings::bs_null, ColorClasses::yellow); }
    if(pointOnField[5].x != 0 || pointOnField[5].y != 0) {boundary.add(pointOnField[5]); CIRCLE("representation:RobotCameraMatrixBH:Field", pointOnField[5].x, pointOnField[5].y, 100, 50, Drawings::ps_solid, ColorClasses::yellow, Drawings::bs_null, ColorClasses::yellow); }

    LINE("representation:RobotCameraMatrixBH:Field", boundary.x.min, boundary.y.min, boundary.x.max, boundary.y.min, 30, Drawings::ps_solid, ColorRGBA(255, 0, 0));
    LINE("representation:RobotCameraMatrixBH:Field", boundary.x.max, boundary.y.min, boundary.x.max, boundary.y.max, 30, Drawings::ps_solid, ColorClasses::yellow);
    LINE("representation:RobotCameraMatrixBH:Field", boundary.x.max, boundary.y.max, boundary.x.min, boundary.y.max, 30, Drawings::ps_solid, ColorClasses::blue);
    LINE("representation:RobotCameraMatrixBH:Field", boundary.x.min, boundary.y.max, boundary.x.min, boundary.y.min, 30, Drawings::ps_solid, ColorClasses::white);

    // fill the bounding rectangle with coordinate system lines (and reproject it to the image)
    int spacing = 100;
    for(int xx = boundary.x.min - boundary.x.min % spacing + spacing ; xx <= boundary.x.max; xx += spacing)
    {
      LINE("representation:RobotCameraMatrixBH:Field", xx, boundary.y.min, xx, boundary.y.max, 5, Drawings::ps_solid, ColorClasses::white);
    }
    for(int yy = boundary.y.min - boundary.y.min % spacing + spacing ; yy <= boundary.y.max; yy += spacing)
    {
      LINE("representation:RobotCameraMatrixBH:Field", boundary.x.min, yy, boundary.x.max, yy, 5, Drawings::ps_solid, ColorClasses::white);
    }
  });// end complex drawing

  COMPLEX_DRAWING("representation:RobotCameraMatrixBH:ImageBH",
  {
    CameraInfoBH cameraInfo; // HACK!
    Vector2BH<int> pointOnField[6];
    // calculate the projection of the four image corners to the ground
    Geometry::calculatePointOnField(0, 0, *this, cameraInfo, pointOnField[0]);
    Geometry::calculatePointOnField(cameraInfo.width, 0, *this, cameraInfo, pointOnField[1]);
    Geometry::calculatePointOnField(cameraInfo.width, cameraInfo.height, *this, cameraInfo, pointOnField[2]);
    Geometry::calculatePointOnField(0, cameraInfo.height, *this, cameraInfo, pointOnField[3]);

    // calculate a line 15 pixels below the horizon in the image
    Geometry::Line horizon = Geometry::calculateHorizon(*this, cameraInfo);
    Geometry::Line lineBelowHorizon;
    Vector2BH<> vertLineDirection(-horizon.direction.y, horizon.direction.x);
    lineBelowHorizon.direction = horizon.direction;
    lineBelowHorizon.base = horizon.base;
    lineBelowHorizon.base += vertLineDirection * 15.0;

    // calculate the projection to the ground of the intersection points of the line parallel to the horizon and the image borders
    Vector2BH<int> beginPoint;
    Vector2BH<int> endPoint;
    if(Geometry::getIntersectionPointsOfLineAndRectangle(Vector2BH<int>(0, 0),
                                                         Vector2BH<int>(cameraInfo.width - 1,
                                                                      cameraInfo.height - 1),
                                                         lineBelowHorizon, beginPoint, endPoint))
    {
      LINE("representation:RobotCameraMatrixBH:ImageBH", beginPoint.x, beginPoint.y, endPoint.x, endPoint.y, 3, Drawings::ps_dash, ColorClasses::white);
      Geometry::calculatePointOnField(beginPoint.x, beginPoint.y, *this, cameraInfo, pointOnField[4]);
      Geometry::calculatePointOnField(endPoint.x, endPoint.y, *this, cameraInfo, pointOnField[5]);
    }

    // determine the boundary of all the points that were projected to the ground
    Boundary<int> boundary(-10000, +10000);
    if(pointOnField[0].x != 0 || pointOnField[0].y != 0) {boundary.add(pointOnField[0]); }
    if(pointOnField[1].x != 0 || pointOnField[1].y != 0) {boundary.add(pointOnField[1]); }
    if(pointOnField[2].x != 0 || pointOnField[2].y != 0) {boundary.add(pointOnField[2]); }
    if(pointOnField[3].x != 0 || pointOnField[3].y != 0) {boundary.add(pointOnField[3]); }
    if(pointOnField[4].x != 0 || pointOnField[4].y != 0) {boundary.add(pointOnField[4]); }
    if(pointOnField[5].x != 0 || pointOnField[5].y != 0) {boundary.add(pointOnField[5]); }

    // fill the bounding rectangle with coordinate system lines (and reproject it to the image)
    int spacing = 100;
    for(int xx = boundary.x.min - boundary.x.min % spacing + spacing ; xx <= boundary.x.max; xx += spacing)
    {
      Geometry::calculatePointInImage(Vector3BH<>((float) xx, (float) boundary.y.min, 0), *this, CameraInfoBH(), beginPoint);
      Geometry::calculatePointInImage(Vector3BH<>((float) xx, (float) boundary.y.max, 0), *this, CameraInfoBH(), endPoint);
      int lineWidth = 0;
      if(xx == 0) lineWidth = 3;
      LINE("representation:RobotCameraMatrixBH:ImageBH",
           beginPoint.x, beginPoint.y,
           endPoint.x, endPoint.y,
           lineWidth, Drawings::ps_solid, ColorClasses::yellow);
    }
    for(int yy = boundary.y.min - boundary.y.min % spacing + spacing ; yy <= boundary.y.max; yy += spacing)
    {
      Geometry::calculatePointInImage(Vector3BH<>((float) boundary.x.min, (float) yy, 0), *this, CameraInfoBH(), beginPoint);
      Geometry::calculatePointInImage(Vector3BH<>((float) boundary.x.max, (float) yy, 0), *this, CameraInfoBH(), endPoint);
      int lineWidth = 0;
      if(yy == 0) lineWidth = 3;
      LINE("representation:RobotCameraMatrixBH:ImageBH",
           beginPoint.x, beginPoint.y,
           endPoint.x, endPoint.y,
           lineWidth, Drawings::ps_solid, ColorClasses::yellow);
    }
  });// end complex drawing
}

RobotCameraMatrixBH::RobotCameraMatrixBH(const RobotDimensionsBH& robotDimensions, const float headYaw, const float headPitch, const CameraCalibrationBH& cameraCalibration, bool upperCamera)
{
  computeRobotCameraMatrix(robotDimensions, headYaw, headPitch, cameraCalibration, upperCamera);
}

void RobotCameraMatrixBH::computeRobotCameraMatrix(const RobotDimensionsBH& robotDimensions, const float headYaw, const float headPitch, const CameraCalibrationBH& cameraCalibration, bool upperCamera)
{
  *this = RobotCameraMatrixBH();

  translate(0., 0., robotDimensions.zLegJoint1ToHeadPan);
  rotateZ(headYaw);
  rotateY(-headPitch);
  if(upperCamera)
  {
    translate(robotDimensions.xHeadTiltToUpperCamera, 0.f, robotDimensions.zHeadTiltToUpperCamera);
    rotateY(robotDimensions.headTiltToUpperCameraTilt + cameraCalibration.upperCameraTiltCorrection);
    rotateX(cameraCalibration.upperCameraRollCorrection);
    rotateZ(cameraCalibration.upperCameraPanCorrection);
  }
  else
  {
    translate(robotDimensions.xHeadTiltToCamera, 0.f, robotDimensions.zHeadTiltToCamera);
    rotateY(robotDimensions.headTiltToCameraTilt + cameraCalibration.cameraTiltCorrection);
    rotateX(cameraCalibration.cameraRollCorrection);
    rotateZ(cameraCalibration.cameraPanCorrection);
  }
}

CameraMatrixBH::CameraMatrixBH(const Pose3DBH& torsoMatrix, const Pose3DBH& robotCameraMatrix, const CameraCalibrationBH& cameraCalibration)
{
  computeCameraMatrix(torsoMatrix, robotCameraMatrix, cameraCalibration);
}

void CameraMatrixBH::computeCameraMatrix(const Pose3DBH& torsoMatrix, const Pose3DBH& robotCameraMatrix, const CameraCalibrationBH& cameraCalibration)
{
  (Pose3DBH&)*this = torsoMatrix;
  translate(cameraCalibration.bodyTranslationCorrection);
  rotateY(cameraCalibration.bodyTiltCorrection);
  rotateX(cameraCalibration.bodyRollCorrection);
  conc(robotCameraMatrix);
}
