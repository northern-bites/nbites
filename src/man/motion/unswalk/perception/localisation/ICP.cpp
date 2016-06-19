#include "ICP.hpp"

#include <float.h>
#include <limits>

#include "utils/Logger.hpp"
#include "utils/speech.hpp"
#include "utils/Timer.hpp"
#include "utils/basic_maths.hpp"
#include "utils/angles.hpp"
#include "utils/SPLDefs.hpp"
#include "LocalisationUtils.hpp"
#include "LocalisationConstantsProvider.hpp"

/* Note that the Eigen library version is Eigen 2 (when looking at documentation)*/

// Weighting on each feature (relative to standard weighting of 1)
#define POST_WEIGHT 1.f
#define FIELD_LINE_WEIGHT 1.f
#define CORNER_WEIGHT 1.f
#define T_WEIGHT 1.f
#define CIRCLE_WEIGHT 2.f // increased to help it compete with the centre line (which has 2 points)
#define PARALLEL_LINE_WEIGHT 1.f

// Threshold for deciding MSE is low enough to stop
#define LOCALISED 200*200 // 20cm
#define STOP_THRESHOLD 50*50 // 5cm
#define IMPROVEMENT_THRESHOLD 20*20 //20*20 // 2cm
#define MAX_ITERATIONS 12

// Team ball/own ball threshold from field centre before we use them to decide which end we are facing
#define TEAM_BALL_THRESH 1000
#define RRC_BALL_THRESH 800

#define PARALLEL_THRESH UNSWDEG2RAD(20) // Threshold to decide if line and field edge are parallel
#define SINGLE_POINT_THRESH 1000 // How close source and target need to be before we trust a single point

#define EDGE_ERROR_PERCENT 0.4f // percentage error allowed on field edge distance from line
#define MIN_EDGE_ERROR 100.f // always allow at least this much edge error
#define POST_ERROR_PERCENT 0.2f // percentage error allowed on post distance from line
#define MIN_POST_ERROR 200.f // always allow at least this much post error

// Variances
static const LocalisationConstantsProvider& constantsProvider(
      LocalisationConstantsProvider::instance());

// Certainty threshold before using goalProb
#define GOAL_PROB_THRESH 0.90f

// for distance calcs, 1 radian error is worth how many mm?
#define RADIAN2MM_SCALING 600 

// The arbitrary distance between 2 points chosen to represent an oriented feature
#define FEATURE_WIDTH 1000

#define NO_MATCH -1


// VARIABLES

// Observations 
static std::vector<PostInfo> postObs;

static std::vector<ParallelLinesInfo> parallelLines; // by convention line1 is the goal line, if it is known,
static std::vector<unsigned int> parallelLineTypes;  // and the points are in clockwise order l1.p1, l1.p2, l2.p1, l2.p2

static std::vector<RRCoord> corners;
static std::vector<unsigned int> cornerTypes;

static std::vector<RRCoord> TJunctions;
static std::vector<unsigned int> TJunctionTypes;

static std::vector<RRCoord> centreCircles;

static std::vector<LineInfo> fieldLines;

static std::vector<float> lineLengths; // line lengths squared
static std::vector<float> linePostDist; // perp distance squared from line to nearest goal post
static std::vector<float> lineEdgeDist; // perp distance squared to field edge

static std::vector<LineInfo> fieldEdgeObs;

static int iteration;                   
static int minIterations;  
static bool nonLineFeature;
static bool x_known;
static bool y_known;   
static RRCoord singleFeature;              // used when we want to know the range when using 1 feature

static float maxDist;                      // max dist between two points in a pair
static float mse;                          // mean squared distance error
static int N;                              // number of points used on this iteration
static int totalN;                         // total N, including features that may not be used this iteration                    
static std::vector<Point> source;          // updated source points rebuilt after each iteration
static std::vector<Point> target;          // absolute, according to our field map
static std::vector<TargetType> targetType; // point, vertical line or horizontal line  
static std::vector<float> distances;       // distances between each point pair
static std::vector<float> weights;         // weights to put on each point pair


// Known features on the field
static std::vector<AbsCoord> allCircles;
static std::vector<AbsCoord> allCorners;
static std::vector<unsigned int> allCornerTypes;
static std::vector<AbsCoord> allTJunctions;
static std::vector<unsigned int> allTJunctionTypes;

// Posts
static std::vector<AbsCoord> allPosts;
static std::vector<unsigned int> allPostTypes;

// Field lines
static std::vector<LineInfo> allFieldLines; // in decreasing order of length
static std::vector<std::string> allLineNames; // for debugging
static std::vector<float> allLineLengths; // line lengths squared
static std::vector<float> allLinePostDist; // perp distance from line to nearest goal post
static std::vector<std::pair<float, float> > allLineEdgeDist; // perp distance from line to both field edges
// Sign convention for linePostDist and lineEdgeDist is that with LineInfo.p1 to your left, 
// and LineInfo.p2 to your right, positive distances are on the other side of the line and 
// negative distances are towards you. If I am on the line, pos distances are to my left.
// For edge distances, positive values are in the first of the pair

// Parallel Lines
static std::vector<std::pair<LineInfo, LineInfo> > parallelFieldLines; // by convention the goal line is first
static std::vector<std::vector <Point> > parallelPoints; // the 4 outside points of goal box, helps us get started
static std::vector<std::pair<float, float> > parallelFieldLengths; // line lengths squared

// Field edges, only used directly if you are off the field
static std::vector<LineInfo> allEdgeLines; // by convention stored with the left point in p1, when you
                                           // are on the field looking at the field edge

// robot best estimate position after combining all feature observations
static AbsCoord combinedObs;
static bool haveInitialisedLandmarks = false;

// Builds the vectors of known features on the field
static void initLandmarks();

// Sort out the different types of features
static void preprocessFeatures(   const std::vector<FieldFeatureInfo> &fieldFeatures,
                           const std::vector<PostInfo> &posts, 
                           const std::vector<FieldEdgeInfo> & fieldEdges);

// High level function to associate features to closest feature on field and build points
// Always call it with association=1 first, if it return true you can call it again and increment the iteration
// It will then generate additional goal post matching scenarios (in the situation that the goal post type is unknown)
static void associateFeatures(int iteration);

// Solves to find robot position that best transforms source points to target points
static void solve();

// does one step of solving, with different weights applied to the importance of each point
static void iterate();

// applies a rotation and translation to a 2d poiFeatureTypent
static Point transform(const Point &point, float tx, float ty, float theta);

// finds the mean squared distance (MSE) and distances vector between source and target points
static void calcMSE();

/* Matches an RR observation with the "closest" landmark, and if matching adds to point cloud 
   The return value is the feature type that it was matched to*/
static int matchObs(const RRCoord rr, std::vector<AbsCoord> &landmarks, float weight, 
      unsigned int type = NO_TYPE, std::vector<unsigned int> landmarkTypes = std::vector<unsigned int>());

 /* Matches an RR line observation with the "closest" line */
static bool matchLine(const LineInfo line, float len2, float postDist2, float edgeDist, bool constrainOrientation = false);

 /* Matches an RR edge observation with the "closest" edge */
static bool matchEdge(const LineInfo line);

/* Matches an RR parallel line observation with the "closest" parallel lines, order known means we know for 
   sure the first line in the pair is the goal line, firstMatch means do a rough matching process that is 
   robust to the orientation being wrong */
static bool matchParallelLine(const LineInfo line1, const LineInfo line2, bool orderKnown = false, bool firstMatch = false);

/* Converts RRCoord to AbsCoord */
static AbsCoord rrToAbs(const RRCoord obs, const AbsCoord robotPos);

/* Converts a RR AbsCoord to a real AbsCoord */
static AbsCoord rrToAbs(const AbsCoord obs, const AbsCoord robotPos);

/* Calculates difference between an absolute observation and a landmark */
static float obsLmkDiff(const AbsCoord obs, const AbsCoord lmk);

// Calculate the distance from a line to a field edge. If field edge not parallel returns 0,
// else positive distance indicates edge is on left of line vector from p1 to p2, negative dist is opposite
static float distLineToEdge(const LineInfo line, const FieldEdgeInfo edge); 

// Calculates squared distance from a point to a line segment, and returns the matching point on the line seg
static float dist2ToLineSeg(const LineInfo line, const Point src, Point &tgt);

// As above, but distances is to an infinitely long line, not a line segment
static float dist2ToLine(const LineInfo line, const Point src, Point &tgt ); 

// Adds points to point cloud and checks if points are identical (multiple identical point pairs can cause
// numerical problems and freeze the main thread, so always use this overloaded function
static void addToPointCloud(Point p1, Point p2, float dist, float weight, TargetType type = POINT);

// Translates a pair of AbsCoords to one point pair, (or two point pairs if has both AbsCoords
// have orientation) and pushes them onto the points vectors
static void addToPointCloud(const AbsCoord obs1, const AbsCoord obs2, float weight);


static void reset(void) {
   iteration = 0;
   maxDist = 0.f;
	mse = 0.f;              
   N = 0;       

   if (!haveInitialisedLandmarks) {
      initLandmarks();
      haveInitialisedLandmarks = true;
   }
}

AbsCoord ICP::getCombinedObs(void){
   return combinedObs;
}

std::vector<LineInfo> ICP::getAllFieldLines(void) {
   return allFieldLines;
}

