#include "perception/vision/OldRobotDetection.hpp"
#include "types/RRCoord.hpp"
#include "utils/basic_maths.hpp"
#include "utils/Logger.hpp"
#include "utils/angles.hpp"

using namespace std;


#define RD_INVALID                  -1


//Sanity checks constants
#define SC_MAX_DETECT_DISTANCE      2500
#define SC_FIELD_LINE_POINT_REDUCE  3
#define SC_SCAN_IGNORE_DISTANCE     6 // distance from field edge to ignore when generating points
#define SC_PIXEL_HEIGHT_IGNORE      3
#define SC_WAIST_BAND_ACCEPTABLE    4
#define SC_MIN_GREEN_ACCEPT         4 // min num neighbouring green points when generating points
#define SC_MIN_PIXELS_WIDTH         6 // min width of a robot BBox
#define SC_SAME_ROBOT_WIDTH         250 
#define SC_SONAR_MATCH_THRESH       0.10f // Sonar is matched if its within 10%
#define SC_ROBOT_CHEST_DEPTH        100 // reflects front to back chest diameter of one robot
#define SC_NEAR_EDGE                4 // how close the foot has to be to bottom of top frame to assume robot is cropped
#define SC_WAIST_BAND_MAX_HEIGHT    340 // height from ground to definitely capture the whole waist band
#define SC_WAIST_BAND_AVG_HEIGHT    300 // height from ground to middle of waist band in a typical stance
#define SC_GREEN_SHRINK             0.35f // if this much of the robot edge is green, shrink it
#define SC_HEADING_MERGE_THRESH     UNSWDEG2RAD(10.f)

#define SONAR_VAR                   0.05f // std dev is 5% of the distance if sonar verified
#define FOOT_VAR                    0.15f // std dev is 15% of the distance if vision can see the feet
#define WAISTBAND_VAR               0.20f // std dev is 20% of the distance if vision estimated range from waistband
#define THETA_VAR                   0.10f // std dev is 10% of the distance in the perpendicular direction
#define MIN_VAR                     SQUARE(50.f) // minimum variance

//Debugging
//#define RD_DEBUG



#ifdef RD_DEBUG
// For spamming a massive information print out
      void debugInformation(bool shouldRemove,
         vector<UNSWRobotInfo>& robots, OldRobotDetection::RDData& robot, BBox& box, 
         Point& waistband, RRCoord& rr,
         int waistbandBlueCount, int waistbandRedCount,
         int r, int saliency_density, bool top);
#endif

//Helpers declaration
void printBoundingBox(BBox& box);
float findRealWidth(const RRCoord& left, const RRCoord& right);
int round(int num);



OldRobotDetection::OldRobotDetection() {
  
}


void OldRobotDetection::findRobots(
            VisionFrame &frame,
            std::vector<Point> edgePoints,
            const Fovea &saliency){

   //Reset containers except for robots which is done externally
   allRobots.clear();
   startOfScan.clear();
   postsCheck.clear();
   
   //Initialise variables
   saliency_cols = (saliency.top) ? TOP_IMAGE_COLS / saliency.density
                                  : BOT_IMAGE_COLS / saliency.density;
   saliency_rows = (saliency.top) ? TOP_IMAGE_ROWS / saliency.density
                                  : BOT_IMAGE_ROWS / saliency.density;  
   saliency_density = saliency.density; 
   saliency_top = saliency.top;

   robotPoints.clear();
   robotPoints.resize(saliency_cols, 0);
   pointsCheck.clear();
   pointsCheck.resize(saliency_cols, false);

   // convert startscan coordinates into this fovea coordinates
   Point start;   
   for(int i = 0; i < saliency_cols; i++) {
      start = saliency.mapFoveaToImage(Point(i,0));
      if (saliency_top) {
         start = saliency.mapImageToFovea(
               Point(start.x(),frame.topStartScanCoords[start.x()]));
      } else {
         start = saliency.mapImageToFovea(
               Point(start.x(), frame.botStartScanCoords[start.x()]));
      }
      startOfScan.push_back(start.y());
   }

   // Posts only found in top camera image
   if(saliency_top){
      std::vector<PostInfo>::iterator post_i;
      for(post_i = frame.posts.begin(); post_i != frame.posts.end(); ++post_i) {
         BBox b      = post_i->imageCoords;
         b.a = saliency.mapImageToFovea(b.a);
         b.b = saliency.mapImageToFovea(b.b);
         
         postsCheck.push_back(b);
         //RRCoord& rr = post_i->rr;
      }
   }
 
   this->edgePoints           = edgePoints; 
   this->convRR               = frame.cameraToRR;
   
   generateDeviationPoints(saliency);
   robotPointsToRegion();
   mergeRobotRegion(saliency);
   sanityCheckRobots(saliency);
}



