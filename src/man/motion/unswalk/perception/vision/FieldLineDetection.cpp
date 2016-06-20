#include "FieldLineDetection.hpp"

#include "utils/Logger.hpp"
#include "utils/LeastSquaresLine.hpp"
#include "utils/BresenhamPtr.hpp"
#include "utils/Timer.hpp"
#include "utils/SPLDefs.hpp"

#include <cmath>

using namespace std;

//#define visionTabOutput

#define EDGE_THRESHOLD                     (250*250)
#define FOVEA_EDGE_THRESHOLD               (200*200)
#define ANGLE_THRESHOLD DEG2RAD            (35)
#define PARALLEL_LINE_THRESHOLD DEG2RAD    (10)
#define PARALLEL_LINE_DISTANCE_THRESHOLD   (200)
#define MIN_PARALLEL_LENGTH                (500*500)
#define PERPENDICULAR_THRESHOLD DEG2RAD    (10)
#define LINE_WIDTH_THRESHOLD               (200*200)
#define FOVEA_LINE_WIDTH_THRESHOLD         (500*500)
#define LINE_MIN_THRESHOLD                 (30*30)
#define EXCLUSION_LENGTH                   (6)
#define MIN_LINE_LENGTH                    (400*400)
#define SHORT_LINE_LENGTH                  (300*300)
#define MIN_POINT_DISTANCE                 (120*120)
#define PENALTY_SPOT_MIN                   (50*50)
#define PENALTY_SPOT_MAX                   (250*250)

#define BOTH    0
#define LINES   1
#define CIRCLES 2

FieldLineDetection::FieldLineDetection() {

   // Setup landmarks

   // Centre circle
   landmarks.push_back(Point(-750, 0));   // Centre Circle
   landmarks.push_back(Point(750, 0));    // Centre Circle
   numCircleLandmarks = 2;

   // Our half
   landmarks.push_back(Point(-4500, 1100));     // Goalbox T-int
   landmarks.push_back(Point(-4500, -1100));    // Goalbox T-int
   landmarks.push_back(Point(-3900, 1100));     // Goalbox corner
   landmarks.push_back(Point(-3900, -1100));    // Goalbox corner
   landmarks.push_back(Point(-4500, 2000));     // Field corner
   landmarks.push_back(Point(-4500, -2000));    // Field corner

   // Their half
   landmarks.push_back(Point(4500, 1100));      // Goalbox T-int
   landmarks.push_back(Point(4500, -1100));     // Goalbox T-int
   landmarks.push_back(Point(3900, 1100));      // Goalbox corner
   landmarks.push_back(Point(3900, -1100));     // Goalbox corner
   landmarks.push_back(Point(4500, 2000));      // Field corner
   landmarks.push_back(Point(4500, -2000));     // Field corner

   // Halfway
   landmarks.push_back(Point(0, 2000));         // Halfway T-int
   landmarks.push_back(Point(0, -2000));        // Halfway T-int

}

void FieldLineDetection::findFieldFeatures(
                        VisionFrame &frame,
                        const Fovea &topFovea,
                        const Fovea &botFovea,
                        unsigned int *seed) {

   std::vector<FieldFeatureInfo> oldFeatures = fieldFeatures;

   // Initialise camera + reset variables
   reset(true);
   convertRR = &frame.cameraToRR;

/*
   // Try to find the same feature as last time
   if (findPreviousFeatures(oldFeatures, frame, fovea, seed)) return;
   reset();
*/

   Timer t;

   // Find Features in bottom fovea
   t.restart();

   findFeaturesInFovea(frame, botFovea, seed);
   fieldFeatures.insert(fieldFeatures.end(),
                        fieldLines.begin(),
                        fieldLines.end());
   llog(VERBOSE) << "Field Features in Bottom Fovea took " << t.elapsed_us();
   llog(VERBOSE) << " us" << endl;

   // Look for far away features - determine foveas, then search each one
   t.restart();
   reset(false);
   searchForFeatures(frame, topFovea, seed);
   llog(VERBOSE) << "Field Features in Foveas took " << t.elapsed_us();
   llog(VERBOSE) << " us" << endl;

   // Tally features so far
   int numCircles = 0;
   int numLines = 0;
   int numStrongFeatures = 0;
   for (uint16_t i = 0; i < fieldLines.size(); i++) {
      FieldFeatureInfo::Type type = fieldFeatures[i].type;
      if (type == FieldFeatureInfo::fCorner) {
         ++numStrongFeatures;
      } else if (type == FieldFeatureInfo::fTJunction) {
         ++numStrongFeatures;
      } else if (type == FieldFeatureInfo::fCentreCircle) {
         ++numCircles;
         ++numStrongFeatures;
      } else if (type == FieldFeatureInfo::fLine) {
         ++numLines;
      }
   }

   // Find Features in top fovea
   if (numStrongFeatures < 1) {
      t.restart();
      reset(false);
      findFeaturesInFovea(frame, topFovea, seed);
      fieldFeatures.insert(fieldFeatures.end(),
                           fieldLines.begin(),
                           fieldLines.end());
      llog(VERBOSE) << "Field Features in Top Fovea took " << t.elapsed_us();
      llog(VERBOSE) << " us" << endl;
   }

   // Find Penalty Spot
   t.restart();
   if (!findPenaltySpot(frame, botFovea)) {
      findPenaltySpot(frame, topFovea);
   }
   llog(VERBOSE) << "Penalty Spot in Bottom Fovea took " << t.elapsed_us();
   llog(VERBOSE) << " us" << endl;

   // Update circle and line
   numCircles = 0;
   numLines = 0;
   for (uint16_t i = 0; i < fieldLines.size(); i++) {
      FieldFeatureInfo::Type type = fieldFeatures[i].type;
      if (type == FieldFeatureInfo::fCentreCircle) {
         ++numCircles;
      } else if (type == FieldFeatureInfo::fLine) {
         ++numLines;
      }
   }
   if (numCircles == 1 and numLines == 1) {
      circleOrientation(&fieldFeatures);
   }

}

void FieldLineDetection::reset(bool full) {
   fieldLinePoints.clear();
   fieldLines.clear();
   linePoints.clear();
   circlePoints.clear();
   if (full) {
      fieldFeatures.clear();
      foveas.clear();
   }
}

bool FieldLineDetection::findPreviousFeatures(
                        std::vector<FieldFeatureInfo> &features,
                        VisionFrame &frame,
                        const Fovea &fovea,
                        unsigned int *seed) {
   Point old(-1, -1);
   for (uint16_t i = 0; i < features.size(); i++) {
      if (features[i].type == FieldFeatureInfo::fCorner) {
         old = features[i].corner.p;
      }
   }

   Point adjust (50, 50);
   int density = 2;
   if (old.x() != -1) {
      Point tl (std::max(old.x() - adjust.x(), 0),
                std::max(old.y() - adjust.y(), 0));
      Point br (std::min(old.x() + adjust.x(), IMAGE_WIDTH),
                std::min(old.y() + adjust.y(), IMAGE_HEIGHT));

      tl /= density;
      br /= density;
      if (tl.y() > br.y()) return false;
/*
      std::cout << "trying to create fovea with bb =("
                << tl.x() << "," << tl.y() << ")" << "and ("
                << br.x() << "," << br.y() << ")" << std::endl;
*/
      boost::shared_ptr<FoveaT<hNone, eGrey> > window(
         new FoveaT<hNone, eGrey>(BBox(tl, br), density));

      window->actuate(frame);
      foveas.push_back(window);

      findFeaturesInFovea(frame, window->asFovea(), seed, LINES);
   }

   int numLines = 0;
   for (uint16_t i = 0; i < fieldLines.size(); i++) {
      if (fieldLines[i].type == FieldFeatureInfo::fCorner) {
         return true;
      }
      else if (fieldLines[i].type == FieldFeatureInfo::fLine) {
         numLines++;
      }
   }
   return false;
}

