#pragma once

#include <vector>
#include <utility>
#include "Images.h"
// #include "FuzzyThr.h"
#include "Gradient.h"
#include "Edge.h"
// #include "Vision.h"

namespace man {
namespace vision {

class Robot {
public:
    Robot(int l, int r, int t, int b);
    Robot();
    int left;
    int right;
    int top;
    int bottom;
};

class RobotDetector {

static const int WHITE_CONFIDENCE_THRESH = 0; // min confidence to consider pixel "white"

public:
    RobotDetector(int wd_, int ht_);
    ~RobotDetector();

    void getWhiteGradImage(ImageLiteU8 whiteImage,
                           EdgeDetector* ed, EdgeList& edges);
    uint8_t getFuzzyValue(uint8_t gradientValue);
    void removeHoughLines(EdgeList& edges);
    void findCandidates();

    struct box {
        int left;
        int right;
        int top;
        int bottom;
    };

#ifdef OFFLINE
    ImageLiteU8 getImage() { return WGImage; }
    std::vector<Robot> getRobots() { return candidates; }
#endif

private:
    int img_wd;
    int img_ht;

    int low_fuzzy_thresh;
    int high_fuzzy_thresh;

    ImageLiteU8 WGImage;

    // For tool right now
    std::vector<Robot> candidates;
};

} // vision
} // man
