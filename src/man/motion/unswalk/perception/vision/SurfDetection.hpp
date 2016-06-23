#pragma once

#include <vector>
#include <Eigen/Eigen>

#include "utils/Logger.hpp"
#include "perception/vision/VisionConstants.hpp"
#include "perception/vision/VisionDefs.hpp"
#include "perception/vision/CameraToRR.hpp"
#include "perception/vision/VisionFrame.hpp"
#include "perception/vision/Integral.hpp"
#include "perception/vision/Fasthessian.hpp"
#include "perception/vision/Vocab.hpp"

#include "types/Ipoint.hpp"

class SurfDetection {
   public:

		SurfDetection();

    //! Loads vocab ready for use, returns the size of the vocab
    int loadVocab(std::string vocabFile);

    // is a vocab loaded
    bool vocabLoaded();

		// find interest points
    void findLandmarks(VisionFrame &frame, const std::vector<UNSWRobotInfo> &robots);


   private:

 		//---------------- Private Functions -----------------//
   
		//! Get all descriptors
    void getHorizonDescriptors();		

		//! Our modified descriptor
    void getHorizonDescriptor();

    //! Calculate Haar wavelet response
		inline float haar(int column, int size);

		//! Round float to nearest integer
		inline int fRound(float flt)
		{
			return (int) floor(flt+0.5f);
		}

   //---------------- Private Variables -----------------//

    // the Vocab used to map features to visual words 
    Vocab vocab;
			
	 // the original image
	 VisionFrame *frame_p;		
		
	 // the integral horizon image
	 float int_img[TOP_IMAGE_COLS / SURF_SUBSAMPLE];

    //! Index of current Ipoint in the vector
    int index;
      
};
