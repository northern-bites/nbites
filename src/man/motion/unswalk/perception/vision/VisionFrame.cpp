#include "VisionFrame.hpp"

#include <sys/time.h>

#include "VisionDefs.hpp"

VisionFrame::VisionFrame(
      const uint8_t *topImage, 
      const NNMC &topNnmc,
      const uint8_t *botImage, 
      const NNMC &botNnmc,
      const CameraToRR &cameraToRR,
      boost::shared_ptr<VisionFrame> last)

   : topImage(topImage), topNnmc(topNnmc),
     botImage(botImage), botNnmc(botNnmc),
     cameraToRR(cameraToRR), last(last)
{
   dxdy = std::make_pair(0,0);   
   balls.reserve(MAX_BALLS);
   posts.reserve(MAX_POSTS);
   feet.reserve(MAX_FEET);
   robots.reserve(MAX_ROBOTS);
   fieldEdges.reserve(MAX_FIELD_EDGES);
   fieldFeatures.reserve(MAX_FIELD_FEATURES);
	landmarks.reserve(MAX_LANDMARKS);
   validOdom = false;
   wordMapped = false;   
   goalArea = PostInfo::pNone;
   awayGoalProb = 0.5f;
   if(last) missedFrames = last->missedFrames + 1;
   else missedFrames = 0;    
  
   struct timeval tv;
   gettimeofday (&tv, NULL);
   timestamp = (int64_t)tv.tv_sec * 1e6 + tv.tv_usec;
}