// Entry function from localisationAdapter
int ICP::localise(  const AbsCoord &robotPos,
                    const std::vector<FieldFeatureInfo> &fieldFeatures,
                    const std::vector<PostInfo> &posts,
                    const float awayGoalProb,
                    const float headYaw,
                    const std::vector<FieldEdgeInfo> &fieldEdges,
                    const AbsCoord &ballRRC, bool isLost,
                    const AbsCoord &teamBall ){

   reset();
   
   Timer timer;
   timer.restart();

   llog(DEBUG1) << "\nICP called\n";
   
   preprocessFeatures(fieldFeatures, posts, fieldEdges);

   if ( totalN==0 ) return ICP_NO_OBS;

   // If we only got 1 field line, we can't localise from lost
   if ( isLost && totalN<=2 && !fieldLines.empty()) return ICP_LOST;

   combinedObs = robotPos;
   associateFeatures(1);
   solve();
   int result = ICP_LOST;

   // Check if localised, and not way off the field, while basing an observation on 1 field line
   if (mse < LOCALISED && 
       abs(combinedObs.x()) < (FULL_FIELD_LENGTH/2 + 500) && 
       abs(combinedObs.y()) < (FULL_FIELD_WIDTH/2 + 500)) {

      // calculate variances, higher if on a line that doesn't tell us sideways position 
      combinedObs.theta() = NORMALISE(combinedObs.theta());

      // nonLineFeature helps to recognise when we see a single post / circle and a line
      // that we should make the variance known in both directions
      if (N>=2 && nonLineFeature) {
         x_known=true;
         y_known=true;
      }

      double smallPosVariance = 1.0;
      double largePosVariance = -1.0;
      double smallHeadingVariance = 1.0;
      double largeHeadingVariance = -1;
      
      if (x_known) {
         combinedObs.var(0, 0) = smallPosVariance;
      } else {
         combinedObs.var(0, 0) = largePosVariance;
         if (y_known) combinedObs.x() = robotPos.x();
      }

      if (y_known) {
         combinedObs.var(1, 1) = smallPosVariance;
      } else {
         combinedObs.var(1, 1) = largePosVariance;
         if (x_known) combinedObs.y() = robotPos.y();
      }

      if (N==1) combinedObs.var(2, 2) = largeHeadingVariance;
      else combinedObs.var(2, 2) = smallHeadingVariance; 
   
      result = N; 
      llog(DEBUG1) << "ICP localised, MSE=" << sqrt(mse) << std::endl;
      
   } else {
      llog(DEBUG1) << "ICP couldn't localise or not on field, MSE=" << sqrt(mse) << std::endl;
   }

   llog(DEBUG1) << "ICP localisation took "  << timer.elapsed_us() << " us" << std::endl;
   if (timer.elapsed_us() > 30000) {
      llog(ERROR) << "ICP took " << timer.elapsed_us() << " us" << std::endl;
   }   

   return result;
}




void preprocessFeatures( const std::vector<FieldFeatureInfo> &fieldFeatures,
                              const std::vector<PostInfo> &posts, 
                              const std::vector<FieldEdgeInfo> &fieldEdges){

   postObs = posts;
   if (!posts.empty()) singleFeature = posts[0].rr;

   fieldEdgeObs.clear();

   parallelLines.clear();
   parallelLineTypes.clear();

   corners.clear();
   cornerTypes.clear();

   TJunctions.clear();
   TJunctionTypes.clear();

   centreCircles.clear();

   fieldLines.clear();
   lineLengths.clear();
   linePostDist.clear();
   lineEdgeDist.clear();

   totalN = 0;
   minIterations= 0;
   
   std::vector<FieldFeatureInfo>::const_iterator feature = fieldFeatures.begin();
   for(; feature!=fieldFeatures.end(); ++feature){
      switch (feature->type) {

         case FieldFeatureInfo::fParallelLines:
            {
               if(parallelLines.empty()) minIterations++;

               // Check for a field edge to tell which line is the goal line
               ParallelLinesInfo pl = feature->parallellines;
               bool plKnown = false;
               float minEdgeDist = UNKNOWN_COORD;            
               std::vector<FieldEdgeInfo>::const_iterator edge = fieldEdges.begin();
               for(; edge!=fieldEdges.end(); ++edge){
                  float dist1 = distLineToEdge(pl.l1, *edge);
                  float dist2 = distLineToEdge(pl.l2, *edge);
                  //llog(DEBUG1) << "Parallel lines edge distances are: " << dist1 << ", " << dist2 << "\n";
                  if (dist1 != 0.f && fabs(dist1) < minEdgeDist && fabs(dist1) < (1.f-EDGE_ERROR_PERCENT)*FIELD_LENGTH){
                     minEdgeDist = fabs(dist1);
                     //llog(DEBUG1) << "Choosing dist1\n";
                     plKnown = true;
                  }
                  if (dist2 != 0.f && fabs(dist2) < minEdgeDist && fabs(dist1) < (1.f-EDGE_ERROR_PERCENT)*FIELD_LENGTH){
                     // swap lines so goal line is in l1
                     plKnown = true;                 
                     //llog(DEBUG1) << "Choosing dist1, swap lines\n"; 
                     minEdgeDist = fabs(dist2);
                     LineInfo temp = pl.l1;
                     pl.l1 = pl.l2;
                     pl.l2 = temp;
                  }                    
               }               
               // If no field edge, look for a post
               if (!plKnown){
                  std::vector<PostInfo>::const_iterator post = postObs.begin();               
                  for(; post!=postObs.end(); ++post){
                     Point postXY = post->rr.toCartesian();
                     Point discard;
                     float postDist1 = dist2ToLine(pl.l1, postXY, discard);
                     float postDist2 = dist2ToLine(pl.l2, postXY, discard);
                     llog(DEBUG1) << "Parallel line post dists are: " << postDist1 << ", " << postDist2 << "\n";
                     if ( fabs(postDist1) < MIN_POST_ERROR){
                        llog(DEBUG1) << "Using dist1 for post\n";
                        plKnown = true;
                        break;
                     } else if ( fabs(postDist2) < MIN_POST_ERROR){
                        llog(DEBUG1) << "Using dist1 for post, swap lines\n";
                        // swap lines so goal line is in l1
                        LineInfo temp = pl.l1;
                        pl.l1 = pl.l2;
                        pl.l2 = temp;
                        plKnown = true;
                        break;
                     }
                  }
               }
               // Now ensure the points are entered in clockwise order from l1.p1
               Point line2mid = (pl.l2.p1 + pl.l2.p2)/2;
               PointF line1Vector = (pl.l1.p2 - pl.l1.p1).cast<float>();
               float side = line1Vector.x()*(line2mid.y()-pl.l1.p1.y()) - line1Vector.y()*(line2mid.x()-pl.l1.p1.x());
               if (side > 0.f){                     
                  Point temp = pl.l1.p1;
                  pl.l1.p1 = pl.l1.p2;
                  pl.l1.p2 = temp;
               } 
               
               Point line1mid = (pl.l1.p1 + pl.l1.p2)/2;
               PointF line2Vector = (pl.l2.p2 - pl.l2.p1).cast<float>();
               side = line2Vector.x()*(line1mid.y()-pl.l2.p1.y()) - line2Vector.y()*(line1mid.x()-pl.l2.p1.x());
               if (side > 0.f){                     
                  Point temp = pl.l2.p1;
                  pl.l2.p1 = pl.l2.p2;
                  pl.l2.p2 = temp;
               } 

               if (plKnown){
                  parallelLineTypes.push_back(PL_KNOWN);   
               } else {
                  parallelLineTypes.push_back(PL_UNKNOWN);
               }
               parallelLines.push_back(pl);

            }
            break;


         case FieldFeatureInfo::fLine:
            if ( !feature->lineUsed){ // if not used in an intersection
               if(fieldLines.empty()) minIterations++;

               fieldLines.push_back(feature->line);

               // Record squared length of the line
               float len2 = DISTANCE_SQR(fieldLines.back().p1.x(), fieldLines.back().p1.y(), 
                     fieldLines.back().p2.x(), fieldLines.back().p2.y());
               lineLengths.push_back(len2);

               // Record distance to nearest goal post
               float postDist2 = UNKNOWN_COORD;
               PointF lineVector = (fieldLines.back().p2 - fieldLines.back().p1).cast<float>();

               std::vector<PostInfo>::const_iterator post = postObs.begin();
               for(; post!=postObs.end(); ++post){
                  Point postXY = post->rr.toCartesian();
                  Point discard;

                  float dist2 = sqrt(dist2ToLine(fieldLines.back(), postXY, discard));
                  if (dist2 < fabs(postDist2)) {
                     // Test which side of the line it's on
                     PointF lineVector = (fieldLines.back().p2 - fieldLines.back().p1).cast<float>();

                     float side = lineVector.x()*(postXY.y()-fieldLines.back().p1.y()) - 
                        lineVector.y()*(postXY.x()-fieldLines.back().p1.x());
                     if (side > 0.f){                     
                        postDist2 = dist2;
                     } else {
                        postDist2 = -dist2;      
                     }
                  }                  
               }

               linePostDist.push_back(postDist2);
               
               // Record distance to nearest parallel field edge
               float edgeDist = UNKNOWN_COORD;               
               std::vector<FieldEdgeInfo>::const_iterator edge = fieldEdges.begin();
               for(; edge!=fieldEdges.end(); ++edge){
                  float dist = distLineToEdge(fieldLines.back(), *edge); 
                  if (dist != 0.f && fabs(dist) < fabs(edgeDist)){
                     edgeDist = dist;
                  }                                   
               }               
               lineEdgeDist.push_back(edgeDist);
            }
            break;

         case FieldFeatureInfo::fCorner:
            {            
               if(corners.empty()) minIterations++;
               corners.push_back(feature->rr);

               // Try to Decide which type of corner by looking at nearby edges
               // Specifically only the goal box corners have a nearby edge that cuts rays 
               // extended from the corner point along the two field lines that form it.
               // Which ray is cut tells you if its the left or right of the goal box
            
               std::vector<FieldEdgeInfo>::const_iterator edge = fieldEdges.begin();
               FeatureType type = C_UNKNOWN;
               // lines to represent the corner, this is all still robot relative cartesian
               LineInfo left_arm, right_arm;
               Point centre = feature->rr.toCartesian();
               float left_arm_orient = NORMALISE(feature->rr.heading() - M_PI + feature->rr.orientation() + M_PI_4);
               float right_arm_orient = NORMALISE(feature->rr.heading() - M_PI + feature->rr.orientation() - M_PI_4); 
               left_arm.p1 = centre - Point(1000*cos(left_arm_orient), 1000*sin(left_arm_orient));
               left_arm.p2 = centre + Point(1000*cos(left_arm_orient), 1000*sin(left_arm_orient));
               right_arm.p1 = centre - Point(1000*cos(right_arm_orient), 1000*sin(right_arm_orient));
               right_arm.p2 = centre + Point(1000*cos(right_arm_orient), 1000*sin(right_arm_orient));

               for(; edge!=fieldEdges.end(); ++edge){

                  // Results are 0 if not parallel, positive on left of ray, negative on right

                  float dist_left = distLineToEdge(left_arm , *edge);
                  float dist_right = distLineToEdge(right_arm , *edge);        
   
                  llog(DEBUG1) << "Corner classification: dist_left, dist_right = " << dist_left << ", " << dist_right << "\n";
         
                  if (dist_left < 0.f && fabs(dist_left) < 
                        (1.f + EDGE_ERROR_PERCENT)*(GOAL_BOX_LENGTH+FIELD_LENGTH_OFFSET)){
                     type = C_GB_RIGHT;

                  } else if (dist_right > 0.f && fabs(dist_right) < 
                        (1.f + EDGE_ERROR_PERCENT)*(GOAL_BOX_LENGTH+FIELD_LENGTH_OFFSET)){
                     type = C_GB_LEFT;

                  } else if (dist_right < 0.f && fabs(dist_right) <
                        (1.f + EDGE_ERROR_PERCENT)*std::max(FIELD_WIDTH_OFFSET,FIELD_LENGTH_OFFSET)){
                     type = C_OUTSIDE;

                  } else if (dist_left > 0.f && fabs(dist_left) <
                        (1.f + EDGE_ERROR_PERCENT)*std::max(FIELD_WIDTH_OFFSET,FIELD_LENGTH_OFFSET)){
                     type = C_OUTSIDE;
                  }

               }
               cornerTypes.push_back(type);            
            }             
            break;

         case FieldFeatureInfo::fTJunction:
            {
               if(TJunctions.empty()) minIterations++;

               TJunctions.push_back(feature->rr);

               // Try to decide what type of TJunction by looking for a nearby post
               FeatureType type = T_NO_POST;
               std::vector<PostInfo>::const_iterator post = postObs.begin();
               for(; post!=postObs.end(); ++post){
                  // This section is in robot relative cartesian coordinates
                  Point tXY = feature->rr.toCartesian();
                  Point postXY = post->rr.toCartesian();
                  float dist2 = DISTANCE_SQR(tXY.x(), tXY.y(), postXY.x(), postXY.y());
                  // Don't divide (GOAL_BOX_WIDTH - GOAL_WIDTH) by 2 since we will have a 2x range limit
                  if (dist2 < SQUARE(GOAL_BOX_WIDTH - GOAL_WIDTH)){
                     // Check which side the post is on 
                     float postDir = atan2(postXY.y() - tXY.y(), postXY.x() - tXY.x());
                     if ( NORMALISE(feature->rr.heading() - feature->rr.orientation() - postDir) > 0 ){
                        type = T_POST_RIGHT;
                     } else {
                        type = T_POST_LEFT;
                     }
                  }   
               }
               TJunctionTypes.push_back(type);
            }            
            break;

         case FieldFeatureInfo::fCentreCircle:
            if(centreCircles.empty()) minIterations++;
            singleFeature = feature->rr;
            if(!isnan(feature->rr.orientation()) ){
               totalN++; // circles with orientation become 2 points
            } 
            centreCircles.push_back(feature->rr);
            break;

         default:
            break;
      }
   }   

   totalN += (int) ( posts.size() + 4*parallelLines.size() + 2*fieldLines.size() + centreCircles.size() +
      2*TJunctions.size() + 2*corners.size() );

   // if no other observations and outside field, we will use field edges
   // Only use edges directly if we are outside the field and don't see anything else
   if (totalN == 0){
      llog(DEBUG1) << "Field edge is only feature\n";
      std::vector<FieldEdgeInfo>::const_iterator edge = fieldEdges.begin();
      for(; edge!=fieldEdges.end(); ++edge){
         LineInfo line;
         line.p1 = edge->rrEdge.p1;
         line.p2 = edge->rrEdge.p2;
         Point robot = Point(0,0);
         Point discard;
         float edgeDist =  fabs(dist2ToLineSeg(line, robot, discard));
         llog(DEBUG1) << "Dist to field edge is : " << sqrt(edgeDist) << "\n";
         
         // Don't use edges unless we are really close, otherwise they are too noisy         
         if ( edgeDist < SQUARE(2*std::max(FIELD_LENGTH_OFFSET, FIELD_WIDTH_OFFSET))){
   
            // Ensure that p1 is on our left in the line, for matching convention
            if (line.p1.y() < line.p2.y()){ 
               Point temp = line.p1;
               line.p1 = line.p2;
               line.p2 = temp;
            }
            fieldEdgeObs.push_back(line);
         }
      }
      totalN = (int) 2*fieldEdgeObs.size();
      minIterations++;
   }

   // two posts get matched on the same iteration, but it gives 2 iterations to converge before
   // any other field features are added in
   minIterations += (int) posts.size(); 
   
   // Similarly parallel lines are given 2 iterations to converge, this helps relocate ourselves
   // if we are a bit lost
   minIterations += (int) parallelLines.size();   

}


