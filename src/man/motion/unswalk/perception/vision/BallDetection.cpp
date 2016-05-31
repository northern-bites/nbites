#include "BallDetection.hpp"

#include <algorithm>
#include <limits>
#include <vector>

#include "Ransac.hpp"

#include "utils/Logger.hpp"
#include "utils/basic_maths.hpp"
#include "utils/BresenhamPtr.hpp"
#include "utils/SPLDefs.hpp"
#include "utils/Timer.hpp"

#include "types/Point.hpp"
#include "types/XYZ_Coord.hpp"

//#include "../../motion/touch/FilteredTouch.hpp"

using namespace std;
using namespace boost::numeric::ublas;

const int BallDetection::trackingSizeX             = 160;//160;
const int BallDetection::trackingSizeY             = 160;//160;

const int BallDetection::ballEdgeThreshold         = 150;
const int BallDetection::ballCloseThreshold        = 450;
const int BallDetection::ballColourRatio           = 512; /* ratio * 1024 */

const int BallDetection::dontTrustKinematicsBeyond = 100;
const int BallDetection::maxPixelsAboveFieldEdge   = 6;// * IMAGE_COLS / 640;

const int BallDetection::maxTrackBallRadius        = 8;

const int BallDetection::maxBallHintAge            = 80;
const int BallDetection::ballHintEdgeThreshold     = 100;

BallDetection::BallDetection()
{
   ballHintAge = 0;
   ball_dx = 0;
}

/* Sort balls according to likelyhood of truth */
struct BallInfoCmp
{
   bool operator() (const BallInfo &a, const BallInfo &b) const
   {
      return a.visionVar < b.visionVar;
   }
};

void BallDetection::findBalls(
      VisionFrame &frame, 
      const Fovea &topFovea,
      const Fovea &botFovea,
      unsigned int *seed)
{
   ballFoveas.clear();
   trackingFoveas.clear();
   points.clear();

   // Update angleX to take into account motion rocking and sensor delay
   for(int i = ANGLE_DELAY-1; i > 0; --i) {
      angleXDelay[i] = angleXDelay[i-1];
   }
   angleXDelay[0] = latestAngleX;

   findBallsR(frame, botFovea, seed);
   if (frame.balls.size() == 0) {
      findBallsR(frame, topFovea, seed);
   }
}

void BallDetection::findBallsR(
      VisionFrame &frame, 
      const Fovea &fovea,
      unsigned int *seed,
      bool terminal)
{

   std::vector<ball_seed_t> seeds;
   populateSeedLocations(frame, fovea, seeds);

   std::vector<ball_seed_t>::reverse_iterator i, j;
   for (i = seeds.rbegin(); i != seeds.rend(); ++ i) {
      for (j = i + 1; j != seeds.rend(); ++ j) {
         if (doSeedsOverlap(*i, *j)) {
            seeds.erase(i.base() - 1) + 1;
            break;
         }
      }
   }

   std::vector<ball_seed_t>::iterator s;
   for (s = seeds.begin(); s != seeds.end(); ++ s) {
      findBall(frame, fovea, *s, seed);
   }

   /* If no balls where found, attempt to track the last known ball,
    * This function recurses on findBallR, so if terminal is specified
    * dont bother
    */
   if (! terminal && frame.balls.empty() && frame.last) {
      if (! frame.last->balls.empty()) {

         /* Only bother trying to track distant balls. If the ball is close
          * and we missed it we will probably get it next frame any way
          */
         if (frame.last->balls[0].radius < maxTrackBallRadius) {
            llog(DEBUG1) << "findBallsR: using tracking fovea" << std::endl;
            trackLastBall(frame, fovea, frame.last->balls[0], seed);
         } 
      }
   }

   // If we still can't find anything, try looking at the localisation ball
   // This is populated by other teams members even if we can't see the ball
   // Only look if the ball is far away, otherwise its probably occluded
   const int min_teamball_dist = 4000;
   if (! terminal && frame.balls.empty() &&
       localisationBall.distance() > min_teamball_dist) {
      float x = localisationBall.distance() * cos (localisationBall.heading());
      float y = localisationBall.distance() * sin (localisationBall.heading());

      Point lBall = frame.cameraToRR.convertToImageXY(Point(x,y));
      if ((lBall.x() > 0) && (lBall.x() < TOP_IMAGE_COLS) &&
          (lBall.y() > 0) && (lBall.y() < TOP_IMAGE_ROWS)) {
         BallInfo ball;
         ball.imageCoords = lBall;
         trackLastBall(frame, fovea, ball, seed);
      }
   }

   std::vector<BallInfo>::iterator b;
   for (b = frame.balls.begin(); b != frame.balls.end(); ++ b) {
      if (isBallRadiusTooBig(frame, *b)) {
         llog(DEBUG1) << "findBallsR: isBallRadiusTooBig (failed)(";
         llog(DEBUG1) << b->imageCoords.x() << ","  << b->imageCoords.y();
         llog(DEBUG1) << ")" << std::endl;
         b = frame.balls.erase(b) - 1;
      }
   }

   std::sort(frame.balls.begin(), frame.balls.end(), BallInfoCmp());

   /* Throw out excess balls */
   if (frame.balls.size() > MAX_BALLS) {
      frame.balls.resize(MAX_BALLS);
   }
}

