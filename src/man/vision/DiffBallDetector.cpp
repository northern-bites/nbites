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
        // Then we are going to filter out all of the blobs that obviously
        // aren't part of the ball
        intPairVector blackSpots;
        intPairVector badBlackSpots;
        spotVector actualBlackSpots;
        spotVector actualWhiteSpots;
        
        
        
        
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
            std::cout<<"at least i got called :P"<<std::endl;
            bool foundBall = false;
            edgeList = &edges;
            
            int startCol = 0;
            int endCol = diffImage.width();
            int endRow = diffImage.width();
            
            ImageLiteU8 smallerYImage;
//            smallerYImage = ImageLiteU8(diffImage, 0, 0, diffImage.width(), diffImage.height());

            
            SpotDetector darkSpotDetector;
            initializeSpotterSettings(darkSpotDetector, true, 3.0f, 3.0f, topCamera,filterThresholdDark, greenThresholdDark, 0.5);

            //START DARK SPOTS
            bool result = darkSpotDetector.spotDetect(diffImage, *homography, &smallerYImage);
            
            
            if(result) {
                SpotList darkSpots = darkSpotDetector.spots();
                std::cout<<"There are "<<darkSpots.size()<<" spots"<<std::endl;
                for(int z = 0; z < darkSpots.size(); z++) {
                    std::cout<<z<<std::endl;
                }
                processDarkSpots(darkSpots, blackSpots, badBlackSpots, actualBlackSpots);
                
            } else {
                std::cout<<"WHY YOU NO WORKING! "<<result<<std::endl;
            }
            
            
            
            
            
            
            
            
            
            //START WHITE SPOTS
            SpotDetector whiteSpotDetector;
            initializeSpotterSettings(whiteSpotDetector, false, 13.0f, 13.0f,
                                      topCamera, filterThresholdBrite, greenThresholdBrite,
                                      0.5);
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
        
        void DiffBallDetector::setImages(ImageLiteU16 yImg,EdgeDetector * edgeD) {
            yImage = yImg;
            edgeDetector = edgeD;
        }

        void DiffBallDetector::initializeSpotterSettings(SpotDetector &s, bool darkSpot,
                                                     float innerDiam, float altInnerDiam,
                                                     bool topCamera, int filterThreshold,
                                                     int greenThreshold, float filterGain)
        {
            s.darkSpot(darkSpot);
            s.innerDiamCm(innerDiam);
            
            if(!topCamera) {
                s.innerDiamCm(altInnerDiam);
            }
            
            s.filterThreshold(filterThreshold);
            s.greenThreshold(greenThreshold);
            s.filterGain(filterGain);
            
        }
        
        
        
        
        void DiffBallDetector::processDarkSpots(SpotList & darkSpots, intPairVector & blackSpots,
                                            intPairVector & badBlackSpots, spotVector & actualBlackSpots)
        {
//            for (auto i = darkSpots.begin(); i != darkSpots.end(); i++) {
//                // convert back to raw coordinates
//                int midX = (*i).ix() + width / 2;
//                int midY = -(*i).iy() + height / 2;
//                if (midX < width - 5 && midX > 5 && midY > 5 && midY < height - 5) {
//                    (*i).rawX = midX;
//                    (*i).rawY = midY;
//                    getColor(midX, midY);
//                    // if the middle of the spot is white or green, ignore it
//                    if (!(isWhite() || isGreen()) &&
//                        (!topCamera || midY > field->horizonAt(midX))) {
//                        if (filterBlackSpots((*i))) {
//                            blackSpots.push_back(std::make_pair(midX, midY));
//                            actualBlackSpots.push_back((*i));
//                            (*i).spotType = SpotType::DARK_CANDIDATE;
//                        } else {
//                            (*i).spotType = SpotType::DARK_REJECT;
//                            badBlackSpots.push_back(std::make_pair(midX, midY));
//                        }
//                        if (debugBall) {
//                            debugBlackSpots.push_back((*i));
//                        }
//                    }
//                }
//            }
        }
        
        bool DiffBallDetector::processWhiteSpots(SpotList & whiteSpots,
                                             intPairVector & blackSpots,
                                             intPairVector & badBlackSpots,
                                             spotVector & actualWhiteSpots,
                                             double cameraHeight, bool & foundBall)
        {
//            for (auto i = whiteSpots.begin(); i != whiteSpots.end(); i++) {
//                int midX = (*i).ix() + width / 2;
//                int midY = -(*i).iy() + height / 2;
//                (*i).rawX = midX;
//                (*i).rawY = midY;
//                /*imagePoint p = imagePoint(midX - width /2, -midY + height / 2);
//                 std::cout << "Spots " << (*i).innerDiam << " " << (2 * projectedBallRadius(p)) << std::endl;
//                 debugDraw.drawBox(midX, midX + 2 * projectedBallRadius(p), midY + 2 * projectedBallRadius(p), midY, RED);
//                 p = imagePoint( - width /2, -midY + height / 2);
//                 debugDraw.drawBox(midX, midX + 2 * projectedBallRadius(p),
//                 midY + 2 * projectedBallRadius(p), midY, RED);*/
//                if (filterWhiteSpot((*i), blackSpots, badBlackSpots)) {
//                    actualWhiteSpots.push_back((*i));
//                    if(debugBall) {
//                        std::cout<<"filterWhiteSpot returned true\n";
//                        debugDraw.drawPoint((*i).ix() + width / 2,
//                                            -(*i).iy() + height / 2, RED);
//                    }
//                    makeBall((*i), cameraHeight, 0.75, foundBall, false);
//#ifdef OFFLINE
//                    foundBall = true;
//#else
//                    return true;
//#endif
//                    (*i).spotType = SpotType::WHITE_CANDIDATE;
//                } else if (!topCamera || midY > field->horizonAt(midX)) {
//                    (*i).spotType = SpotType::WHITE_REJECT;
//                }
//                if (debugBall && (!topCamera || midY > field->horizonAt(midX))) {
//                    debugWhiteSpots.push_back((*i));
//                }
//            }
//            return foundBall;
        }

    }
}



