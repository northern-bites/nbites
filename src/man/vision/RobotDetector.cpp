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
      high_fuzzy_thresh(11),
      current_direction(none)
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
    candidates.clear();
    unmergedCandidates.clear();
}

// Run every frame from VisionModule.cpp
bool RobotDetector::detectRobots(ImageLiteU8 whiteImage,
                                 EdgeDetector* ed, EdgeList& edges,
                                 FieldHomography* hom, bool is_top)
{
    candidates.clear();
    unmergedCandidates.clear();

    uint8_t min = 255;
    uint8_t max = 0;

    // HACK: Do this better -> might impact other things
    ed->gradientThreshold(low_fuzzy_thresh);

    int startCol = 1;
    int endCol = img_wd - 1;
    int bottomRow = .9 * img_ht;
    if (!is_top) {
        int val = findAzimuthRestrictions(hom);
        if (val < 0) { endCol = -val; }
        else if (val > 1) { startCol = val; }
        startCol -= 15;
        if (startCol < 1) { startCol = 1; }
        endCol += 15;
        if (endCol > img_wd - 1) { endCol = img_wd - 1; }
    }

    for (int j = 1; j < img_ht-1; ++j) {
        if (j > bottomRow) {
            for (int i = 1; i < img_wd -1; ++i)
                *WGImage.pixelAddr(i,j) = 0;
            continue;
        }
        for (int i = 1; i < img_wd -1; ++i) {
            if (i < startCol || i >= endCol) {
                *WGImage.pixelAddr(i,j) = 0;
                continue;
            }
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

    if (candidates.size() > 0) {
        return true;
    }
    return false;
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
    double az = hom->azimuth();

    float percentOfImage;
    if (az > 0) {
        percentOfImage = 0.8994*az*az - 0.0036*az - 0.5767;
    } else {
        percentOfImage = 0.8994*az*az + 0.0036*az - 0.5767;
    }

    int val = percentOfImage * img_wd;

    // detect everything
    if (val <= 0) { return 0; }

    // detect nothing
    if (val >= img_wd -1) { return img_wd-1; }

    if (az > 0) { return -1*(img_wd - val); } // right side (negative lets us know it is the "end col")
    else { return val; } // left side = start col
}

void RobotDetector::getCurrentDirection(FieldHomography* hom)
{
    double az = hom->azimuth();
    if (az > 1.825) { current_direction = southeast; }
    else if (az > 1.125) { current_direction = east; }
    else if (az > 0.375) { current_direction = northeast; }
    else if (az > -0.375) { current_direction = north; }
    else if (az > -1.125) { current_direction = northwest; }
    else if (az > -1.825) { current_direction = west; }
    else {current_direction = southwest; }
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
    int boxW = 90;
    int boxH = 140;
    unsigned long brightness_thresh = 120;
    if (!is_top) {
        boxW = 50;
        boxH = 60;
        brightness_thresh = 150;
    }

    // Just for me to see what size box I'm using
    // Robot testRobot(0, boxW, 0, boxH);
    // candidates.push_back(testRobot);

    // Just for now, find best box candidate
    unsigned int bestID = 0;
    unsigned int bestBrightness = 0;

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
                unmergedCandidates.push_back(Robot(i, i+boxW, j, j+boxH));
                if (candidates.empty()) {
                    // std::cout<<"candidates empty"<<std::endl;
                    candidates.push_back(Robot(i, i+boxW, j, j+boxH));
                } else {
                    // std::cout<<"candidates not empty"<<std::endl;
                    // std::cout<<"Box before merge: "<<i<<", "<<i+boxW<<", "<<j<<", "<<j+boxH<<std::endl;
                    mergeCandidate(i, i+boxW, j, j+boxH);
                }
            }
        }
    }
    // int c1, c2;
    // c1 = 50;
    // c2 = 70;
    // // candidates.push_back(Robot(c1, c1+boxW, c1, c1+boxH));
    // candidates.push_back(Robot(c2+boxW/2, c2+boxW/2+boxW, c2, c2+boxH));
    // unmergedCandidates.push_back(Robot(c1, c1+boxW, c1, c1+boxH));
    // unmergedCandidates.push_back(Robot(c2+boxW/2, c2+boxW/2+boxW, c2, c2+boxH));
    // std::cout<<"First Candidate: "<<c1<<", "<<c1+boxW<<", "<<c1<<", "<<c1+boxH<<std::endl;
    // std::cout<<"Second Candidate: "<<c2+boxW/2<<", "<<c2+boxW/2+boxW<<", "<<c2<<", "<<c2+boxH<<std::endl;
    // // mergeCandidate(c2+boxW/2, c2+boxW+boxW/2, c2, c2+boxH);
    // mergeCandidate(c1, c1+boxW, c1, c1+boxH);

    // printCandidates("[ ROBOT DETECTOR ] FINAL CANDIDATES:");
}