/**
 * Field edge deviation detection method
 */
void OldRobotDetection::generateDeviationPoints(const Fovea &saliency) {
   unsigned int edgePointIndex = 0;
   Point p(RD_INVALID,RD_INVALID);
   bool takeNextPoint = false;
   bool requireOwnScan;
   
   if(edgePoints.size() > 0) {
      takeNextPoint = true;
   }
   
   for(int c = 0; c < saliency_cols; c++) {
      requireOwnScan = false;
      
      if(takeNextPoint) {
         takeNextPoint = false;

         if(edgePointIndex < edgePoints.size()) {
            p = saliency.mapImageToFovea(edgePoints.at(edgePointIndex));  
         } else {
            p.x() = p.y() = RD_INVALID;
         }
      }
      
      if(p.x() == c) {
         if(p.y() >= (int)startOfScan[c] && 
            saliency.colour(c,p.y()) == cFIELD_GREEN &&
            getColorAmount(saliency, p.y(),c,cFIELD_GREEN, 1) >= SC_MIN_GREEN_ACCEPT) {
               addRobotPoint(p.y(),c);
         } else {
            requireOwnScan = true;
         }
         takeNextPoint = true;
         edgePointIndex++;
      } else {
         requireOwnScan = true;
      }
      
      if(requireOwnScan) {
         bool earlyExit = false;
         for(int r = startOfScan[c]; r < saliency_rows; ++r) {
            if(saliency.colour(c,r) == cFIELD_GREEN && 
               getColorAmount(saliency, r,c,cFIELD_GREEN, 1) >= SC_MIN_GREEN_ACCEPT) {
                  addRobotPoint(r,c);
               earlyExit = true;
               break;
            }
         }
         if(!earlyExit) {
            robotPoints[c] = saliency_rows-1;
            pointsCheck[c] = true;
         }
      }
   }
}



inline int OldRobotDetection::getColorAmount(const Fovea &saliency, int r, int c, Colour colour, int lookDistance) {
   int rStart, rEnd;
   int cStart, cEnd;
   
   //Default
   rStart = cStart = -lookDistance;
   rEnd   = cEnd = lookDistance;
   
   // Check for memory access outside the fovea
   if(r+rStart < 0) {
      rStart = 0;
   } if(c+cStart <= 0) {
      cStart = 0;
   } if(r+rEnd >= saliency_rows) {
      rEnd = (saliency_rows-r-lookDistance);
   } if(c+cEnd >= saliency_cols) {
      cEnd = (saliency_cols-c-lookDistance);
   }
  
   int amount = 0;
   for(int rinc = rStart; rinc <= rEnd; rinc++) {
      for(int cinc = cStart; cinc <= cEnd; cinc++) {
         if(saliency.colour(c+cinc,r+rinc) == colour) {
            amount++;
         }
      }
   }

   return amount;
}


