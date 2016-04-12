/*
 *  ------------------------------
 * |  Visual Robot Detection 2016 |
 *  ------------------------------
 */

#include "RobotDetector.h"

namespace man {
namespace vision {

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
    ed->gradientThreshold(0); // set to lower fuzzy thresh

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
    // Robot testRobot(0, 150, 0, 100);
    // candidates.push_back(testRobot);

    int boxW = 70;
    int boxH = 170;
    uint8_t brightnessThresh = 120;
    float percent = .9;
    int countThresh = ((float)boxW * (float)boxH * percent);

    // init accumulators:
    int accumulators[img_wd];
    int currSum = 0;

    // initialize accumulators for each column
    //    as sum of box height in that column, and
    // init current sum
    // std::cout<<"ACCUMULATORS: ";
    for (int i = 0; i < img_wd; ++i) {
        accumulators[i] = 0;
        for (int j = 0; j < boxH; ++j) {
            accumulators[i] += *WGImage.pixelAddr(i,j);
        }
        if (i < boxW) {
            currSum += accumulators[i];
        }
        // std::cout<<accumulators[i]<<", ";
    }
    // std::cout<<std::endl;

    // misisng avg > percent for upper left corner

    // Go through first row: differen't than other rows
    for (int i = boxW; i < img_wd; ++i) {
        // update sum
        currSum -= accumulators[i - boxW];
        currSum += accumulators[i];

        float avg = (float)currSum / ((float)boxW * (float)boxH * 255.);
        if (avg > percent) {
            candidates.push_back(Robot(i-boxW, i, 0, boxH));
        }
    }

    for (int j = 1; j < img_ht - boxH; ++j) {
        for (int i = boxW; i < img_wd; ++i) {
            // update accumulators
            accumulators[i] -= *WGImage.pixelAddr(i,j-1);
            accumulators[i] += *WGImage.pixelAddr(i,j-1 + boxH);

            // update sum
            currSum -= accumulators[i - boxW];
            currSum += accumulators[i];

            float avg = (float)currSum / ((float)boxW * (float)boxH * 255.);
            if (avg > percent && !(i%15) && !(j%15)) {
                candidates.push_back(Robot(i - boxW, i, j, j+boxH));
            }
        }
    }
}

Robot::Robot(int l, int r, int t, int b)
    : left(l),
      right(r),
      top(t),
      bottom(b) {}

} //namespace vision
} //namespace man