void BallDetection::findBall(
      VisionFrame &frame, 
      const Fovea &fovea,
      ball_seed_t &ball,
      unsigned int *seed)
{
   const int desiredBallRadius = 12;

   int density;
   if (ball.radius * fovea.density / 1 < desiredBallRadius) {
      density = 1;
   } else if (ball.radius * fovea.density / 2 < desiredBallRadius) {
      density = 2;
   } else if (ball.radius * fovea.density / 4 < desiredBallRadius) {
      density = 4;
   } else if (ball.radius * fovea.density / 8 < desiredBallRadius) {
      density = 8;
   } else {
      /* Ball is too big for saliency based methods. Use large ball
       * detector instead
       */
      density = 8; /* -1 */
   }

   llog(DEBUG1) << "findBall: seed at (" << ball.centre.x() << ",";
   llog(DEBUG1) << ball.centre.y() << ") radius = " << ball.radius;
   llog(DEBUG1) << std::endl;

   points.push_back(fovea.mapFoveaToImage(ball.centre));

   // Previous calculations were for 640x480 image
   // Now that we use 1280x960, need to double density for top camera
   if (fovea.top) density *= 2;

   std::vector<Point> edges;
   if (density == -1) {
      /* use large ball detector */
      /* TODO(carl) write a large ball detector */
   } else {
      /* Create a zoomed in ball fovea */
      int size = ball.radius + (8 / fovea.density) + (density / 4);
      int extraW = 0; //2*size; part of refereeing code
      int extraH = 0; //size;
      Point tl(std::max(ball.centre.x() - (size + extraW), 0),
               std::max(ball.centre.y() - (size + extraH), 0));

      Point br(std::min(ball.centre.x() + (size + extraW), fovea.bb.width ()),
               std::min(ball.centre.y() + (size + extraH), fovea.bb.height()));

      tl = (tl + fovea.bb.a) * fovea.density / density;
      br = (br + fovea.bb.a) * fovea.density / density;

      boost::shared_ptr<FoveaT<hBall, eBall> > ballFovea(
            new FoveaT<hBall, eBall>(BBox(tl, br), density, 0, fovea.top));

      ballFovea->actuate(frame);
      ballFovea->xhistogram.applyWindowFilter(HIST_AVE_WINDOW_SIZE, hBall);
      ballFovea->yhistogram.applyWindowFilter(HIST_AVE_WINDOW_SIZE, hBall);

      /* Update ball seed. */
      ball.centre = fovea.mapFoveaToImage(ball.centre);
      ball.centre = ballFovea->mapImageToFovea(ball.centre);

      if (density < (int)fovea.density) {
         /* Refine the radius guestimate */
         ball.radius = ballRadiusFromHistogram(ballFovea->asFovea(),
                                               ballFovea->xhistogram,
                                               ballFovea->yhistogram,
                                               ball);
      }

      ballFoveas.push_back(ballFovea);

      findBallEdges(ballFovea->asFovea(), edges);

      std::vector<Point>::iterator p;
      for (p = edges.begin(); p != edges.end(); ++ p) {
         points.push_back(ballFovea->mapFoveaToImage(*p));
      }
      ransacBall(frame, ballFovea->asFovea(), edges, ball.radius, seed);
   }
}