float distLineToEdge(const LineInfo line, const FieldEdgeInfo edge){

   PointF lineVector = (line.p2 - line.p1).cast<float>();
   lineVector /= hypot(lineVector.x(), lineVector.y());               
   
   PointF edgeVector = (edge.rrEdge.p1 - edge.rrEdge.p2).cast<float>();
   edgeVector /= hypot(edgeVector.x(), edgeVector.y());   

   // Check if edge is close to parallel                  
   float dot = lineVector.dot(edgeVector);
   if ( fabs(dot) > cos(PARALLEL_THRESH) ){
      LineInfo edgeline;
      edgeline.p1 = edge.rrEdge.p1;
      edgeline.p2 = edge.rrEdge.p2;
      Point fieldEdgePoint;
      float dist = sqrt(dist2ToLine(edgeline, (line.p1 + line.p2)/2, fieldEdgePoint));
      // Test which side of the line it's on
      float side = lineVector.x()*(fieldEdgePoint.y()-line.p1.y()) - 
         lineVector.y()*(fieldEdgePoint.x()-line.p1.x());       
      if (side > 0.f){                     
         return dist;
      } else {
         return -dist;      
      }
   } else {
      return 0.f;                              
   }                  

}


void associateFeatures(int association){

/* Matching priority as follows:
   Priority 1: Two posts
   Priority 2: Parallel Lines
   Priority 3: Corner
   Priority 4: T Junction
   Priority 5: One post
   Priority 6: Circles
   Priority 7: Lines 
   Priority 8: Field Edge if you see nothing else and are very close
*/

   source.clear();
   target.clear();
   targetType.clear();
   distances.clear();
   weights.clear();
   N = 0;
   int thisAssociation = 0;
   x_known=false;
   y_known=false;
   nonLineFeature=false;

   llog(DEBUG1) << "Associating these features:\n";   

   // Priority 1: Two Posts
   if (postObs.size() == 2){
      llog(DEBUG1) << "Two posts\n";
      std::vector<PostInfo>::const_iterator post = postObs.begin();
      unsigned int lastPost = PostInfo::pNone;
      for(; post!=postObs.end(); ++post){

         // Prevents two posts being matched to different field ends by checking which
         // end the first post was associated to
         unsigned int thisPost = post->type | (lastPost & (PostInfo::pHome | PostInfo::pAway));
         //llog(DEBUG1) << "Given post :" << post->type << ", changed to: " << thisPost << "\n";
                  
         lastPost = matchObs(post->rr, allPosts, POST_WEIGHT, thisPost, allPostTypes);
         //llog(DEBUG1) << ", matched to: " << lastPost << "\n";
      }
      nonLineFeature = true;
      x_known = true;         
      y_known = true;

      // Give 2 iterations to a two post update to make sure we are in the right spot
      if(++thisAssociation == association) {
         return;
      }
      if(++thisAssociation == association) {
         return;
      }
   }

   // Priority 2: Parallel Lines
   if(!parallelLines.empty()){
      for (int i=0; i<(int)parallelLines.size(); i++){
         if (parallelLineTypes[i] == PL_KNOWN){
            llog(DEBUG1) << "Parallel Lines, points are clockwise, goal line listed first\n";
            matchParallelLine(parallelLines[i].l1, parallelLines[i].l2, true, (association==1));
         } else {
            llog(DEBUG1) << "Parallel Lines, points are clockwise, goal line is not known\n";
            matchParallelLine(parallelLines[i].l1, parallelLines[i].l2, false, (association==1));
         }
      }
      // Give 2 iterations to a parallel line observation to help us converge from anywhere
      if(++thisAssociation == association) {
         return;
      }
      if(++thisAssociation == association) {
         return;
      }
   }

   // Priority 3: Corners
   if(!corners.empty()){
      for (int i=0; i<(int)corners.size(); i++){
         if (cornerTypes[i] == C_GB_RIGHT){
            llog(DEBUG1) << "Corner, GB RIGHT\n";
         } else if (cornerTypes[i] == C_GB_LEFT){
            llog(DEBUG1) << "Corner, GB LEFT\n";
         } else if (cornerTypes[i] == C_OUTSIDE){
            llog(DEBUG1) << "Corner, OUTSIDE\n";
         } else {
            llog(DEBUG1) << "Corner, UNKNOWN\n";
         }      
         matchObs(corners[i], allCorners, CORNER_WEIGHT, cornerTypes[i], allCornerTypes);
      }
      nonLineFeature = true;
      x_known = true;         
      y_known = true;      
      if(++thisAssociation == association) {
         return;
      }
   }
   
   // Priority 4: T Junctions
   if(!TJunctions.empty()){
      for (int i=0; i<(int)TJunctions.size(); i++){
         if (TJunctionTypes[i] == T_POST_RIGHT){
            llog(DEBUG1) << "T Junction, POST RIGHT\n";
         } else if (TJunctionTypes[i] == T_POST_LEFT){
            llog(DEBUG1) << "T Junction, POST LEFT\n";
         } else {
            llog(DEBUG1) << "T Junction, NO POST\n";
         }
         matchObs(TJunctions[i], allTJunctions, T_WEIGHT, TJunctionTypes[i], allTJunctionTypes);
      }
      x_known = true;         
      y_known = true;      
      nonLineFeature = true;
      if(++thisAssociation == association) {
         return;
      }
   }

   // Priority 5: One Post
   if (postObs.size() == 1){
      llog(DEBUG1) << "One post\n";

      matchObs(postObs[0].rr, allPosts, POST_WEIGHT, postObs[0].type, allPostTypes);
      nonLineFeature = true;
      if(++thisAssociation == association) {
         return;
      }
   }

   // Priority 6: Circles
   if(!centreCircles.empty()){
      for (int i=0; i<(int)centreCircles.size(); i++){
         llog(DEBUG1) << "Centre circle\n";      
         matchObs(centreCircles[i], allCircles, CIRCLE_WEIGHT);
      }
      nonLineFeature = true;
      if(++thisAssociation == association) {
         return;
      }
   }

   // Priority 7: Field lines
   if(!fieldLines.empty()){
      
      // If we already have some good information, we can be a bit toughter
      // on how lines are matched to make sure they have the right orientation
      bool constrainOrientation = false;
      // the second part checks that all are points aren't centrecircles without orientation
      if (N>=2 && (nonLineFeature || !parallelLines.empty()) && (int)centreCircles.size()!=N ){
         constrainOrientation = true;
      }
      for (int i=0; i<(int)fieldLines.size(); i++){
         llog(DEBUG1) << "Field line with length: " << sqrt(lineLengths[i]) << ", post dist: " <<
            linePostDist[i] << ", edge dist: " << lineEdgeDist[i] << ", constrain Orientation: "
            << constrainOrientation << "\n";
         matchLine(fieldLines[i], lineLengths[i], linePostDist[i], lineEdgeDist[i], constrainOrientation);
      }
      if(++thisAssociation == association) {
         return;
      }
   }

   //Priority 8: Field Edge if you see nothing else and are very close
   if(N==0 && !fieldEdgeObs.empty()){

      std::vector<LineInfo>::const_iterator edge = fieldEdgeObs.begin();
      for(; edge!=fieldEdgeObs.end(); ++edge){
         llog(DEBUG1) << "Field edge\n";            
         matchEdge(*edge);
      }
      if(++thisAssociation == association) {
         return;
      }

   }

   return;

}


