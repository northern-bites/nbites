#include "perception/vision/FieldEdgeDetection.hpp"

#include <limits>

#include "Ransac.hpp"

#include "utils/Logger.hpp"
#include "utils/LeastSquaresLine.hpp"
#include "utils/basic_maths.hpp"
#include "utils/angles.hpp"
#include "utils/SPLDefs.hpp"

using namespace std;


const int FieldEdgeDetection::consecutive_green = 2;

FieldEdgeDetection::FieldEdgeDetection() {
   // reserve space for the maximum number of field edge points we may have
   // note, this is a lot faster than letting the vector resize itself
   edgePointsTop.reserve(TOP_SALIENCY_COLS);
   edgePointsBot.reserve(BOT_SALIENCY_COLS);
}

void FieldEdgeDetection::findFieldEdges(VisionFrame &frame,
                                   const Fovea &topFovea,
                                   const Fovea &botFovea,
                                   CameraToRR *convRR,
                                   unsigned int *seed) {
   // Reset Variables
   edgePointsBot.clear();
   edgePointsTop.clear();
   fieldEdges.clear ();

   // Run on top fovea
   fieldEdgePoints(frame, topFovea, true);
   fieldEdgeLines(seed, convRR, true);
   
   findStartScanCoords(frame, topFovea);

   // Repeat on bottom fovea
   fieldEdgePoints(frame, botFovea, false);
   fieldEdgeLines(seed, convRR, false);
   
   findStartScanCoords(frame, botFovea);

}




/**
 * Edge Point Detection
 *
 * 1. Divide frame into two sections: top and bottom.
 *    top is defined as the projected width of a field line
 *    at the top of the image using camera to RR
 * 
 * 2. Detect edges
 * psuedo code for bottom of image
 * for each pixel in column starting from top {
 *    if (`consecutive_green' pixels below are green) {
 *       add point
 *    }
 *    if (pixels below contain `consecutive_green' green pixels
 *     && contain non-consecutive white or unclassified pixels)
 *    {
 *       add point
 *    }
 *    if (current pixel is white and either of above conditions are true) {
 *       add point
 *    }
 * }
 *
 * psuedo code for top of image
 * for each pixel in column starting from top {
 *    if (so far only white and unclassified pixels have been seen) {
 *       continue
 *    }
 *
 *    as above
 *
 * }
 */