void FieldLineDetection::findFeaturesInFovea(
                        VisionFrame &frame,
                        const Fovea &fovea,
                        unsigned int *seed,
                        int type) {
   Timer t;
   t.restart();
   fieldLinePoints.reserve(MAX_FIELD_LINE_POINTS);
   fieldFeatures.reserve(MAX_FIELD_FEATURES);
   fieldLines.reserve(MAX_FIELD_FEATURES);

   findFoveaPoints(frame, fovea);

   llog(DEBUG1) << "findFieldLinePoints took " << t.elapsed_us();
   llog(DEBUG1) << " us" << endl;

   FieldFeatureInfo f = FieldFeatureInfo(RRCoord(0,0), fieldLinePoints);

   t.restart();
   if (type == BOTH) {
      findFieldLinesAndCircles(fieldLinePoints,
                               &fieldLines,
                               &linePoints,
                               fovea,
                               seed);
   } else if (type == LINES) {
      findFieldLinesInFovea(fieldLinePoints,
                            &fieldLines,
                            &linePoints,
                            fovea,
                            seed);
   }

   llog(DEBUG1) << "findFieldLinesAndCircles took " << t.elapsed_us();
   llog(DEBUG1) << " us" << endl;

   t.restart();
   findIntersections(fieldLines, &fieldLines, fovea);
   llog(DEBUG1) << "findIntersections took " << t.elapsed_us();
   llog(DEBUG1) << " us" << endl;

   fieldLines.push_back(f);
}

void FieldLineDetection::searchForFeatures(
                        VisionFrame &frame,
                        const Fovea &f,
                        unsigned int *seed) {

   int density = 2;
   int desiredW = 40;
   int desiredH = 30;
   int count = 0;
   int minDistance = 3000;
   const int maxDistance = 5500;
   int searchFor = LINES;

   // Search through field landmarks
   std::vector<Point>::const_iterator l;
   Point prev (-1,-1);
   int i = 0;
   for (l = landmarks.begin(); l != landmarks.end(); ++l, ++i) {
   
      if (count >= 2) return; // Stop once we've searched 2 foveas

      if (i < numCircleLandmarks) {
         minDistance = 1500;
         searchFor = BOTH;
         desiredW = 60;
      } else {
         minDistance = 3000;
         searchFor = LINES;
         desiredW = 40;
      }

      // Calculate their relative distance, heading, etc.
      //robotPos.x() = -2700;
      //robotPos.y() = 100;
      float distance = sqrtf( (l->x() - robotPos.x()) * (l->x() - robotPos.x()) +
                              (l->y() - robotPos.y()) * (l->y() - robotPos.y()) );
      float heading = NORMALISE(atan2f(l->y() - robotPos.y(), l->x() - robotPos.x()) - robotPos.theta());
      float x = distance * cos(heading);
      float y = distance * sin(heading);

      if (distance < minDistance || distance > maxDistance) continue;

      // See if they are in the image
      Point landmark = convertRR->convertToImageXY(Point(x,y));
      if ((landmark.x() > 0) && (landmark.x() < TOP_IMAGE_COLS) &&
          (landmark.y() > 0) && (landmark.y() < TOP_IMAGE_ROWS)) {

         // Make sure we haven't looked here already
         if ((prev.x() != -1) &&
             (abs(prev.x() - landmark.x()) < desiredW*density*2) &&
             (abs(prev.y() - landmark.y()) < desiredH*density*2)) {
            continue;
         }

         // Decide where fovea should go (clip the edge of the image)
         Point tl(std::max(landmark.x()/density - desiredW, 0),
                  std::max(landmark.y()/density - desiredH, 0));
         Point br(std::min(landmark.x()/density + desiredW, TOP_IMAGE_COLS / density),
                  std::min(landmark.y()/density + desiredH, TOP_IMAGE_ROWS / density));

         // Make fovea and store it
         reset(false);
         boost::shared_ptr<FoveaT<hNone, eGrey> > fovea(
            new FoveaT<hNone, eGrey>(BBox(tl, br), density));
         fovea->actuate(frame);
         foveas.push_back(fovea);

         // Look for features in fovea :)
         findFeaturesInFovea(frame, fovea->asFovea(), seed, searchFor);
         fieldFeatures.insert(fieldFeatures.end(),
                              fieldLines.begin(),
                              fieldLines.end());

         // Keep count of how many features we look at
         ++count;
         if (i < numCircleLandmarks) ++count; // Finding circle is expensive, only do it once

         // Track the previous one
         prev = landmark;
      }
   }
   /*
   // Constants
   const int exclusion = 4;
   const int density = 4;

   int numLines = 0; 
   LineInfo l = LineInfo();
   for (uint16_t i = 0; i < fieldLines.size(); i++) {
      if (fieldLines[i].type == FieldFeatureInfo::fLine) {
         numLines++;
         l = fieldLines[i].line;
      }
   }

   // If just one line found and
   // one of its end points inside at the edge of the fovea
   // check for an intersection
   if (numLines == 1) {
      Point p1 = convertRR->convertToImageXY(l.p1);
      if (p1.x() > exclusion && p1.x() < (fovea.bb.width()-exclusion) &&
          p1.y() > exclusion && p1.y() < (fovea.bb.height()-exclusion)) {
   
         // Make fovea
         Point tl = Point(0,0);
         Point br = Point(0,0);
         boost::shared_ptr<FoveaT<hNone, eGrey> > window(
               new FoveaT<hNone, eGrey>(BBox(tl, br), density));

         window->actuate(frame);
         foveas.push_back(window);

         findFeaturesInFovea(frame, window->asFovea(), seed, LINES);
      }

      Point p2 = convertRR->convertToImageXY(l.p2);
      if (p2.x() > exclusion && p2.x() < (fovea.bb.width()-exclusion) &&
          p2.y() > exclusion && p2.y() < (fovea.bb.height()-exclusion)) {
         // Make fovea and search
      }
   }
   */
}