/* Matches an RR line observation with the "closest" line */
bool matchParallelLine(const LineInfo line1, const LineInfo line2, bool orderKnown, bool firstMatch){

   bool result = false;

   // Transform to abs coords
   LineInfo absline1, absline2;
   absline1.p1 = transform(line1.p1, combinedObs.x(), combinedObs.y(), combinedObs.theta());
   absline1.p2 = transform(line1.p2, combinedObs.x(), combinedObs.y(), combinedObs.theta());
   absline2.p1 = transform(line2.p1, combinedObs.x(), combinedObs.y(), combinedObs.theta());
   absline2.p2 = transform(line2.p2, combinedObs.x(), combinedObs.y(), combinedObs.theta());

   // Get length squared  
   float len1 = DISTANCE_SQR(absline1.p1.x(), absline1.p1.y(), absline1.p2.x(), absline1.p2.y());
   float len2 = DISTANCE_SQR(absline2.p1.x(), absline2.p1.y(), absline2.p2.x(), absline2.p2.y());

   // Match to best parallel lines on the field
   float min = FLT_MAX;
   float diff = 0.f;
   Point best_tgt1p1 = Point(0,0), best_tgt1p2 = Point(0,0), best_tgt2p1 = Point(0,0), best_tgt2p2 = Point(0,0);
   Point tgt1p1, tgt1p2, tgt2p1, tgt2p2;  // two lines and two points per line
   TargetType tgt1type1=POINT, tgt1type2=POINT, tgt2type1=POINT, tgt2type2=POINT;

   // try to match, first line to first line and second line to second line, 
   for(int lmk = 0; lmk < (int)parallelFieldLines.size(); lmk++){ 

      // Every 2nd parallel line has the lines in wrong order( goal box line first instead of second)
      if (orderKnown && (lmk%2==1) ) {       
         continue;
      }
         
      // Discard if lines are too long to fit     
      if (parallelFieldLengths[lmk].first < len1 || parallelFieldLengths[lmk].second < len2) continue;

      // Match to closest part of the line
      diff = dist2ToLineSeg(parallelFieldLines[lmk].first, absline1.p1, tgt1p1);
      diff += dist2ToLineSeg(parallelFieldLines[lmk].first, absline1.p2, tgt1p2);
      diff += dist2ToLineSeg(parallelFieldLines[lmk].second, absline2.p1, tgt2p1);
      diff += dist2ToLineSeg(parallelFieldLines[lmk].second, absline2.p2, tgt2p2);

      float gradient = float(absline1.p2.y() - absline1.p1.y())/float(absline1.p2.x() - absline1.p2.x());
      if (firstMatch && (diff > 4*SQUARE(1000) || fabs(gradient) < 1.f) ){ 
         // If distance is large or orientation bad, match to known points that will get us oriented correctly first
         llog(DEBUG1) << "Evaluating Parallel line match: matching to 4 goal box corners\n";

         tgt1p1 = parallelPoints[lmk][0];        
         diff = DISTANCE_SQR(absline1.p1.x(), absline1.p1.y(), tgt1p1.x(), tgt1p1.y());
         tgt1p2 = parallelPoints[lmk][1];        
         diff += DISTANCE_SQR(absline1.p2.x(), absline1.p2.y(), tgt1p2.x(), tgt1p2.y());
         tgt2p1 = parallelPoints[lmk][2];        
         diff += DISTANCE_SQR(absline2.p1.x(), absline2.p1.y(), tgt2p1.x(), tgt2p1.y());
         tgt2p2 = parallelPoints[lmk][3];        
         diff += DISTANCE_SQR(absline2.p2.x(), absline2.p2.y(), tgt2p2.x(), tgt2p2.y());

      } 

      if (diff < min) {
   
         // Prevent line matching from sliding past the end of the real parallel lines, or if no other 
         // observations don't allow them to slide at all
         if(tgt1p1 == parallelFieldLines[lmk].first.p1 || tgt1p1 == parallelFieldLines[lmk].first.p2 || totalN<5) 
            tgt1type1 = POINT;
         else tgt1type1 = VERT_LINE;
         if(tgt1p2 == parallelFieldLines[lmk].first.p1 || tgt1p2 == parallelFieldLines[lmk].first.p2 || totalN<5) 
            tgt1type2 = POINT;
         else tgt1type2 = VERT_LINE;
         if(tgt2p1 == parallelFieldLines[lmk].second.p1 || tgt2p1 == parallelFieldLines[lmk].first.p2 || totalN<5) 
            tgt2type1 = POINT;
         else tgt2type1 = VERT_LINE;
         if(tgt2p2 == parallelFieldLines[lmk].second.p1 || tgt2p2 == parallelFieldLines[lmk].first.p2 || totalN<5) 
            tgt2type2 = POINT;
         else tgt2type2 = VERT_LINE;

         min = diff;
         best_tgt1p1 = tgt1p1;
         best_tgt1p2 = tgt1p2;
         best_tgt2p1 = tgt2p1;
         best_tgt2p2 = tgt2p2;
         result = true;
      }
   }

   if (result) {

      addToPointCloud(absline1.p1, best_tgt1p1, sqrt(min), PARALLEL_LINE_WEIGHT/(hypot(line1.p1.x(), line1.p1.y())/1000), tgt1type1);
      addToPointCloud(absline1.p2, best_tgt1p2, sqrt(min), PARALLEL_LINE_WEIGHT/(hypot(line1.p2.x(), line1.p2.y())/1000), tgt1type2);
      addToPointCloud(absline2.p1, best_tgt2p1, sqrt(min), PARALLEL_LINE_WEIGHT/(hypot(line2.p1.x(), line2.p1.y())/1000), tgt2type1);
      addToPointCloud(absline2.p2, best_tgt2p2, sqrt(min), PARALLEL_LINE_WEIGHT/(hypot(line2.p2.x(), line2.p2.y())/1000), tgt2type2);
      x_known = true;

   }
   return result;

}


/* Matches an RR line observation with the "closest" line */
bool matchLine(const LineInfo line, float len2, float postDist2, float edgeDist, bool constrainOrientation){

   bool result = false;

   // Transform to abs coords
   LineInfo absline;
   absline.p1 = transform(line.p1, combinedObs.x(), combinedObs.y(), combinedObs.theta());
   absline.p2 = transform(line.p2, combinedObs.x(), combinedObs.y(), combinedObs.theta());

   // Match to best line on the field
   float min = FLT_MAX;
   float diff = 0.f;
   Point best_tgt1 = Point(0,0), best_tgt2 = Point(0,0);
   Point tgt1, tgt2;
   TargetType type1 = POINT;
   TargetType type2 = POINT;

   // Get line absolute orientation of observed line mapped to the field
   float absorient = atan2(absline.p1.y() - absline.p2.y(), absline.p1.x() - absline.p2.x());   
   if (absorient < 0.f){ 
      absorient += M_PI;
   }

   for(int lmk = 0; lmk < (int)allFieldLines.size(); lmk++){

      LineInfo fieldLine = allFieldLines[lmk];

      // check line length is within limit         
      if (allLineLengths[lmk] < len2) {        
         break;
      }

      // find orientation of the line we are trying to match to
      bool vertical = true;
      if (fieldLine.p1.y() == fieldLine.p2.y()) vertical = false;

      // Check if orientation is near enough to field line to try to match
      if (vertical && (absorient < UNSWDEG2RAD(30) || absorient > UNSWDEG2RAD(150))){
         // the field line is a vertical line, but this line looks strongly horizontal
         if (constrainOrientation) continue;
      }
      if (!vertical && (absorient > UNSWDEG2RAD(60) && absorient < UNSWDEG2RAD(120))){
         // the field line is a horizontal line, but this line looks strongly vertical
         if (constrainOrientation) continue;
      }


      // Check whether the observed line and actual line are oriented the same direction
      // before testing against post and edges, otherwise we will be checking the wrong side of the line
      // All lines are stored with larger value in line.p1
      float actualPostDist2 = allLinePostDist[lmk];
      std::pair<float,float> actualEdgeDist = allLineEdgeDist[lmk];   
      if ( vertical && absline.p2.y() > absline.p1.y() ){
         // orient it to vertical lines, but it's the wrong way around so..
         actualPostDist2 = -actualPostDist2;
         actualEdgeDist = std::make_pair(-actualEdgeDist.second, -actualEdgeDist.first);
      } else if ( !vertical && absline.p2.x() > absline.p1.x() ) {
         // orient to horizontal lines but wrong way around
         actualPostDist2 = -actualPostDist2;
         actualEdgeDist = std::make_pair(-actualEdgeDist.second, -actualEdgeDist.first);
      }  
      
      // check goal post position is reasonable
      if (postDist2 != UNKNOWN_COORD){
         float postDiff = fabs(postDist2 - actualPostDist2);
         if ( postDiff > MIN_POST_ERROR + fabs(actualPostDist2)*POST_ERROR_PERCENT ) {
            continue;
         }
      }

      // check field edge position is reasonable
      if (edgeDist != UNKNOWN_COORD){
         float actual = actualEdgeDist.first;         
         if (edgeDist < 0){
            actual = actualEdgeDist.second;             
         }
         float edgeDiff = fabs(edgeDist - actual); 
         if ( edgeDiff > MIN_EDGE_ERROR + fabs(actual)*EDGE_ERROR_PERCENT ) {
            continue;
         }
      }
      llog(DEBUG1) << "\tPossible " << allLineNames[lmk] << "\n";
      diff = dist2ToLineSeg(fieldLine, absline.p1, tgt1);
      diff += dist2ToLineSeg(fieldLine, absline.p2, tgt2);
      if (diff < min) {
         // if N>=3 we allow lines to slide, but must prevent
         // lines from sliding past the end of the actual field line
         if(tgt1 == fieldLine.p1 || tgt1 == fieldLine.p2 || totalN<3 )  type1 = POINT;
         else type1 = HOR_LINE;
         if(tgt2 == fieldLine.p1 || tgt2 == fieldLine.p2 || totalN<3 )  type2 = POINT;
         else type2 = HOR_LINE;
         min = diff;
         best_tgt1 = tgt1;
         best_tgt2 = tgt2;
         result = true;
      }
   }

   if (result) {

      if ( best_tgt1.x()==best_tgt2.x() ){ // line is matched to a vertical line
         x_known = true;
         if (type1 == HOR_LINE) type1 = VERT_LINE;
         if (type2 == HOR_LINE) type2 = VERT_LINE;
      } else {
         y_known = true;
      }

      addToPointCloud(absline.p1, best_tgt1, sqrt(min), FIELD_LINE_WEIGHT/(hypot(line.p1.x(), line.p1.y())/1000), type1);
      addToPointCloud(absline.p2, best_tgt2, sqrt(min), FIELD_LINE_WEIGHT/(hypot(line.p2.x(), line.p2.y())/1000), type2);

   }
   return result;
}


