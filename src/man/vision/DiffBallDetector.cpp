//
//  DiffBallDetector.cpp
//  nbites
//
//  Created by Cory Alini on 1/15/18.
//  Copyright © 2018 nbites.coryalini. All rights reserved.
//

#include "DiffBallDetector.hpp"



namespace man {
    namespace vision {
        
        DiffBallDetector::DiffBallDetector(FieldHomography* homography_,Field* field_, bool topCamera_):
            homography(homography_),
            field(field_),
        topCamera(topCamera_){
            if (topCamera) {
                std::cout<<"YO! you are passing in a bottom camera image! get yo act together."<<std::endl;
            }
            
        }
        DiffBallDetector::~DiffBallDetector() {}

        bool DiffBallDetector::findBallYImage(ImageLiteU16 diffImage,double cameraHeight, EdgeList& edges) {
            
            bool foundBall = false;
            edgeList = &edges;
            ImageLiteU8 smallerGreen = NULL;
        
            SpotDetector darkSpotDetector;
            initializeSpotterSettings(darkSpotDetector, true, 3.0f, 3.0f, topCamera,filterThresholdDark, greenThresholdDark, 0.5);
            
            //START DARK SPOTS
            if(darkSpotDetector.spotDetect(smallerY, *homography, &smallerGreen)) {
                SpotList darkSpots = darkSpotDetector.spots();
                processDarkSpots(darkSpots, blackSpots, badBlackSpots, actualBlackSpots);
                
                
                
            }
            
            //START WHITE SPOTS
            SpotList whiteSpots = whiteSpotDetector.spots();
            if(processWhiteSpots(whiteSpots, blackSpots, badBlackSpots, actualWhiteSpots,
                                 cameraHeight,foundBall)) {
#ifdef OFFLINE
                foundBall = true;
#else
                return true;
#endif
            }
        }

       
            
        }
        
    
    
        
    }
}


