#pragma once

#include "RoboGrams.h"
#include <vector>
#include <utility>
#include "Images.h"
// #include "FuzzyThr.h"
#include "Gradient.h"
#include "Edge.h"
// #include "Vision.h"
#include "VisionRobot.pb.h"

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
                           EdgeDetector* ed, EdgeList& edges,
                           FieldHomography* hom, bool is_top);
    uint8_t getFuzzyValue(uint8_t gradientValue);
    void removeHoughLines(EdgeList& edges);
    void findCandidates(bool is_top);
    void mergeCandidate(int lf, int rt, int tp, int bt);
    // Tells us which part of the image to ignore if we are looking too
    // far to the right or to the left (would pick up shoulder)
    int findAzimuthRestrictions(FieldHomography* hom);
    void getCurrentDirection(FieldHomography* hom);
    void getDetectedRobots(bool* detectedObstacles, int size);
    void printCandidates(std::string message);

    struct box {
        int left;
        int right;
        int top;
        int bottom;
    };

#ifdef OFFLINE
    ImageLiteU8 getImage() { return WGImage; }
    std::vector<Robot> getRobots() { return candidates; }
    std::vector<Robot> getOldRobots() { return unmergedCandidates; }
#endif

private:
    enum Direction { none, north, northeast, east, southeast,
                     south, southwest, west, northwest };

    int img_wd;
    int img_ht;

    uint8_t low_fuzzy_thresh;
    uint8_t high_fuzzy_thresh;

    Direction current_direction;

    ImageLiteU8 WGImage;

    // For tool right now
    std::vector<Robot> candidates;
    std::vector<Robot> unmergedCandidates;
};

} // vision
} // man
