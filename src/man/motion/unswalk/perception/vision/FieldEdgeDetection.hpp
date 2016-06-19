#pragma once

#include <vector>
#include <utility>

#include "VisionConstants.hpp"
#include "VisionDefs.hpp"
#include "CameraToRR.hpp"
#include "VisionFrame.hpp"
#include "Fovea.hpp"

#include "types/FieldEdgeInfo.hpp"
#include "types/Point.hpp"

class FieldEdgeDetection
{
   public:
      /**
       * Points to be ransaced
      **/
      std::vector<Point> edgePointsTop;
      std::vector<Point> edgePointsBot;

      /**
       * The coordinates of the top of the field in the image
       * The coordinates are given in image coordinates
       **/
      int topStartScanCoords[IMAGE_COLS];
      int botStartScanCoords[IMAGE_COLS];

      void findFieldEdges(VisionFrame &frame,
                          const Fovea &topFovea,
                          const Fovea &botFovea,
                          CameraToRR *convRR,
                          unsigned int *seed);


      /**
       * Find coordinates of points that may be at the edge
       * of the field by using the saliency scan
       * @param frame      Current vision frame
       * @param fovea      Current fovea to be searched
       **/
      void fieldEdgePoints(VisionFrame &frame,
                           const Fovea &fovea,
                           bool top);

      /**
       * Find up to two lines formed by field edge points
       * using the RANSAC algorithm
       **/
      void fieldEdgeLines(unsigned int *seed,
                          CameraToRR *convRR,
                          bool top);

      /**
       * Fills the startScanCoords array to find the coordinates
       * in the saliency scan where the field starts
       **/
      void findStartScanCoords(VisionFrame &frame,
                               const Fovea &fovea);

      explicit FieldEdgeDetection();

      std::vector<FieldEdgeInfo> fieldEdges;

   private:
      static const int consecutive_green;

      void lsRefineLine(
            RANSACLine               &line,
            const std::vector<Point> &points,
            const std::vector<bool>  &cons);


      Point adjustVertEdgePoint(
            VisionFrame &frame,
            const Fovea &fovea,
            Point p);

      /**
       * A cummulative count of green pixels occuring
       * at the top of the image
       */
      int greenTops[TOP_SALIENCY_COLS];
      int totalGreens;
};

