//
//  DiffBallDetector.hpp
//  nbites
//
//  Created by Cory Alini on 1/15/18.
//  Copyright © 2018 nbites.coryalini. All rights reserved.
//

#ifndef DiffBallDetector_hpp
#define DiffBallDetector_hpp

#include "BallDetector.h"

class DiffBallDetector  {
public:
    DiffBallDetector(FieldHomography* homography_, Field* field_, bool topCamera);
    ~DiffBallDetector();
    
    
    
    
    
    
    
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
    ImageLiteU8 whiteImage, greenImage, blackImage;
    ImageLiteU16 yImage;
    EdgeDetector * edgeDetector;
    EdgeList * edgeList;

    
    
    
    
    
    
}


#endif /* DiffBallDetector_hpp */

