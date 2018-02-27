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
    
    intPairVector rejectedBlackSpots;
    intPairVector rejectedWhiteSpots;
    
    spotVector blackSpots;
    spotVector whiteSpots;
    
    SpotList darkSpots;
    SpotList brightSpots;
    
    DiffBallDetector::DiffBallDetector(FieldHomography* homography_,Field* field_, bool topCamera_):
    homography(homography_),field(field_),topCamera(topCamera_) {
        if (topCamera) {
            std::cout<<"YO! you are passing in a bottom camera image! get yo act together."<<std::endl;
        }
    }
    DiffBallDetector::~DiffBallDetector() {}
    
    bool DiffBallDetector::findBallYImage(ImageLiteU16 diffImage,double cameraHeight, EdgeList& edges) {
        
        bool foundBall = false;
        edgeList = &edges;
        
        int startCol = 0;
        int endCol = diffImage.width();
        int endRow = diffImage.width();
        width =diffImage.width();
        height = diffImage.height();
        ImageLiteU8 smallerYImage;
        
        //START DARK SPOTS
        SpotDetector darkSpotDetector;
        initializeSpotterSettings(darkSpotDetector, true, 3.0f, 3.0f, topCamera,filterThresholdDark, greenThresholdDark, 0.5);
        if(darkSpotDetector.spotDetect(diffImage, *homography, NULL)) {
            darkSpots = darkSpotDetector.spots();
            processDarkSpots(darkSpots, blackSpots);
            std::cout<<"There are "<<blackSpots.size()<<" blackspots"<<std::endl;
        }
        
        //START WHITE SPOTS
        SpotDetector whiteSpotDetector;
        initializeSpotterSettings(whiteSpotDetector, false, 13.0f, 13.0f,
                                  topCamera, filterThresholdBrite, greenThresholdBrite,0.5);
        if(whiteSpotDetector.spotDetect(diffImage, *homography, NULL)) {
            brightSpots = whiteSpotDetector.spots();
            processWhiteSpots(brightSpots, whiteSpots);
            
        }
    }
    
    void DiffBallDetector::initializeSpotterSettings(SpotDetector &s, bool darkSpot,
                                                     float innerDiam, float altInnerDiam,
                                                     bool topCamera, int filterThreshold,
                                                     int greenThreshold, float filterGain) {
        s.darkSpot(darkSpot);
        s.innerDiamCm(innerDiam);
        
        if(!topCamera) {
            s.innerDiamCm(altInnerDiam);
        }
        
        s.filterThreshold(filterThreshold);
        s.greenThreshold(greenThreshold);
        s.filterGain(filterGain);
        
    }
    
    void DiffBallDetector::processDarkSpots(SpotList & darkSpots,spotVector & blackSpots) {
        for (auto i = darkSpots.begin(); i != darkSpots.end(); i++) {
            // convert back to raw coordinates
//            blackSpots.push_back((*i));
            int midX = (*i).ix() + width / 2;
            int midY = -(*i).iy() + height / 2;
            if (midX < width - 5 && midX > 5 && midY > 5 && midY < height - 5) {
                (*i).rawX = midX;
                (*i).rawY = midY;
                // if the middle of the spot is white or green, ignore it
                if ((midY > field->horizonAt(midX))) {
                    if (filterBlackSpots((*i))) {
                        blackSpots.push_back((*i));
                        (*i).spotType = SpotType::DARK_CANDIDATE;
                    } else {
                        (*i).spotType = SpotType::DARK_REJECT;
                        rejectedBlackSpots.push_back(std::make_pair(midX, midY));
                    }
                }
            }
        }
    }
    
    void DiffBallDetector::processWhiteSpots(SpotList & brightSpots,
                                             spotVector & whiteSpots) {
        for (auto i = brightSpots.begin(); i != brightSpots.end(); i++) {
//            whiteSpots.push_back((*i));

            int midX = (*i).ix() + width / 2;
            int midY = -(*i).iy() + height / 2;
            (*i).rawX = midX;
            (*i).rawY = midY;
            if (filterWhiteSpots((*i))) {
                whiteSpots.push_back((*i));
                (*i).spotType = SpotType::WHITE_CANDIDATE;
            } else if (!topCamera || midY > field->horizonAt(midX)) {
                rejectedWhiteSpots.push_back(std::make_pair(midX, midY));
                (*i).spotType = SpotType::WHITE_REJECT;
            }
        }
    }
    
    bool DiffBallDetector::filterBlackSpots(Spot currentSpot) {
        return true;
    }
    bool DiffBallDetector::filterWhiteSpots(Spot spot) {
        // convert back to raw coordinates
        int leftX = spot.ix() + width / 2 - spot.innerDiam / 4;
        int rightX = spot.ix() + width / 2 + spot.innerDiam / 4;
        int topY = -spot.iy() + height / 2 - spot.innerDiam / 4;
        int bottomY = -spot.iy() + height / 2 + spot.innerDiam / 4;
        int midX = spot.ix() + width / 2;
        int midY = -spot.iy() + height / 2;
        
        // don't  bother if off the field
        if (midY < field->horizonAt(midX)) {
            return false;
        }
        return true;
    }
    void DiffBallDetector::setImages(ImageLiteU16 yImg,EdgeDetector * edgeD) {
        yImage = yImg;
        edgeDetector = edgeD;
        blackSpots.clear();
        whiteSpots.clear();
    }
    
    void DiffBallDetector::getDarkSpots(std::vector<Spot >&spots)  {
        for (auto i = blackSpots.begin(); i != blackSpots.end(); i++) {
            spots.push_back(*i);
        }
    }
    void DiffBallDetector::getBrightSpots(std::vector<Spot >&spots)  {
        for (auto i = whiteSpots.begin(); i != whiteSpots.end(); i++) {
            spots.push_back(*i);
        }
    }
    void DiffBallDetector::setParams(std::vector<int>&params) {
        filterThresholdDark = params[0];
        greenThresholdDark = params[1];
        filterThresholdBrite = params[2];
        greenThresholdBrite = params[3];
    }
}
}