inline void OldRobotDetection::addRobotPoint(int row, int col) {
   bool goodCandidate = true;
   
   // Check if the robot point is in a post
   for(unsigned int p = 0; p < postsCheck.size(); ++p) {
      if(col >= postsCheck[p].a.x()-SC_SCAN_IGNORE_DISTANCE 
            && col <= postsCheck[p].b.x()+SC_SCAN_IGNORE_DISTANCE) {
         if(abs(row-postsCheck[p].b.y()) <= SC_SCAN_IGNORE_DISTANCE) {
            goodCandidate = false;
         }
      }
   }

   // Check if point is below the field edge   
   if(goodCandidate && (row >= startOfScan[col]+SC_SCAN_IGNORE_DISTANCE)) {
      robotPoints[col] = row;
      pointsCheck[col] = true;
   } else {
      robotPoints[col] = startOfScan[col];
      pointsCheck[col] = false;
   }
}


void OldRobotDetection::robotPointsToRegion() {
   bool goodPrev = false;
   for(int p = 0; p < saliency_cols; ++p) {
      if(pointsCheck[p]) { // previous point is valid
         if(!goodPrev) { // start a new region
            completePreviousRobotRegion();
            
            UNSWRobotInfo ri;
            ri.imageCoords = BBox(Point(p,startOfScan[p]),
                                  Point(p,robotPoints[p]));
            ri.type        = UNSWRobotInfo::rUnknown;
            
            RDData rdData;
            rdData.robot   = ri;
            allRobots.push_back(rdData);
         } else {
            updateBBox(allRobots.back().robot.imageCoords, p, robotPoints[p]);
         }
         goodPrev = true;
      } else {
         goodPrev = false;
      }
   }
   completePreviousRobotRegion();
}


inline void OldRobotDetection::completePreviousRobotRegion() {
   if(allRobots.size() > 0) {
      RDData& rdData = allRobots.back();
      BBox& refBox = rdData.robot.imageCoords;
      if(refBox.a.x() > 0) {
         --refBox.a.x();
      }
      if(refBox.b.x() < (saliency_cols-1)) {
         ++refBox.b.x();
      }
   }
}


void OldRobotDetection::mergeRobotRegion(const Fovea &saliency) {
   
   for(unsigned int r = 0; r < allRobots.size(); r++) {
      RDData& robot = allRobots[r];
      RRCoord& rr = robot.robot.rr;
      BBox& box = robot.robot.imageCoords;
      
      robot.midFoot = Point((box.width() >> 1) + box.a.x(), box.b.y());
      rr = convRR.pose.imageToRobotRelative(saliency.mapFoveaToImage(robot.midFoot));

      if(r > 0) {
         RDData& prevRobot = allRobots[r-1];
         float width = findRealWidth(prevRobot.robot.rr, rr);
         if(width < SC_SAME_ROBOT_WIDTH) {
            prevRobot.skip = true;
            box.a          = prevRobot.robot.imageCoords.a;
            robot.midFoot  = Point((box.width() >> 1) + box.a.x(), box.b.y()); 
            rr = convRR.pose.imageToRobotRelative(saliency.mapFoveaToImage(robot.midFoot));
         }
      }
      robot.skip = false;
   }
}



