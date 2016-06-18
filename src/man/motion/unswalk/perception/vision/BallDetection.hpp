#pragma once

#include <utility>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "CameraToRR.hpp"
#include "Fovea.hpp"
#include "ImageRegion.hpp"
#include "VisionConstants.hpp"
#include "VisionDefs.hpp"

#include "types/BallInfo.hpp"
#include "types/UNSWSensorValues.hpp"

#define ANGLE_DELAY 4

class Vision;

class BallDetection
{
   public:
      /**
       * The coordinates that have been detected to be on
       * the edge of the ball. For debugging display
       **/
      std::vector<Point> ballEdgePoints;

      explicit BallDetection();


      /**
       * findBalls
       * Find all balls in an image. Can potentially return more than
       * one ball.
       *
       * @param frame   : VisionFrame associated with current data
       * @param fovea   : Region of image to search
       * @param seed    : Seed value past to rand_r
       * @param terminal: Prevent ball detector from recursing on smaller foveas
       */
      void findBalls(
            VisionFrame &frame, 
            const Fovea &topFovea,
            const Fovea &botFovea,
            unsigned int *seed);

      /* Pretty stuff for offnao */
      std::vector<boost::shared_ptr<FoveaT<hBall, eBall> > > ballFoveas;
      std::vector<boost::shared_ptr<FoveaT<hNone, eNone> > > trackingFoveas;;
      std::vector<Point> points;

      // Localisation ball
      RRCoord localisationBall;

      // Robot Rock Angle (for smoothing ball distances)
      float latestAngleX;

   private:

      int ballHintAge; // how many frames since we last saw the ball
      int ball_dx; // pixel speed of the ball based on 1 observation 
      // TODO can actual put a decent filter ddon this

      float angleXDelay[ANGLE_DELAY];

      static const int dontTrustKinematicsBeyond;
      static const int ballEdgeThreshold;
      static const int ballCloseThreshold;
      static const int ballColourRatio;
      static const int maxPixelsAboveFieldEdge;
      static const int trackingSizeX;
      static const int trackingSizeY;
      static const int maxTrackBallRadius;
      static const int maxBallHintAge;
      static const int ballHintEdgeThreshold;

      struct ball_seed_t
      {
         Point centre;
         int radius;
         int count;
      };

      /**
       * findBallsR
       * Recursive helper for findBalls
       *
       * @param frame   : VisionFrame associated with current data
       * @param fovea   : Region of image to search
       * @param seed    : Seed value past to rand_r
       */
      void findBallsR(
            VisionFrame &frame, 
            const Fovea &fovea,
            unsigned int *seed,
            bool terminal = false);

      /**
       * findBall
       * Find all balls in an image. Can potentially return more than
       * one ball.
       *
       * @param frame   : VisionFrame associated with current data
       * @param fovea   : Fovea in which seed location exists
       * @param ball    : Seed location to search for ball
       * @param seed    : Seed value past to rand_r
       */
      void findBall(
            VisionFrame &frame, 
            const Fovea &fovea,
            ball_seed_t &ball,
            unsigned int *seed);

      /**
       * trackLastBall
       * Attempt to relocate a previously seen ball. Only really useful for
       * balls of small radii 
       *
       * @param frame   : VisionFrame associated with current data
       * @param fovea   : Fovea in which seed location exists
       * @param ball    : Previously known BallInfo
       * @param seed    : Seed value past to rand_r
       */
      void trackLastBall(
            VisionFrame &frame, 
            const Fovea &fovea, 
            const BallInfo &ball,
            unsigned int *seed);


      /**
       * ransacBall
       * Search vector of points for circle. If good match is found,
       * add found circle to list of detected balls
       *
       * @param frame      : VisionFrame associated with current data
       * @param fovea      : Region of image to search
       * @param ballEdges  : Vector of edge points to search
       * @param radius     : Guessed radius of ball to search for
       * @param seed       : Seed value past to rand_r
       */
      void ransacBall(
            VisionFrame &frame,
            const Fovea &fovea,
            const std::vector<Point> &ballEdges,
            int radius,
            unsigned int *seed);

      /**
       * populateSeedLocations
       * Generate list of viable hypothises for ball locations
       * to seed into findBall()
       *
       * @param frame   : VisionFrame associated with current data
       * @param fovea   : Region of image to search
       * @param seeds   : Vector to fill with seed locations
       * @see findBall
       */
      void populateSeedLocations(
            VisionFrame &frame,
            const Fovea &fovea,
            std::vector<ball_seed_t> &seeds);


      /**
       * guessRadiusAndValidateSeed
       * Perform an initial set of sanity checks on a potential ball. If
       * seed passes initial checks, guess the radius of the ball from
       * either saliency or kinematics.
       *
       * @param frame      : VisionFrame associated with current data
       * @param fovea      : Region of image to search
       * @param xhistogram : orange x histogram below field edge
       * @param xhistogram : orange y histogram below field edge
       * @param ball       : Seed location of ball
       *
       * @return true if seed is valid
       * @see findBall
       */
      bool guessRadiusAndValidateSeed(
            VisionFrame &frame,
            const Fovea &fovea,
            const Histogram<int, cNUM_COLOURS> &xhistogram,
            const Histogram<int, cNUM_COLOURS> &yhistogram,
            ball_seed_t &ball);

