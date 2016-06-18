#include "GoalDetection.hpp"
#include "Vision.hpp"

#include <vector>
#include <algorithm>
#include <utility>
#include "utils/Logger.hpp"
#include "utils/basic_maths.hpp"
#include "utils/SPLDefs.hpp"
#include "utils/Timer.hpp"

#define MIN_COLOUR_THRESHOLD 30
#define MIN_H_COLOUR_THRESHOLD 30
#define MIN_EDGE_THRESHOLD 20000
#define COLOUR_RATIO_THRESHOLD 0.5

using namespace std;

void GoalDetection::findGoals(
         VisionFrame    &frame,
         const Fovea    &fovea, 
         unsigned int   *seed)
{

   goalFoveas.clear();
   std::vector<PostInfo> posts;

   // Find candidate regions to look for goals
   std::vector<BBox> regions;
   findCandidateRegions(fovea, regions);

   // Sanity check goals to eliminate crappy ones
   performSanityChecks(fovea, frame, regions);

   // Convert into goal posts
   std::vector<BBox>::const_iterator it;
   for (it = regions.begin(); it != regions.end(); ++it) {

      PostInfo p;
      BBox bb (fovea.mapFoveaToImage(it->a), fovea.mapFoveaToImage(it->b));

      // If we are in the top camera, ensure we don't round the BBox into the bottom camera
      if (fovea.top && bb.b.y() == TOP_IMAGE_ROWS) {
         bb.b.y() = (TOP_IMAGE_ROWS-1);
      }

      // Calculate best distance to goal - also fine tunes the BBox
      // Fine tunes and sets the BBox
      // Sets rr, kDistance, wDistance, trustDistance on variable p
      RRCoord rr = findDistanceToPost(frame, fovea, bb, regions.size(), p);
      //if (rr.distance() > 2000) p.trustDistance = false;

      // Work out left and right goals
      // Sets type and dir
      determineLeftRightPost(fovea, regions, *it, p);
      
      // Save goal post
      posts.push_back(p);
   }

   // Sanity check the LHS / RHS of goals
   if (posts.size() == 2) {
      PostInfo pLeft, pRight;
      if (posts[0].type == PostInfo::pLeft) {
         pLeft  = posts[0];
         pRight = posts[1];
      } else {
         pLeft  = posts[1];
         pRight = posts[0];
      }

      if (pLeft.dir == PostInfo::pToLeftOf &&
          pLeft.rr.distance() >= pRight.rr.distance()) {
         posts[0].dir = PostInfo::pUnknown;
         posts[1].dir = PostInfo::pUnknown;
      } else if (pLeft.dir == PostInfo::pToRightOf &&
          pRight.rr.distance() >= pLeft.rr.distance()) {
         posts[0].dir = PostInfo::pUnknown;
         posts[1].dir = PostInfo::pUnknown;
      }
   }

   // Merge top and bottom cameras
   const float headingThreshold = UNSWDEG2RAD(7);
   if (frame.posts.empty()) {
      frame.posts = posts;
   } else if (fovea.top && posts.size() > 0 && frame.posts.size() == 1) { 
      for (unsigned int i = 0; i < posts.size(); i++) {
         if (abs(posts[i].rr.heading() - frame.posts[0].rr.heading())
               > headingThreshold) {
            frame.posts.push_back(posts[i]);
         }
      }

      // Ensure if we have 2 posts, they are left and right
      if (frame.posts.size() == 2) {
         if (frame.posts[0].rr.heading() < frame.posts[1].rr.heading()) {
            frame.posts[0].type = PostInfo::pRight;
            frame.posts[1].type = PostInfo::pLeft;
         } else {
            frame.posts[0].type = PostInfo::pLeft;
            frame.posts[1].type = PostInfo::pRight;
         }
      }
   }
}