void FieldEdgeDetection::fieldEdgePoints(
      VisionFrame &frame,
      const Fovea &fovea,
      bool top)
{

   /* Old parameter hax */
   const std::pair<int, int> &horizon = frame.cameraToRR.pose.getHorizon();
   /* End hax */

   const int COLS = (top) ? TOP_IMAGE_COLS : BOT_IMAGE_COLS;
   const int ROWS = (top) ? TOP_IMAGE_ROWS : BOT_IMAGE_ROWS;

   const Pose &pose = frame.cameraToRR.pose;

   float gradient = (horizon.second - horizon.first) / float(COLS) / fovea.density;

   int intercept = horizon.first / fovea.density;

   int i, j, start;
   int image_end, fovea_end;
   int horizon_ave;
   int fieldline_width_at_top;
   int green_count, white_count, overshoot;
   bool possible_fieldline;
   const Colour *c;


   /* Calculate width of field line if it were to be present at the top of
    * the field.
    */
   Point topRR, topImage, topFovea;
   
   horizon_ave = (horizon.first + horizon.second) / 2 / fovea.density;
   if (horizon_ave < 0) {
      horizon_ave = 0;
   }

   /* Only calculate fieldline width if the horizon is not at the
    * top of the image.
    */
   if (horizon_ave == 0) {
//      if (top) {
         topRR = pose.imageToRobotXY(Point(COLS/ 2, horizon_ave));
//      } else {
//         topRR = pose.imageToRobotXY(
//            Point(IMAGE_WIDTH / 2, horizon_ave + IMAGE_ROWS));
//      }

      static const float width_tol = 1.5;
      topRR.x() -= (FIELD_LINE_WIDTH * width_tol);
      if (topRR.x() < 0) {
         fieldline_width_at_top = 0;
      } else {
         topImage = pose.robotToImageXY(topRR);
         topImage.y();
         if (topImage.y() > ROWS) {
            topImage.y() = ROWS;
         }
         topFovea = fovea.mapImageToFovea (topImage);
         fieldline_width_at_top = topFovea.y();
      }
   } else {
      fieldline_width_at_top = 0;
   }

   int cols = TOP_SALIENCY_COLS;
   if (!top) cols = BOT_SALIENCY_COLS;
   for (i = 0; i < cols; ++i) {
      if (i != 0) {
         greenTops[i] = greenTops[i - 1];
      } else {
         greenTops[0] = 0;
      }

      float horizonIntercept = gradient * i + intercept;
      if (!top) horizonIntercept -= ROWS / fovea.density;
      start = std::min(std::max(0.0f, horizonIntercept), (float)(ROWS / fovea.density)); //start of the scan must not got below the image

      if (top) {
         image_end = frame.cameraToRR.topEndScanCoords
                     [fovea.mapFoveaToImage(Point(i, 0)).x()];
      } else {
         image_end = frame.cameraToRR.botEndScanCoords
                     [fovea.mapFoveaToImage(Point(i, 0)).x()];
      }
      fovea_end = fovea.mapImageToFovea(Point(0, image_end)).y();

      white_count = 0;
      overshoot   = 0;

      j = start;
      c = &fovea.colour(i, j);

      /* Test for fieldline at top of image */
      possible_fieldline = false;
      for (; j < fieldline_width_at_top; ++ j, ++ c) {
         if (*c == cWHITE) {
            possible_fieldline = true;
            break;
         }
      }
      
      /* Now search for the field edge */
      green_count =  0;
      white_count =  0;
      overshoot   = -1;

      c -= (j - start);
      j  = start;

      // NOTE: REMOVE THIS AFTER BRAZIL, THIS IS A SPECIAL CASE FOR THE NICE BLACK BORDER WE GET
      //bool seenBlack = false;
      for (; j < fovea_end; ++ j, ++ c) {

         //if (*c == cBLACK) seenBlack = true;
         //if (!seenBlack) continue;

         if (*c == cFIELD_GREEN) {
            ++ green_count;
            ++ overshoot;
            white_count = 0;

            if (green_count == consecutive_green) {
               j -= overshoot;

               /* Check that we didn't detect a fieldline */
               if (possible_fieldline && j < start + fieldline_width_at_top) {
                  break;
               }
               if (j != 0) {
                  if (top) {
                     edgePointsTop.push_back(Point(i, j) * fovea.density);
                  } else {
                     Point p = Point(i, j) * fovea.density;
                     p.y() += ROWS;
                     edgePointsBot.push_back(p);
                  }
               } else {
                  ++ greenTops[i];
               }

               break;
            }
         } else if ((*c == cWHITE || *c == cUNCLASSIFIED)
                 && (white_count < 1 && green_count != 0))
         {
            /* Allow for white between two green pixels */
            ++ overshoot;
            ++ white_count;
         } else if (*c == cWHITE && green_count == 0) {
            /* first pixel can be white */
            overshoot   = 0;
            white_count = 1;
         } else {
            green_count =  0;
            white_count =  0;
            overshoot   = -1;
         }
      }
   }
}