/* Matches an RR edge observation with the "closest" edge */
bool matchEdge(const LineInfo line){

   bool result = false;

   // Transform to abs coords
   LineInfo absline;
   absline.p1 = transform(line.p1, combinedObs.x(), combinedObs.y(), combinedObs.theta());
   absline.p2 = transform(line.p2, combinedObs.x(), combinedObs.y(), combinedObs.theta());

   // Match to closest edge on the field
   float min = FLT_MAX;
   float diff = 0.f;
   Point best_tgt1 = Point(0,0), best_tgt2 = Point(0,0);
   Point tgt1, tgt2;
   TargetType type1 = POINT;
   TargetType type2 = POINT;

   for(int lmk = 0; lmk < (int)allEdgeLines.size(); lmk++){

      LineInfo edgeLine = allEdgeLines[lmk];

      // All lines are stored with larger value in line.p1
      diff = dist2ToLineSeg(edgeLine, absline.p1, tgt1);
      diff += dist2ToLineSeg(edgeLine, absline.p2, tgt2);
      if (diff < min) {
         
         type1 = HOR_LINE;
         type2 = HOR_LINE;
         min = diff;
         
         // Flip targets if necessary to ensure that we are matching from inside the field, not out
         // By convention edgeLine.p1 holds the left end of the field edge (when you are on field)
         bool flipTargets = false;
         //llog(DEBUG1) << "Edgeline: \n" << edgeLine.p1 << "\n" << edgeLine.p2 << "\n";
         if (edgeLine.p1.y() < edgeLine.p2.y()  && tgt1.y() > tgt2.y()) flipTargets = true;
         if (edgeLine.p2.y() < edgeLine.p1.y()  && tgt2.y() > tgt1.y()) flipTargets = true;
         if (edgeLine.p1.x() < edgeLine.p2.x()  && tgt1.x() > tgt2.x()) flipTargets = true;
         if (edgeLine.p2.x() < edgeLine.p1.x()  && tgt2.x() > tgt1.x()) flipTargets = true;
         //llog(DEBUG1) << "Flip targets: " << flipTargets << "\n";         
         if (flipTargets){
            best_tgt1 = tgt2;
            best_tgt2 = tgt1;
         } else {
            best_tgt1 = tgt1;
            best_tgt2 = tgt2;
         }
         result = true;
      }
   }

   if (result) {

      if ( best_tgt1.x()==best_tgt2.x() ){ // matched to a vertical edge
         x_known = true;
         if (type1 == HOR_LINE) type1 = VERT_LINE;
         if (type2 == HOR_LINE) type2 = VERT_LINE;
      } else {
         y_known = true;
      }

      addToPointCloud(absline.p1, best_tgt1, sqrt(min), FIELD_LINE_WEIGHT/(hypot(line.p1.x(), line.p1.y())/1000), type1);
      addToPointCloud(absline.p2, best_tgt2, sqrt(min), FIELD_LINE_WEIGHT/(hypot(line.p2.x(), line.p2.y())/1000), type2);

   }
   return result;
}


// Calculates squared distance from a point to an infinitely long line, and returns the matching point
float dist2ToLine(const LineInfo line, const Point src, Point &tgt){
   
   const float l2 = DISTANCE_SQR(line.p1.x(), line.p1.y(), line.p2.x(), line.p2.y());
   if (l2 == 0.0f) { // line is really a point
      tgt = line.p1;
      return DISTANCE_SQR(tgt.x(), tgt.y(), src.x(), src.y());
   }
   
   const float t = (src-line.p1).dot(line.p2-line.p1) / l2; 
   PointF len = (line.p2 - line.p1).cast<float>();
   len = len*t;
   tgt = line.p1 + len.cast<int>();
   return DISTANCE_SQR(tgt.x(), tgt.y(), src.x(), src.y());
}



// Calculates squared distance from a point to a line segment, and returns the matching point on the line seg
float dist2ToLineSeg(const LineInfo line, const Point src, Point &tgt){
   
   const float l2 = DISTANCE_SQR(line.p1.x(), line.p1.y(), line.p2.x(), line.p2.y());
   if (l2 == 0.0f) { // line is really a point
      tgt = line.p1;
      return DISTANCE_SQR(tgt.x(), tgt.y(), src.x(), src.y());
   }

   bool vertical = true;
   if (line.p1.y() == line.p2.y()) vertical = false;
   
   const float t = (src-line.p1).dot(line.p2-line.p1) / l2; 
   if (t < 0.f) {
      tgt = line.p1;
      if (vertical) y_known = true;
      else x_known = true;
   } else if ( t > 1.f){
      tgt = line.p2;
      if (vertical) y_known = true;
      else x_known = true;
   } else {
      PointF len = (line.p2 - line.p1).cast<float>();
      len = len*t;
      tgt = line.p1 + len.cast<int>();
   }
   return DISTANCE_SQR(tgt.x(), tgt.y(), src.x(), src.y());
}


/* Matches an RR observation with the "closest" landmark, and if matching adds to point cloud */
int matchObs(const RRCoord rr, std::vector<AbsCoord> &landmarks, float weight, 
               unsigned int type, std::vector<unsigned int> landmarkTypes) {

   AbsCoord obsAbs = rrToAbs(rr, combinedObs);   
   // circles have different orientation to Ts and corners
   if( landmarks == allCircles){
      obsAbs.theta() = NORMALISE(obsAbs.theta() - M_PI); 
   }   

   int match = NO_MATCH;
   float min = FLT_MAX;
   for(int lmk = 0; lmk < (int)landmarks.size(); lmk++){

      // Check if the observation is the correct type
      if( !landmarkTypes.empty() ){

         // Restrictions on T junctions
         if (type==T_POST_LEFT && landmarkTypes[lmk]!=T_POST_LEFT) continue;
         if (type==T_POST_RIGHT && landmarkTypes[lmk]!=T_POST_RIGHT) continue;

         // Restrictions on corners
         if (type==C_GB_LEFT && landmarkTypes[lmk]!=C_GB_LEFT) continue;
         if (type==C_GB_RIGHT && landmarkTypes[lmk]!=C_GB_RIGHT) continue;
         if (type==C_OUTSIDE && landmarkTypes[lmk]!=C_OUTSIDE) continue;

         // Restrictions on posts
         if (type>=PostInfo::pNone && type <=PostInfo::pAwayRight){
            if ( (type^landmarkTypes[lmk]) & type) {
               continue;
            }
         }
         
      }
         
      float diff = obsLmkDiff(obsAbs, landmarks[lmk]);
      //llog(DEBUG1) << "Diff to landmark is : " << diff << "\n";
      if (diff < min) {
         min = diff;
         match = lmk;
      }
      //llog(DEBUG1) << "match is : " << match << "\n";
   }
   if (match!= NO_MATCH){
      // Weight is scaled by 1/distance to landmark in m, since further observations have more noise
      addToPointCloud(obsAbs, landmarks[match], weight/(rr.distance()/1000));
      if (!landmarkTypes.empty()){
         llog(DEBUG1) << "Observation matched with landmark type: " << landmarkTypes[match] << "\n";
         return landmarkTypes[match];
      } else { 
         return NO_TYPE;
      }      
   } else {
      return NO_TYPE;      
   }
}

/* Converts RRCoord to AbsCoord */
AbsCoord rrToAbs(const RRCoord obs, const AbsCoord robotPos) {
   Point obsCartesian = obs.toCartesian();
   Point obsAbsPos = transform(obsCartesian, robotPos.x(), robotPos.y(), robotPos.theta());
   float obsTheta = NORMALISE(robotPos.theta() + obs.heading() - obs.orientation() + M_PI );   
   return AbsCoord(obsAbsPos.x(), obsAbsPos.y(), obsTheta);
}

/* Converts a RR AbsCoord to a real AbsCoord */
AbsCoord rrToAbs(const AbsCoord obs, const AbsCoord robotPos) {
   Point obsCartesian = Point (obs.x(), obs.y());
   float obsHeading = atan2(obs.y(), obs.x());
   Point obsAbsPos = transform(obsCartesian, robotPos.x(), robotPos.y(), robotPos.theta());
   float obsTheta = NORMALISE(robotPos.theta() + obsHeading - obs.theta() + M_PI );   
   return AbsCoord(obsAbsPos.x(), obsAbsPos.y(), obsTheta);
}