// Uses colour to make rough bounding boxes
void GoalDetection::findCandidateRegions(const Fovea &fovea,
                                         std::vector<BBox> &regions) {

   const YHistogram &yhistogram = fovea.yhistogram;
   const XHistogram &xhistogram = fovea.xhistogram;

   std::vector<BBox> candidates(regions);

   // Find rough vertical bounding boxes for goal posts
   int start = 0;
   for (int i = 0; i < xhistogram.size; ++i) {
      if (xhistogram._counts[i][cGOAL_YELLOW] > MIN_COLOUR_THRESHOLD) {

         if (start == 0) {
            start = i;
         }

      } else if (start != 0) {

         // Save region
         candidates.push_back(BBox(Point(start, 0), Point(i, fovea.bb.height())));
         start = 0;
      }
   }

   // Find roughly where the crossbar is
   start = 0;
   int max = 0;
   int maxI = 0;
   int prev = 0;
   for (int i = 0; i < yhistogram.size; ++i) {
      int current = yhistogram._counts[i][cGOAL_YELLOW];
      if ((current > max) &&
          (current > MIN_H_COLOUR_THRESHOLD) && 
          (current > (5*prev))) {
         max = current;
         maxI = i;
      }
      prev = current;
   }

   // Now that we have regions, find the bottom
   std::vector<BBox> candidates2 (candidates);
   candidates.clear();
   std::vector<BBox>::const_iterator it;
   for (it = candidates2.begin(); it != candidates2.end(); ++it) {
      int start = it->a.x();
      int end = it->b.x();

      // Identified a region horizontally (start-end), now clip it vertically
      int middle = (start + end) /2;

      // Find longest run of colour
      int s = 0;
      int length = 0;
      int bestS = 0;
      int bestLength = 0;
      int lastYellow = 0;
      bool skip = false;
      for (int j = 0; j < fovea.bb.height(); ++j) {

         Colour c = fovea.colour(middle, j);

         // Keep track of last yellow
         if (c == cGOAL_YELLOW && s != 0) lastYellow = j;

         // If yellow, start counting at first
         if (c == cGOAL_YELLOW && s == 0) {
            s = j;
            length = 0;
         }

         // If not green or background keep tracking run
         //if (c != cBACKGROUND && c != cFIELD_GREEN) {
         if (c != cFIELD_GREEN) {
            ++length;
            if (skip) ++length;
         }

         // If not, maybe end a run
         else if (s != 0) {
            // If gap is just 1 pixel (not green), don't end
            if (j <= (s+length) && c != cFIELD_GREEN) {
               skip = true;
               continue;
            }

            // Otherwise end region
            if (length > bestLength) {
               bestS = s;
               bestLength = length;
            }
            s = 0;
            length = 0;
         }
         skip = false;
      }

      // Case for bottom of image
      if (length > bestLength) {
         bestS = s;
         bestLength = length;
      }

      // Don't over extend the bottom way past the last yellow pixel we saw
      int base = std::min(lastYellow, bestS+bestLength);

      // Save region
      if (maxI != 0) {
         candidates.push_back(BBox(Point(start, maxI), Point(end, base)));
      } else {
         candidates.push_back(BBox(Point(start, bestS), Point(end, base)));
      }
      start = 0;
   }

   regions = candidates;
}

