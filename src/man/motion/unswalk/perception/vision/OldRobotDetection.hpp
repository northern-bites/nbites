#pragma once

#include <vector>
#include "perception/vision/VisionConstants.hpp"
#include "perception/vision/VisionDefs.hpp"
#include "perception/vision/CameraToRR.hpp"
#include "perception/vision/ImageRegion.hpp"
#include "Fovea.hpp"
#include "utils/body.hpp"
#include "types/UNSWRobotInfo.hpp"
#include "types/AbsCoord.hpp"


class OldRobotDetection {
   friend class VisionTab;
   
   public:

      // sonar distance in mm
      std::vector< std::vector <int> > sonar; 
      
      OldRobotDetection();
      
      std::vector<UNSWRobotInfo> robots;

      void findRobots(VisionFrame &frame,
                      std::vector<Point> edgePoints,
                      const Fovea &saliency
                     );

   private:
      //Reference data
      CameraToRR convRR;
      bool saliency_top;
      int saliency_cols;
      int saliency_rows;
      int saliency_density;
      std::vector<int> startOfScan;
      std::vector<Point> edgePoints;

      
      //Internal + debug data
      typedef struct RDData {
         UNSWRobotInfo robot;
         int colourProfile[cNUM_COLOURS+1]; // last value is for edge count
         int colourTotal;
         Point midFoot;
         float width;
         bool skip;
      } RDData;
      
      std::vector<RDData>  allRobots;
      std::vector<int>     robotPoints;
      std::vector<bool>    pointsCheck;
      std::vector<BBox>    postsCheck;

      
      //Field edge deviation method
      void generateDeviationPoints(const Fovea &saliency);

      // Find the number of pixels or a certain colour in a lookDistance neighbourhood      
      inline int  getColorAmount(const Fovea &saliency, int r, int c, Colour colour, int lookDistance);

      // Adds a robot point, checking if its below the field edge and not in a post
      inline void addRobotPoint(int value, int index);

      // Creates robot regions out of robot points
      void robotPointsToRegion();
      inline void completePreviousRobotRegion();

      // Enlarges the BBox to capture the new point (x,y)
      inline void updateBBox(BBox& box, int x, int y);

      // Rejoining robots that are closed together based on RR distance
      void mergeRobotRegion(const Fovea &saliency);

      // Check robots for reasonable colour, edges, presence of waistband and sonar
      void sanityCheckRobots(const Fovea &saliency);

      // Try to reduce the size of the bounding box if it has green on left, bottom or right edges
      inline void greenShrink(const Fovea & saliency, BBox &box);

      // Update the distance to match sonar if sufficiently close, and return true if verified
      bool SonarVerifyDist(std::vector<int> &sonar, RRCoord &location, bool footSeen = true);
    
    
};