void FieldLineDetection::findFoveaPoints(
                        VisionFrame &frame,
                        const Fovea &fovea) {

   Point localMax = Point(0,0);
   PointF localMaxMT = PointF(0,0);
   vector<PointF> magTheta;
   magTheta.reserve(MAX_FIELD_LINE_POINTS);

   // Vertical Scan
   Point start;
   int fieldTop;
   const int cols = fovea.bb.width();
   const int rows = fovea.bb.height();
   vector<bool> taken(rows, false);

   Point top = Point(0,0);
   PointF topMT = PointF(0,0);
   Point bottom = Point(0,0);
   PointF bottomMT = PointF(0,0);
   int lastGreen = -1;

   for (int i = 0; i < cols; i++) {
      start = fovea.mapFoveaToImage(Point(i, 0));
      if (fovea.top) {
         fieldTop = frame.topStartScanCoords[start.x()];
      } else {
         fieldTop = frame.botStartScanCoords[start.x()];
      }
      start.y() = std::max(start.y(), fieldTop);
      start = fovea.mapImageToFovea(start);

      top = Point(0,0);
      topMT = PointF(0,0);
      bottom = Point(0,0);
      bottomMT = PointF(0,0);
      lastGreen = -1;
      for (int j = start.y(); j < rows; j++) {
         if (fovea.colour(i,j) == cFIELD_GREEN) {
            lastGreen = j;
         }
     
         int dx = fovea.edge(i,j).x();
         int dy = fovea.edge(i,j).y();
         float magnitude = (dx * dx) + (dy * dy);

         // Only consider points with a strong edge
         if (magnitude > FOVEA_EDGE_THRESHOLD) {
            if ((abs(j-lastGreen) > 2) && (topMT.x() == 0)) {
               continue;
            }
            float theta = atan2(dy, dx);
            if (topMT.x() == 0.f){
               top = Point(i, j);
               topMT = PointF(magnitude, theta);
            } else if (fabs(topMT.y() - theta) < ANGLE_THRESHOLD) {
               if (magnitude > topMT.x()) {
                  top = Point(i, j);
                  topMT = PointF(magnitude, theta);
               }
            } else if (bottomMT.x() == 0) {
               if(fabs(fabs(topMT.y() - theta) - M_PI) < ANGLE_THRESHOLD) {
                  bottom = Point(i, j);
                  bottomMT = PointF(magnitude, theta);
               } else {
                  top = Point(i, j);
                  topMT = PointF(magnitude, theta);
               }
            } else if ((magnitude > bottomMT.x()) &&
                       (fabs(bottomMT.y() - theta) < ANGLE_THRESHOLD)) {
               bottom = Point(i, j);
               bottomMT = PointF(magnitude, theta);
            }
         } else if (bottomMT.x() != 0) {
           
            int x = (top.x() + bottom.x()) / 2;
            int y = (top.y() + bottom.y()) / 2;
            
            // Sanity checks

            // Check colour of pixel in image
//            Colour c = fovea.colour(x,y);
            int pixel = checkPixelColour(Point(x,y), fovea);
            if (pixel == 0) {
//            if ((c != cWHITE) && (c != cUNCLASSIFIED)){ 
               top = bottom;
               topMT = bottomMT;
               bottom = Point(0,0);
               bottomMT = PointF(0,0);
               continue;
            } else {

               if (pixel == 1) {
                  y -= 1;
               } else if (pixel == -1) {
                  y += 1;
               }
            }

            // Check distance between top and bottom point

            Point imageTop = fovea.mapFoveaToImage(top);
            Point imageBottom = fovea.mapFoveaToImage(bottom);
            Point topRR = frame.cameraToRR.pose.imageToRobotXY(imageTop);
            Point bottomRR =frame.cameraToRR.pose.imageToRobotXY(imageBottom);
            int dist = DISTANCE_SQR(topRR.x(), topRR.y(),
                                bottomRR.x(), bottomRR.y());
            if ((dist < LINE_MIN_THRESHOLD) ||
                (dist > FOVEA_LINE_WIDTH_THRESHOLD)) {
               top = bottom;
               topMT = bottomMT;
               bottom = Point(0,0);
               bottomMT = PointF(0,0);
               continue;
            }

            Point middle = fovea.mapFoveaToImage(Point(x, y));
            Point p = frame.cameraToRR.pose.imageToRobotXY(middle);

            // Check point isn't too close
            dist = p.x()*p.x() + p.y()*p.y();
            if (dist < MIN_POINT_DISTANCE) {
               top = bottom;
               topMT = bottomMT;
               bottom = Point(0,0);
               bottomMT = PointF(0,0);
               continue;
            }

            FieldLinePointInfo f = FieldLinePointInfo(middle, p);
            fieldLinePoints.push_back(f);
            taken[y] = true;
            
            top = Point(0,0);
            topMT = PointF(0,0);
            bottom = Point(0,0);
            bottomMT = PointF(0,0);
         }
      }
   }

   // Horizontal Scan
   for (int j = 0; j < rows; j++) {
      if (taken[j]) continue;
      top = Point(0,0);
      topMT = PointF(0,0);
      bottom = Point(0,0);
      bottomMT = PointF(0,0);
      lastGreen = -1;

      for (int i = 0; i < cols; i++) {
         start = fovea.mapFoveaToImage(Point(i, j));
         if (fovea.top) {
            fieldTop = frame.topStartScanCoords[start.x()];
         } else {
            fieldTop = frame.botStartScanCoords[start.x()];
         }

         if (start.y() < fieldTop) {
            continue;
         }

         if (fovea.colour(i,j) == cFIELD_GREEN) {
            lastGreen = i;
         }
     
         int dx = fovea.edge(i,j).x();
         int dy = fovea.edge(i,j).y();
         float magnitude = (dx * dx) + (dy * dy);

         // Only consider points with a strong edge
         if (magnitude > FOVEA_EDGE_THRESHOLD) {
            if ((abs(i-lastGreen) > 2) && (topMT.x() == 0)) {
               continue;
            }
            float theta = atan2(dy, dx);
            if (topMT.x() == 0){
               top = Point(i, j);
               topMT = PointF(magnitude, theta);
            } else if (fabs(topMT.y() - theta) < ANGLE_THRESHOLD) {
               if (magnitude > topMT.x()) {
                  top = Point(i, j);
                  topMT = PointF(magnitude, theta);
               }
            } else if (bottomMT.x() == 0) {
               if(fabs(fabs(topMT.y() - theta) - M_PI) < ANGLE_THRESHOLD) {
                  bottom = Point(i, j);
                  bottomMT = PointF(magnitude, theta);
               } else {
                  top = Point(i, j);
                  topMT = PointF(magnitude, theta);
               }
            } else if ((magnitude > bottomMT.x()) &&
                       (fabs(bottomMT.y() - theta) < ANGLE_THRESHOLD)) {
               bottom = Point(i, j);
               bottomMT = PointF(magnitude, theta);
            }
         } else if (bottomMT.x() != 0) {
           
            int x = (top.x() + bottom.x()) / 2;
            int y = (top.y() + bottom.y()) / 2;
            
            // Sanity checks

            // Check colour of pixel in image
            Colour c = fovea.colour(x,y);
            if ((c != cWHITE) && (c != cUNCLASSIFIED)){ 
               top = bottom;
               topMT = bottomMT;
               bottom = Point(0,0);
               bottomMT = PointF(0,0);
               continue;
            }

            // Check distance between top and bottom point
            Point imageTop = fovea.mapFoveaToImage(top);
            Point imageBottom = fovea.mapFoveaToImage(bottom);
            Point topRR = frame.cameraToRR.pose.imageToRobotXY(imageTop);
            Point bottomRR =frame.cameraToRR.pose.imageToRobotXY(imageBottom);
            int dist = DISTANCE_SQR(topRR.x(), topRR.y(),
                                bottomRR.x(), bottomRR.y());
            if ((dist < LINE_MIN_THRESHOLD) ||
                (dist > FOVEA_LINE_WIDTH_THRESHOLD)) {
               top = bottom;
               topMT = bottomMT;
               bottom = Point(0,0);
               bottomMT = PointF(0,0);
               continue;
            }

            Point middle = fovea.mapFoveaToImage(Point(x, y));
            Point p = frame.cameraToRR.pose.imageToRobotXY(middle);

            // Check point isn't too close
            dist = p.x()*p.x() + p.y()*p.y();
            if (dist < MIN_POINT_DISTANCE) {
               top = bottom;
               topMT = bottomMT;
               bottom = Point(0,0);
               bottomMT = PointF(0,0);
               continue;
            }

            FieldLinePointInfo f = FieldLinePointInfo(middle, p);
            fieldLinePoints.push_back(f);
            
            top = Point(0,0);
            topMT = PointF(0,0);
            bottom = Point(0,0);
            bottomMT = PointF(0,0);
         }
      }
   }


}