void GoalDetection::performSanityChecks(const Fovea &fovea,
                                        const VisionFrame &frame,
                                        std::vector<BBox> &regions) {
   std::vector<BBox> candidates (regions);
   regions.clear();
   std::vector<BBox>::iterator it;
   for (it = candidates.begin(); it != candidates.end(); ++it) {

      // Check that the middle and the bottom 75% for edges
      // Only need 1 strong edge in each to be good

      int middle = (it->a.y() + it->b.y()) / 2;
      int bottom = it->a.y() + (it->b.y() - it->a.y()) * 0.75;
      bool keepM = false;
      bool keepB = false;
      for (int col = it->a.x(); col <= it->b.x(); ++col) {
         
         // Check middle
         Point edge = fovea.edge(col, middle);
         float magnitude = (edge.x() * edge.x()) + (edge.y() * edge.y());
         if (magnitude > MIN_EDGE_THRESHOLD) keepM = true;

         // Check bottom
         edge = fovea.edge(col, bottom);
         magnitude = (edge.x() * edge.x()) + (edge.y() * edge.y());
         if (magnitude > MIN_EDGE_THRESHOLD) keepB = true;
      }
      if (!keepM) {
         //std::cout << "throwing away since no keepM" << std::endl;
         continue              ;
      }
      if (!keepB) {
         //std::cout << "throwing away since no keepB" << std::endl;
         continue;
      }

      // Check % of colour in goal post - ie compare length and colour
      float length = it->b.y() - it->a.y();
      int centre = (it->a.x() + it->b.x()) / 2;
      float numColourPixels = 0;
      for (int row = it->a.y(); row < it->b.y(); ++row) {
         if (fovea.colour(centre, row) == cGOAL_YELLOW) {
            ++numColourPixels;
         }
      }

      if ((numColourPixels / length) < COLOUR_RATIO_THRESHOLD) {
         //std::cout << "throwing away since not enough colour" << std::endl;
         continue;
      }

      // Check bottom of goal post is below field edge
      Point fieldEdge = fovea.mapFoveaToImage(Point(centre, 0));
      int fieldEdgeY = 0;
      if (fovea.top) {
         fieldEdgeY = frame.topStartScanCoords[fieldEdge.x()];
      } else {
         fieldEdgeY = frame.botStartScanCoords[fieldEdge.x()];
      }
      fieldEdge.y() = std::max(fieldEdge.y(), fieldEdgeY);
      fieldEdge = fovea.mapImageToFovea(fieldEdge);

      if (fieldEdge.y() > it->b.y()) {
         //std::cout << "throwing away since above field edge" << std::endl;
         continue;
      }

      regions.push_back(*it);
   }

   // If more than 2 goals, things have gone wrong, so panic
   if (regions.size() > 2) regions.clear();
}

// Finds the distance to post
// Tunes the BBox using higher resolution foveas
RRCoord GoalDetection::findDistanceToPost(VisionFrame &frame,
                                          const Fovea& fovea,
                                          BBox& goal,
                                          int numPosts,
                                          PostInfo& p) {

   bool trustDistance = true;
   float differenceThreshold = 1.7;

   // **** Try finding the distance using kinematics of the base ****
   findBaseOfPost(frame, goal);

   const CameraToRR *convRR = &frame.cameraToRR;
   Point base = Point((goal.a.x()+goal.b.x())/2, goal.b.y());
   RRCoord rr = convRR->convertToRR(base, false);
   float kdistance = rr.distance();
   
   // **** Try using the width the find the distance ****
   // Calculate width distance at 3 points and take median
   std::map<float, float> distances;
   for (float h = 0.89; h < 1; h += 0.05) {
      float d = widthDistanceToPost(frame, goal, h);
      distances.insert(std::make_pair(d,h));
   }
   std::map<float, float>::iterator i = distances.begin();
   if (distances.size() > 1) ++i;
   float wdistance = widthDistanceToPost(frame, goal, i->second, true);

   // **** Decide which distance to use ****
   // Kinematics is usually more accurate, so use it unless we know it's wrong

   // If post ends at bottom of image, probably not the bottom, so use width
   bool width = false;
   if (fovea.top && goal.b.y() > (TOP_IMAGE_ROWS-10)) {
      width = true;
   }

   // If still yellow below the base, probably missed the bottom, so use width
   // Only for 1 post though
   if (numPosts == 1) {
      Point fTop = fovea.mapImageToFovea(goal.a);
      Point fBot = fovea.mapImageToFovea(goal.b);
      const YHistogram &yhistogram = fovea.yhistogram;
      int height = (fBot.y() - fTop.y()) / 4; // set max scan size
      int endPoint = std::min(fovea.bb.height(), fBot.y() + height);
      int noYellow = fBot.y();

      for (int i = fBot.y(); i < endPoint; ++i) {
         int current = yhistogram._counts[i][cGOAL_YELLOW];
         if (current < 10) noYellow = i;
      }
      if (noYellow == fBot.y()) trustDistance = false;
   }
   
   // Decided to use width distance
   if (width) {
      if (wdistance < 1500) rr.distance() = wdistance;
      else trustDistance = false;
//      differenceThreshold = 1.7;
   }

   // Check that kinematics and width distances are similar
   else if (kdistance < 2500) {
   }

   else if (((kdistance / wdistance) > differenceThreshold) ||
       ((wdistance / kdistance) > differenceThreshold)) {
      trustDistance = false;
   }

   // Check distance is reasonable
   if (rr.distance() > 12000) {
      trustDistance = false;
      rr.distance() = 12000;
   }

   // Set variables in PostInfo
   p.rr = rr;
   p.kDistance = kdistance;
   p.wDistance = wdistance;
   p.trustDistance = trustDistance;
   p.imageCoords = goal;

   return rr;
}

