//
//  DiffBallDetector.hpp
//  nbites
//
//  Created by Cory Alini on 1/15/18.
//  Copyright © 2018 nbites.coryalini. All rights reserved.
//
//

#ifndef DiffBallDetector_hpp
#define DiffBallDetector_hpp
#include <algorithm>    // std::max

#include "BallDetector.h"
namespace man {
namespace vision {
class DiffBallDetector  {
public:
    DiffBallDetector(FieldHomography* homography_, Field* field_, bool topCamera);
    ~DiffBallDetector();
    
    
    void initializeSpotterSettings(SpotDetector &s, bool darkSpot,
                                                     float innerDiam, float altInnerDiam,
                                                     bool topCamera, int filterThreshold,
                                   int greenThreshold, float filterGain);
    bool findBallYImage(ImageLiteU16 diffImage,double cameraHeight, EdgeList& edges);

    void processDarkSpots(SpotList& darkSpots,spotVector& blackSpots);
    void processWhiteSpots(SpotList & brightSpots, spotVector& whiteSpots,ImageLiteU16* diffImage);
    bool filterBlackSpots(Spot currentSpot);
    bool filterWhiteSpots(Spot spot);
    void setImages(ImageLiteU16 yImg,EdgeDetector * edgeD);

    void getDarkSpots(std::vector<Spot >&spots);
    void getBrightSpots(std::vector<Spot >&spots);
    void setParams(std::vector<int>&params);
    void spotHistogram(Spot spot,ImageLiteU16* diffImage);


private:
    
    FieldHomography* homography;
    Field* field;
    bool topCamera;
    int width;
    int height;
    int currentX, currentY;
    std::vector<Edge> goodEdges;
    spotVector debugBlackSpots;
    spotVector debugWhiteSpots;
    Connectivity blobber;
    
    DebugImage debugDraw;
    ImageLiteU16 yImage;
    EdgeDetector * edgeDetector;
    EdgeList * edgeList;
    enum SpotType {
        WHITE_CANDIDATE = 1,
        WHITE_REJECT,
        DARK_CANDIDATE,
        DARK_REJECT,
        WHITE_BLOB,
        WHITE_BLOB_BAD
    };
#ifdef OFFLINE
    bool debugBall;
    bool debugSpots;
    int filterThresholdDark;
    int greenThresholdDark;
    int filterThresholdBrite;
    int greenThresholdBrite;
#else

    static const bool debugBall = false;
    static const bool debugSpots = false;
    static const int filterThresholdDark = 104;
    static const int greenThresholdDark = 12;
    static const int filterThresholdBrite = 200;
    static const int greenThresholdBrite = 80;
#endif

    
    
    
    
}; 

}
}
#endif /* DiffBallDetector_hpp */