void BallDetection::trackLastBall(
      VisionFrame &frame, 
      const Fovea &fovea, 
      const BallInfo &ball,
      unsigned int *seed)
{
   /* Make a new fovea somewhere around where we think the ball will be. Use a
    * relatively high resolution.
    *
    * Note: cost of creating a new fovea is determined by two factors.
    * 1. Area
    * 2. Are edge values calculated.
    *
    * For the tracking, we have two options, create one big fovea around
    * where the ball was last seen, or create an intermediate fovea
    * at half full resolution, and then create a full resolution fovea.
    * As we are only tracking distant balls, it really doesnt make sense
    * to attempt to find the ball in a subsampled fovea. Therefore the
    * costs of our two options are:
    *
    * Area is the area of the tracking fovea
    * 30x30 is an observation that zoom foveas tend to be of this size
    * density is the density of the intermediate fovea
    *
    * 1. Using an intermediate fovea:
    *    Area * colour_cost / density**2 + 30x30 * (colour_cost + edge_cost)
    * 2. Using full fovea
    *    Area * (colour_cost + edge_cost)
    *
    * It should be clear that creating the intermediate fovea will save us
    * processing time unless we can get the tracking fovea to be small.
    */

   const int density = 2;
   const int sizex = trackingSizeX / 2;
   const int sizey = trackingSizeY / 2;

   const int W = (fovea.top) ? TOP_IMAGE_COLS : BOT_IMAGE_COLS;
   const int H = (fovea.top) ? TOP_IMAGE_ROWS : BOT_IMAGE_ROWS;

   const Point centre = ball.imageCoords;
   if (centre.y() > TOP_IMAGE_ROWS && fovea.top) return;
   if (centre.y() < TOP_IMAGE_ROWS && !fovea.top) return;
   
   Point tl(std::max(centre.x() - sizex, 0),
            std::max(centre.y() - sizey, 0));

   Point br(std::min(centre.x() + sizex, W),
            std::min(centre.y() + sizey, H));

   /* If part of the tracking fovea is above the field edge
    * remove it
    */
   int fieldTopL = frame.topStartScanCoords[tl.x()] - maxPixelsAboveFieldEdge;
   int fieldTopR = frame.topStartScanCoords[br.x()] - maxPixelsAboveFieldEdge;
   if (!fovea.top) {
      fieldTopL = frame.botStartScanCoords[tl.x()] - maxPixelsAboveFieldEdge;
      fieldTopR = frame.botStartScanCoords[br.x()] - maxPixelsAboveFieldEdge;
   }
  
   int fieldTop  = std::min(fieldTopR, fieldTopL);


   tl.y() = std::max(fieldTop, tl.y());

   tl /= density;
   br /= density;

   if (tl.y() >= br.y()) {
      return;
   }

   boost::shared_ptr<FoveaT<hNone, eNone> > trackingFovea(
         new FoveaT<hNone, eNone>(BBox(tl, br), density, 0, fovea.top));

   trackingFovea->actuate(frame);

   trackingFoveas.push_back(trackingFovea);

   findBallsR(frame, trackingFovea->asFovea(), seed, true);
}

