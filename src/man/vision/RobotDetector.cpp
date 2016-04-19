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

RobotDetector::~RobotDetector() {
    // candidates.clear();
    // delete candidates;
}

// Run every frame from VisionModule.cpp
void RobotDetector::getWhiteGradImage(ImageLiteU8 whiteImage,
                                      EdgeDetector* ed, EdgeList& edges,
                                      FieldHomography* hom, bool is_top)
{
    candidates.clear();

    uint8_t min = 255;
    uint8_t max = 0;
    uint8_t* pixels = new uint8_t[img_ht*img_wd];

    // HACK: Do this better -> might impact other things
    ed->gradientThreshold(low_fuzzy_thresh);

    int startCol = 1;
    int endCol = img_wd - 1;
    if (!is_top) {
        int val = findAzimuthRestrictions(hom);
        if (val < 0) { endCol = -val; }
        else if (val > 1) { startCol = val; }
    }
    // std::cout<<"Start col: "<<startCol<<", endCol = "<<endCol<<", wd = "<<img_wd<<std::endl;

    std::cout<<"[ ROBOT DETECTOR] Az = "<<hom->azimuth()<<std::endl;

    for (int j = 1; j < img_ht-1; ++j) {
        for (int i = startCol; i < endCol; ++i) {
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
    findCandidates(is_top);
}

uint8_t RobotDetector::getFuzzyValue(uint8_t gradientValue)
{
    if (gradientValue <= low_fuzzy_thresh) { return 0; }
    if (gradientValue >= high_fuzzy_thresh) { return 255; }
    uint8_t top = gradientValue - low_fuzzy_thresh;
    uint8_t bot = high_fuzzy_thresh - low_fuzzy_thresh;
    return top * 255 / bot;
}

int RobotDetector::findAzimuthRestrictions(FieldHomography* hom)
{

    // A little hacky: TODO make function / calculate better values / use constants
    // HACK US OPEN 2016 SORRY!!
    double az = hom->azimuth();
    int val = 1;

    // if abs(az) 1.3, too great to detect obstacle: return width
    if (az > 1.3 || az < -1.3) { return img_wd-1; }

    // if abs(az) is > 1.1 and <= 1.3, ignore 2/3
    if (az > 1.1 || az < -1.1) { val = (int)(.6666 * (double)img_wd); }

    // if abs(az) is > 1 and <= 1.1, ignore 1/2
    else if (az > 1.0 || az < -1.0) { val = (int)(.5 * (double)img_wd); }

    // if abs(az) is > 0.98 and <= 1, ignore 1/3
    else if (az > 0.98 || az < -0.98) { val = (int)(.3333 * (double)img_wd); }

    // if abs(az) is > 0.93 and <= 0.98, ignore 1/4
    else if (az > 0.93 || az < -0.93) { val = (int)(.25 * (double)img_wd); }

    // else, no shoulder obstacle! return 0 to detect everything
    else { return 0; }

    // if we've gotten to this point, we want to ignore part of the
    // image. Side of image depends on sign of azimuth.
    if (az > 0) { return -1*(img_wd - val); } // right side
    else { return val; } // left side (negative lets us know it is the "end col")

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

            // remove all pixels within "boxsize" of hough line pixel
            int boxSize = 3;
            int houghx = xi + img_wd/2;
            int houghy = img_ht/2 - yi;
            for (int i = houghx - boxSize; i <= houghx + boxSize; ++i) {
                for (int j = houghy - boxSize; j <= houghy + boxSize; ++j) {
                    if (i < 0 || j < 0) { continue; }
                    if (i >= img_wd || j >= img_ht) { continue; }
                    *WGImage.pixelAddr(i, j) = 0;
                }
            }
        }
    }
}

void RobotDetector::findCandidates(bool is_top)
{
    int boxW = 100;
    int boxH = 160;
    if (!is_top) {
        boxW = 50;
        boxH = 60;
    }
    unsigned long brightness_thresh = 120;

    // Just for me to see what size box I'm using
    // Robot testRobot(0, boxW, 0, boxH);
    // candidates.push_back(testRobot);

    // init accumulators:
    unsigned long accumulators[img_wd];
    unsigned long grid[img_wd - boxW + 1][img_ht - boxH + 1];
    unsigned long currSum = 0;

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
            // if (j > 5 && !is_top) { grid[i][j] = 0; } // ignore boxes low in image
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
                    if (m >= img_wd-boxW+1 || n >= img_ht-boxH+1) { continue; }
                    if (m == i && n == j) { continue; }
                    if (grid[i][j] < grid[m][n]) {
                        amPeak = false;
                    }
                }
            }

            // I am above thresh and am greater than all neighbors: I AM ROBOT
            if (amPeak) {
                candidates.push_back(Robot(i, i+boxW, j, j+boxH));
            }
        }
    }
}

} //namespace vision
} //namespace man
