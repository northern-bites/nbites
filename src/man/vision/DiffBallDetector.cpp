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
        processWhiteSpots(brightSpots, whiteSpots,&diffImage);
        std::cout<<"Finished process white spots"<<std::endl;
        
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
void DiffBallDetector::spotHistogram(Spot spot,ImageLiteU16* diffImage) {
    
//    for(int w = 0; w < width; w++){
//        for(int h = 0 ; h < height ; h++){
//            int16_t actual  = * ( (int16_t *) diffImage->pixelAddr(w,h));
//
//            if (actual > 1023) {
//                printf("oh toesies %u %u %u\n",
//                       w, h, actual);
//                exit(-1);
//            }
//        }
//    }
    
    int leftX = spot.ix() + width / 2 - spot.innerDiam / 4;
    int rightX = spot.ix() + width / 2 + spot.innerDiam / 4;
    int topY = -spot.iy() + height / 2 - spot.innerDiam / 4;
    int bottomY = -spot.iy() + height / 2 + spot.innerDiam / 4;
    std::cout<<"b4("<<leftX<<","<<topY<<")  "<<"("<<rightX<<","<<bottomY<<")"<<std::endl;
    leftX = std::max(0, leftX);
    rightX = std::min(width - 1, rightX);
    topY = std::max(0, topY);
    bottomY = std::min(height - 1,bottomY);
    std::cout<<"l8r("<<leftX<<","<<topY<<")  "<<"("<<rightX<<","<<bottomY<<")"<<std::endl;

    int totalSize = (bottomY-topY)*(rightX-leftX);
    int vec[totalSize];
    bzero(vec, totalSize * sizeof(int));
    
    int max = 0;
    int sumcheck = 0;
    std::cout<<"width "<<rightX-leftX<<" height "<<bottomY-topY<<std::endl;
    
    int arraysize = 1024;
        int array[1024] = {0};
    for(int w = leftX; w < rightX; w++){
        for(int h = topY ; h < bottomY ; h++){
            if (w >= width || h >= height || w < 0 || h < 0
                ) {
                printf("oh noesies %u %u\n", w, h);
                exit(-1);
            }
            
            int16_t actual  = * ( (int16_t *) diffImage->pixelAddr(w,h));
//            std::cout << actual << ", ";
            array[*(diffImage->pixelAddr(w,h))]++;
        }
//        std::cout<<std::endl;

    }
//    std::cout<<std::endl;
    
    for (int range = 0; range < arraysize; range++) {
        if(array[range] != 0){
            std::cout<< range<<": "<<array[range]<< std::endl;
        }
    }
    std::cout<<"FINISHED!"<<std::endl;
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
                    //                        rejectedBlackSpots.push_back(std::make_pair(midX, midY));
                }
            }
        }
    }
}

void DiffBallDetector::processWhiteSpots(SpotList & brightSpots,
                                         spotVector & whiteSpots,ImageLiteU16* diffImage) {
    int count = 0;
    for (auto i = brightSpots.begin(); i != brightSpots.end(); i++) {
        int midX = (*i).ix() + width / 2;
        int midY = -(*i).iy() + height / 2;
        (*i).rawX = midX;
        (*i).rawY = midY;
        if (filterWhiteSpots((*i))) {
            whiteSpots.push_back((*i));
            (*i).spotType = SpotType::WHITE_CANDIDATE;
            std::cout<<"----------------SPOT "<<count<<"----------------"<<std::endl;
            spotHistogram((*i),diffImage);
            count++;

        } else if (!topCamera || midY > field->horizonAt(midX)) {
            //                rejectedWhiteSpots.push_back(std::make_pair(midX, midY));
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