void BallDetection::ransacBall(
      VisionFrame &frame,
      const Fovea &fovea,
      const std::vector<Point> &ballEdges,
      int radius,
      unsigned int *seed)
{
   static std::vector<bool> buf[2];

   /* Dynamically resize buffers whenever needed, after a few iterations,
    * this will not be needed any more
    */
   if (buf[0].size() < ballEdges.size()) {
      buf[0].resize(ballEdges.size());
      buf[1].resize(ballEdges.size());
   }

   std::vector<bool> *cons;
   RANSACCircle result;

   const float ballError   = 1.5;//1.5;
   const float radiusError = 3; //2 * ballError;

   if (RANSAC::findCircleOfRadius3P(ballEdges, radius, radiusError,
            &cons, result, 50, ballError,
            ballEdges.size() / 2, buf, seed))
   {
      BallInfo best;
      best.imageCoords = result.centre.cast<int>();
      best.radius      = result.radius * fovea.density;
      best.visionVar   = result.var / best.radius;
      best.imageCoords = fovea.mapFoveaToImage(best.imageCoords);
      best.topCamera   = fovea.top;

      if (best.visionVar < -0.1) {
         llog(DEBUG1) << "tossing ball due to var = " << best.visionVar << std::endl;
         return;
      }

      /* Compensate for sparse sampling
       * Diameter is increased by density / 2, Radius by 4
       * Centre is increased by density * 1
       * 0.5 * sampling + 0.5 * edge image offset
       */
      int compensation = fovea.density;
      best.imageCoords += Point(compensation, compensation);
      best.radius += fovea.density / 4;

      // Check centre of ball isn't green
      Point p = fovea.mapImageToFovea(best.imageCoords);
      if (fovea.bb.validIndex(p)) {
         Colour c = fovea.colour(fovea.mapImageToFovea(best.imageCoords));
         if (c == cFIELD_GREEN) {
            llog(DEBUG1) << "centre of the ball is green" << std::endl;
            return;
         }
      }

      // Check bottom of ball is below field edge
      int baseY = best.imageCoords.y() + best.radius;
      int fieldEdgeY = frame.topStartScanCoords[best.imageCoords.x()];
      if (!fovea.top) fieldEdgeY = frame.botStartScanCoords[best.imageCoords.x()];
      if (baseY < fieldEdgeY) {
         llog(DEBUG1) << "tossing ball because above field edge" << std::endl;
         return;
      }

      Point b = frame.cameraToRR.pose.imageToRobotXY(
            best.imageCoords,
            BALL_RADIUS);

      float diff = 190*tan(angleXDelay[0]);

//      std::cout << "d=" << diff << std::endl;
//      std::cout << "r=" << b.y() << std::endl;

      b.y() -= diff;
      RRCoord rr;
      rr.distance() = hypotf(b.y(), b.x());
      rr.heading() = atan2f(b.y(), b.x());
      best.rr = rr;

//      std::cout << "f=" << b.y() << std::endl;

      //best.rr = frame.cameraToRR.pose.imageToRobotRelative(
      //      best.imageCoords,
      //      BALL_RADIUS);

      float robot_height = 500;
      float error = 30 * best.rr.distance() / robot_height;
      best.rr.distance() -= error;
      
      best.neckRelative = frame.cameraToRR.pose.robotRelativeToNeckCoord(best.rr, BALL_RADIUS);

      frame.balls.push_back(best);

      /******************************************/
////      static int xOffset = 330;
//      static TorsoStateFilter kf;
//      static int yOffset = 395;//370;
//      static float prev = DEG2RAD(-(best.imageCoords.y() - yOffset)*34.8/480.0);
//      static Timer t;
//      float dt = t.elapsed_us()/1000000.0;
//      kf.init(dt, 0.0008, 0.024, false);
////      cout << dt << endl;
////      float dt = 1/30.0;
//      static matrix<float> curObs(2, 1);
//      curObs(0, 0) = DEG2RAD(-(best.imageCoords.y() - yOffset)*34.8/480.0);
//      curObs(1, 0) = (curObs(0, 0) - prev)/dt;
//      float footPos[2][3];
//      float CoP[2][2];
//      for(int i = 0; i < 3; i++){
//         footPos[0][i] = FilteredTouch::lf(i, 0);
//         footPos[1][i] = FilteredTouch::rf(i, 0);
//      }
//      CoP[0][0] = FilteredTouch::state.sensors[Sensors::LFoot_FSR_CenterOfPressure_X] * 1000;
//      CoP[0][1] = FilteredTouch::state.sensors[Sensors::LFoot_FSR_CenterOfPressure_Y] * 1000;
//      CoP[1][0] = FilteredTouch::state.sensors[Sensors::RFoot_FSR_CenterOfPressure_X] * 1000;
//      CoP[1][1] = FilteredTouch::state.sensors[Sensors::RFoot_FSR_CenterOfPressure_Y] * 1000;
//      matrix<float> est = kf.update(curObs, CoP, footPos);
//
//      FilteredTouch::ang = est(0, 0);
//      FilteredTouch::vel = est(1, 0);
//      FilteredTouch::ang2 = curObs(0, 0);
//      FilteredTouch::vel2 = curObs(1, 0);
//      prev = curObs(0, 0);
//      t.restart();
      /**************************************/

      llog(DEBUG1) << "ransacBall: ball at (" << best.imageCoords.x() << ",";
      llog(DEBUG1) << best.imageCoords.y() << ") radius = " << best.radius;
      llog(DEBUG1) << std::endl;
   }
   llog(DEBUG1) << "ransacBall: failed" << std::endl;
}