void GoalDetection::findBaseOfPost(VisionFrame &frame,
                                   BBox& goal) {

   // Calculate where fovea should go
   // Add +-20 pixels around the current base
   int padding = 10;
   int x = (goal.a.x() + goal.b.x())/2;
   Point tl = Point (x-2, goal.b.y() - padding);
   Point br = Point (x+1, goal.b.y() + padding);
   int density = 1;

   // Create a high res fovea
   boost::shared_ptr<FoveaT<hGoals, eGrey> > goalFovea(
          new FoveaT<hGoals, eGrey>(BBox(tl, br), density, 0, true));
   goalFovea->actuate(frame);
   goalFoveas.push_back(goalFovea);

   // Find base point
   int lastYellow = -1;
   int maxNotYellow = 5;
   for (int y = 0; y < goalFovea->bb.height(); ++y) {
      Colour c = goalFovea->colour(0,y);
      if (c == cGOAL_YELLOW) {
         lastYellow = y;
      } else if (abs(lastYellow - y) > maxNotYellow) {
         break;
      }
   }
   if (lastYellow != -1) {
      goal.b.y() = goalFovea->mapFoveaToImage(Point(0, lastYellow)).y();
   }
}

float GoalDetection::widthDistanceToPost(VisionFrame &frame,
                                           BBox& goal,
                                           float h,
                                           bool update) {

   const CameraToRR *convRR = &frame.cameraToRR;
   float distance = -1;

   // Calculate where fovea should go
   int y = goal.a.y() + (goal.b.y() - goal.a.y()) * h;
   Point tl = Point (goal.a.x(), y-1);
   Point br = Point (goal.b.x(), y+2);
   int width  = br.x() - tl.x();
   int density = 1;

   // Try and extend fovea a bit
   // Note add extra to left since edge data seems slightly skewed left
   tl.x() = std::max(tl.x() - width/2, 0);
   br.x() = std::min(br.x() + width/2, TOP_IMAGE_COLS);

   // Create a high res fovea
   boost::shared_ptr<FoveaT<hGoals, eGrey> > goalFovea(
          new FoveaT<hGoals, eGrey>(BBox(tl, br), density, 0, true));
   goalFovea->actuate(frame);
   goalFoveas.push_back(goalFovea);

   width  = br.x() - tl.x();


   // Trace the yellow from the centre outwards
   // Note: left and right are in fovea coords
   int left = -1;
   int right = -1;
   int max = 0;
   int lastYellow = width/2;
   int maxNotYellow = std::max(4, width/30);

   for (int x = width/2; x > 0; --x) {

      // Check the colour, give up after n non-yellow in a row
      Colour c = goalFovea->colour(x,0);
      if (c == cGOAL_YELLOW) {
         lastYellow = x;
      } else if (abs(lastYellow - x) > maxNotYellow) {
         break;
      }

      // Keep track of max edge so far
      Point edge = goalFovea->edge(x,0);
      int magnitude = (edge.x() * edge.x()) + (edge.y() * edge.y());
      if (magnitude > max) {
         left = x;
         max = magnitude;
      }

      // Delete early edges
      if (abs(lastYellow - left) > 10) {
         left = -1;
         max = 0;
      }
   }

   lastYellow = width/2;
   max = 0;
   for (int x = width / 2; x < width; ++x) {

      // Check the colour, give up after n non-yellow in a row
      Colour c = goalFovea->colour(x,0);
      if (c == cGOAL_YELLOW) {
         lastYellow = x;
      } else if (abs(lastYellow - x) > maxNotYellow) {
         break;
      }

      // Keep track of max edge so far
      Point edge = goalFovea->edge(x,0);
      int magnitude = (edge.x() * edge.x()) + (edge.y() * edge.y());
      if (magnitude > max) {
         right = x;
         max = magnitude;
      }

      // Delete early edges
      if (abs(lastYellow - right) > 10) {
         right = -1;
         max = 0;
      }
   }

   if (left >= 0 && right > 0 && (right-left > 0)) {
      distance = convRR->pixelSeparationToDistance
                           (right-left, GOAL_POST_DIAMETER);
      if (update) {
         goal.a.x() = tl.x() + left;
         goal.b.x() = tl.x() + right;
      }
   }

   return distance;
}