      /**
       * ballRadiusFromHistogram
       * Guess the radius of the ball based on the histograms
       *
       * @param ball    : Seed location to search for ball
       * @param xhistogram : orange x histogram below field edge
       * @param xhistogram : orange y histogram below field edge
       *
       * @return radius of ball
       */
      int ballRadiusFromHistogram(
            const Fovea &fovea,
            const Histogram<int, cNUM_COLOURS> &xhistogram,
            const Histogram<int, cNUM_COLOURS> &yhistogram,
            const ball_seed_t &ball);

      /**
       * ballRadiusFromHistogram
       * Guess the radius of the ball based on kinematic projection
       *
       * @param distance: Distance to ball as calculated from
       *                  the kinematic chain
       *
       * @return radius of ball
       */
      int ballRadiusFromDistance(int distance, bool top);

      /**
       * findSmallBallEdges
       * Generate a list of ball edge points from a small ball.
       *
       * @param fovea   : Region of image to search for ball edges.
       *                  This is highly intensive and should be performed on
       *                  the smallest fovea possible. The fovea must also
       *                  have been calculated using the eBall edge weightings
       * @param edges   : Vector onto which edges will be appended
       */
      void findSmallBallEdges(const Fovea &fovea, std::vector<Point> &edges);

      /**
       * findBallEdges
       * Generate a list of ball edge points from a small ball.
       *
       * @param fovea   : Region of image to search for ball edges.
       *                  This is highly intensive and should be performed on
       *                  the smallest fovea possible. The fovea must also
       *                  have been calculated using the eBall edge weightings
       * @param edges   : Vector onto which edges will be appended
       */
      void findBallEdges(const Fovea &fovea, std::vector<Point> &edges);

      /**
       * scanBoxForEdges
       * Scan a box for edges in a configurable scan pattern
       *
       * @param fovea   : Region of image to search for ball edges.
       *                  This is highly intensive and should be performed on
       *                  the smallest fovea possible. The fovea must also
       *                  have been calculated using the eBall edge weightings
       * @param start   : Point to start scanning from
       * @param size    : Length of scan box edges
       * @param dx      : dx component of top edge
       * @param dy      : dy component of top edge
       * @param edges   : Vector onto which edges will be appended
       */
      void scanBoxForEdges(
            const Fovea &fovea,
            const Point start,
            const int size,
            const int dx,
            const int dy,
            std::vector<Point> &edges);
      /**
       * Count the number orange pixels in a square region. Assumes entire
       * image region is a valid part of the fovea. 
       * 
       * @param fovea   : Region of image to search.
       * @param ball    : Seed location to search around.
       * @param radius  : Distance to search around ball seed. A radius of 1
       *                  would test 1 pixel, radius of 2 would test 9 pixels.
       *
       * return number of orange pixels around the seed point
       */
      int countOrangeAround(
            const Fovea &fovea, 
            const ball_seed_t &ball,
            const int radius);

      /**
       * makeBallHistsBelowFieldEdge
       * 
       * @param frame   : VisionFrame associated with current data
       * @param fovea   : Region of image to search.
       * @param xhist   : XHistogram to output to
       * @param yhist   : YHistogram to output to
       *
       * return true if ball is below field edge 
       */
      void makeBallHistsBelowFieldEdge(
            VisionFrame &frame,
            const Fovea &fovea,
            Histogram<int, cNUM_COLOURS> &xhist,
            Histogram<int, cNUM_COLOURS> &yhist);



      /**
       * doSeedsOverlap
       * 
       * @param a       : first ball seed
       * @param b       : second ball seed
       *
       * return true if ball seeds overlap
       */
      bool doSeedsOverlap(
            const ball_seed_t &a,
            const ball_seed_t &b);


      int checkSeed(
            const Fovea       &fovea,
            const ball_seed_t &seed);

      int countColourInBox(
            const Fovea &fovea,
            BBox         roi,
            int          density,
            Colour       colour);

      int countColourAroundBox(
            const Fovea &fovea,
            BBox         roi,
            int          density,
            Colour       colour);
      
      /**
       * Sanity Checks
       */
      bool isBallRadiusTooBig(const VisionFrame &frame, const BallInfo &ball);
      /* TODO(carl) port these sanity checks over to the new architecture */
      /**
       * A dirty hack to delete cases where some of the background is below
       * the field edge, mainly due to looking side on to the goal posts
       * causing the field edge detectino to be wrong, and balls get seen
       * in this area
       **/
      bool isBallAboveMissedEdge(ImageRegion *ballRegion, Vision *vision,
            const std::pair<int, int> &horizon);
      /**
       * Another dirty hack to remove balls from robots that are
       * occassionally missed
       **/
      bool isBallInMissedRobot(Vision *vision);

      /**
       * Tests to see if the detected ball contains the original
       * ballRegion
       **/
      bool isBallInsideBallRegion(ImageRegion *ballRegion);
};

