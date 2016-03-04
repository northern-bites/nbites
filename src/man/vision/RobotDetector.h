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

class RobotDetector {

static const int WHITE_CONFIDENCE_THRESH = 0; // min confidence to consider pixel "white"

public:
    RobotDetector(int wd_, int ht_);
    ~RobotDetector();

    void getWhiteGradImage(ImageLiteU8& WG, ImageLiteU8 whiteImage,
                           EdgeDetector* ed, EdgeList& edges);
    uint8_t getFuzzyValue(uint8_t gradientValue);
    void removeHoughLines(ImageLiteU8& WG, EdgeList& edges);
    void findCandidates(ImageLiteU8& temp, ImageLiteU8& WG);
private:

// #ifdef OFFLINE
//     void buildRobotImage(int wd_, int ht_,
//                          const EdgeDetector* ed,
//                          const ImageLiteU8 whiteImage);
//     messages::PackedImage8 robotImage;
// #endif

// #ifdef OFFLINE
//             const std::vector<Ball>& getRobots() const { return candidates; }
//             // void setDebugBall(bool debug) {debugBall = debug;}
// #endif

    int img_wd;
    int img_ht;

    int low_fuzzy_thresh;
    int high_fuzzy_thresh;

    // struct robotBox {
    //     int left;
    //     int right;
    //     int top;
    //     int bottom;
    // };

    // // For tool
    // std::vector<robotBox> candidates;
};

} // vision
} // man