/* Calculates difference between an absolute observation and a landmark */
float obsLmkDiff(const AbsCoord obs, const AbsCoord lmk) {
   // if there is no orientation, difference should be distance only
   float thetaDiff = 0;
   if (!isnan(obs.theta()) && !isnan(lmk.theta())) {
      thetaDiff = fabs(NORMALISE(lmk.theta() - obs.theta()));
   }
   // consider using DISTANCE_SQR
   float posDiff = DISTANCE(lmk.x(), lmk.y(), obs.x(), obs.y());
   return posDiff + RADIAN2MM_SCALING*thetaDiff;
}

void addToPointCloud(Point p1, Point p2, float dist, float weight, TargetType type){

   /* check if the source and target points are already perfectly matched,
      if so add a little noise (1mm) since this is numerical dangerous
      we have verified that 3 or more copies of the same two perfectly matching point pairs
      will cause it to freeze the main thread. An example that will cause this is:
      (10, 184) (10,184)
      (10, 184) (10,184)
      (10, 184) (10,184)
      (0, -125) (0, -125)
      (0, -125) (0, -125)
      (0, -125) (0, -125)
   */
   
   if(p1.x() == p2.x() && p1.y() == p2.y()){
      p1.x() = p1.x()+1;
      p1.y() = p1.y()+1;
   }

   llog(DEBUG1) << "Source point: (" << p1.x() << ", " << p1.y() << "), ";
   llog(DEBUG1) << "\tTarget point: (" << p2.x() << ", " << p2.y() << "), ";
   llog(DEBUG1) << "\tDistance: " << DISTANCE(p1.x(), p1.y(), p2.x(), p2.y()) << ", ";
   llog(DEBUG1) << "\tWeight: " << weight << ", ";
   llog(DEBUG1) << "\tTargetType: ";
   if (type == POINT ) llog(DEBUG1) << "POINT\n";
   if (type == VERT_LINE ) llog(DEBUG1) << "VERT_LINE\n";
   if (type == HOR_LINE ) llog(DEBUG1) << "HOR_LINE\n";

   source.push_back(p1);
   target.push_back(p2);
   targetType.push_back(type);
   distances.push_back(dist);
   weights.push_back(weight);
   N = (int) source.size();

}

// Translates a pair of AbsCoords to one point pair, or two point pairs if has both AbsCoords
// have orientation, and pushes them onto the points vectors
void addToPointCloud(const AbsCoord obs1, const AbsCoord obs2, float weight){
   Point centre1 = Point( obs1.x(), obs1.y() );
   Point centre2 = Point( obs2.x(), obs2.y() );

   if (isnan(obs1.theta()) || isnan(obs2.theta())) {
    
      // if no orientation then generate 1 point pair
      float dist = DISTANCE(centre1.x(), centre1.y(), centre2.x(), centre2.y());
      addToPointCloud(centre1, centre2, dist, weight);

   } else {
    
      // given orientation, generate two point pairs
      Point offset1 = Point( cos(obs1.theta())*FEATURE_WIDTH/2, sin(obs1.theta())*FEATURE_WIDTH/2 ); 
      Point offset2 = Point( cos(obs2.theta())*FEATURE_WIDTH/2, sin(obs2.theta())*FEATURE_WIDTH/2 );       

      Point src1 = centre1+offset1;
      Point tgt1 = centre2+offset2;
      Point src2 = centre1-offset1;
      Point tgt2 = centre2-offset2;

      float dist = DISTANCE(src1.x(), src1.y(), tgt1.x(), tgt1.y());
      dist += DISTANCE(src2.x(), src2.y(), tgt2.x(), tgt2.y());
      dist /= 2; // average of the two point pair distances

      addToPointCloud(src1, tgt1, dist, weight);
      addToPointCloud(src2, tgt2, dist, weight);

      x_known = true;
      y_known = true;        
   }

}



// Solves to find the tx, ty, theta that best transforms source points to target points
void solve(){

   calcMSE();
	llog(DEBUG1) << "Starting Mean Error: "<< sqrt(mse) << " mm\n";
   iteration = 1;
	bool converging = false; 
   llog(DEBUG1) << "\nIteration: " << iteration << "\n";


   // Keep track of the best position, since sometimes we go past it to check convergence
   AbsCoord bestPos;
   float bestMSE = FLT_MAX;

	while(true){

		iterate();

      // Regenerate the source points using new robot start position
      associateFeatures(iteration+1);

		// Check the mean squared distance error
      float old_mse = mse;
		calcMSE();
		llog(DEBUG1) << "Mean Error: "<< sqrt(mse) << " mm\n";
   
      if(iteration >= minIterations && mse < bestMSE){
         bestMSE = mse;
         bestPos = combinedObs;
      }

		if (iteration >= minIterations && mse < STOP_THRESHOLD ) {
			llog(DEBUG1) << "\nStopping since error below threshold" << std::endl;
   		break;
		}
		if (iteration >= minIterations && old_mse-mse < IMPROVEMENT_THRESHOLD) {
			if(converging){ // need to see no improvement in 2 consecutive iterations
				llog(DEBUG1) << "\nStopping due to convergence" << std::endl;
				break;
			} else {
				converging = true;
			}
		} else {
			converging = false;
		}
		if (iteration>=MAX_ITERATIONS){
			llog(DEBUG1) << "\nStopping after max " << MAX_ITERATIONS <<" iterations" << std::endl;
			break;
		}

      iteration++;
      llog(DEBUG1) << "\nIteration: " << iteration << "\n";
      
	}
   // Reinstate best position
   combinedObs = bestPos;
   mse = bestMSE;

}


// Does on iteration of Iterative Closest Point and returns tx, ty, theta in result
void iterate(){

   // Can get N==0 when the first association doesn't work
   if (N==0) return;

   // If N==1, we give up unless the source and target are sufficiently close that we trust
   if (N==1){
      if (DISTANCE_SQR(source[0].x(), source[0].y(), target[0].x(), target[0].y()) > SQUARE(SINGLE_POINT_THRESH)){
         return;
      } else if (singleFeature.distance() > CENTER_CIRCLE_DIAMETER/2) {
         // If range > 300, adjust heading first (because the iteration only will only give translation)

         // Calulate difference of robot-target heading and robot-source heading
         float headingDiff = atan2(combinedObs.y() - target[0].y(), combinedObs.x() - target[0].x())
                           - atan2(combinedObs.y() - source[0].y(), combinedObs.x() - source[0].x());

         RRCoord adjustedFeature = singleFeature;

         // Add heading difference to original robot relative observation.
         adjustedFeature.heading() += atan2(combinedObs.y() - target[0].y(), combinedObs.x() - target[0].x())
                                    - atan2(combinedObs.y() - source[0].y(), combinedObs.x() - source[0].x());

         // Create new source point from adjusted singlepoint.
         AbsCoord newSource = rrToAbs(adjustedFeature, combinedObs);
         source[0] = Point(newSource.x(), newSource.y());

         // Add headingDiff to robot's theta
         combinedObs.theta() = NORMALISE(combinedObs.theta() + headingDiff);   
      }
   }

	// First calculate the centre of mass from each point set
	Point source_com = Eigen::Vector2i::Zero(); 
	Point target_com = Eigen::Vector2i::Zero();
	for(int i=0; i<N; i++){
		source_com = source_com + source[i];
		target_com = target_com + target[i];
	}
	source_com = source_com / N;
	target_com = target_com / N;

	// This matrix system represents trying to set the partial derivatives of the MSE error function
	// wrt tx, ty and theta to zero. Therefore 3 unknowns and 3*num_of_points equations.
	const int m = N*3;

	Eigen::MatrixXf A(m,3);
	Eigen::MatrixXf b(m,1);
   Eigen::Vector3f result;

	int Ar = 0;
	int Ac = 0;
	int br = 0;
	for(int i=0; i<N; i++){
	
		float w = weights[i];
		Point p = source[i] - source_com;
		Point q = target[i] - target_com;
      TargetType type = targetType[i];

      if (type == POINT) {

         // Matrix for matching a point to a point
		   A.coeffRef(Ar, Ac++) = w;
		   A.coeffRef(Ar, Ac++) = 0;
		   A.coeffRef(Ar++, Ac) = -w*p.y();
		   Ac = 0;
	
		   A.coeffRef(Ar, Ac++) = 0;
		   A.coeffRef(Ar, Ac++) = w;
		   A.coeffRef(Ar++, Ac) = w*p.x();
		   Ac = 0;

		   A.coeffRef(Ar, Ac++) = -w*p.y();
		   A.coeffRef(Ar, Ac++) = w*p.x();
		   A.coeffRef(Ar++, Ac) = w*(p.x()*p.x()+p.y()*p.y());
		   Ac = 0;

		   b.coeffRef(br++) = w*(q.x() - p.x());
		   b.coeffRef(br++) = w*(q.y() - p.y());
		   b.coeffRef(br++) = w*(p.x()*(q.y()-p.y()) - p.y()*(q.x()-p.x()));

      } else if (type == VERT_LINE){

         // Matrix for matching a point to a vertical line
		   A.coeffRef(Ar, Ac++) = w;
		   A.coeffRef(Ar, Ac++) = 0;
		   A.coeffRef(Ar++, Ac) = -w*p.y();
		   Ac = 0;
	
		   A.coeffRef(Ar, Ac++) = 0;
		   A.coeffRef(Ar, Ac++) = 0;
		   A.coeffRef(Ar++, Ac) = 0;
		   Ac = 0;

		   A.coeffRef(Ar, Ac++) = -w*p.y();
		   A.coeffRef(Ar, Ac++) = 0;
		   A.coeffRef(Ar++, Ac) = w*(p.y()*p.y());
		   Ac = 0;

		   b.coeffRef(br++) = w*(q.x() - p.x());
		   b.coeffRef(br++) = 0;
		   b.coeffRef(br++) = w*( -p.y()*(q.x()-p.x()));

      } else if (type == HOR_LINE){

         // Matrix for matching a point to a horizontal line
		   A.coeffRef(Ar, Ac++) = 0;
		   A.coeffRef(Ar, Ac++) = 0;
		   A.coeffRef(Ar++, Ac) = 0;
		   Ac = 0;
	
		   A.coeffRef(Ar, Ac++) = 0;
		   A.coeffRef(Ar, Ac++) = w;
		   A.coeffRef(Ar++, Ac) = w*p.x();
		   Ac = 0;

		   A.coeffRef(Ar, Ac++) = 0;
		   A.coeffRef(Ar, Ac++) = w*p.x();
		   A.coeffRef(Ar++, Ac) = w*(p.x()*p.x());
		   Ac = 0;

		   b.coeffRef(br++) = 0;
		   b.coeffRef(br++) = w*(q.y() - p.y());
		   b.coeffRef(br++) = w*(p.x()*(q.y()-p.y()) );

      }
	
	}

	A.svd().solve(b,&result);

   // transform the starting robot position to our new estimate
   llog(DEBUG1) << "Robot position update:\n";
   llog(DEBUG1) << "From: (x,y,theta) = (" << combinedObs.x() << ", " << 
      combinedObs.y() << ", " << RAD2DEG(combinedObs.theta()) << ")\n";

   Point oldrobot = Point(combinedObs.x(), combinedObs.y());
   Point oldrobot_less_com = oldrobot - source_com;
   Point newrobot = transform( oldrobot_less_com, result(0), result(1), result(2)) + target_com;

   Point oldsrc = source[0]; // pick any observation point, they should all work
   Point oldsrc_less_com = oldsrc - source_com;   
   Point newsrc = transform( oldsrc_less_com, result(0), result(1), result(2)) + target_com;
   
   float robotHeadingChg = atan2(newrobot.y() - newsrc.y(), newrobot.x() - newsrc.x())
      - atan2(oldrobot.y() - oldsrc.y(), oldrobot.x() - oldsrc.x());

   combinedObs = AbsCoord(newrobot.x(), newrobot.y(), combinedObs.theta()+robotHeadingChg);
   llog(DEBUG1) << "To:   (x,y,theta) = (" << combinedObs.x() << ", " << 
      combinedObs.y() << ", " << RAD2DEG(combinedObs.theta()) << ")\n";

}