void FieldEdgeDetection::fieldEdgeLines(unsigned int *seed,
                                        CameraToRR *convRR,
                                        bool top)
{

   // Set up some constants based on the camera
   const int COLS = (top) ? TOP_IMAGE_COLS : BOT_IMAGE_COLS;
   const int ROWS = (top) ? TOP_IMAGE_ROWS : BOT_IMAGE_ROWS;
   int density = (top) ? TOP_SALIENCY_DENSITY : BOT_SALIENCY_DENSITY;
   int cols = (top) ? TOP_SALIENCY_COLS : BOT_SALIENCY_COLS;

   /* some magic constants (see RANSAC documentation) */
   static const unsigned int k = 40;
   static const float        e = 8.0;
   static const unsigned int n = (140 / density);

   /* If there are n or greater green tops, which usually fall below
    * the field edge, above a field line, invalidate it
    */
   static const int invalidateIfNTops  = 40  / density;

   unsigned int i;

   std::vector<Point> *edgePoints;
   if(top) edgePoints = &edgePointsTop;
   else edgePoints = &edgePointsBot;

   std::vector<bool> *cons, consBuf[2];
   consBuf[0].resize(cols * 2);
   consBuf[1].resize(cols * 2);

   std::vector<RANSACLine> lines(1);
   RANSACLine result;

   RANSAC::Generator<RANSACLine> g;
   RANSAC::Ransac<RANSACLine> ransac;

   if (ransac(g, *edgePoints, &cons, lines[0], k, e, n, consBuf, seed)) {

      /**
       * Line found, now try find a second line.
       * First remove points belonging to the first line
       * Then run RANSAC again
       */ 
      std::vector<Point> edgePoints2;
      for (i = 0; i < edgePoints->size(); ++i) {
         if (! (*cons)[i]) {
            edgePoints2.push_back(edgePoints->at(i));
         }
      }

      lines.resize(2);
      if (! ransac(g, edgePoints2, &cons, lines[1], k, e, n, consBuf, seed)) {
         lines.resize(1);
      }

   } else {
      lines.resize(0);
   }

   /* Invalidate any line with too much green above it */
   std::vector<RANSACLine>::iterator line;
   for (line = lines.begin(); line != lines.end(); ++ line) {
      int n;
      if (line->t1 == 0) {
         n = greenTops[cols - 1];
      } else {
         /* Solve for x when y = SALIENCY_DENSITY. At y = 0 aliased green
          * pixels from a legitimate field edge can be along the top of the
          * image
          *
          * t1x + t2y + t3 = 0
          * x = -(t3 + t2*SALIENCY_DENSITY) / t1
          */
         int x;
         if (top) {
            x = -(line->t3 + line->t2 * density) / line->t1;
         } else {
            x = -(line->t3 + line->t2*(density+ROWS)) /line->t1;
         }

         /* Convert to saliency image resolution */
         x /= density;

         if (x < 0 || x >= cols) {
            n = greenTops[cols - 1];
         } else {
            bool positiveSlope = (line->t1 >= 0) == (line->t2 >= 0);
            if (positiveSlope) {
               n = greenTops[x];
            } else {
               n = greenTops[cols - 1] - greenTops[x];
            }
         }
      }
      if (n >= invalidateIfNTops) {
         line = lines.erase(line) - 1;
      }
   }

   if (lines.size() == 2) {
      /**
       * begin sanity checks, sort lines by gradient
       */
      RANSACLine *l1;
      RANSACLine *l2;

      if (abs(lines[0].t2 * lines[1].t1) < abs(lines[1].t2 * lines[0].t1)) {
         l1 = &lines[0];
         l2 = &lines[1];
      } else {
         l1 = &lines[1];
         l2 = &lines[0];
      }

      /**
       * sanity check 1: avoid lines that intersect outside the image
       */
      llog(DEBUG2) << "sanity check 1: intersect inside frame" << endl;
      float x_intercept = (l1->t3*l2->t2 - l2->t3*l1->t2) /
                          (float)(l1->t2*l2->t1 - l1->t1*l2->t2);
      float y_intercept = (l1->t3*l2->t1 - l2->t3*l1->t1)/
                          (float)(l1->t1*l2->t2 - l2->t1*l1->t2);

      if (x_intercept < 0 || x_intercept > COLS
       || y_intercept < 0 || y_intercept > (2*ROWS))
      {
         lines.pop_back();
      }

      if (lines.size() == 2) {
         /**
          * sanity check 2: avoid lines that are close to being parallel
          */
         llog(DEBUG2) << "sanity check 2: angle between lines" << endl;
         RANSACLine rrl1 = convRR->convertToRRLine(*l1);
         RANSACLine rrl2 = convRR->convertToRRLine(*l2);
         float m1 = -rrl1.t1 / (float) rrl1.t2;
         float m2 = -rrl2.t1/ (float) rrl2.t2;
         float theta = atan(abs((m1 - m2) / (1 + m1 * m2)));
         // float deviation = M_PI / 2 - theta;
         // if (deviation > MAX_EDGE_DEVIATION_FROM_PERPENDICULAR)
         if (theta < MIN_ANGLE_BETWEEN_EDGE_LINES)
         {
            lines.pop_back();
            llog(DEBUG2) << "Failed sanity check 2" << std::endl;
            llog(DEBUG2) << "m1: " << m1 << ", m2: " << m2 << ", theta: " << RAD2DEG(theta) << std::endl;
         }
      }
   }

   for (i = 0; i < lines.size(); ++ i) {
      RRCoord r1, r2;

      r1 = convRR->convertToRR(lines[i].p1, false);
      r2 = convRR->convertToRR(lines[i].p2, false);

      int32_t x1 = r1.distance() * cos(r1.heading());
      int32_t y1 = r1.distance() * sin(r1.heading());
      int32_t x2 = r2.distance() * cos(r2.heading());
      int32_t y2 = r2.distance() * sin(r2.heading());

      // Find perpendicular distance of this line from the origin
      float dist = abs((x2-x1)*y1 - (y2-y1)*x1)/
         sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));

      float var = SQUARE(1000) + dist + lines[i].var * 1000;
      RANSACLine rrLine(Point(x1, y1), Point(x2, y2), var);
      //std::cout << lines[i].p1 << std::endl << lines[i].p2 << std::endl;
      fieldEdges.push_back(FieldEdgeInfo(rrLine, lines[i]));
   }
}