void OldRobotDetection::sanityCheckRobots(const Fovea &saliency) {
   
   
   for(unsigned int r = 0; r < allRobots.size(); ++r) {
      RDData& robot       = allRobots[r];
      BBox& box           = robot.robot.imageCoords;
      RRCoord& rr         = robot.robot.rr;
      //Point& p            = robot.midFoot;
      bool shouldRemove   = false;

      if (robot.skip){ 
         continue; //Instructed to skip since it was merged with another BBox
      }

      /**
       * Try to 'green shrink' the bounding box by reducing bottom and sides
       * if they have too much green in them
       */
      greenShrink(saliency, box);
     

      if (saliency.top){
         llog(DEBUG1) << "Robot check, top frame, "; 
      } else {
         llog(DEBUG1) << "Robot check, bot frame, ";
      }
      llog(DEBUG1) << "Low-High: " 
            << "(" << box.a.x() << ", " << box.a.y() << ") - " 
            << "(" << box.b.x() << ", " << box.b.y() << "), ";

      /**
       * Basic sanity checks
       */

      if ((box.width() > saliency_cols - 2) || //Box too wide (the whole screen)
          (box.width() < SC_MIN_PIXELS_WIDTH) || //Box too narrow
          (box.height() < SC_MIN_PIXELS_WIDTH)  // Box not high enough
          ){  
         //shouldRemove = true;
         llog(DEBUG1) << "Fail basic sanity checks, Discarded\n";
         continue;
      }

      /**
       * Check robot real world width
       */
      Point botleft = saliency.mapFoveaToImage(Point(box.a.x(), box.b.y()));
      Point botright = saliency.mapFoveaToImage(box.b);
      robot.width  = findRealWidth(convRR.pose.imageToRobotRelative(botleft), 
                        convRR.pose.imageToRobotRelative(botright));
      llog(DEBUG1) << "Real width: " << robot.width << ", ";
      if (robot.width < SC_ROBOT_CHEST_DEPTH){
         llog(DEBUG1) << "Discarded\n";
         continue;
      }

      /**
       * Update RR coords for the green shrinking above
       */
      robot.midFoot = Point((box.width() >> 1) + box.a.x(), box.b.y());
      robot.robot.rr = convRR.pose.imageToRobotRelative(saliency.mapFoveaToImage(robot.midFoot));
      llog(DEBUG1) << "Distance: " << rr.distance() << ", ";

      /**
       * Remove long distance observations
       */

      if(rr.distance() > SC_MAX_DETECT_DISTANCE){
         //shouldRemove = true;
         llog(DEBUG1) << "Discarded\n";
         continue;      
      }
      
      /**
       * Increase the height of BBox so that the waistband must be captured, even if its above field edge
       */
      if(saliency_top){
         Point bboxTop = saliency.mapImageToFovea(convRR.pose.robotToImageXY(rr.toCartesian(), SC_WAIST_BAND_MAX_HEIGHT));
         if (bboxTop.y() < 0) bboxTop.y() = 0; // ensure we don't go above the saliency image
         if (bboxTop.y() < box.a.y()){ // check that it will enlarge it though    
            updateBBox(box, box.a.x(), bboxTop.y());
         }
      }
   

      /**
       * Generate colour profile, assume that BBox is still in the saliency space
       */   
      
      for(int ip = 0; ip < cNUM_COLOURS+1; ip++) {
         robot.colourProfile[ip] = 0;
      }
      robot.colourTotal   = 0;

      int total_mag = 16 * (((saliency.edge_weights >>  0) & 0xFF) +
                         ((saliency.edge_weights >>  8) & 0xFF) +
                         ((saliency.edge_weights >> 16) & 0xFF));
      int dx, dy, mag;
      Point blue_waistband = Point(0,0);
      Point red_waistband = Point(0,0);
      int waistbandBlueCount = 0;
      int waistbandRedCount = 0;

      for(int x = box.a.x(); x < box.b.x(); ++x) {
         for(int y = box.a.y(); y < box.b.y(); ++y) {
            Colour c = saliency.colour(x,y);            
            ++robot.colourProfile[c];
            ++robot.colourTotal;

            // Also do an edge count as well
            const Point &e = saliency.edge(x,y);
            dx = e[0];
            dy = e[1];
            mag = (dy*dy+dx*dx) / total_mag;
            if(mag>150) ++robot.colourProfile[cNUM_COLOURS];

            // Also find the point of maximum waistband
            if(c == cROBOT_BLUE ){
               int blue = getColorAmount(saliency, y, x, cROBOT_BLUE, 1);
               if(blue > waistbandBlueCount){
                  waistbandBlueCount = blue;
                  blue_waistband = Point(x,y);
               }
            } else if (c == cROBOT_RED) {
               int red = getColorAmount(saliency, y, x, cROBOT_RED, 1);
               if(red > waistbandRedCount){
                  waistbandRedCount = red;
                  red_waistband = Point(x,y);
               }
            }
 
         }
      }   

      /**
       * Basic colour profile checks
       */ 

      // require 10% white in the bounding box - can't make this higher, since its amazing how much
      // they appear as background and not white! Just enough to discard people in jeans
      float whiteCRatio  = robot.colourProfile[cWHITE]/(float)robot.colourTotal;
      llog(DEBUG1) << "White: " << whiteCRatio << ", ";
      if (whiteCRatio < 0.10f) {
         //shouldRemove = true; 
         llog(DEBUG1) << "Discarded\n";         
         continue; 
      } 

      float yellowCRatio  = robot.colourProfile[cGOAL_YELLOW]/(float)robot.colourTotal;
      llog(DEBUG1) << "Yellow: " << yellowCRatio << ", ";
      if (yellowCRatio > 0.15f) {
         //shouldRemove = true; // try to discard goal post triangles
         llog(DEBUG1) << "Discarded\n";         
         continue; 
      } 

      // require at least 2 pixels are red or blue (some waist band visible)
      // This may be relaxed in future
      //if (robot.colourProfile[cROBOT_BLUE] <2 && robot.colourProfile[cROBOT_RED] <2) shouldRemove = true;

      // require at least 75% edges in the bounding box, can't make this too much higher or it fails when looking
      // close up at a smooth robot arm
      float edgeRatio  = robot.colourProfile[cNUM_COLOURS]/(float)robot.colourTotal;
      llog(DEBUG1) << "Edge: " << edgeRatio << ", ";
      if (edgeRatio < 0.75f){
         //shouldRemove = true; 
         llog(DEBUG1) << "Discarded\n";
         continue;
      }


      /**
       * Set waistband colour
       */
      Point waistband = Point(0,0);
      if(waistbandBlueCount > waistbandRedCount && waistbandBlueCount > SC_WAIST_BAND_ACCEPTABLE) {
         robot.robot.type = UNSWRobotInfo::rBlue;
         waistband = blue_waistband;
         llog(DEBUG1) << "Waistband blue, ";
      } else if (waistbandRedCount > waistbandBlueCount && waistbandRedCount > SC_WAIST_BAND_ACCEPTABLE) {
         robot.robot.type = UNSWRobotInfo::rRed;
         waistband = red_waistband;
         llog(DEBUG1) << "Waistband red, ";
      } else {
         robot.robot.type = UNSWRobotInfo::rUnknown;
         llog(DEBUG1) << "Waistband unknown, ";
      }

     
      /**
       * If the foot is close to bottom of top frame and might be cropped, 
       * try to update the range estimate by using the waistband instead (assuming standing),
       * and set the variance of the observation
       */
      bool feetSeen = true;
      if (saliency.top  &&  robot.robot.type != UNSWRobotInfo::rUnknown 
           && TOP_IMAGE_ROWS/saliency.density - box.b.y() < SC_NEAR_EDGE ){
         
         RRCoord waistband_loc = convRR.pose.imageToRobotRelative(saliency.mapFoveaToImage(waistband), 
            SC_WAIST_BAND_AVG_HEIGHT);
         robot.robot.rr.distance() = waistband_loc.distance();
         llog(DEBUG1) << "Foot cropped so new waistband distance: " << robot.robot.rr.distance() << ", ";
         robot.robot.rr.var(0,0) = MIN_VAR + SQUARE(WAISTBAND_VAR * robot.robot.rr.distance());
         robot.robot.rr.var(1,1) = MIN_VAR + SQUARE(THETA_VAR * robot.robot.rr.distance());
         feetSeen = false; 
      } else {
         robot.robot.rr.var(0,0) = MIN_VAR + SQUARE(FOOT_VAR * robot.robot.rr.distance());
         robot.robot.rr.var(1,1) = MIN_VAR + SQUARE(THETA_VAR * robot.robot.rr.distance());
      }

     
      /**
       * Convert to raw image space
       */
      box.a = saliency.mapFoveaToImage(box.a);
      box.b = saliency.mapFoveaToImage(box.b);
      robot.robot.imageCoords = box;


      /**
       * Sonar range adjustment - give preference to trying to verify with middle sonar first
       */

      bool sonarVerified = false;
      if (  rr.heading() > Sonar::MIDDLE_START && 
            rr.heading() < Sonar::MIDDLE_END && 
            SonarVerifyDist(sonar[Sonar::MIDDLE], robot.robot.rr, feetSeen)){
            llog(DEBUG1) << "Middle Sonar verified, new distance: " << robot.robot.rr.distance() << "\n";
            sonarVerified = true;
      } else if ( rr.heading() > Sonar::RIGHT_START && 
                  rr.heading() < Sonar::RIGHT_END &&
                  SonarVerifyDist(sonar[Sonar::RIGHT], robot.robot.rr, feetSeen)){ 
            llog(DEBUG1) << "Right Sonar verified, new distance: " << robot.robot.rr.distance() << "\n";
            sonarVerified = true;
      } else if ( rr.heading() > Sonar::LEFT_START && 
                  rr.heading() < Sonar::LEFT_END &&
                  SonarVerifyDist(sonar[Sonar::LEFT], robot.robot.rr, feetSeen)){   
            llog(DEBUG1) << "Left Sonar verified, new distance: " << robot.robot.rr.distance() << "\n";
            sonarVerified = true;
      } else {
         llog(DEBUG1) << "Sonar unverified\n";
      }


      // Don't accept a waistband unknown robot that isn't sonar verified, too many false positives 
      if (!shouldRemove && (sonarVerified || (robot.robot.type != UNSWRobotInfo::rUnknown)) ){

         // Match robots that are in both cameras so we don't get two different ranges on the same robot
         if (!saliency.top){
            for (int i=0; i<(int)robots.size(); i++){
               float headingDiff = fabs(robots[i].rr.heading() - robot.robot.rr.heading());
               if( headingDiff < SC_HEADING_MERGE_THRESH){ 
                  llog(DEBUG1) << "Changing robot at heading: " << RAD2DEG(robots[i].rr.heading()) <<
                     " from " << robots[i].rr.distance() << " to " << robot.robot.rr.distance() << "\n"; 
                  robots[i].rr.distance() = robot.robot.rr.distance();
               }
            }
         }

         Point robotPoint = robot.robot.rr.toCartesian();
         llog(DEBUG1) << "Robot detection found robot with std dev (" << 
               sqrt(robot.robot.rr.var(0,0)) << ", " << sqrt(robot.robot.rr.var(1,1)) << ") at (" <<
               robotPoint.x() << ", " << robotPoint.y() << ")\n";
                
         robots.push_back(robot.robot);
      }
   
      #ifdef RD_DEBUG
      debugInformation(shouldRemove,
         robots, robot, box, waistband,
         rr, waistbandBlueCount, waistbandRedCount,
         r, saliency_density, saliency_top
      );
      #endif
      
   }
}