void BallDetection::populateSeedLocations(
      VisionFrame &frame,
      const Fovea &fovea,
      std::vector<ball_seed_t> &seeds)
{
   const int min_edge_area = 12;

   Histogram<int, cNUM_COLOURS> xhist(fovea.bb.width());
   Histogram<int, cNUM_COLOURS> yhist(fovea.bb.height());

   makeBallHistsBelowFieldEdge(frame, fovea, xhist, yhist);

   typedef std::pair<int, int> cut;
   std::vector<cut> horz, vert;

   int e, s = -1;
   for (e = 0; e < xhist.size; ++ e) {
      if (xhist.counts(e, cBALL) > 0) {
         if (s == -1) {
            s = e;
         }
      } else {
         if (s != -1) {
            horz.push_back(cut(s, e));
            s = -1;
         }
      }
   }
   if (s != -1) {
            horz.push_back(cut(s, e));
   }

   for (e = 0; e < yhist.size; ++ e) {
      if (yhist.counts(e, cBALL) > 0) {
         if (s == -1) {
            s = e;
         }
      } else {
         if (s != -1) {
            vert.push_back(cut(s, e));
            s = -1;
         }
      }
   }
   if (s != -1) {
      vert.push_back(cut(s, e));
   }

   std::vector<cut>::iterator i, j;
   for (j = vert.begin(); j != vert.end(); ++ j) {
      for (i = horz.begin(); i != horz.end(); ++ i) {
         ball_seed_t ball;

         int w = i->second - i->first;
         int h = j->second - j->first;

         /* If on an edge, dont bother looking for small balls */
         if (i->first == 0 || i->second == fovea.bb.width () - 1
          || j->first == 0 || j->second == fovea.bb.height() - 1)
         {
            if (w * h < min_edge_area) {
               continue;
            }
         }

         ball.centre.x() = (i->second + i->first) / 2;
         ball.centre.y() = (j->second + j->first) / 2;

         /* Round radius up */
         ball.radius = std::max((w + 1) / 2, (h + 1) / 2);
         if (checkSeed(fovea, ball)) {
            seeds.push_back(ball);
         }
      }
   }

   /*
   xhist.applyWindowFilter(HIST_AVE_WINDOW_SIZE, hBall);
   yhist.applyWindowFilter(HIST_AVE_WINDOW_SIZE, hBall);

   int x, y;
   for (y = 0; yhist.peaks(y, cBALL) != -1; ++ y) {
      for (x = 0; xhist.peaks(x, cBALL) != -1; ++ x) {
         ball_seed_t ball;
         ball.centre = Point(xhist.peaks(x, cBALL),
                             yhist.peaks(y, cBALL));

         if (guessRadiusAndValidateSeed(frame, fovea, xhist, yhist, ball)) {
            seeds.push_back(ball);
         }
      }
   }
   */
}

void BallDetection::findBallEdges(
      const Fovea &fovea,
      std::vector<Point> &edges)
{
   const int t2 = ballEdgeThreshold * ballEdgeThreshold;

   const int x_ends[16] = {4, 4, 4, 3, 2, 1, 0, 0, 0, 0, 0, 1, 2, 3, 4, 4};
   const int y_ends[16] = {2, 1, 0, 0, 0, 0, 0, 1, 2, 3, 4, 4, 4, 4, 4, 3};

   /* Fovea data is stored in columns, not rows. To use the BresenhamPtr's
    * to iterate across the fovea buffers, invert x and y
    */
   Point start(fovea.xhistogram.maxPeak(cBALL),
               fovea.yhistogram.maxPeak(cBALL));

   /* If start.[xy]() == -1, then there are no peaks, likely because the
    * ball is on the edge of the image. Work out which one
    */
   enum
   {
      edge_top    = 1 << 0,
      edge_bottom = 1 << 1,
      edge_left   = 1 << 2,
      edge_right  = 1 << 3
   } edge;


   if (start.x() == -1) {
      if (fovea.xhistogram.counts(0                   , cBALL) >
          fovea.xhistogram.counts(fovea.bb.width() - 1, cBALL))
      {
         start.x() = 0;
         edge = edge_left;
      } else {
         start.x() = fovea.bb.width() - 1;
         edge = edge_right;
      }
   }

   if (start.y() == -1) {
      if (fovea.yhistogram.counts(0                    , cBALL) >
          fovea.yhistogram.counts(fovea.bb.height() - 1, cBALL))
      {
         start.y() = 0;
         edge = edge_top;
      } else {
         start.y() = fovea.bb.height() - 1;
         edge = edge_bottom;
      }
   }

   int n_ball, n_total, n, green_white_count;;

   int i;
   for (i = 0; i < 16; ++ i) {
      Point end(fovea.bb.height() * y_ends[i] / 4,
                fovea.bb.width()  * x_ends[i] / 4);

      BresenhamPtr<const Colour> cbp(fovea._colour, start, end,
                                     fovea.bb.height());
      BresenhamPtr<const Colour>::iterator cp = cbp.begin();

      BresenhamPtr<const Point> ebp(fovea._edge, start, end,
                                    fovea.bb.height());
      BresenhamPtr<const Point>::iterator ep = ebp.begin();

      Point bestPoint = Point(-1,-1);
      int best = std::numeric_limits<int>::min();

      n_ball = n_total = n = green_white_count = 0;
      while (fovea.bb.validIndex(cp.point())) {
         if (*cp == cFIELD_GREEN || *cp == cWHITE) {
            ++ green_white_count;
         }
         if (green_white_count > 2) {
            break;
         }

         if (*cp == cBALL) {
            ++ n_ball;
         }
         ++ n;

         // Uncomment this to show the scan lines
         /*Colour *c = const_cast<Colour *>(cp.get());
         *c = cBLACK;*/

         if (best < ep->squaredNorm() && ep->squaredNorm() >= t2) {
            best = ep->squaredNorm();
            bestPoint = ep.point();
            n_total = n;
         }
      
         ++ cp, ++ ep;
      }

      if ((n_ball * 1024 > n_total * ballColourRatio) &&
          (bestPoint.x() != -1)) {
         edges.push_back(bestPoint);
      }
   }
}