int FieldLineDetection::checkPixelColour(Point p, const Fovea &fovea) {
   bool colour = (fovea.colour(p.x(), p.y()) ==  cWHITE) ||
                 (fovea.colour(p.x(), p.y()) == cUNCLASSIFIED);
   if (colour) return 10;
   bool top = false;
   bool bottom = false;
   if (p.y() > 1) {
      top = (fovea.colour(p.x(), p.y()-1) ==  cWHITE) ||
            (fovea.colour(p.x(), p.y()-1) == cUNCLASSIFIED);
   }
   if (top) return 1;
   if (p.y() < fovea.bb.height()-1) {
      bottom = (fovea.colour(p.x(), p.y()+1) ==  cWHITE) ||
               (fovea.colour(p.x(), p.y()+1) == cUNCLASSIFIED);
   }
   if (bottom) return -1;
   return 0;
//   return colour || top || bottom;
} 


void FieldLineDetection::findIntersections(
                  vector<FieldFeatureInfo> &lines,
                  vector<FieldFeatureInfo> *features,
                  const Fovea &fovea) {
   if (lines.size() < 2) return;
   for (uint16_t i = 0; i < (lines.size()); i++) {
      while (lines[i].type != FieldFeatureInfo::fLine) {
         i++;
         if (i >= lines.size()) {
            return;
         }
      }
      LineInfo l1 = lines[i].line;
      for (uint16_t j = i+1; j < (lines.size()); j++) {
         while (lines[j].type != FieldFeatureInfo::fLine) {
            j++;
            if (j >= lines.size()) {
               return;
            }
         }
         LineInfo l2 = lines[j].line;

         Point p = Point(0,0);
         p = intersect(l1, l2);
         if ((p.x() != 0) && perpendicular(l1,l2)) {
            FieldFeatureInfo f;

            // If X intersection
            if (possibleT(l1, l2) &&
                possibleT(l2, l1)) {
               f = FieldFeatureInfo();
            }

            // If T intersection
            else if (possibleT(l1, l2)) {
               float angle = findTAngle(p, l2);
               float distance = sqrt(p.x() * p.x() + p.y() * p.y());
               float heading = atan2(p.y(), p.x());
               RRCoord r = RRCoord(distance, heading, -angle);
               p = convertRR->convertToImageXY(p);
               TJunctionInfo t = TJunctionInfo(p);
               f = FieldFeatureInfo(r, t);
            }
            // If T intersection
            else if (possibleT(l2, l1)) {
               float angle = findTAngle(p, l1);
               float distance = sqrt(p.x() * p.x() + p.y() * p.y());
               float heading = atan2(p.y(), p.x());
               RRCoord r = RRCoord(distance, heading, -angle);
               p = convertRR->convertToImageXY(p);
               TJunctionInfo t = TJunctionInfo(p);
               f = FieldFeatureInfo(r, t);
            }
            // If corner
            else {
               float angle = findCAngle(p, l1, l2);
               if (isBadCorner(l1,l2)) {
                  //cout << "bad corner" << endl;
                  continue;
               }
               float distance = sqrt(p.x() * p.x() + p.y() * p.y());
               float heading = atan2(p.y(), p.x());
               RRCoord r = RRCoord(distance, heading, -angle);
               p = convertRR->convertToImageXY(p);
               CornerInfo c = CornerInfo(p);
               f = FieldFeatureInfo(r, c);
            }
            if (!closeToEdge(fovea, f)) {
               features->push_back(f);
               lines[i].lineUsed = true;
               lines[j].lineUsed = true;
            } else {
               //std::cout << "CLOSE CORNER FAIL :(" << std::endl;
            }
            break;
         }
/*
         else if (p.x() != 0) {
            RRCoord r = RRCoord(0,0);
            if (parallelPair(l1, l2, &r)) {
               lines[i].lineUsed = true;
               lines[j].lineUsed = true;
               r = RRCoord(0,0);
               ParallelLinesInfo l = ParallelLinesInfo(l1,l2);
               FieldFeatureInfo f = FieldFeatureInfo(r, l);
               //features->clear();
               features->push_back(f);
            } else {
//               cout << "clear" << endl;
               //features->clear();
               return;
            }
         }
*/
      }
   }
}

void FieldLineDetection::circleOrientation(
                  vector<FieldFeatureInfo> *features) {
   LineInfo line = LineInfo();
   FieldFeatureInfo circle = FieldFeatureInfo();

   // find line and circle from list of features
   for (uint16_t i = 0; i < features->size(); i++) {
      if ((*features)[i].type == FieldFeatureInfo::fLine) {
         line = (*features)[i].line;
      } else if ((*features)[i].type == FieldFeatureInfo::fCentreCircle) {
         circle = (*features)[i];
      }
   }
   // maybe add check here to ensure line and circle are set
   Point c = circle.rr.toCartesian();
   float denom = sqrt (line.t1*line.t1 + line.t2*line.t2);
   float dist = (line.t1 * c.x() + line.t2 * c.y() + line.t3) / denom;
   if (fabs(dist) > 200) return; // check line is within 20cm of circle centre

   // check intersection
   Point centreRight = line.p1;
   Point centreLeft = line.p2;
   if (centreRight.y() > centreLeft.y()) {
      centreRight = line.p2;
      centreLeft = line.p1;
   }
   float centreLineAngle = atan2(centreRight.y() - centreLeft.y() ,
                             centreRight.x() - centreLeft.x());

   Point directionRight = Point(0,0);
   Point directionLeft = circle.rr.toCartesian();
   if (directionRight.y() > directionLeft.y()) {
      directionRight = circle.rr.toCartesian();
      directionLeft = Point(0,0);
   }
   float robotCentreAngle = atan2(directionRight.y() - directionLeft.y(),
                                  directionRight.x() - directionLeft.x());

   float angle = robotCentreAngle - centreLineAngle;
   //check nan?

   if (angle < 0) {
      angle += M_PI;
   }

   circle.rr.orientation() = angle;

   // TODO: Sean maybe remove the clear from  here?
   //features->clear();
   features->push_back(circle);

}

bool FieldLineDetection::closeToEdge(const Fovea &fovea, FieldFeatureInfo f) {
   Point p;
   if (f.type == FieldFeatureInfo::fCorner) {
      p = fovea.mapImageToFovea(f.corner.p);
   } else if (f.type == FieldFeatureInfo::fTJunction) {
      p = fovea.mapImageToFovea(f.tjunction.p);
   } else {
      return true;
   }

   if (p.x() < EXCLUSION_LENGTH) return true;
   if (p.y() < EXCLUSION_LENGTH) return true;
   if (p.x() > (fovea.bb.width() - EXCLUSION_LENGTH)) return true;
   if (p.y() > (fovea.bb.height() - EXCLUSION_LENGTH)) return true;
   return false;
}