inline void OldRobotDetection::greenShrink(const Fovea & saliency, BBox &box){

   bool repeat = true;
   while(repeat){

      if(box.width() < 6 || box.height() < 6) return;

      repeat = false;
      int green_left = 0;
      int green_right = 0;      
      
      // try to green shrink sides and bottom in steps of 4 pixels
      for(int x = 1; x<=4; x++){
         for(int y = box.a.y(); y < box.b.y(); ++y) {
            if(saliency.colour(box.a.x()+x, y) == cFIELD_GREEN) green_left++;
            if(saliency.colour(box.b.x()-x, y) == cFIELD_GREEN) green_right++;
         }
      }

      float green_ratio_left = ((float)green_left)/((float)(4*box.height()));      
      float green_ratio_right = ((float)green_right)/((float)(4*box.height()));

      if (green_ratio_left > green_ratio_right && green_ratio_left > SC_GREEN_SHRINK && box.width() > 6){
         box.a.x() += 4;
         repeat = true;
      } else if (green_ratio_right > green_ratio_left && green_ratio_right > SC_GREEN_SHRINK && box.width() > 6){      
         box.b.x() -= 4;
         repeat = true;
      } 
   }
}


bool OldRobotDetection::SonarVerifyDist(std::vector<int> &sonar, RRCoord &location, bool footSeen){

   int diff = numeric_limits<int>::max();
   int curr = static_cast<int>(location.distance());     
   int match = -1;   
   for (unsigned int i=0; i<sonar.size(); i++){
      int sonarDist = sonar[i] + SC_ROBOT_CHEST_DEPTH;         
      if( footSeen && abs(curr-sonarDist) < diff){
         diff = abs(curr-sonarDist);
         match = i;

      // if !footSeen, will only verify with sonar measurements that are closer than vision   
      } else if ( !footSeen && (curr-sonarDist) < diff && (curr-sonarDist) > 0.f ){ 
         diff = curr-sonarDist;
         match = i;
      }     
   }
   if ( (footSeen && diff < static_cast<int>(SC_SONAR_MATCH_THRESH*location.distance())) 
         || (!footSeen && diff < static_cast<int>(2*SC_SONAR_MATCH_THRESH*location.distance())) ){
      location.distance() = static_cast<float>(sonar[match] + SC_ROBOT_CHEST_DEPTH);
      location.var(0,0) = MIN_VAR + SQUARE(SONAR_VAR * location.distance());
      location.var(1,1) = MIN_VAR + SQUARE(THETA_VAR * location.distance());
      return true;
   } else {
      return false;
   }
}