void BallDetection::findSmallBallEdges(
      const Fovea &fovea,
      std::vector<Point> &edges)
{
   /* Scan 8 times in all directions, rotating pi/4 at each iteration.
    * Scan grid looks something like this
    *
    *     +------+
    *    /|\    /|\
    *   / | \  / | \
    * +<--+--><--+-->+
    * | \ | /  \ | / |
    * |  \|/    \|/  |
    * |  /|\    /|\  |
    * | / | \  / | \ |
    * +<--+--><--+-->+
    *   \ | /  \ | /
    *    \|/    \|/
    *     +------+
    */

   Point start;
   /* Length of edges of square boxes */
   int s_size = fovea.bb.width() / 3;
   /* Length of edges of diamond boxes */
   int d_size = fovea.bb.width() / 4;

   (void)d_size;
   /* Square scans */
   start = Point(fovea.bb.width(), s_size);
   scanBoxForEdges(fovea, start, s_size, 1, 0, edges);
   
}


void BallDetection::scanBoxForEdges(
      const Fovea &fovea,
      const Point start,
      const int size,
      const int dx,
      const int dy,
      std::vector<Point> &edges)
{
   const int minEdge2 = ballEdgeThreshold * ballEdgeThreshold;

   Point s = start, best(0, 0);

   int edge_mag2, best_mag2;
   int x, y;
   do {
      x = s.x();
      y = s.y();

      best_mag2 = 0;
      do {
         const Point &edge = fovea.edge(x, y);
         edge_mag2 = edge[0] * edge[0] + edge[1] * edge[1];

         if (edge_mag2 >= minEdge2 && edge_mag2 >= best_mag2) {
            best_mag2 = edge_mag2;
            best = Point(x, y);
         }

         x += dx;
         y += dy;
      } while (x != s.x() + size * dx || y != s.y() + size * dy);

      if (best_mag2 != 0) {
         edges.push_back(best);
      }

      s.x() -= dy;
      s.y() += dx;
   } while (s.x() != start.x() + size * (-dy) || s.y() != start.y() + size * dx);
}

bool BallDetection::guessRadiusAndValidateSeed(
      VisionFrame &frame,
      const Fovea &fovea,
      const Histogram<int, cNUM_COLOURS> &xhistogram,
      const Histogram<int, cNUM_COLOURS> &yhistogram,
      ball_seed_t &ball)
{
   /*
   const int ballRadiusAtMaxTrustDist =
      ballRadiusFromDistance(dontTrustKinematicsBeyond) / fovea.density;
   */

   const Pose &pose = frame.cameraToRR.pose;
   int distRadius, histRadius;

   RRCoord toBall = pose.imageToRobotRelative(ball.centre, BALL_RADIUS);

   /* TODO(carl) use a variable scheme instead of a hard limit */
   if (false && toBall.distance() < dontTrustKinematicsBeyond) {
      /* Trust the kinematic radius calculation as the word of god */
      // NOT USED SO TOP JUST MADE FALSE TO COMPILE
      bool top = false; //(ball.imageCoords.y <= TOP_IMAGE_ROWS)
      distRadius = ballRadiusFromDistance(toBall.distance(), top);
      ball.radius = distRadius / fovea.density;
   } else {
      /* if there is a ball beyond the trust distance, use the radius
       * as guessed from the histogram. If the histogram guess is larger
       * than the ball radius at the trust limit + a tolerance, invalidate
       * the seed
       */
      histRadius = ballRadiusFromHistogram(fovea, xhistogram, yhistogram, ball);
      /*
      if (histRadius <= ballRadiusAtMaxTrustDist * 1.1) {
      */
         ball.radius = histRadius;
      /*
      } else {
         return false;
      }
      */
   }

   const int C = (fovea.top) ? TOP_IMAGE_COLS : BOT_IMAGE_COLS;
   const int R = (fovea.top) ? TOP_IMAGE_ROWS : BOT_IMAGE_ROWS;

   static const int orangeTestRadius = 1;
   Point orangeTestPoint;
   orangeTestPoint.x() = crop(ball.centre.x(),
                              orangeTestRadius,
                              C - orangeTestRadius - 1
                             );
   orangeTestPoint.y() = crop(ball.centre.x(),
                              orangeTestRadius,
                              R - orangeTestRadius - 1
                             );

   if (countOrangeAround(fovea, ball, orangeTestRadius) < 1) {
      return false;
   }

   return true;
}