void RobotDetector::mergeCandidate(int lf, int rt, int tp, int bt)
{
    // Merge box with existing candidates:
    std::vector<Robot>::iterator it;
    for(it = candidates.begin(); it != candidates.end(); ++it) {
        if (lf <= (*it).left && (*it).left <= ((rt+lf)/2)) {
            // I'm halfway overlapping in the x direction
            // new box is to left of old box
            if (tp <= (*it).top && (*it).top <= ((tp+bt)/2)) {
                // new box is upper left of candidate box
                bt = (*it).bottom;
                rt = (*it).right;
                candidates.erase(it);
                it = candidates.begin()-1; // loop through them all again
            } else if ((*it).top <= tp && tp <= (((*it).top+(*it).bottom)/2)) {
                // new box is to lower left of candidate box
                tp = (*it).top;
                rt = (*it).right;
                candidates.erase(it);
                it = candidates.begin()-1; // loop through them all again
            }
        } else if ((*it).left <= lf && lf <= (((*it).left+(*it).right)/2)) {
            // I'm halfway overlapping in the y direction
            // new box is above old box
            if (tp <= (*it).top && (*it).top <= ((tp+bt)/2)) {
                // new box is upper right of candidate box
                bt = (*it).bottom;
                lf = (*it).left;
                candidates.erase(it);
                it = candidates.begin()-1; // loop through them all again
            } else if ((*it).top <= tp && tp <= (((*it).top+(*it).bottom)/2)) {
                // new box is to lower right of candidate box
                tp = (*it).top;
                lf = (*it).left;
                candidates.erase(it);
                it = candidates.begin()-1; // loop through them all again
            }
        }
    }
    candidates.push_back(Robot(lf, rt, tp, bt));
}

// This should be different, just testing bottom camera stuff out right now.
// void RobotDetector::toFieldCoordinates(FieldHomography* hom, float* obstacleBox, int lowestCol)
// {
//     // If we haven't found an obstacle:
//     if (lowestCol == -1) { return; }

//     // Homography takes an image coordinate as a parameter, so we must
//     // convert to that coordinate system
//     //      FROM: y+ forward, x+ right, TO: x+ forward, y+ left

//     // let's map the lowest point to field coordinates
//     double x1, y1, xb, yb;
//     x1 = (double)lowestCol - (double)img_wd/2;
//     y1 = (double)img_ht/2 - (double)obstacleBox[1]; // bottom of obstacle box
//     if (!hom->fieldCoords(x1, y1, xb, yb)) {
//         // we were not successful in our first mapping
//         resetObstacleBox(obstacleBox);
//         return;
//     }

//     // now let's map the left point of the box
//     double x2, y2, xl, yl;
//     x2 = (double)obstacleBox[2] - (double)img_wd/2; // left side of obstacle box
//     if (!hom->fieldCoords(x2, y1, xl, yl)) {
//         // we were not successful in our second mapping
//         resetObstacleBox(obstacleBox);
//         return;
//     }

//     // finally let's map the right point of the box
//     double x3, y3, xr, yr;
//     x3 = (double)obstacleBox[3] - (double)img_wd/2; // left side of obstacle box
//     if (!hom->fieldCoords(x3, y1, xr, yr)) {
//         // we were not successful in our third mapping
//         resetObstacleBox(obstacleBox);
//         return;
//     }

