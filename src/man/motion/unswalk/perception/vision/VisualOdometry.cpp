
#include "VisualOdometry.hpp"
#include "perception/vision/CameraToRR.hpp"
#include "utils/Logger.hpp"
#include "utils/Timer.hpp"
#include "utils/speech.hpp"
#include "utils/basic_maths.hpp"


#define ALPHA 0.3f

VisualOdometry::VisualOdometry() {

   scoreHistory.reserve(NUM_FRAME_BUFFERS);
   
   lastOdomValid = false;
   last_horizon = 0;
   slipAverage = 0.f;
   slips = 0;
   slipOnCount = 0;

   caughtLeft = false;
   caughtRight = false;

}



// finds correspondence and our robot odometry between two subsequent frames
// Other robots should already be removed from the landmarks
void VisualOdometry::findOdometry(VisionFrame &frame, unsigned int *seed){

      curr_frame = &frame;
      candidate = curr_frame->last;
      
      // save score history    
      scoreHistory.insert(scoreHistory.begin(), INVALID); // negative indicates no history
      if ((int)scoreHistory.size() > NUM_FRAME_BUFFERS){
        scoreHistory.pop_back();
      }
      unsigned int position = 1;

      int score;
      int best_score = 0;
      Odometry movement;

      bool firstcandidate = true;

      if(candidate){ 
        curr_frame->odometry = candidate->odometry; // by default its unchanged
      } else {
         // Need to initialise last_horizon the first time through
         const Pose &pose = frame.cameraToRR.pose;
			last_horizon = (pose.getHorizon().first + pose.getHorizon().second)/2;
      }
      while(candidate){
  
		   int dx = 0; //
         score = getOdometry(candidate->landmarks, curr_frame->landmarks, movement, dx);
         llog(DEBUG1) << "VisualOdometry: Match frame " << position <<", score " << score << ", scoreHistory " 
            << scoreHistory[position] << ", prev best " << best_score << ", turn " << curr_frame->odometry.turn << std::endl;

         // Calculate the dxdy from previous frame to help other components in vision
         if(firstcandidate){
            firstcandidate = false;
            // Get horizon location
            const Pose &pose = frame.cameraToRR.pose;
			   int horizon = (pose.getHorizon().first + pose.getHorizon().second)/2;
            curr_frame->dxdy = std::make_pair(dx, horizon - last_horizon );
            last_horizon = horizon;
            llog(DEBUG1) << "DxDy from last frame: (" << curr_frame->dxdy.first << ", " << curr_frame->dxdy.second << ")\n";
         }

         // use the match if it is a good match, and the frame it matches to is also well matched...
         if ( score > best_score && (scoreHistory[position] > best_score || scoreHistory[position] == INVALID) ) {

            if(headYaw.size() > position){
               float headmove = headYaw[0] - headYaw[position];
               movement.turn -= headmove;
            }

            curr_frame->odometry = candidate->odometry + movement;
            curr_frame->validOdom = true;
            curr_frame->missedFrames = candidate->missedFrames;

            if (curr_frame->wordMapped && candidate->wordMapped){
               // Create the augmented landmark term frequency that will be used by GoalMatcher
               curr_frame->landmark_tf_aug = curr_frame->landmark_tf + candidate->landmark_tf;
               curr_frame->landmark_pixLoc_aug = curr_frame->landmark_pixLoc;  

               float x_adjust = movement.turn / IMAGE_HFOV; 
               x_adjust = x_adjust * (TOP_IMAGE_COLS/SURF_SUBSAMPLE);  
        
               for(int i=0; i<(int)curr_frame->landmark_pixLoc.size(); i++){
                  for(int j=0; j< (int)candidate->landmark_pixLoc[i].size(); j++){
                     curr_frame->landmark_pixLoc_aug[i].push_back(candidate->landmark_pixLoc[i][j] + x_adjust);
                  }
               }
            }
            // keep the min as best score, therefore requires both current and previous match be decent
            best_score = score;
            if (score > scoreHistory[position]) best_score = scoreHistory[position];
            scoreHistory[0] = score;

         } 
         candidate = candidate->last;
         position++;
      }

      
      // Now choose between visual odometry and walk odometry to find the best of both
      dualOdometry.forward = walkOdometry.forward;
      dualOdometry.left = walkOdometry.left;            

      // Check if walking, if not we assume stationary unless we get a bit impact
      bool walking = true;
      Odometry walkDiff = walkOdometry - walkOdometryLast;
      if (walkDiff.turn == 0.0f && walkDiff.left == 0.0f && walkDiff.forward == 0.0f) walking = false;

      float slipError = 0.f;
      float walkTurn = walkOdometry.turn - walkOdometryLast.turn;
      float visualTurn = curr_frame->odometry.turn - visualOdometryLast.turn;
      if (lastOdomValid && curr_frame->validOdom){
         slipError = visualTurn - walkTurn;
      }
      slipAverage = ALPHA*slipError + (1.f - ALPHA)*slipAverage;

      // usual visual odom if its valid and we are walking, or stationary but with big slip
      if (lastOdomValid && curr_frame->validOdom && (walking || fabs(slipAverage) > SLIP_AMOUNT)){
         dualOdometry.turn = dualOdometry.turn + visualTurn;
      } else {
         dualOdometry.turn = dualOdometry.turn + walkTurn;
      }

      // test whether an untangling action is required. This should be used for behaviour
      if ( slipAverage > SLIP_AMOUNT) {
         slips++; // slipping left
         llog(DEBUG1) << "Slipping left, slips: " << slips << "\n";
      } else if ( slipAverage < -SLIP_AMOUNT ){
         slips--;            
         llog(DEBUG1) << "Slipping right, slips: " << slips << "\n";     
      } else {
         slips = 0; // might need to change this so it resets more slowly
      }

      llog(DEBUG1) << "Visual Odom slipAverage, slips, slipOnCount: "<< slipAverage << ", " << slips << ", " << slipOnCount << "\n";

      // keep reseting slipOnCount if we are still slipping
      if( slips > DETANGLE_SLIPS ){
   
         if (slipOnCount == 0) {
            //SAY("Slipping left\n");
         }

         slipOnCount = 1;
         caughtLeft = true;
         caughtRight = false;
         
      } else if( slips < -DETANGLE_SLIPS  ){
         
         if (slipOnCount == 0) {
            //SAY("Slipping right\n");
         }

         // Reset slipOnCount
         slipOnCount = 1;
         caughtRight = true;
         caughtLeft = false;

      } else {
         if (slipOnCount > DETANGLE_TIME){
            slipOnCount = 0;
            caughtLeft = false;
            caughtRight = false;
         } else if (slipOnCount != 0){
            slipOnCount++;
         }
      }

      walkOdometryLast = walkOdometry;  
      visualOdometryLast = curr_frame->odometry;
      lastOdomValid = curr_frame->validOdom;            
      return;

}




    
int VisualOdometry::getOdometry(std::vector<Ipoint> &ipts1, std::vector<Ipoint> &ipts2, Odometry &odom, int &dx){

   float dist, d1, d2;
   Ipoint *match;
	std::vector<float> flow;
   flow.reserve(ipts1.size());

   // First do nearest neighbour point matching
   for(unsigned int i = 0; i < ipts1.size(); i++) 
  {

    d1 = d2 = std::numeric_limits<float>::max();
    match = &ipts2[0];

    for(unsigned int j = 0; j < ipts2.size(); j++) 
    {

      dist = ipts1[i] - ipts2[j]; 

      if(dist<d1) // if this feature matches better than current best
      {
        d2 = d1;
        d1 = dist;
        match = &ipts2[j];
      }
      else if(dist<d2) // this feature matches better than second best
      {
        d2 = dist;
      }
    }

    // If match has a d1:d2 ratio < 0.75 ipoints are a match
    if(d1/d2 < 0.75) 
    { 
      // Store the pixel movement between matches
      flow.push_back(match->x - ipts1[i].x);
    }
  }
  std::sort(flow.begin(), flow.end());
  
  int count = 0;
  float displacement = 0.f;  
  int bestCount = 0;
  float bestDisplacement = 0.f;
  int secondCount = 0;
  float secondDisplacement = 0.f;


  // Now calculate the mode plus next most frequent observation
  // Using the best 2 modes helps differentiate slipping from object motion in the frame
  for(int i=0; i< (int)flow.size(); i++){

    if(i==0 || flow[i] != flow[i-1]){
      displacement = flow[i];
      count = 1;
    } else {
      count++;
    }

    if (i==(int)flow.size() -1 || flow[i] != flow[i+1]){
      if (count > bestCount) {
        secondCount = bestCount;        
        bestCount = count;
        secondDisplacement = bestDisplacement;
        bestDisplacement = displacement;
      } else if (count > secondCount) {
        secondCount = count;
        secondDisplacement = displacement;
      }
    }
  }
  llog(DEBUG1) << "Best displacement: " << bestDisplacement << ", count: " << bestCount << 
        ", 2nd disp: " << secondDisplacement << ", count: " << secondCount << "\n";
   
  int score = 0.f;
  if (bestCount - secondCount >= 5 && bestCount >= 8 ){  // conditions before the bestmode can be trusted
    score = bestCount - secondCount;
    dx = (int)bestDisplacement*SURF_SUBSAMPLE;
    odom.turn = (bestDisplacement/(TOP_IMAGE_COLS/SURF_SUBSAMPLE))*IMAGE_HFOV;
    llog(DEBUG1) << "Pixel movement: " << bestDisplacement << "\n";
  } else if ( (bestDisplacement - secondDisplacement < 3.f) && // check if two modes are really close together anyway
              (bestDisplacement - secondDisplacement > -3.f) && 
              (bestCount + secondCount > 10) ){
    score = 1; // valid but not very good
    dx = (int)((bestDisplacement+secondDisplacement)*SURF_SUBSAMPLE/2.f);
    odom.turn = ((bestDisplacement + secondDisplacement)/2.f/(TOP_IMAGE_COLS/SURF_SUBSAMPLE))*IMAGE_HFOV;
    llog(DEBUG1) << "Pixel movement: " << (bestDisplacement + secondDisplacement)/2.f << "\n";
  } else {
    llog(DEBUG1) << "Pixel movement: OBSTRUCTION!\n";
  }
  return score;

}





