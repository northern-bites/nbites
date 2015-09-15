/*
 * @file RobotObstacle.h
 * @author Megan Maher
 * @created July 2015
 * @modified July 2015
 *                        -------------------
 *                       |  Robot Detection  |
 *                        -------------------
 * This module is simple and has many opporunities for improvement.
 *
 * Input: White image, list of edges in image, pointer to Vision
 *        Module's "Obstacle Box" so we can update it, and homography.
 *
 * We go through all edges found previously in vision, exlude the ones
 * that are part of hough lines, then look to find the bottom-most edge
 * and the top-most edge in each column of the image.
 *
 * Once we have the bottom edge in every column, we go through each of the
 * columns and find runs of columns with evidence (i.e. how many columns
 * in a row have significant enough evidence). At this stage, we name
 * significant evidence to be the following:
 *      - Bottom edge not too high in the image
 *        (no way to account for field cross)
 *      - Bottom edge not too low in the image
 *        (hack so we certainly can't detect ourselves)
 *      - If more than half the pixels above the edge are white
 *        with at least the min confidence
 *
 * We consider up to "MAX_DIST" number of pixels as blanks in between columns
 * of the to account for image error. This means that if we have 6 pixels
 * with evidence, 4 blank pixels, and 5 more pixels with evidence, we
 * count this as a run of 15. (For MAX_DIST = 4)
 *
 * If our max run length is at least as long as our MIN_LENGTH, then we
 * use that run as our obstacle. We keep track of the lowest point in this
 * run as we make our first loop through the image columns, so we use this
 * as our "obstacle box" bottom. We use the start of the run as our
 * "obstacle box" left, then can determine the right of the box with our
 * max run length.
 *
 * The obstacle box constricts the obstacle, exists in the Vision Module,
 * and is then passed out as a protobuf to the obstacle module for processing.
 *
 * Opportunities for further work:
 *      - Dealing with field crosses and center circle in image
 */

#pragma once

#include <string.h>
#include "Hough.h"

namespace man {
namespace vision {

class RobotObstacle
{
    static const int MAX_DIST = 4;     // max columns we can have "empty" in a row for a run
    static const int MIN_LENGTH = 35;  // min number for a run
    static const int BARRIER_TOP = 15; // amount on top of image we don't want to process
    static const int BARRIER_BOT = 30; // amount on bot of image we don't want to process
    static const int WHITE_CONFIDENCE_THRESH = 128; // min confidence to consider pixel "white"

public:
    RobotObstacle(int wd_, int ht_);

    /* Run every frame from Vision Module, for bottom image only */
    void updateVisionObstacle(ImageLiteU8 whiteImage, EdgeList& edges,
                              float* obstacleBox, FieldHomography* hom);

private:
    /* Initializes arrays. */
    void initAccumulators(float* obstacleBox);
    void resetObstacleBox(float* obstacleBox);

    /* Finds the topmost and bottommost edges in each column of the image. */
    void getBottomAndTopEdges(EdgeList& edges);

    // Tells us which part of the image to ignore if we are looking too
    // far to the right or to the left (would pick up shoulder)
    int findAzimuthRestrictions(FieldHomography* hom);

    /* Uses white image to determine which columns have a robot obstacle in them
     * and then where the longest run of these columns is.
     * Returns the column with the lowest point of the obstacle. */
    int findObstacle(ImageLiteU8 whiteImage, float* obstacleBox, int startCol);

    /* Convert image coordinate obstacle box to robot-relative coordinates */
    void toFieldCoordinates(FieldHomography* hom, float* obstacleBox, int lowestCol);

    /* Helper print methods */
    void printArray(int* array, int size, std::string name);
    void printArray(bool* array, int size, std::string name);

    int img_wd;
    int img_ht;

    int* bottom;      // highest edge that points up (not used currently)
    int* top;         // lowest edge that points down (not used currently)
    int* maxbottom;   // lowest edge that points up
    int* mintop;      // highest edge that points down (not currently used)
};

} // namespace vision
} // namespace man