bool FieldLineDetection::isBadCorner(LineInfo l1, LineInfo l2) {
   float denom = sqrt(l1.t1*l1.t1 + l1.t2*l1.t2);
   float distl1p1 = (l1.t1*l2.p1.x() + l1.t2*l2.p1.y() + l1.t3) / denom;
   float distl1p2 = (l1.t1*l2.p2.x() + l1.t2*l2.p2.y() + l1.t3) / denom;

   if ((fabs(distl1p1) > 200.f) && (fabs(distl1p2) > 200.f)) {
      return true;
   }
   denom = sqrt(l2.t1*l2.t1 + l2.t2*l2.t2);
   float distl2p1 = (l2.t1*l1.p1.x() + l2.t2*l1.p1.y() + l2.t3) / denom;
   float distl2p2 = (l2.t1*l1.p2.x() + l2.t2*l1.p2.y() + l2.t3) / denom;
   if ((fabs(distl2p1) > 200.f) && (fabs(distl2p2) > 200.f)) {
      return true;
   }
   return false;
}

float FieldLineDetection::findTAngle(Point p, LineInfo l) {
   float angle = findGradient(l, p);
   float theta = atan2(p.x(), p.y());

   if (angle > 0) {
      angle = theta + UNSWDEG2RAD(180) - angle;
   } else {
      angle = theta - (UNSWDEG2RAD(180) + angle);
   }

   if (angle > M_PI) {
      angle = NORMALISE(angle);
   }
   return angle;
}


float FieldLineDetection::findCAngle(Point p, LineInfo l1, LineInfo l2) {
   float g1 = findGradient(l1, p);
   float g2 = findGradient(l2, p);
   float angle = (g1+g2)/2;
   float roughQuadrantLimit = UNSWDEG2RAD(80);
   if ((g1 > 0) && (g2 < 0) &&
         (g1 > roughQuadrantLimit) && (g2 < -roughQuadrantLimit)) {
      g2 += 2*M_PI;
      angle = (g1+g2)/2;
   }
   if ((g2 > 0) && (g1 < 0) &&
         (g2 > roughQuadrantLimit) && (g1 < -roughQuadrantLimit)) {
      g1 += 2*M_PI;
      angle = (g1+g2)/2;
   }
   if (angle > M_PI) angle -= 2*M_PI;

   float theta = atan2(p.x(), p.y());

   if (angle > 0) {
      angle = theta + UNSWDEG2RAD(180) - angle;
   } else {
      angle = theta - (UNSWDEG2RAD(180) + angle);
   }

   if (angle > M_PI) {
      angle = NORMALISE(angle);
   }
   return angle;
}

float FieldLineDetection::findGradient(LineInfo l, Point p) {
   // Work out which direction to find gradient in
   float distp1 = DISTANCE_SQR(p.x(), p.y(), l.p1.x(), l.p1.y());
   float distp2 = DISTANCE_SQR(p.x(), p.y(), l.p2.x(), l.p2.y());
   Point far;
   Point close;
   if (distp1 > distp2) {
      far = l.p1;
      close = l.p2;
   } else {
      far = l.p2;
      close = l.p1;
   }
   return (atan2(far.x() - close.x(), far.y() - close.y()));
}


bool FieldLineDetection::possibleT(LineInfo l1,
                                   LineInfo l2) {

   // Return true if l1 could be the top line in the T

   // Check endpoints of l1 are sufficiently far away from l2
   float denom  = sqrt(l2.t1*l2.t1 + l2.t2*l2.t2);
   float distp1 = (l2.t1*l1.p1.x() + l2.t2*l1.p1.y() + l2.t3) / denom;
   float distp2 = (l2.t1*l1.p2.x() + l2.t2*l1.p2.y() + l2.t3) / denom;

   if ((distp1 < 0.f) ^ (distp2 < 0.f)) {
      if (fabs(distp1) > 40.f && fabs(distp2) > 40.f) {

         // Check at least one endpoint of l2 is near l1
         denom = sqrt(l1.t1*l1.t1 + l1.t2*l1.t2);
         distp1 = (l1.t1*l2.p1.x() + l1.t2*l2.p1.y() + l1.t3) / denom;
         if (fabs(distp1) < 200.f) return true;
         distp2 = (l1.t1*l2.p2.x() + l1.t2*l2.p2.y() + l1.t3) / denom;
         if (fabs(distp2) < 200.f) return true;
      }
   }

   return false;
} 

Point FieldLineDetection::intersect(LineInfo l1, LineInfo l2) {
   Eigen::Matrix2f A;
   Eigen::Vector2f b, x;
   Point p = Point(0,0);

   A << l1.t1, l1.t2,
        l2.t1, l2.t2;
   b << -l1.t3, -l2.t3;

   if (A.lu().solve(b, &x)) {
     p = Point(x.x(), x.y());
   }
   return p;
}

bool FieldLineDetection::perpendicular(LineInfo l1, LineInfo l2) {

   float theta = atan2f(
              (-1*(l1.t1)/(float)(l1.t2) + (l2.t1)/(float)(l2.t2)),
              (1 + (l1.t1)/(float)(l1.t2) * (l2.t1)/(float)(l2.t2)));
   theta = fabs(theta) - UNSWDEG2RAD(90);
   if (isnan(theta)) return false;
   if ((fabs(theta) < PERPENDICULAR_THRESHOLD)) {
      return true;
   }
   return false;
}

bool FieldLineDetection::parallelPair(LineInfo l1,
                                      LineInfo l2,
                                      RRCoord *r) {
   bool parallel = false;

   float angle1 = atan2 (l1.p1.x() - l1.p2.x(), l1.p1.y() - l1.p2.y());
   float angle2 = atan2 (l2.p1.x() - l2.p2.x(), l2.p1.y() - l2.p2.y());
   if (fabs(angle1-angle2) < PARALLEL_LINE_THRESHOLD) {
      parallel = true;
   }

   // Make sure we try swapping p1 and p2, incase they were the wrong way
   float angle3 = atan2 (l2.p2.x() - l2.p1.x(), l2.p2.y() - l2.p1.y());
   if (fabs(angle1-angle3) < PARALLEL_LINE_THRESHOLD) {
      parallel = true;
   }
   if (!parallel) return false;

   float lenSqr1 = DISTANCE_SQR(l1.p1.x(), l1.p1.y(), l1.p2.x(), l1.p2.y()); 
   float lenSqr2 = DISTANCE_SQR(l2.p1.x(), l2.p1.y(), l2.p2.x(), l2.p2.y());
   if (lenSqr1 < MIN_PARALLEL_LENGTH || lenSqr2 < MIN_PARALLEL_LENGTH) return false;

   float dist1 = fabs(l1.t3) / sqrt(SQUARE(l1.t1) + SQUARE(l1.t2));
   float dist2 = fabs(l2.t3) / sqrt(SQUARE(l2.t1) + SQUARE(l2.t2));
   if (fabs (fabs(dist1-dist2) - GOAL_BOX_LENGTH) < PARALLEL_LINE_DISTANCE_THRESHOLD) {
      if (dist1 < dist2) {
         float theta = atan2f (((float)-l1.t1) * ((float)l1.t3),
                               ((float)l1.t2)  * ((float)l1.t3));
         *r = RRCoord(dist1, theta);
      } else {
         float theta = atan2f (((float)-l2.t1) * ((float)l2.t3),
                               ((float)l2.t2)  * ((float)l2.t3));
         *r = RRCoord(dist2, theta);
      }
      return true;
   }
   return false;
}

