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

public:
    RobotDetector(int wd_, int ht_);
    ~RobotDetector();

    void getWhiteGradImage(ImageLiteU8& WG, ImageLiteU8 whiteImage,
                           EdgeDetector* ed, EdgeList& edges);
    uint8_t getFuzzyValue(uint8_t gradientValue);
    void removeHoughLines(ImageLiteU8& WG, EdgeList& edges);
private:

// #ifdef OFFLINE
//     void buildRobotImage(int wd_, int ht_,
//                          const EdgeDetector* ed,
//                          const ImageLiteU8 whiteImage);
//     messages::PackedImage8 robotImage;
// #endif

    int img_wd;
    int img_ht;

    int low_fuzzy_thresh;
    int high_fuzzy_thresh;

};

} // vision
} // man