// Does a rotation and translation transformation
Point transform(const Point &point, float tx, float ty, float theta){
	Point result;
	result.x() = cos(theta)*point.x() - sin(theta)*point.y() + tx;
	result.y() = sin(theta)*point.x() + cos(theta)*point.y() + ty;
	return result;
}

// Finds the MSE and builds a vector of distances between points, doesn't verify source and target lengths
void calcMSE(){
	
   // find maxDist
   maxDist = 0.f;
   for(int i=0; i<N; i++){
		float dist = distances[i];
		if(dist>maxDist) maxDist = dist;
	}

   // Now find MSE including the weighting
   mse = 0.f;
   float weight_sum = 0.f;
	for(int i=0; i<N; i++){
		float dist = distances[i];
		mse += (dist*dist)*weights[i];
      weight_sum += weights[i];
	}
   if (N>0){
	   mse = mse / ((float)N*weight_sum);
   }
}


void initLandmarks() {

   allCircles.clear();
   allCorners.clear();
   allCornerTypes.clear();   
   allTJunctions.clear();
   allTJunctionTypes.clear();   

   allPosts.clear();
   allPostTypes.clear();

   allFieldLines.clear();
   allLineNames.clear();
   allLineLengths.clear();
   allLinePostDist.clear();
   allLineEdgeDist.clear();

   parallelFieldLines.clear();
   parallelPoints.clear();
   parallelFieldLengths.clear();

   allEdgeLines.clear();

   AbsCoord coord;

   // Circle from left
   coord.x() = 0.0f;
   coord.y() = 0.0f;
   coord.theta() = -M_PI_2;
   allCircles.push_back(coord);
   // Circle from right
   coord.x() = 0.0f;
   coord.y() = 0.0f;
   coord.theta() = M_PI_2;
   allCircles.push_back(coord);

   // Opposition left post
   coord.x() = FIELD_LENGTH/2;
   coord.y() = GOAL_WIDTH/2;
   coord.theta() = std::numeric_limits<float>::quiet_NaN( );
   allPosts.push_back(coord);
   allPostTypes.push_back(PostInfo::pAwayLeft);
   // Opposition right post
   coord.x() = FIELD_LENGTH/2;
   coord.y() = -GOAL_WIDTH/2;
   coord.theta() = std::numeric_limits<float>::quiet_NaN( );
   allPosts.push_back(coord);
   allPostTypes.push_back(PostInfo::pAwayRight);
   // Home left post
   coord.x() = -FIELD_LENGTH/2;
   coord.y() = -GOAL_WIDTH/2;
   coord.theta() = std::numeric_limits<float>::quiet_NaN( );
   allPosts.push_back(coord);
   allPostTypes.push_back(PostInfo::pHomeLeft);
   // Home right post
   coord.x() = -FIELD_LENGTH/2;
   coord.y() = GOAL_WIDTH/2;
   coord.theta() = std::numeric_limits<float>::quiet_NaN( );
   allPosts.push_back(coord);
   allPostTypes.push_back(PostInfo::pHomeRight);

   // Away-left corner
   coord.x() = FIELD_LENGTH/2;
   coord.y() = FIELD_WIDTH/2;
   coord.theta() = -3*M_PI_4;
   allCorners.push_back(coord);
   allCornerTypes.push_back(C_OUTSIDE);
   // Away-right corner
   coord.x() = FIELD_LENGTH/2;
   coord.y() = -FIELD_WIDTH/2;
   coord.theta() = 3*M_PI_4;
   allCorners.push_back(coord);
   allCornerTypes.push_back(C_OUTSIDE);
   // Home right corner
   coord.x() = -FIELD_LENGTH/2;
   coord.y() = FIELD_WIDTH/2;
   coord.theta() = -M_PI_4;
   allCorners.push_back(coord);
   allCornerTypes.push_back(C_OUTSIDE);
   // Home left corner
   coord.x() = -FIELD_LENGTH/2;
   coord.y() = -FIELD_WIDTH/2;
   coord.theta() = M_PI_4;
   allCorners.push_back(coord);
   allCornerTypes.push_back(C_OUTSIDE);
   // Away left goal box
   coord.x() = FIELD_LENGTH/2-GOAL_BOX_LENGTH;
   coord.y() = GOAL_BOX_WIDTH/2;
   coord.theta() = -M_PI_4;
   allCorners.push_back(coord);
   allCornerTypes.push_back(C_GB_LEFT);
   // Away right goal box
   coord.x() = FIELD_LENGTH/2-GOAL_BOX_LENGTH;
   coord.y() = -GOAL_BOX_WIDTH/2;
   coord.theta() = M_PI_4;
   allCorners.push_back(coord);
   allCornerTypes.push_back(C_GB_RIGHT);
   // Home right goal box
   coord.x() = -FIELD_LENGTH/2+GOAL_BOX_LENGTH;
   coord.y() = GOAL_BOX_WIDTH/2;
   coord.theta() = -3*M_PI_4;
   allCorners.push_back(coord);
   allCornerTypes.push_back(C_GB_RIGHT);
   // Home left goal box
   coord.x() = -FIELD_LENGTH/2+GOAL_BOX_LENGTH;
   coord.y() = -GOAL_BOX_WIDTH/2;
   coord.theta() = 3*M_PI_4;
   allCorners.push_back(coord);
   allCornerTypes.push_back(C_GB_LEFT);


   // Left half-way line T
   coord.x() = 0;
   coord.y() = -FIELD_WIDTH/2;
   coord.theta() = M_PI_2;
   allTJunctions.push_back(coord);
   allTJunctionTypes.push_back(T_NO_POST);
   // Right half-way line T
   coord.x() = 0;
   coord.y() = FIELD_WIDTH/2;
   coord.theta() = -M_PI_2;
   allTJunctions.push_back(coord);
   allTJunctionTypes.push_back(T_NO_POST);
   // Away-left goal T
   coord.x() = FIELD_LENGTH/2;
   coord.y() = GOAL_BOX_WIDTH/2;
   coord.theta() = -M_PI;
   allTJunctions.push_back(coord);
   allTJunctionTypes.push_back(T_POST_RIGHT);   
   // Away-right goal T
   coord.x() = FIELD_LENGTH/2;
   coord.y() = -GOAL_BOX_WIDTH/2;
   coord.theta() = -M_PI;
   allTJunctions.push_back(coord);
   allTJunctionTypes.push_back(T_POST_LEFT);
   // Home-right goal T
   coord.x() = -FIELD_LENGTH/2;
   coord.y() = GOAL_BOX_WIDTH/2;
   coord.theta() = 0;
   allTJunctions.push_back(coord);
   allTJunctionTypes.push_back(T_POST_LEFT);
   // Home-left goal T
   coord.x() = -FIELD_LENGTH/2;
   coord.y() = -GOAL_BOX_WIDTH/2;
   coord.theta() = 0;
   allTJunctions.push_back(coord);
   allTJunctionTypes.push_back(T_POST_RIGHT);


   LineInfo line;
   LineInfo line1;
   LineInfo line2;
   
   // Field lines
   // For easy processing, these all entered with larger values on line.p1
   // and positive values in the first entry of the allLineEdgeDist pair

   // Right side line
   line.p1 = Point(+FIELD_LENGTH/2,-FIELD_WIDTH/2);
   line.p2 = Point(-FIELD_LENGTH/2,-FIELD_WIDTH/2);  
   allFieldLines.push_back(line);
   allLineNames.push_back("Right Sideline");
   allLineLengths.push_back(SQUARE(FIELD_LENGTH));
   allLinePostDist.push_back(-(FIELD_WIDTH-GOAL_WIDTH)/2 );
   allLineEdgeDist.push_back(std::make_pair(+(FIELD_WIDTH_OFFSET),-(FIELD_WIDTH_OFFSET+FIELD_WIDTH)));

   // Left side line
   line.p1 = Point(+FIELD_LENGTH/2, FIELD_WIDTH/2);
   line.p2 = Point(-FIELD_LENGTH/2, FIELD_WIDTH/2);  
   allFieldLines.push_back(line);
   allLineNames.push_back("Left Sideline");
   allLineLengths.push_back(SQUARE(FIELD_LENGTH));
   allLinePostDist.push_back(+(FIELD_WIDTH-GOAL_WIDTH)/2 );
   allLineEdgeDist.push_back(std::make_pair(+(FIELD_WIDTH_OFFSET+FIELD_WIDTH), -FIELD_WIDTH_OFFSET));

   // home goal line
   line.p1 = Point(-FIELD_LENGTH/2, FIELD_WIDTH/2);
   line.p2 = Point(-FIELD_LENGTH/2,-FIELD_WIDTH/2);  
   allFieldLines.push_back(line);
   allLineNames.push_back("Home Goal Line");
   allLineLengths.push_back(SQUARE(FIELD_WIDTH));
   allLinePostDist.push_back( 0.f );
   allLineEdgeDist.push_back(std::make_pair( FIELD_LENGTH+FIELD_LENGTH_OFFSET,-FIELD_LENGTH_OFFSET));

   // away goal line
   line1.p1 = Point(FIELD_LENGTH/2, FIELD_WIDTH/2);  
   line1.p2 = Point(FIELD_LENGTH/2,-FIELD_WIDTH/2);
   allFieldLines.push_back(line1);
   allLineNames.push_back("Away Goal Line");
   allLineLengths.push_back(SQUARE(FIELD_WIDTH));   
   allLinePostDist.push_back( 0.f );
   allLineEdgeDist.push_back(std::make_pair(+FIELD_LENGTH_OFFSET, -(FIELD_LENGTH+FIELD_LENGTH_OFFSET)));

   // half way line
   line2.p1 = Point(0, FIELD_WIDTH/2);
   line2.p2 = Point(0,-FIELD_WIDTH/2);  
   allFieldLines.push_back(line2);
   allLineNames.push_back("Half Way Line");
   allLineLengths.push_back(SQUARE(FIELD_WIDTH));
   allLinePostDist.push_back(FIELD_LENGTH/2);
   allLineEdgeDist.push_back(std::make_pair(+(FIELD_LENGTH/2+FIELD_LENGTH_OFFSET), 
      -(FIELD_LENGTH/2+FIELD_LENGTH_OFFSET)));

   // home goalie box line
   line2.p1 = Point(-FIELD_LENGTH/2+GOAL_BOX_LENGTH, +GOAL_BOX_WIDTH/2);
   line2.p2 = Point(-FIELD_LENGTH/2+GOAL_BOX_LENGTH, -GOAL_BOX_WIDTH/2);
   allFieldLines.push_back(line2);
   allLineNames.push_back("Home Goal Box Line");
   allLineLengths.push_back(SQUARE(GOAL_BOX_WIDTH));
   allLinePostDist.push_back(-GOAL_BOX_LENGTH);
   allLineEdgeDist.push_back(std::make_pair((FIELD_LENGTH - GOAL_BOX_LENGTH + FIELD_LENGTH_OFFSET),
         -(GOAL_BOX_LENGTH + FIELD_LENGTH_OFFSET)));

   // home parallel lines
   parallelFieldLines.push_back( std::make_pair(line, line2));
   parallelFieldLengths.push_back( std::make_pair(SQUARE(FIELD_WIDTH), SQUARE(GOAL_BOX_WIDTH)));
   std::vector<Point> points;
   points.push_back(Point(-FIELD_LENGTH/2,-GOAL_BOX_WIDTH/2));
   points.push_back(Point(-FIELD_LENGTH/2,GOAL_BOX_WIDTH/2));
   points.push_back(line2.p1);
   points.push_back(line2.p2);
   parallelPoints.push_back(points);

   parallelFieldLines.push_back( std::make_pair(line2, line1));
   parallelFieldLengths.push_back( std::make_pair(SQUARE(GOAL_BOX_WIDTH), SQUARE(FIELD_WIDTH)));
   points.clear();
   points.push_back(line2.p1);
   points.push_back(line2.p2);
   points.push_back(Point(-FIELD_LENGTH/2,-GOAL_BOX_WIDTH/2));
   points.push_back(Point(-FIELD_LENGTH/2,GOAL_BOX_WIDTH/2));
   parallelPoints.push_back(points);

   
   // away goalie box line
   line2.p1 = Point(FIELD_LENGTH/2-GOAL_BOX_LENGTH, +GOAL_BOX_WIDTH/2);
   line2.p2 = Point(FIELD_LENGTH/2-GOAL_BOX_LENGTH, -GOAL_BOX_WIDTH/2);
   allFieldLines.push_back(line2);
   allLineNames.push_back("Away Goal Box Line");
   allLineLengths.push_back(SQUARE(GOAL_BOX_WIDTH));
   allLinePostDist.push_back(GOAL_BOX_LENGTH);
   allLineEdgeDist.push_back(std::make_pair(+(GOAL_BOX_LENGTH + FIELD_LENGTH_OFFSET),
         -(FIELD_LENGTH - GOAL_BOX_LENGTH + FIELD_LENGTH_OFFSET)));

   // away parallel lines
   parallelFieldLines.push_back( std::make_pair(line1, line2));
   parallelFieldLengths.push_back( std::make_pair(SQUARE(FIELD_WIDTH), SQUARE(GOAL_BOX_WIDTH)));
   points.clear();
   points.push_back(Point(FIELD_LENGTH/2, GOAL_BOX_WIDTH/2));
   points.push_back(Point(FIELD_LENGTH/2, -GOAL_BOX_WIDTH/2));
   points.push_back(line2.p2);
   points.push_back(line2.p1);
   parallelPoints.push_back(points);

   parallelFieldLines.push_back( std::make_pair(line2, line1));
   parallelFieldLengths.push_back( std::make_pair(SQUARE(GOAL_BOX_WIDTH), SQUARE(FIELD_WIDTH)));
   points.clear();
   points.push_back(line2.p2);
   points.push_back(line2.p1);
   points.push_back(Point(FIELD_LENGTH/2, GOAL_BOX_WIDTH/2));
   points.push_back(Point(FIELD_LENGTH/2, -GOAL_BOX_WIDTH/2));
   parallelPoints.push_back(points);
   

   // home goal box side left side (when facing goal)
   line.p1 = Point(-FIELD_LENGTH/2+GOAL_BOX_LENGTH, -GOAL_BOX_WIDTH/2);
   line.p2 = Point(-FIELD_LENGTH/2, -GOAL_BOX_WIDTH/2);
   allFieldLines.push_back(line);
   allLineNames.push_back("Home Goal Box Left Side");
   allLineLengths.push_back(SQUARE(GOAL_BOX_LENGTH));
   allLinePostDist.push_back(-(GOAL_BOX_WIDTH-GOAL_WIDTH)/2 );
   allLineEdgeDist.push_back(std::make_pair(+( (FULL_FIELD_WIDTH-GOAL_BOX_WIDTH)/2 ),
         -( FULL_FIELD_WIDTH - FIELD_WIDTH_OFFSET - (FIELD_WIDTH-GOAL_BOX_WIDTH)/2)));

   // home goal box right side (when facing goal)
   line.p1 = Point(-FIELD_LENGTH/2+GOAL_BOX_LENGTH, GOAL_BOX_WIDTH/2);
   line.p2 = Point(-FIELD_LENGTH/2, GOAL_BOX_WIDTH/2);
   allFieldLines.push_back(line);
   allLineNames.push_back("Home Goal Box Right Side");
   allLineLengths.push_back(SQUARE(GOAL_BOX_LENGTH));
   allLinePostDist.push_back((GOAL_BOX_WIDTH-GOAL_WIDTH)/2 );
   allLineEdgeDist.push_back(std::make_pair(FULL_FIELD_WIDTH - FIELD_WIDTH_OFFSET - (FIELD_WIDTH-GOAL_BOX_WIDTH)/2,
         -( (FULL_FIELD_WIDTH-GOAL_BOX_WIDTH)/2 )));

   // away goal box right wide (when facing goal)
   line.p1 = Point(FIELD_LENGTH/2, -GOAL_BOX_WIDTH/2);
   line.p2 = Point(FIELD_LENGTH/2-GOAL_BOX_LENGTH, -GOAL_BOX_WIDTH/2);
   allFieldLines.push_back(line);
   allLineNames.push_back("Away Goal Box Right Side");
   allLineLengths.push_back(SQUARE(GOAL_BOX_LENGTH));
   allLinePostDist.push_back(-(GOAL_BOX_WIDTH-GOAL_WIDTH)/2 );
   allLineEdgeDist.push_back(std::make_pair((FULL_FIELD_WIDTH-GOAL_BOX_WIDTH)/2,
         -( FULL_FIELD_WIDTH - FIELD_WIDTH_OFFSET - (FIELD_WIDTH-GOAL_BOX_WIDTH)/2)));

   // away goal box left side (when facing goal)
   line.p1 = Point(FIELD_LENGTH/2, GOAL_BOX_WIDTH/2);
   line.p2 = Point(FIELD_LENGTH/2-GOAL_BOX_LENGTH, GOAL_BOX_WIDTH/2);
   allFieldLines.push_back(line);
   allLineNames.push_back("Away Goal Box Left Side");
   allLineLengths.push_back(SQUARE(GOAL_BOX_LENGTH));
   allLinePostDist.push_back((GOAL_BOX_WIDTH-GOAL_WIDTH)/2 );
   allLineEdgeDist.push_back(std::make_pair(FULL_FIELD_WIDTH - FIELD_WIDTH_OFFSET - (FIELD_WIDTH-GOAL_BOX_WIDTH)/2,
         -(FULL_FIELD_WIDTH-GOAL_BOX_WIDTH)/2 ));

   
   // Vertical field edge lines   
   // home goal edge
   line.p1 = Point(-FULL_FIELD_LENGTH/2,-FULL_FIELD_WIDTH/2);
   line.p2 = Point(-FULL_FIELD_LENGTH/2,FULL_FIELD_WIDTH/2);  
   allEdgeLines.push_back(line);

   // away goal edge
   line.p1 = Point(FULL_FIELD_LENGTH/2,FULL_FIELD_WIDTH/2);
   line.p2 = Point(FULL_FIELD_LENGTH/2,-FULL_FIELD_WIDTH/2);  
   allEdgeLines.push_back(line);

   // Horizontal field edge lines
   // side edge 1
   line.p1 = Point(+FULL_FIELD_LENGTH/2,-FULL_FIELD_WIDTH/2);
   line.p2 = Point(-FULL_FIELD_LENGTH/2,-FULL_FIELD_WIDTH/2);  
   allEdgeLines.push_back(line);

   // side edge 2
   line.p1 = Point(-FULL_FIELD_LENGTH/2, FULL_FIELD_WIDTH/2);
   line.p2 = Point(+FULL_FIELD_LENGTH/2, FULL_FIELD_WIDTH/2);  
   allEdgeLines.push_back(line);


}