void FieldLineDetection::findFieldLinesAndCircles
                           (const vector<FieldLinePointInfo> &points,
                            vector<FieldFeatureInfo> *lines,
                            vector< vector<FieldLinePointInfo> >*linePoints,
                            const Fovea &fovea,
                            unsigned int *seed) {
   vector<Point> fieldPoints1;
   vector<Point> fieldPoints2;
   fieldPoints1.reserve(points.size());
   fieldPoints2.reserve(points.size());
   
   // RANSAC variables
   uint16_t k = 40;
   float e =  15.0;
   uint16_t n = 20;
   if (fovea.top) n += 5;
   vector<bool> *con, consBuf[2];
   consBuf[0].resize(points.size());
   consBuf[1].resize(points.size());
   con = &consBuf[0];
   RANSACLine resultLine = RANSACLine(Point(0,0), Point(0,0));
   RANSACCircle resultCircle = RANSACCircle(PointF(0,0), 0.0);
   bool line = true;

   vector<bool> takenPoints;
   takenPoints.reserve(points.size());
   for (vector<FieldLinePointInfo>::const_iterator i = points.begin();
         i != points.end(); i++) {
      takenPoints.push_back(false);
      fieldPoints1.push_back(i->rrp);
   }

   vector<FieldLinePointInfo> onLinePoints;
   onLinePoints.reserve(points.size());

   // Use RANSAC to find field lines
   for (uint16_t i = 0; i < 3; i++) {
      consBuf[0].clear();
      consBuf[1].clear();
      resultLine = RANSACLine(Point(0,0), Point(0,0));
      resultCircle = RANSACCircle(PointF(0,0), 0.0);
      if (RANSAC::findLinesAndCircles
            (fieldPoints1, CENTER_CIRCLE_DIAMETER/2, &con, resultLine, resultCircle,
             k, e, n, consBuf, seed)) {

         uint16_t b = 0;
         onLinePoints.clear();
         Point max = resultLine.p1;
         Point min = resultLine.p2;
         bool horizontal = false;
         if (abs(min.x() - max.x()) < 50 && abs(min.y() - max.y()) > 50) {
            horizontal = true;
         }
         if (min.x() == 0 && min.y() == 0 && max.x() == 0 && max.y() == 0) {
            line = false;
         }

         // Loop through and remove all the points on the line/circle
         for (uint16_t a = 0; a < fieldPoints1.size(); a++, b++){
            while (takenPoints[b] == true) b++;
            if (con[0][a] == true) {
               onLinePoints.push_back(points[b]);
               takenPoints[b] = true;

               // Keep track of endpoints
               if (horizontal) {
                  if (fieldPoints1[a].y() < min.y()) {
                     min = fieldPoints1[a];
                  } if (fieldPoints1[a].y() > max.y()) {
                     max = fieldPoints1[a];
                  }
               } else {
                  if (fieldPoints1[a].x() < min.x()) {
                     min = fieldPoints1[a];
                  } if (fieldPoints1[a].x() > max.x()) {
                     max = fieldPoints1[a];
                  }
               }
            } else {
               fieldPoints2.push_back(fieldPoints1[a]);
            }
         }
         if ((resultCircle.var <= -10) &&
            ((resultCircle.var) < resultLine.var)) {

            // Calculate RRCoord to centre and save circle info
            PointF centre = resultCircle.centre;
            float distance = sqrt(centre.x() * centre.x() +
                                  centre.y() * centre.y());
            float heading = atan2 (centre.y(), centre.x());
            RRCoord c = RRCoord(distance,heading);
            CentreCircleInfo l = CentreCircleInfo();
            FieldFeatureInfo f = FieldFeatureInfo(c, l);
            lines->push_back(f);

            // If debug stuff
            #ifdef visionTabOutput
            linePoints->push_back(onLinePoints);
            circlePoints = onLinePoints;
            #endif

         } else if (line) { //if (resultLine.var <= -10) {

            // Check line and save info
            LineInfo l (resultLine.p1, resultLine.p2);
            l.p1 = min;
            l.p2 = max;

            // Check the line isn't too short
            int dist2 = DISTANCE_SQR(min.x(), min.y(), max.x(), max.y()); 
            if ( dist2 > MIN_LINE_LENGTH || (dist2>SHORT_LINE_LENGTH && resultLine.var <=-16.f) ) {

               // Trim down the line if it is too long
               sort (onLinePoints.begin(), onLinePoints.end(),
                     cmpPoints());
               checkLine(onLinePoints, &l);

               // Check the line still isn't too short
               dist2 = DISTANCE_SQR(l.p1.x(), l.p1.y(), l.p2.x(), l.p2.y()); 
               if (dist2 > MIN_LINE_LENGTH || (dist2>SHORT_LINE_LENGTH && resultLine.var <=-16.f) ) {
                  RRCoord c (0,0);
                  FieldFeatureInfo f = FieldFeatureInfo(c, l);
                  lines->push_back(f);

                  // If debug stuff
                  #ifdef visionTabOutput
                  linePoints->push_back(onLinePoints);
                  #endif
               } 
            } 
         }

         fieldPoints1.clear();
         fieldPoints1 = fieldPoints2;
         fieldPoints2.clear();
      }

      // If RANSAC didn't find anything, don't bother trying again.
      else {
         return;
      }
   }
}


void FieldLineDetection::findFieldLinesInFovea
                           (const vector<FieldLinePointInfo> &points,
                            vector<FieldFeatureInfo> *lines,
                            vector< vector<FieldLinePointInfo> >*linePoints,
                            const Fovea &fovea,
                            unsigned int *seed) {
   vector<Point> fieldPoints1;
   vector<Point> fieldPoints2;
   fieldPoints1.reserve(points.size());
   fieldPoints2.reserve(points.size());
   
   // RANSAC variables
   uint16_t k = 40;
   float e =  15.0;
   uint16_t n = 20;
   vector<bool> *con, consBuf[2];
   consBuf[0].resize(points.size());
   consBuf[1].resize(points.size());
   RANSACLine resultLine = RANSACLine(Point(0,0), Point(0,0));

   vector<bool> takenPoints;
   takenPoints.reserve(points.size());
   for (vector<FieldLinePointInfo>::const_iterator i = points.begin();
         i != points.end(); i++) {
      takenPoints.push_back(false);
      fieldPoints1.push_back(i->rrp);
   }

   vector<FieldLinePointInfo> onLinePoints;
   onLinePoints.reserve(points.size());

   // Use RANSAC to find field lines
   for (uint16_t i = 0; i < 2; i++) {
      consBuf[0].clear();
      consBuf[1].clear();
      resultLine = RANSACLine(Point(0,0), Point(0,0));
      if (RANSAC::findLine (fieldPoints1, &con, resultLine,
             k, e, n, consBuf, seed)) {

         uint16_t b = 0;
         onLinePoints.clear();
         Point max = resultLine.p1;
         Point min = resultLine.p2;
         bool horizontal = false;
         if (abs(min.x() - max.x()) < 50 && abs(min.y() - max.y()) > 50) {
            horizontal = true;
         }

         // Loop through and remove all the points on the line/circle
         for (uint16_t a = 0; a < fieldPoints1.size(); a++, b++){
            while (takenPoints[b] == true) b++;
            if (con[0][a] == true) {
               onLinePoints.push_back(points[b]);
               takenPoints[b] = true;

               // Keep track of endpoints
               if (horizontal) {
                  if (fieldPoints1[a].y() < min.y()) {
                     min = fieldPoints1[a];
                  } if (fieldPoints1[a].y() > max.y()) {
                     max = fieldPoints1[a];
                  }
               } else {
                  if (fieldPoints1[a].x() < min.x()) {
                     min = fieldPoints1[a];
                  } if (fieldPoints1[a].x() > max.x()) {
                     max = fieldPoints1[a];
                  }
               }
            } else {
               fieldPoints2.push_back(fieldPoints1[a]);
            }
         }

         // Check line and save info
         LineInfo l (resultLine.p1, resultLine.p2);
         l.p1 = min;
         l.p2 = max;
         RRCoord c (0,0);
         FieldFeatureInfo f = FieldFeatureInfo(c, l);
         lines->push_back(f);

         // If debug stuff
         #ifdef visionTabOutput
         linePoints->push_back(onLinePoints);
         #endif

         fieldPoints1.clear();
         fieldPoints1 = fieldPoints2;
         fieldPoints2.clear();
      }

      // If RANSAC didn't find anything, don't bother trying again.
      else {
         return;
      }
   }
}