int BallDetection::ballRadiusFromDistance(int distance, bool top)
{
   const float PIXEL = (top) ? TOP_PIXEL_SIZE : BOT_PIXEL_SIZE;
   return (1 / PIXEL) * (FOCAL_LENGTH * BALL_RADIUS) / distance;
}

int BallDetection::ballRadiusFromHistogram(
      const Fovea &fovea,
      const Histogram<int, cNUM_COLOURS> &xhistogram,
      const Histogram<int, cNUM_COLOURS> &yhistogram,
      const ball_seed_t &ball)
{
   const int x = ball.centre.x();
   const int y = ball.centre.y();

   const int x_peak  = xhistogram.window(x);
   const int x_start = HIST_AVE_WINDOW_SIZE / 2;
   const int x_end   = xhistogram.size - (HIST_AVE_WINDOW_SIZE - 1) / 2;
   const int x_min   = x_peak < 2 ? 1 : 2;

   const int y_peak  = yhistogram.window(y);
   const int y_start = HIST_AVE_WINDOW_SIZE / 2;
   const int y_end   = yhistogram.size - (HIST_AVE_WINDOW_SIZE - 1) / 2;
   const int y_min   = y_peak < 2 ? 1 : 2;

   /* Initial size is 1 pixel.
    *
    * The sliding window fattens input by 'HIST_AVE_WINDOW_SIZE - 1',
    * so compensate by removing this ammount from the initial count
    *
    * Add 1 such that radius is rounded up at final /2 step
    */
   int x_size = 1 - (HIST_AVE_WINDOW_SIZE - 1) + 1;
   int y_size = 1 - (HIST_AVE_WINDOW_SIZE - 1) + 1;
   
   int i;
   for (i = x + 1; i < x_end && xhistogram.window(i) >= x_min; ++ i) {
      ++ x_size;
   }
   for (i = x - 1; i >= x_start && xhistogram.window(i) >= x_min; -- i) {
      ++ x_size;
   }
   for (i = y + 1; i < y_end && yhistogram.window(i) >= y_min; ++ i) {
      ++ y_size;
   }
   for (i = y - 1; i >= y_start && yhistogram.window(i) >= y_min; -- i) {
      ++ y_size;
   }

   return std::max(x_size / 2, y_size / 2);
}

int BallDetection::countOrangeAround(
      const Fovea &fovea,
      const ball_seed_t &ball,
      const int radius)
{
   int x, y;
   int count = 0;
   for (y = ball.centre.y() - radius; y <= ball.centre.y() + radius; ++ y) {
      for (x = ball.centre.x() - radius; x <= ball.centre.x() + radius; ++ x) {
         if (fovea.colour(x, y) == cBALL) {
            ++ count;
         }
      }
   }

   return count;
}

void BallDetection::makeBallHistsBelowFieldEdge(
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
      yhist._counts[y][cBALL] = 0;
   }
   for (x = 0; x < fovea.bb.width(); ++ x) {
      xhist._counts[x][cBALL] = 0;
   }

   histogram_ptr_t yhist_ptr, xhist_ptr = xhist._counts;

   for (x = 0; x < fovea.bb.width(); ++ x) {

      /* Take advantage of sequencial vertical access */
      start = fovea.mapFoveaToImage(Point(x, 0));
      fieldTop = frame.topStartScanCoords[start.x()] - maxPixelsAboveFieldEdge;
      if (!fovea.top) {
         fieldTop =frame.botStartScanCoords[start.x()]-maxPixelsAboveFieldEdge;
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
         if (*saliencyPixel == cBALL) {
            ++ (*xhist_ptr)[cBALL];
            ++ (*yhist_ptr)[cBALL];
         }

         ++ saliencyPixel;
         ++ yhist_ptr;
      }
      ++ xhist_ptr;
   }
}

