#pragma once

#include <stdint.h>
#include <math.h>
#include "types/RRCoord.hpp"
#include "types/RansacTypes.hpp"
#include "perception/vision/Camera.hpp"
#include "types/UNSWSensorValues.hpp"
#include "perception/vision/VisionDefs.hpp"
#include "perception/vision/CameraDefs.hpp"
#include "perception/kinematics/Pose.hpp"

#include "types/RRCoord.hpp"
#include "types/UNSWSensorValues.hpp"
#include "types/Point.hpp"

/**
 * All constant measurements are in mm or radians
 **/

class CameraToRR {
   public:
      CameraToRR();
      ~CameraToRR();

      void setCamera(Camera *camera);
      void updateAngles(UNSWSensorValues values);
      RRCoord convertToRR(int16_t i, int16_t j, bool isBall) const;
      RRCoord convertToRR(const Point &p, bool isBall) const;
      Point convertToRRXY(const Point &p) const;
      RANSACLine convertToRRLine(const RANSACLine &l) const;
      Point convertToImageXY(const Point &p) const;

      Pose pose;
      float pixelSeparationToDistance(int pixelSeparation, int realSeparation) const;
      float ballDistanceByRadius(int radius, bool top) const;
      bool isRobotMoving() const;

      /**
       * Finds the saliency scan coordinates where vertical scans
       * should be stopped to avoid considering the robots own body
       * The coordinates in the array returned are image co-ords
       **/
      void findEndScanValues();

      int topEndScanCoords[IMAGE_COLS];
      int botEndScanCoords[IMAGE_COLS];

   private:
      Camera *camera;
      UNSWSensorValues values;
};



