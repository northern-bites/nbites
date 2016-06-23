#pragma once

#include "perception/vision/CameraToRR.hpp"
#include "perception/vision/VisionConstants.hpp"
#include "perception/vision/VisionDefs.hpp"
#include "perception/vision/VisionFrame.hpp"
#include "perception/vision/Ransac.hpp"
#include "perception/vision/Fovea.hpp"

#include "types/FieldFeatureInfo.hpp"
#include "types/AbsCoord.hpp"
#include "utils/Histogram.hpp"

class FieldLineDetection {
   public:

      FieldLineDetection();

      // Field Line Detection
      void findFieldFeatures(
            VisionFrame &frame,
            const Fovea &botSaliency,
            const Fovea &topSaliency,
            unsigned int *seed);

      void findFoveaPoints(
            VisionFrame &frame,
            const Fovea &fovea);

      void findFieldLinesAndCircles(
            const std::vector<FieldLinePointInfo> &points,
            std::vector<FieldFeatureInfo> *features,
            std::vector< std::vector<FieldLinePointInfo> > *linePoints,
            const Fovea &fovea,
            unsigned int * seed);

      void findFieldLinesInFovea(
            const std::vector<FieldLinePointInfo> &points,
            std::vector<FieldFeatureInfo> *features,
            std::vector< std::vector<FieldLinePointInfo> > *linePoints,
            const Fovea &fovea,
            unsigned int * seed);

      void findIntersections(
            std::vector<FieldFeatureInfo> &lines,
            std::vector<FieldFeatureInfo> *features,
            const Fovea &fovea);

      bool findPreviousFeatures(
            std::vector<FieldFeatureInfo> &features,
            VisionFrame &frame,
            const Fovea &saliency,
            unsigned int *seed);

      void findFeaturesInFovea(
            VisionFrame &frame,
            const Fovea &saliency,
            unsigned int *seed,
            int type = 0);

      void searchForFeatures(
            VisionFrame &frame,
            const Fovea &saliency,
            unsigned int *seed);

      bool findPenaltySpot(
            VisionFrame &frame,
            const Fovea &saliency);

      // Helper Functions
      void reset(bool full = false);

      void circleOrientation(
            std::vector<FieldFeatureInfo> *features);

      int checkPixelColour(
            Point p,
            const Fovea &fovea);

      bool closeToEdge(
            const Fovea &fovea,
            FieldFeatureInfo f);

      bool isBadCorner(
            LineInfo l1,
            LineInfo l2);

      Point intersect(
            LineInfo l1,
            LineInfo l2);

      bool perpendicular(
            LineInfo l1,
            LineInfo l2);

      LineInfo lineToRR(
            LineInfo l);

      bool possibleT(
            LineInfo l1,
            LineInfo l2);

      float findCAngle(
            Point p,
            LineInfo l1,
            LineInfo l2);

      float findTAngle(
            Point p,
            LineInfo l);

      float findGradient(
            LineInfo l,
            Point p);
      
      bool parallelPair(
            LineInfo l1,
            LineInfo l2,
            RRCoord* r);

      bool isLineActuallyCircle(
            std::vector<FieldLinePointInfo> lpoints,
            RRCoord c,
            int radius,
            int e);
      
      bool isLineTooLong(
            const Fovea &fovea,
            LineInfo l1,
            LineInfo* l2);

      void checkLine(
            const std::vector<FieldLinePointInfo> &points,
            LineInfo *l1);

      void findParallelLines(
            std::vector<FieldFeatureInfo>* lines);

      void makeWhiteHistsBelowFieldEdge(
            VisionFrame &frame,
            const Fovea &fovea,
            Histogram<int, cNUM_COLOURS> &xhist,
            Histogram<int, cNUM_COLOURS> &yhist);


      // Variables
      std::vector<FieldFeatureInfo> fieldFeatures; // final list
      std::vector<FieldFeatureInfo> fieldLines; // current fovea list
      std::vector<boost::shared_ptr<FoveaT<hNone, eGrey> > > foveas;
      int startFieldLinePoints[80];
      const CameraToRR *convertRR;
      AbsCoord robotPos;
      std::vector<Point> landmarks; // landmarks around the field to look at
      int numCircleLandmarks;

      // Offnao debugging
      std::vector<FieldLinePointInfo>  fieldLinePoints;

      // Vision tab debugging - need to enable visionTabOutput to get these
      std::vector<std::vector<FieldLinePointInfo> > linePoints;
      std::vector<FieldLinePointInfo> circlePoints;

      // Typedefs
      typedef Histogram<int, cNUM_COLOURS> XHistogram;
      typedef Histogram<int, cNUM_COLOURS> YHistogram;
};

struct cmpPoints {
   bool operator()(const FieldLinePointInfo &p1,
                   const FieldLinePointInfo &p2) const {
         if (p1.rrp.x() == p2.rrp.x()) return (p1.rrp.y() < p2.rrp.y());
         return (p1.rrp.x() < p2.rrp.x());
      }
};