/**
 * Helper functions
 */
void printBoundingBox(BBox& box) {
   cout << "Low-High: " 
        << "(" << box.a.x() << ", " << box.a.y() << ") - " 
        << "(" << box.b.x() << ", " << box.b.y() << ")\n"; 
}    

inline void OldRobotDetection::updateBBox(BBox& box, int xCol, int yRow) {
   box.a.x() = min(box.a.x(), xCol);
   box.a.y() = min(box.a.y(), yRow);
   
   box.b.x() = max(box.b.x(), xCol);
   box.b.y() = max(box.b.y(), yRow);
}

float findRealWidth(const RRCoord& left, const RRCoord& right) {
   float x1 = left.distance()*cos(left.heading());
   float y1 = left.distance()*sin(left.heading());
   
   float x2 = right.distance()*cos(right.heading());
   float y2 = right.distance()*sin(right.heading());
   
   return (hypot(fabs(x1-x2), fabs(y1-y2)));
}
int round(int num) {
   return (num > 0.0) ? (int)floor(num + 0.5) : (int)ceil(num - 0.5);
}

void debugInformation(bool shouldRemove, vector<UNSWRobotInfo>& robots, OldRobotDetection::RDData& robot, 
                      BBox& box, Point& waistband, RRCoord& rr,
                      int waistbandBlueCount, int waistbandRedCount,
                      int r, int saliency_density, bool top
                     ) {
   cout << "----------------------------------" << endl;
   if (top) cout << "Check robot " << r << ", Top frame, ";
   else cout << "Check robot " << r << ", Bottom frame, " ;
   if (shouldRemove) cout << "Removed!\n";
   else cout << "Valid\n";
   printBoundingBox(box);
   
   ////Massive prints out
   cout << "Robot colour histogram (Total " << robot.colourTotal << ")" << endl;
   for(unsigned int pro = 0; pro < cNUM_COLOURS; pro++) {
      printf("%13s : ", ColourNames[pro]);
      printf("%02d", robot.colourProfile[pro]);
      cout << " > Ratio > " << robot.colourProfile[pro]/(double)robot.colourTotal << endl;
   }
   cout << "\tedges : " << robot.colourProfile[cNUM_COLOURS] << " > Ratio > " <<  
      robot.colourProfile[cNUM_COLOURS]/(double)robot.colourTotal << endl;
   
   cout << "RR to base foot d/h : " << rr.distance() << "/" << rr.heading() << endl;
   cout << "Waist band blue/red : " << waistbandBlueCount << " / " << waistbandRedCount << endl;
   cout << "BBox width size     : " << robot.width << endl;
 //  cout << "Sonar L/R values    : " << sonarL << "/" << sonarR << " ==  " << sonarToDistance(sonarL) << "/" << sonarToDistance(sonarR) << endl; 
}