bool BallDetection::doSeedsOverlap(
      const ball_seed_t &a,
      const ball_seed_t &b)
{
   int rads = a.radius + b.radius;
   return (a.centre - b.centre).squaredNorm() < rads * rads;
}

bool BallDetection::isBallRadiusTooBig(
      const VisionFrame &frame,
      const BallInfo &ball)
{
   const Pose &pose = frame.cameraToRR.pose;

   RRCoord toBall = pose.imageToRobotRelative(ball.imageCoords, BALL_RADIUS);
   bool top = (ball.imageCoords.y() <= TOP_IMAGE_ROWS);
   int distRadius = ballRadiusFromDistance(toBall.distance(), top);
   if (distRadius * 2 < ball.radius) {
      return true;
   }

   // Also check ball isn't too small
   if (distRadius / 3 > ball.radius) {
      return true;
   }
   return false;
}

int BallDetection::checkSeed(
      const Fovea       &fovea,
      const ball_seed_t &seed)
{
   int numOrange, numGreen;
   int density;

   BBox roi(seed.centre - Point(1, 1),
            seed.centre + Point(2, 2));

   numOrange = countColourInBox(fovea, roi, 1, cBALL);
   if (numOrange < 1) {
      return false;
   }

   /* For one pixel balls that are potentially close to the field edge,
    * search a 5x2 box on and below the ball
    *
    * ..o..
    * .....
    *
    * Otherwise scan around it at radius width
    *
    * ....
    * .oo.
    * .oo.
    * ....
    */
   
   if (seed.radius == 1) {
      roi = BBox(seed.centre - Point(2, 0), seed.centre + Point(3, 2));
      numGreen = countColourInBox(fovea, roi, 1, cFIELD_GREEN);
   } else {
      density = seed.radius / 2;
      if (density == 0) {
         density = 1;
      }

      roi = BBox(seed.centre - Point(seed.radius, seed.radius),
                 seed.centre + Point(seed.radius + 2, seed.radius + 2));
      numGreen = countColourAroundBox(fovea, roi, density, cFIELD_GREEN);
   }


   if (numGreen < 1) {
      return false;
   }

   return true;
}

int BallDetection::countColourInBox(
      const Fovea &fovea,
      BBox         roi,
      int          density,
      Colour       colour)
{
   int x, y;
   int count;

   roi.a.x() = std::max(0, roi.a.x());
   roi.a.y() = std::max(0, roi.a.y());

   roi.b.x() = std::min(fovea.bb.width() , roi.b.x());
   roi.b.y() = std::min(fovea.bb.height(), roi.b.y());

   count = 0;
   for (y = roi.a.y(); y < roi.b.y(); y += density) {
      for (x = roi.a.x(); x < roi.b.x(); x += density) {
         if (fovea.colour(x, y) == colour) {
            ++ count;
         }
      }
   }

   return count;
}

int BallDetection::countColourAroundBox(
      const Fovea &fovea,
      BBox         roi,
      int          density,
      Colour       colour)
{
   int x, y;
   int count;

   bool count_top   = true;
   bool count_left  = true;
   bool count_bot   = true;
   bool count_right = true;

   if (roi.a.x() < 0) {
      count_left = false;
      roi.a.x() = 0;
   }

   if (roi.a.y() < 0) {
      count_top = false;
      roi.a.y() = 0;
   }

   if (roi.b.x() > fovea.bb.width()) {
      count_right = false;
      roi.b.x() = fovea.bb.width();
   }

   if (roi.b.x() > fovea.bb.height()) {
      count_bot = false;
      roi.b.y() = fovea.bb.height();
   }

   count = 0;

   if (count_top) {
      for (x = roi.a.x(); x < roi.b.x(); x += density) {
         if (fovea.colour(x, roi.a.y()) == colour) {
            ++ count;
         }
      }
   }

   if (count_left) {
      for (y = roi.a.y(); y < roi.b.y(); y += density) {
         if (fovea.colour(roi.a.x(), y) == colour) {
            ++ count;
         }
      }
   }

   if (count_bot) {
      for (x = roi.a.x(); x < roi.b.x(); x += density) {
         if (fovea.colour(x, roi.b.y() - 1) == colour) {
            ++ count;
         }
      }
   }

   if (count_right) {
      for (y = roi.a.y(); y < roi.b.y(); y += density) {
         if (fovea.colour(roi.b.x() - 1, y) == colour) {
            ++ count;
         }
      }
   }

   return count;
}
   