void FieldLineDetection::checkLine(
            const std::vector<FieldLinePointInfo> &points,
            LineInfo *l1) {

   std::vector<FieldLinePointInfo>::const_iterator it;
   std::vector<FieldLinePointInfo>::const_iterator start = points.end();
   std::vector<FieldLinePointInfo>::const_iterator end = points.end();
   std::vector<FieldLinePointInfo>::const_iterator bestStart = points.begin();
   std::vector<FieldLinePointInfo>::const_iterator bestEnd = points.end();

   int min = 300 * 300;

   Point prev = Point(0,0);
   for (it = points.begin(); it != points.end(); ++it) {
      if (prev.x() != 0) {
         Point curr = it->rrp;
         int dist = DISTANCE_SQR(curr.x(), curr.y(), prev.x(), prev.y());
         if (dist > min) {
            if (start == points.end()) {
               start = it;
               bestEnd = it;
            } else if (end == points.end()) {
               end = it;
               if (end - start > bestEnd - bestStart) {
                  bestStart = start;
                  bestEnd = end;
               }
               start = it;
               end = points.end();
            }
         }
      }
      prev = it->rrp;
   }
   // Case for the end of the vector
   end = it;
   if (end - start > bestEnd - bestStart) {
      bestStart = start;
      bestEnd = end;
   }
   bestEnd--;
 
  l1->p1 = bestStart->rrp;
  l1->p2 = bestEnd->rrp;

}

bool FieldLineDetection::isLineTooLong (const Fovea &fovea, 
                                        LineInfo l1, LineInfo* l2) {

   Point start = convertRR->convertToImageXY(l1.p1);
   Point end   = convertRR->convertToImageXY(l1.p2);
   start = fovea.mapImageToFovea(start);
   end   = fovea.mapImageToFovea(end);

   BresenhamPtr<const Colour> cbp(fovea._colour, start, end,
         fovea.bb.height());
   BresenhamPtr<const Colour>::iterator cp = cbp.begin();

   int it_count = 0;
   int green_count = 0;
   while (fovea.bb.validIndex(cp.point())) {
      ++cp;
      it_count++;
      if (it_count > 6) break;
      if (*cp == cFIELD_GREEN) {
         green_count++;
         if (green_count < 2) continue;
         while(fovea.bb.validIndex(cp.point()) && (*cp != cWHITE)) {
            ++cp;
         }
         if (*cp == cWHITE) {
            Point p2 = fovea.mapFoveaToImage(cp.point());
            Point newEnd = convertRR->convertToRRXY(p2);
            *l2 = LineInfo(l1.p2, newEnd);
            return true;
         }
      }
   }

   cbp = BresenhamPtr<const Colour>
      (fovea._colour, end, start, fovea.bb.height());
   cp = cbp.begin();

   it_count = 0;
   green_count = 0;
   while (fovea.bb.validIndex(cp.point())) {
      ++cp;
      it_count++;
      if (it_count > 6) break;
      if (*cp == cFIELD_GREEN) {
         green_count++;
         if (green_count < 2) continue;
         while(fovea.bb.validIndex(cp.point()) && (*cp != cWHITE)) {
            ++cp;
         }
         if (*cp == cWHITE) {
            Point p2 = fovea.mapFoveaToImage(cp.point());
            Point newEnd = convertRR->convertToRRXY(p2);
            *l2 = LineInfo(l1.p1, newEnd);
            return true;
         }
      }
   }
   return false;
}

void FieldLineDetection::findParallelLines(
               std::vector<FieldFeatureInfo>* lines) {
   for (unsigned int i = 0; i < lines->size(); ++i) {
      if ((*lines)[i].type == FieldFeatureInfo::fLine) {
         for (unsigned int j = i; j < lines->size(); ++j) {
            if ((*lines)[j].type == FieldFeatureInfo::fLine) {
               RRCoord r = RRCoord(0,0);
               LineInfo l1 = (*lines)[i].line;
               LineInfo l2 = (*lines)[j].line;
               if (parallelPair(l1, l2, &r)) {
                  (*lines)[i].lineUsed = true;
                  (*lines)[j].lineUsed = true;
                  r = RRCoord(0,0);
                  ParallelLinesInfo l = ParallelLinesInfo(l1,l2);
                  FieldFeatureInfo f = FieldFeatureInfo(r, l);
                  lines->push_back(f);
                  return;
               }
            }
         }
      }
   }
}

