
#include "SurfDetection.hpp"


SurfDetection::SurfDetection() {

}

bool SurfDetection::vocabLoaded(){

   if (vocab.getSize() == 0) return false;
   else return true;

}

// Loads vocab ready for use, returns the size of the vocab in use
int SurfDetection::loadVocab(std::string vocabFile){

   vocab.loadVocabFile(vocabFile);
   int T = vocab.getSize();
   llog(VERBOSE) << "Loaded vocab of " << T << " words\n"; 
   return T;

}

void SurfDetection::findLandmarks(VisionFrame &frame, const std::vector<UNSWRobotInfo> &robots){

      frame_p = &frame;

		// Get horizon location
      const Pose &pose = frame.cameraToRR.pose;
		int left_horizon = pose.getHorizon().first;
		int right_horizon = pose.getHorizon().second;

      // Check horizon is within frame
      if(left_horizon > 0  && left_horizon < TOP_IMAGE_ROWS
         && right_horizon > 0 && right_horizon < TOP_IMAGE_ROWS){

		   // Create integral-image representation of the image
     		Integral(int_img, frame_p, left_horizon, right_horizon);

		   // Create Fast Hessian Object
		   FastHessian fh(int_img, frame_p->landmarks);

		   // Extract interest points and store in vector ipts
		   fh.getIpoints();
      }

      int totaln = frame_p->landmarks.size();
      // Remove interest points located on a robot
      for (int i=0; i< (int)robots.size(); i++){         
         if (robots[i].cameras == UNSWRobotInfo::BOT_CAMERA) continue;
         BBox robotBox = robots[i].topImageCoords; 
         for (int j=0; j < (int)frame_p->landmarks.size(); j++){
            float ipoint_x = frame_p->landmarks.at(j).x;
            if( ipoint_x*SURF_SUBSAMPLE >= (float)robotBox.a.x() && ipoint_x*SURF_SUBSAMPLE <= (float)robotBox.b.x()){
               frame_p->landmarks.erase(frame_p->landmarks.begin()+j);
               j--;
            }
         }
      }
      llog(VERBOSE) << "SURF landmarks found: " << frame_p->landmarks.size() << " (robot landmarks removed: " << 
         totaln - frame_p->landmarks.size() << ")\n";
      
      
      // Check there are Ipoints to be described
      if (frame_p->landmarks.size() > 0){
		   // Extract the descriptors for the ipts
		   getHorizonDescriptors();
      }

      // If no vocab loaded, it just finds the raw interest points
      if (vocab.getSize() != 0){
         // Map the interest points to visual words, if no landmarks it will correctly initialise
         frame_p->landmark_tf = vocab.mapToVec(frame_p->landmarks, frame_p->landmark_pixLoc);
         frame_p->wordMapped = true;    
      } 

}


//! Describe all features in the supplied vector
void SurfDetection::getHorizonDescriptors()
{

   // Get the size of the vector for fixed loop bounds
   int ipts_size = (int)frame_p->landmarks.size();

   for (int i = 0; i < ipts_size; ++i)
   {
      // Set the Ipoint to be described
      index = i;
      getHorizonDescriptor();
   }

}



//-------------------------------------------------------



//! Our modified descriptor.
inline void SurfDetection::getHorizonDescriptor()
{
  int x, sample_x, count=0;
  float scale, *desc, dx, mdx;
  float rx = 0.f, len = 0.f;

  Ipoint *ipt = &frame_p->landmarks[index];
  scale = ipt->scale;
  x = fRound(ipt->x); 
  desc = ipt->descriptor;

	for(int subregion = -1; subregion<2; subregion++){

		dx=mdx=0.f;

		for(int sample = 0; sample < SURF_DESCRIPTOR_SAMPLES; sample++){
      sample_x = fRound(x + (subregion*SURF_DESCRIPTOR_SAMPLES*scale + 
        (sample-(SURF_DESCRIPTOR_SAMPLES/2))*scale) );
			
			//Get the haar wavelet responses 
			rx = haar(sample_x, 2*fRound(scale));
      dx += rx;
      mdx += fabs(rx);
		}
		desc[count++] = dx;
    desc[count++] = mdx;
	
    len += (dx*dx + mdx*mdx);
	}

  //Convert to Unit Vector
  len = sqrt(len);
  for(int i = 0; i < SURF_DESCRIPTOR_LENGTH; ++i){
    desc[i] /= len;
	}
}



//-------------------------------------------------------


//! Calculate Haar wavelet responses in x direction (1D only)
inline float SurfDetection::haar(int column, int s)
{
  return BoxIntegral(int_img, column, s/2) 
    -1.f * BoxIntegral(int_img, column-s/2, s/2);
}

//-------------------------------------------------------

