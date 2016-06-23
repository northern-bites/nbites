#include "CameraToRR.hpp"
#include "utils/SPLDefs.hpp"
#include "utils/body.hpp"
#include "utils/Logger.hpp"
#include "utils/basic_maths.hpp"

using namespace std;

CameraToRR::CameraToRR()
{
   camera = 0;

   for (int i = 0; i < IMAGE_COLS; i++) {
      topEndScanCoords[i] = IMAGE_ROWS;
      botEndScanCoords[i] = IMAGE_ROWS*2;
   }
}

CameraToRR::~CameraToRR()
{
}

void CameraToRR::setCamera(Camera *cam) {
   camera = cam;
}

void CameraToRR::updateAngles(UNSWSensorValues val)
{
   values = val;
}

RRCoord CameraToRR::convertToRR(const Point &p, bool isBall) const
{
   return convertToRR(p.x (), p.y (), isBall);
}

RRCoord CameraToRR::convertToRR(int16_t i, int16_t j, bool isBall) const
{
   RRCoord myloc = pose.imageToRobotRelative(i, j, isBall ? 35 : 0);
   return myloc;
}

Point CameraToRR::convertToRRXY(const Point &p) const
{
   Point myloc = pose.imageToRobotXY(p, 0);
   return myloc;
}

RANSACLine CameraToRR::convertToRRLine(const RANSACLine &l) const
{
   return RANSACLine(convertToRRXY(l.p1), convertToRRXY(l.p2));
}

Point CameraToRR::convertToImageXY(const Point &p) const
{
   Point myloc = pose.robotToImageXY(p, 0);
   return myloc;
}

// TODO: Sean make this not just use top pixel size
float CameraToRR::pixelSeparationToDistance(int pixelSeparation,
      int realSeparation) const
{
   return (FOCAL_LENGTH * realSeparation) /
      (TOP_PIXEL_SIZE * pixelSeparation);
}

float CameraToRR::ballDistanceByRadius(int radius, bool top) const
{
   const float PIXEL = (top) ? TOP_PIXEL_SIZE : BOT_PIXEL_SIZE;
   extern bool offNao;
   radius += 2;
   bool isTopCamera = true;
   float height_to_neck = 440;
   float neck_pitch = 0;
   if (!offNao) {
      if (camera->getCamera() == BOTTOM_CAMERA) {
         isTopCamera = false;
      }
      const float *angles = values.joints.angles;
      neck_pitch = angles[Joints::HeadPitch];
   }
   float height;
   if (isTopCamera) {
      height = height_to_neck + NECK_TO_TOP_CAMERA *
            -sin(neck_pitch + ANGLE_OFFSET_TOP_CAMERA);
   } else {
      height = height_to_neck + NECK_TO_BOTTOM_CAMERA *
         -sin(neck_pitch + ANGLE_OFFSET_BOTTOM_CAMERA);
   }
   height -= BALL_RADIUS;
   float distance = (FOCAL_LENGTH * BALL_RADIUS * 1.3) /
      (PIXEL * radius);

   if (distance < height) {
      return -1;
   }
   distance = sqrt(SQUARE(distance) - SQUARE(height));
   return distance;
}

bool CameraToRR::isRobotMoving() const
{
   return true;
}

void CameraToRR::findEndScanValues() {
   const int exclusionRes = Pose::EXCLUSION_RESOLUTION;
   const int16_t *topPoints = pose.getTopExclusionArray();
   const int16_t *botPoints = pose.getBotExclusionArray();
   for (int i = 0; i < TOP_IMAGE_COLS; ++i) {
      if (topPoints[(i * exclusionRes)/TOP_IMAGE_COLS] < IMAGE_ROWS) {
         topEndScanCoords[i] = topPoints[(i * exclusionRes)/TOP_IMAGE_COLS];
      } else {
         topEndScanCoords[i] = IMAGE_ROWS;
      }
   }
   for (int i = 0; i < BOT_IMAGE_COLS; ++i) {
      if (botPoints[(i * exclusionRes)/BOT_IMAGE_COLS] < IMAGE_ROWS) {
         botEndScanCoords[i] = botPoints[(i * exclusionRes)/BOT_IMAGE_COLS];
      } else {
         botEndScanCoords[i] = IMAGE_ROWS*2;
      }
   }
}
