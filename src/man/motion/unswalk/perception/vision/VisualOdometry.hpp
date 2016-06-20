#pragma once

#include <vector>
#include <boost/shared_ptr.hpp>
#include "perception/vision/VisionConstants.hpp"
#include "perception/vision/VisionDefs.hpp"
#include "perception/vision/CameraToRR.hpp"
#include "perception/vision/VisionFrame.hpp"
#include "perception/vision/NaoCamera.hpp"

#include "types/Ipoint.hpp"
#include "types/Odometry.hpp"

#define INVALID -1

// how much slip on average frame before we decide we are slipping
#define SLIP_AMOUNT UNSWDEG2RAD(1.5f) 

// how many slipping frames before deciding to do a disentangle action
#define DETANGLE_SLIPS 6


// how long we maintain the detangle action once it has started
#define DETANGLE_TIME 30
 

class VisualOdometry {
   public:

    std::vector<float> headYaw;
    Odometry walkOdometry;
    Odometry dualOdometry;  // the best of walk engine and vision
    bool caughtLeft;
    bool caughtRight;  


	 VisualOdometry();

	 // find robot movement between this frame and the previous frame
    void findOdometry(VisionFrame &frame, unsigned int *seed);



   private:

    Odometry walkOdometryLast;
    Odometry visualOdometryLast;
    int last_horizon;

    bool lastOdomValid;
    float slipAverage;
    int slips;
    int slipOnCount;

    VisionFrame *curr_frame;
    boost::shared_ptr<VisionFrame> candidate;	
    std::vector<float> scoreHistory;

    // Get odometry between two frames and return an integer matching score for the confidence
    int getOdometry(std::vector<Ipoint> &ipts1, std::vector<Ipoint> &ipts2, Odometry &odom, int &dx);


      
};