bool FieldLineDetection::findPenaltySpot(VisionFrame &frame,
                                         const Fovea& fovea) {
   XHistogram xhistogram(fovea.bb.width());// = fovea.xhistogram;
   YHistogram yhistogram(fovea.bb.height());// = fovea.yhistogram;

   makeWhiteHistsBelowFieldEdge(frame, fovea, xhistogram, yhistogram);

   int start = -1;
   std::vector<std::pair<int, int> > horz;
   std::vector<std::pair<int, int> > vert;

   // Look for horizontal regions
   int i;
   for (i = 0; i < xhistogram.size; ++i) {
      if (xhistogram._counts[i][cWHITE] > 0) {
         if (start == -1) {
            start = i;
         }
      } else {
         if (start != -1) {
            horz.push_back(std::make_pair(start, i));
            start = -1;
         }
      }
   }
   // Case for end of run
   if (start != -1) horz.push_back(std::make_pair(start,i));

   // Look for vertical regions
   start = -1;
   for (i = 0; i < yhistogram.size; ++i) {
      if (yhistogram._counts[i][cWHITE] > 0) {
         if (start == -1) {
            start = i;
         }
      } else {
         if (start != -1) {
            vert.push_back(std::make_pair(start, i));
            start = -1;
         }
      }
   }
   // Case for end of run
   if (start != -1) vert.push_back(std::make_pair(start,i));

   std::vector<std::pair<int, int> >::const_iterator it;
   std::vector<std::pair<int, int> >::const_iterator jt;
   for (it = horz.begin(); it != horz.end(); ++it) {
      for (jt = vert.begin(); jt != vert.end(); ++jt) {

         // Initial area
         int hStart = it->first;
         int hEnd = it->second;
         int hMid = (hStart + hEnd) / 2;
         int vStart = jt->first;
         int vEnd = jt->second;
         int vMid = (vStart + vEnd) / 2;

         // Try and shrink the box incase it has been ruined by a line
         for (int i = hStart; i < hEnd; ++i) {
            if (fovea.colour(i, vMid) != cWHITE) hStart++;
            else break;
         }
         for (int i = hEnd-1; i >= hStart; --i) {
            if (fovea.colour(i, vMid) != cWHITE) hEnd--;
            else break;
         }
         for (int i = vStart; i < vEnd; ++i) {
            if (fovea.colour(hMid, i) != cWHITE) vStart++;
            else break;
         }
         for (int i = vEnd-1; i >= vStart; --i) {
            if (fovea.colour(hMid, i) != cWHITE) vEnd--;
            else break;
         }

         // Make sure we have some region left
         if (hEnd <= hStart || vEnd <= vStart) {
            //std::cout << "no white" << std::endl;
            continue;
            //hStart = it->first;
            //hEnd = it->second;
            //vStart = jt->first;
            //vEnd = jt->second;
         }

         // Width, height and points in fovea
         // Adding 3: +1 to ensure round up
         // +3 to ensure an extra pixel around penalty spot to check is green
         int w = hEnd - hStart + 4;
         int h = vEnd - vStart + 4;
         Point centre ((hStart + hEnd)/2, (vStart + vEnd)/2);
         Point tl (centre.x() - w/2, centre.y() - h/2);
         Point br (centre.x() + w/2, centre.y() + h/2);

         // Throw away if at edge of image
         if ((tl.x() < 0) || (br.x() > fovea.bb.width()-1) ||
             (tl.y() < 0) || (br.y() > fovea.bb.height()-1)) {
            //std::cout << "edge of image" << std::endl;
            continue;
         }

         // Width and height should be in ratio
         if ((h > (w*1.1)) || (h < (w*0.5))) {
            //std::cout << "ratio no good" << std::endl;
            continue;
         }

         // Examine the colour breakdown of the bbox
         int top = 0, bottom = 0, left = 0, right = 0; // green
         int vertical = 0, horizontal = 0;             // white
         for (int i = tl.x(); i < br.x(); ++i) {
            if (fovea.colour(i, tl.y()) == cFIELD_GREEN) ++top;
            if (fovea.colour(i, br.y()) == cFIELD_GREEN) ++bottom;
            if (fovea.colour(i, centre.y()) == cWHITE ||
                fovea.colour(i, centre.y()) == cUNCLASSIFIED) ++horizontal;
         }
         for (int i = tl.y(); i < br.y(); ++i) {
            if (fovea.colour(tl.x(), i) == cFIELD_GREEN) ++left;
            if (fovea.colour(br.x(), i) == cFIELD_GREEN) ++right;
            if (fovea.colour(centre.x(), i) == cWHITE ||
                fovea.colour(centre.x(), i) == cUNCLASSIFIED) ++vertical;
         }

         // Check enough green around the outside
         int wgRatio = w * 0.8;
         int hgRatio = h * 0.8;
         if ((top < wgRatio)  || (bottom < wgRatio) ||
             (left < hgRatio) || (right < hgRatio)) {
            //std::cout << "not enough green" << std::endl;
            continue;
         }

         // Check enough white through the middle
         int wwRatio = w * 0.5;
         int hwRatio = h * 0.5;
         if ((horizontal < wwRatio)  || (vertical < hwRatio)) {
            //std::cout << "not enough white" << std::endl;
            continue;
         }


         // Width, height and points in image
         centre = fovea.mapFoveaToImage(centre);
         tl = fovea.mapFoveaToImage(tl);
         br = fovea.mapFoveaToImage(br);
         w = (centre.x() - tl.x()) * 2;
         h = (centre.y() - tl.y()) * 2;

         // Check distance between top and bottom
         Point topRR = frame.cameraToRR.pose.imageToRobotXY(Point(centre.x(), tl.y()));
         Point bottomRR =frame.cameraToRR.pose.imageToRobotXY(Point(centre.x(), br.y()));
         int dist = DISTANCE_SQR(topRR.x(), topRR.y(),
                                 bottomRR.x(), bottomRR.y());
         if ((dist < PENALTY_SPOT_MIN) || (dist > PENALTY_SPOT_MAX)) {
            //std::cout << "kinematics distance says no" << dist << std::endl;
            continue;
         }

         PenaltySpotInfo p (centre, w, h);
         RRCoord rr = frame.cameraToRR.pose.imageToRobotRelative(centre);
         FieldFeatureInfo f (rr, p);
         fieldFeatures.push_back(f);
         return true;
      }
   }
   return false;
}

void FieldLineDetection::makeWhiteHistsBelowFieldEdge(
      VisionFrame &frame,
      const Fovea &fovea,
      Histogram<int, cNUM_COLOURS> &xhist,
      Histogram<int, cNUM_COLOURS> &yhist)
{
   typedef int (*histogram_ptr_t)[cNUM_COLOURS];

   Point start, end;
   int x, y;
   int fieldTop;

   const Colour *saliencyPixel, *saliencyEnd;

   for (y = 0; y < fovea.bb.height(); ++ y) {
      yhist._counts[y][cWHITE] = 0;
   }
   for (x = 0; x < fovea.bb.width(); ++ x) {
      xhist._counts[x][cWHITE] = 0;
   }

   histogram_ptr_t yhist_ptr, xhist_ptr = xhist._counts;

   for (x = 0; x < fovea.bb.width(); ++ x) {

      /* Take advantage of sequencial vertical access */
      start = fovea.mapFoveaToImage(Point(x, 0));
      fieldTop = frame.topStartScanCoords[start.x()];
      if (!fovea.top) {
         fieldTop =frame.botStartScanCoords[start.x()];
      }
      start.y() = std::max(start.y(), fieldTop);
      start = fovea.mapImageToFovea(start);

      end = fovea.mapFoveaToImage(Point(x, fovea.bb.height()));
      if (fovea.top) {
         end.y() =std::min(end.y(),frame.cameraToRR.topEndScanCoords[end.x()]);
      } else {
         end.y() = TOP_IMAGE_ROWS +
               std::min(end.y(),frame.cameraToRR.botEndScanCoords[end.x()]);
      }
      end = fovea.mapImageToFovea(end);

      end.y() = std::max(end.y(), 0);
      end.y() = std::min(end.y(), fovea.bb.height());

      start.y() = std::max(start.y(), 0);
      start.y() = std::min(start.y(), end.y());

      yhist_ptr = yhist._counts + start.y();

      saliencyPixel = fovea._colour + start.x() * fovea.bb.height() + start.y();
      saliencyEnd   = fovea._colour + end.x()   * fovea.bb.height() + end.y();

      while (saliencyPixel != saliencyEnd) {
         if (*saliencyPixel == cWHITE) {
            ++ (*xhist_ptr)[cWHITE];
            ++ (*yhist_ptr)[cWHITE];
         }

         ++ saliencyPixel;
         ++ yhist_ptr;
      }
      ++ xhist_ptr;
   }
}
