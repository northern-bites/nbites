/*
 *  ------------------------------
 * |  Visual Robot Detection 2016 |
 *  ------------------------------
 */

#include "RobotDetector.h"

namespace man {
namespace vision {

Robot::Robot(int l, int r, int t, int b)
    : left(l),
      right(r),
      top(t),
      bottom(b) {}

RobotDetector::RobotDetector(int wd_, int ht_)
    : img_wd(wd_),
      img_ht(ht_),
      low_fuzzy_thresh(2),
      high_fuzzy_thresh(11)
{
    std::cout<<"[ ROBOT DETECTOR] width = "<<wd_<<", height = "<<ht_<<std::endl;

    // init pixels for WG Image
    uint8_t* pixels = new uint8_t[img_ht*img_wd];
    for (int j = 1; j < img_ht-1; ++j) {
        for (int i = 1; i < img_wd-1; ++i) {
            pixels[i + img_wd*j] = 0;
        }
    }
    WGImage = ImageLiteU8(0, 0, img_wd, img_ht, img_wd, pixels);
}

// Run every frame from VisionModule.cpp
void RobotDetector::getWhiteGradImage(ImageLiteU8 whiteImage,
                                      EdgeDetector* ed, EdgeList& edges)
{
    candidates.clear();

    uint8_t min = 255;
    uint8_t max = 0;
    uint8_t* pixels = new uint8_t[img_ht*img_wd];

    // HACK: Do this better -> might impact other things
    ed->gradientThreshold(low_fuzzy_thresh);

    for (int j = 1; j < img_ht-1; ++j) {
        for (int i = 1; i < img_wd-1; ++i) {
            uint8_t grad = ed->mag(i-1,j-1);
            uint8_t fuzzy = getFuzzyValue(grad);
            uint8_t whiteVal = *(whiteImage.pixelAddr(i,j));
            if (fuzzy < whiteVal) {
                *WGImage.pixelAddr(i,j) = fuzzy;
            } else {
                *WGImage.pixelAddr(i,j) = whiteVal;
            }

            if (grad < min) { min = grad; }
            if (grad > max) { max = grad; }
        }
    }

    removeHoughLines(edges);
    findCandidates();
}

uint8_t RobotDetector::getFuzzyValue(uint8_t gradientValue)
{
    if (gradientValue <= low_fuzzy_thresh) { return 0; }
    if (gradientValue >= high_fuzzy_thresh) { return 255; }
    uint8_t top = gradientValue - low_fuzzy_thresh;
    uint8_t bot = high_fuzzy_thresh - low_fuzzy_thresh;
    return top * 255 / bot;
}

void RobotDetector::removeHoughLines(EdgeList& edges)
{
    // Get edges from vision
    AngleBinsIterator<Edge> abi(edges);
    for (Edge* e = *abi; e; e = *++abi){
        // If we are part of a hough line, we are not a robot
        if (e->memberOf()) {
            int xi = e->x();
            int yi = e->y();

            *WGImage.pixelAddr(xi + img_wd/2, img_ht/2 - yi) = 0;

            // remove all pixels within 1 of hough line pixel
            *WGImage.pixelAddr(xi + img_wd/2 + 1, img_ht/2 - yi) = 0;
            *WGImage.pixelAddr(xi + img_wd/2, img_ht/2 - yi + 1) = 0;
            *WGImage.pixelAddr(xi + img_wd/2 - 1, img_ht/2 - yi) = 0;
            *WGImage.pixelAddr(xi + img_wd/2, img_ht/2 - yi - 1) = 0;
            *WGImage.pixelAddr(xi + img_wd/2 + 1, img_ht/2 - yi + 1) = 0;
            *WGImage.pixelAddr(xi + img_wd/2 + 1, img_ht/2 - yi - 1) = 0;
            *WGImage.pixelAddr(xi + img_wd/2 - 1, img_ht/2 - yi + 1) = 0;
            *WGImage.pixelAddr(xi + img_wd/2 - 1, img_ht/2 - yi - 1) = 0;

            // remove all pixels within 2 of hough line pixel
            *WGImage.pixelAddr(xi + img_wd/2 + 2, img_ht/2 - yi) = 0;
            *WGImage.pixelAddr(xi + img_wd/2, img_ht/2 - yi + 2) = 0;
            *WGImage.pixelAddr(xi + img_wd/2 - 2, img_ht/2 - yi) = 0;
            *WGImage.pixelAddr(xi + img_wd/2, img_ht/2 - yi - 2) = 0;
            *WGImage.pixelAddr(xi + img_wd/2 + 2, img_ht/2 - yi + 2) = 0;
            *WGImage.pixelAddr(xi + img_wd/2 + 2, img_ht/2 - yi - 2) = 0;
            *WGImage.pixelAddr(xi + img_wd/2 - 2, img_ht/2 - yi + 2) = 0;
            *WGImage.pixelAddr(xi + img_wd/2 - 2, img_ht/2 - yi - 2) = 0;

            // remove all pixels within 3 of hough line pixel
            *WGImage.pixelAddr(xi + img_wd/2 + 3, img_ht/2 - yi) = 0;
            *WGImage.pixelAddr(xi + img_wd/2, img_ht/2 - yi + 3) = 0;
            *WGImage.pixelAddr(xi + img_wd/2 - 3, img_ht/2 - yi) = 0;
            *WGImage.pixelAddr(xi + img_wd/2, img_ht/2 - yi - 3) = 0;
            *WGImage.pixelAddr(xi + img_wd/2 + 3, img_ht/2 - yi + 3) = 0;
            *WGImage.pixelAddr(xi + img_wd/2 + 3, img_ht/2 - yi - 3) = 0;
            *WGImage.pixelAddr(xi + img_wd/2 - 3, img_ht/2 - yi + 3) = 0;
            *WGImage.pixelAddr(xi + img_wd/2 - 3, img_ht/2 - yi - 3) = 0;
        }
    }
}

void RobotDetector::findCandidates()
{
    int boxW = 100;
    int boxH = 160;
    unsigned long brightness_thresh = 120;
    float percent = .9;
    int countThresh = ((float)boxW * (float)boxH * percent);
    int num_candidates = 0;

    // Just for me to see what size box I'm using
    // Robot testRobot(0, boxW, 0, boxH);
    // candidates.push_back(testRobot);

    // init accumulators:
    unsigned long accumulators[img_wd];
    unsigned long grid[img_wd - boxW + 1][img_ht - boxH + 1];
    unsigned long currSum = 0;

    std::cout<<"Checking first line"<<std::endl;

    // initialize accumulators for each column
    //    as sum of box height in that column, and
    // init current sum
    for (int i = 0; i < img_wd; ++i) {
        accumulators[i] = 0;
        for (int j = 0; j < boxH; ++j) {
            accumulators[i] += *WGImage.pixelAddr(i,j);
        }
        if (i < boxW) {
            currSum += accumulators[i];
            if (i == boxW-1) { grid[0][0] = currSum; }
            continue;
        } else {
            currSum -= accumulators[i - boxW];
            currSum += accumulators[i];
            grid[i - boxW + 1][0] = currSum;
        }
    }

    for (int j = 1; j <= img_ht - boxH; ++j) {
        currSum = 0;
        for (int i = 0; i < boxW; ++i) {
            accumulators[i] -= *WGImage.pixelAddr(i,j-1);
            accumulators[i] += *WGImage.pixelAddr(i,j-1 + boxH);
            currSum += accumulators[i];
        }
        grid[0][j] = currSum;
        for (int i = 1; i <= img_wd - boxW; ++i) {
            accumulators[i-1 + boxW] -= *WGImage.pixelAddr(i-1+boxW,j-1);
            accumulators[i-1 + boxW] += *WGImage.pixelAddr(i-1+boxW,j-1+boxH);
            currSum -= accumulators[i-1];
            currSum += accumulators[i-1 + boxW];
            grid[i][j] = currSum;
        }
    }

    for (int i = 0; i < img_wd - boxW +1; ++i) {
        for (int j = 0; j < img_ht - boxH +1; ++j) {
            // I am not above the thresh, I am not a robot candidate
            unsigned long avg_val = grid[i][j] / (boxW * boxH);
            if (avg_val < brightness_thresh) { continue; }

            // Now make sure I have a greater value than all my neighbors
            bool amPeak = true;
            for (int m = i - 2; m <= i + 2; ++m) {
                for (int n = j - 2; n <= j+2; ++n) {
                    if (m < 0 || n < 0) { continue; }
                    if (m == i && n == j) { continue; }
                    if (grid[i][j] < grid[m][n]) {
                        amPeak = false;
                    }
                }
            }

            // I am above thresh and am greater than all neighbors: ROBOT
            if (amPeak) {
                candidates.push_back(Robot(i, i+boxW, j, j+boxH));
            }
        }
    }
}

} //namespace vision
} //namespace man