//     // All the mapping was successful!
//     // now that we have them in robot relative coordinates... we have to
//     // convert them to the correct coordinate system, with same origin:
//     // FROM: y+ forward, x+ right, TO: x+ forward, y+ to left
//     obstacleBox[0] = -1.f*(float)xb;     // bottom x coordinate (now y)
//     obstacleBox[1] = (float)yb;          // bottom y coordinate (now x)
//     obstacleBox[2] = -1.f*(float)xl;     // left x coordinate (now y)
//     obstacleBox[3] = -1.f*(float)xr;     // right x coordinate (now y)
// }

// Right now, to figure out the robot's position, we are using the left and right coordinates of
// the box to determine in which direction the obstructing robot is. This could definitely be
// improved, but right now we don't know the distance to the robot because the box doesn't
// necessarily extend to the actual bottom of the robot... meaning homography will fail. We
// divide the image into three sections:
//
//               ___|___________|___________|___
//              |       |               |       |
//              | Left  |    Middle     | Right |
//              |Section|    Section    |Section|
//              |       |               |       |
//              |  25%  |      50%,     |  25%  |
//              |       |    Current    |       |
//              |       |   Direction   |       |
//              |_______|_______________|_______|
//                  |           |           |
//
void RobotDetector::getDetectedRobots(bool* detectedObstacles, int size) {
    // Now we take information and return relevant obstacles
    // portals::Message<messages::RobotObstacle> current(0);

    // Make array bigger than 8 directions so we can do direction +- 1 without error
    for (int i = 0; i < size; ++i) {
        detectedObstacles[i] = false;
    }

    // int left_barrier = img_wd / 4;
    // int right_barrier = (img_wd / 4)*3;
    // int left_halfway = img_wd / 8;
    // int middle_halfway = img_wd / 2;
    // int right_halfway = (img_wd / 8)*7;
    int left_barrier = img_wd / 3;
    int right_barrier = (img_wd / 3)*2;
    int left_halfway = img_wd / 6;
    int middle_halfway = img_wd / 2;
    int right_halfway = (img_wd / 6)*5;

    std::vector<Robot>::iterator it;
    for(it = candidates.begin(); it != candidates.end(); ++it) {
        if ((*it).right < left_barrier) {
            // box is entirely contained in left section of image
            detectedObstacles[current_direction-1] = true;
        } else if ((*it).right < middle_halfway) {
            // box is either in left section or middle section
            if ((*it).left < left_halfway) {
                // left section
                detectedObstacles[current_direction-1] = true;
            } else {
                // between two: choose middle
                detectedObstacles[current_direction] = true;
            }
        } else if ((*it).right < right_halfway) { // right halway and right barrier were same
            // box is either in middle section, or both left and middle
            if ((*it).left < left_halfway) {
                // both sections
                detectedObstacles[current_direction-1] = true;
                detectedObstacles[current_direction] = true;
            } else {
                // middle section
                detectedObstacles[current_direction] = true;
            }
        } else {
            // right section, right+middle, or right+middle+left
            if ((*it).left < left_halfway) {
                // all sections
                detectedObstacles[current_direction-1] = true;
                detectedObstacles[current_direction] = true;
                detectedObstacles[current_direction+1] = true;
            } else if ((*it).left < middle_halfway) {
                // right and middle sections
                detectedObstacles[current_direction] = true;
                detectedObstacles[current_direction+1] = true;
            } else {
                // right section
                detectedObstacles[current_direction+1] = true;
            }
        }
    }

    // Account for buffer space in array
    if (detectedObstacles[0]) { detectedObstacles[8] = true; }
    if (detectedObstacles[9]) { detectedObstacles[1] = true; }
}

void RobotDetector::printCandidates(std::string message) {
    std::cout<<message<<std::endl;
    std::vector<Robot>::iterator it;
    int counter = 0;
    for(it = candidates.begin(); it != candidates.end(); ++it) {
        std::cout<<"   "<<++counter<<": "<<(*it).left<<", "<<(*it).right<<", "<<(*it).top<<", "<<(*it).bottom<<std::endl;
    }
}

} //namespace vision
} //namespace man
