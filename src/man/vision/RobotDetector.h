/*
 * @class RobotDetector
 * @author Megan Maher
 * @date June 2016
 */

#pragma once

#include "RoboGrams.h"
#include <vector>
#include <utility>

#include "Images.h"
#include "Gradient.h"
#include "Edge.h"


namespace man {
namespace vision {

/* The bounding box that surround a discovered robot, in image coordinates */
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

// How bright does a pixel have to be for us to consider it part of a robot
static const unsigned long UPPER_BRIGHT_CONFIDENCE_THRESH = 120; // top image
static const unsigned long LOWER_BRIGHT_CONFIDENCE_THRESH = 150; // bottom image

// Bounding box height and width for each image
static const int UPPER_BOX_HEIGHT = 140;
static const int UPPER_BOX_WIDTH = 90;
static const int LOWER_BOX_HEIGHT = 60;
static const int LOWER_BOX_WIDTH = 50;

// Upper and lower fuzzy gradient values for constructor
static const uint8_t LOWER_FUZZY_GRAD_THRESH = 2;
static const uint8_t UPPER_FUZZY_GRAD_THRESH = 11;

public:
    RobotDetector(int wd_, int ht_);
    ~RobotDetector();

    /* Run every frame from vision module, returns whether or not we have found a robot */
    bool detectRobots(ImageLiteU8 whiteImage,
                      EdgeDetector* ed, EdgeList& edges,
                      FieldHomography* hom, bool is_top);

    /* Determine direction in which robots were found so vision module can send message */
    void getDetectedRobots(bool* detectedObstacles, int size);

    /* Return a vector of robot boxes, in image coordinates */
    const std::vector<Robot>& getRobotList() const { return candidates; }

#ifdef OFFLINE
    ImageLiteU8 getImage() { return WGImage; }
    std::vector<Robot> getRobots() { return candidates; }
    std::vector<Robot> getOldRobots() { return unmergedCandidates; }
#endif

private:
    /* Get a fuzzy value from the gradient magnitude. */
    uint8_t getFuzzyValue(uint8_t gradientValue);

    /* Darken pixels in the WG Image that are part of a known field hough line */
    void removeHoughLines(EdgeList& edges);

    /* Pass a box around the image and check for areas of high spatial density.
     * If average brightness contained inside the box is above a certain threshold
     * and greater than neighbors' values, then it is part of a robot. */
    void findCandidates(bool is_top);

    /* *Merge robot boxes together if they overlap significantly enough */
    void mergeCandidate(int lf, int rt, int tp, int bt);

    /* Restrict which part of the image we process according to the robot's azimuth */
    int findAzimuthRestrictions(FieldHomography* hom);

    /* Get the direction in which the robot's head is currently facing */
    void getCurrentDirection(FieldHomography* hom);

    /* Print the current vector of discovered robots */
    void printCandidates(std::string message);

    int img_wd, img_ht;

    /* Fuzzy thresholds for turning gradient magnitude into fuzzy logic value */
    uint8_t low_fuzzy_thresh, high_fuzzy_thresh;

    /* White-Gradient (WG) Image: Brightness corresponds to confidence there is high
     *                            gradient magnitude and a large amount of white */
    ImageLiteU8 WGImage;

    /* Where is the discovered robot in relation to where my body is facing? */
    enum Direction { none, north, northeast, east, southeast,
                     south, southwest, west, northwest };
    Direction current_direction;

    /* List of found robot boxes in image coordinates */
    std::vector<Robot> candidates;

#ifdef OFFLINE
    /* List of found robots before their boxes are merged: used in tool only */
    std::vector<Robot> unmergedCandidates;
#endif
};

} // vision
} // man
