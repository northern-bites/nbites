#pragma once

#include <stdint.h>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <Eigen/Eigen>

#include "CameraToRR.hpp"
#include "NNMC.hpp"
#include "WhichCamera.hpp"
#include "types/FootInfo.hpp"
#include "types/BallInfo.hpp"
#include "types/PostInfo.hpp"
#include "types/UNSWRobotInfo.hpp"
#include "types/FieldEdgeInfo.hpp"
#include "types/FieldFeatureInfo.hpp"
#include "types/Ipoint.hpp"
#include "types/Odometry.hpp"

struct VisionFrame
{
   /* Constructs a new frame, timestamp is automatically added */
   VisionFrame(const uint8_t *topImage,
               const NNMC &topNnmc,
               const uint8_t *botImage,
               const NNMC &botNnmc,
               const CameraToRR &cameraToRR,
               boost::shared_ptr<VisionFrame> last);

   /* Constant members */
   const struct
   {
      const uint8_t *topImage;
      const NNMC &topNnmc;
      const uint8_t *botImage;
      const NNMC &botNnmc;
      const CameraToRR &cameraToRR;
   };

   /* Output members */
   int64_t timestamp;

   std::pair<int, int>                dxdy; // the expected movement of objects in the frame relative to prior frame
   std::vector<BallInfo>              balls;
   BallHint                           ballHint;
   std::vector<PostInfo>              posts;
   std::vector<FootInfo>              feet;
   std::vector<UNSWRobotInfo>             robots;
   std::vector<FieldEdgeInfo>         fieldEdges;
   std::vector<FieldFeatureInfo>      fieldFeatures;
	std::vector<Ipoint> 					  landmarks;
   Eigen::VectorXf                    landmark_tf; // term frequency of landmarks
   Eigen::VectorXf                    landmark_tf_aug; // term frequency of landmarks augmented with a previous matching frame
   std::vector< std::vector<float> >  landmark_pixLoc; // the pixel locations of terms in landmark_tf
   std::vector< std::vector<float> >  landmark_pixLoc_aug; // the pixel locations of terms in landmark_tf_aug
   Odometry                           odometry; // visual odometry
   bool                               validOdom; // if a valid surf match was made to previous frames
   bool                               wordMapped; // if landmarks are mapped to visual words
   unsigned int                       missedFrames; // total num frames that haven't been matched
   PostInfo::Type                     goalArea;
   float                              awayGoalProb;

   /* TODO move this array into the frame struct */
   int *topStartScanCoords;
   int *botStartScanCoords;
   WhichCamera whichCamera;
   boost::shared_ptr<VisionFrame> last;
};