PostInfo::Type GoalDetection::determineLeftRightPost(const Fovea& fovea,
                                                     const std::vector<BBox>& regions,
                                                     const BBox& goal,
                                                     PostInfo& p) {
   // Default Case
   PostInfo::Type type = PostInfo::pNone;
   PostInfo::Direction dir = PostInfo::pUnknown;

   // Two Posts Case - just look which is left and which is right
   if (regions.size() == 2) {
      
      // Make 'it' the other post
      std::vector<BBox>::const_iterator it;
      for (it = regions.begin(); it != regions.end(); ++it) {
         if (goal != *it) break;
      }

      if (goal.a.x() < it->a.x()) type = PostInfo::pLeft;
      else type = PostInfo::pRight;

      // For two posts, also work out if we are on RHS or LHS of posts
      // Do this by checking where the base pixel of each is
      int leftY  = 0;
      int rightY = 0;
      if (type == PostInfo::pLeft) {
         leftY  = goal.b.y();
         rightY = it->b.y();
      } else {
         leftY  = it->b.y();
         rightY = goal.b.y();
      }

      // If the base is higher for the left post, it is further away, so RHS
      // If the base is higher for the right post, it is further away, so LHS
      if (leftY < rightY) {
         dir = PostInfo::pToRightOf;
      } else {
         dir = PostInfo::pToLeftOf;
      }
   }

   // One Post Case - try and use the crossbar to determine left or right
   if (regions.size() == 1) {
      const XHistogram &xhistogram = fovea.xhistogram;

      // Start at post and search left for yellow
      int leftLength = 0;
      for (int i = goal.a.x(); i > 0; --i) {
         int current = xhistogram._counts[i][cGOAL_YELLOW];
         if (current > 0) {
            ++leftLength;
         } else {
            break;
         }
      }

      // Start at post and search right for yellow
      int rightLength = 0;
      for (int i = goal.b.x(); i < xhistogram.size; ++i) {
         int current = xhistogram._counts[i][cGOAL_YELLOW];
         if (current > 0) {
            ++rightLength;
         } else {
            break;
         }
      }

      // Check that the lengths we have are significant
      int postLength = goal.b.x() - goal.a.x();
      postLength *= 2;
      //std::cout << "l = " << leftLength << " r = " << rightLength << std::endl;

      if (leftLength > postLength || rightLength > postLength) {

         // If we can definitely pick a side, do so :)
         if (leftLength > (4*rightLength)) type = PostInfo::pRight;
         else if (rightLength > (4*leftLength)) type = PostInfo::pLeft;
      }

   }

   p.type = type;
   p.dir = dir;
   return type;

}
