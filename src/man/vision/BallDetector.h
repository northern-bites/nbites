#pragma once

#include <vector>
#include <string>
#include <math.h>

#include "Images.h"
#include "Camera.h"
#include "FastBlob.h"
#include "Homography.h"


namespace man {
namespace vision {

const double BALL_RADIUS = 3.25;
const double VERT_FOV_DEG = 47.64;
const double VERT_FOV_RAD = VERT_FOV_DEG * M_PI / 180;
const double HORIZ_FOV_DEG = 60.97;
const double HORIZ_FOV_RAD = HORIZ_FOV_DEG * M_PI / 180;

class Ball {
public:
    Ball(Blob& b, double x_, double y_, double cameraH_, int imgHeight_, int imgWidth_, bool top);
    Ball();

    std::string properties();

    double confidence() const { return _confidence; }

    // For tool
    Blob& getBlob() { return blob; }
//private: should be private. leaving public for now
    void compute();

    double pixDiameterFromDist(double d) const;

    Blob blob;
    FuzzyThr thresh;
    FuzzyThr radThresh;

    double x_rel;
    double y_rel;
    double cameraH;
    double dist;

    int imgHeight, imgWidth;
    double expectedDiam;
    double diameterRatio;

    double _confidence;

    std::string details;
};

class BallDetector {
public:
    BallDetector(FieldHomography* homography_, bool topCamera);
    ~BallDetector();

    bool findBall(ImageLiteU8 orange, double cameraHeight);

    Ball& best() { return _best; }

    // For tool
#ifdef OFFLINE
    const std::vector<Ball>& getBalls() const { return candidates; }
    Connectivity* getBlobber() { return &blobber; }
#endif
private:
    Connectivity blobber;
    FieldHomography* homography;
    bool topCamera;

    Ball _best;

    // For tool
#ifdef OFFLINE
    std::vector<Ball> candidates;
#endif
};

}
}