void FieldEdgeDetection::findStartScanCoords(VisionFrame &frame,
                                             const Fovea &fovea) {

   const int COLS = (fovea.top) ? TOP_IMAGE_COLS : BOT_IMAGE_COLS;
   const int ROWS = (fovea.top) ? TOP_IMAGE_ROWS : BOT_IMAGE_ROWS;
   int cols = (fovea.top) ? TOP_SALIENCY_COLS : BOT_SALIENCY_COLS;
   const std::pair<int, int> &horizon = frame.cameraToRR.pose.getHorizon();
   float gradient = (horizon.second - horizon.first) / float(COLS);
   int intercept = horizon.first / fovea.density;

   uint16_t i;
   int32_t intersec;

   /* Calculate the number of field edges in this image */
   int numEdges = 0;
   for (vector<FieldEdgeInfo>::const_iterator it = fieldEdges.begin();
        it != fieldEdges.end(); ++ it) {
      if ((*it).imageEdge.p1.y() < ROWS && fovea.top) {
         numEdges++;
      } else if ((*it).imageEdge.p1.y() > ROWS && !fovea.top) {
         numEdges++;
      }
   }

   /* If there are no lines, work out if the entire image consists of
    * the field or the entire image is of the background.
    */
   if (numEdges == 0) {
      int numGreen = 0;
      for (int x = 0; x < cols; x++) {
         if (fovea.colour(x,0) == cFIELD_GREEN) {
            numGreen++;
         }
      }
      if (numGreen < MIN_GREEN_THRESHOLD) {
         for (int x = 0; x < COLS; x++) {
            // There is no field seen, so give all the array values that
            // far exceed the number of rows in the saliency scan so that
            // no scan lines are used
            if (fovea.top) {
               topStartScanCoords[x] = ROWS;
            } else {
               botStartScanCoords[x] = ROWS*2;
            }
         }
      } else {
         if (fovea.top) {
            for (int x = 0; x < COLS; x++) {
               int horizonIntercept = gradient * x + intercept;
               topStartScanCoords[x] = std::max(0, horizonIntercept);
            }
         } else {
            for (int x = 0; x < COLS; x++) {
               botStartScanCoords[x] = ROWS;
            }
         }
      }
   } else {
      for (int x = 0; x < COLS; x++) {
         int maxVal = -1;
         for (i = 0; i < fieldEdges.size(); ++ i) {
            const RANSACLine &line = fieldEdges[i].imageEdge;
            if (line.p1.y() < ROWS && fovea.top) {
               intersec = (-line.t3 - (x * line.t1))/line.t2;
               if (intersec > maxVal) {
                  maxVal = intersec;
               }
            } else if (line.p1.y() >= ROWS && !fovea.top) {
               intersec = (-line.t3 - (x * line.t1))/line.t2;
               if (intersec > maxVal) {
                  maxVal = intersec;
               }
            }
         }
         if (maxVal < 0) {
            maxVal = 0;
         }
         if (fovea.top) {
            topStartScanCoords[x] = std::max(0, std::min(ROWS, maxVal));
         } else {
            botStartScanCoords[x] =
                  std::max(ROWS, std::min(ROWS*2, maxVal));
         }
      }
   }
}

Point FieldEdgeDetection::adjustVertEdgePoint(
      VisionFrame &frame,
      const Fovea &fovea,
      Point p)
{
   const int saliency_backtrace = 2;
   const int adjust_density     = 2;
   const int adjust_step        = adjust_density * IMAGE_ROW_STEP;

   Point image   = fovea.mapFoveaToImage(p);
   int   y_start = std::max<int>(image.y() - saliency_backtrace * fovea.density, 0);
   int   y_end   = image.y();

   const uint8_t *ss, *se;
   ss = frame.topImage + (y_start * IMAGE_COLS + image.x()) * IMAGE_DEPTH;
   se = frame.topImage + (y_end   * IMAGE_COLS + image.x()) * IMAGE_DEPTH;

   int green_count = 0;

   for (; ss < se; ss += adjust_step) {
      /* TODO (carl) use the fast classifier */
      if (frame.topNnmc.classify(ss) == cFIELD_GREEN) {
         ++ green_count;
         if (green_count == consecutive_green) {
            Point ret(image.x(), y_start + (se - ss) / IMAGE_ROW_STEP);
            /* Remove the excess from counting consecutive pixels */
            ret.y() -= consecutive_green - 1;
            return ret;
         }
      } else {
         green_count = 0;
      }
   }

   return Point(-1, -1);
}

void FieldEdgeDetection::lsRefineLine(
      RANSACLine               &line,
      const std::vector<Point> &points,
      const std::vector<bool>  &cons)
{
   LeastSquaresLine ls;
   std::vector<Point>::const_iterator p = points.begin();
   std::vector<bool> ::const_iterator c = cons.begin();

   for (; p != points.end(); ++ p, ++ c) {
      if (*c) {
         ls.addPoint(*p);
      }
   }

   ls.getLineABC(&line.t1, &line.t2, &line.t3);
}

