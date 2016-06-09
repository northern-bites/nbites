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
    // candidates.clear();
    // unmergedCandidates.clear();
    // delete candidates;
    // delete unmergedCandidates;
}

// Run every frame from VisionModule.cpp
void RobotDetector::getWhiteGradImage(ImageLiteU8 whiteImage,
                                      EdgeDetector* ed, EdgeList& edges,
                                      FieldHomography* hom, bool is_top)
{
    candidates.clear();
    unmergedCandidates.clear();

    uint8_t min = 255;
    uint8_t max = 0;
    // uint8_t* pixels = new uint8_t[img_ht*img_wd];

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

    // std::cout<<"[ ROBOT DETECTOR] Az = "<<hom->azimuth()<<std::endl;

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

    // if abs(az) is > 1.15 and <= 1.3, ignore 2/3
    if (az > 1.15 || az < -1.15) { val = (int)(.6666 * (double)img_wd); }

    // if abs(az) is > 1.05 and <= 1.15, ignore 1/2
    else if (az > 1.05 || az < -1.05) { val = (int)(.5 * (double)img_wd); }

    // if abs(az) is > 0.95 and <= 1.05, ignore 1/3
    else if (az > 0.95 || az < -0.95) { val = (int)(.3333 * (double)img_wd); }

    // if abs(az) is > 0.91 and <= 0.95, ignore 1/4
    else if (az > 0.91 || az < -0.91) { val = (int)(.25 * (double)img_wd); }

    // else, no shoulder obstacle! return 0 to detect everything
    else { return 0; }

    // if we've gotten to this point, we want to ignore part of the
    // image. Side of image depends on sign of azimuth.
    if (az > 0) { return -1*(img_wd - val); } // right side
    else { return val; } // left side (negative lets us know it is the "end col")

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
    // std::cout<<"Current merging box: "<<lf<<", "<<rt<<", "<<tp<<", "<<bt<<std::endl;
    // printCandidates("Printing candidates before merging:");
    // Merge box with existing candidates:
    std::vector<Robot>::iterator it;
    for(it = candidates.begin(); it != candidates.end(); ++it) {
        // std::cout<<"candidate box: "<<(*it).left<<", "<<(*it).right<<", "<<(*it).top<<", "<<(*it).bottom<<std::endl;
        // return;
        // std::cout<<"Robot merge: "<<it->
        /* std::cout << *it; ... */
        // std::cout<<lf<<"<="<<(*it).left<<" && "<<(*it).left<<"<="<<((rt+lf)/2)<<"?"<<std::endl;
        // std::cout<<(*it).left<<"<="<<lf<<" && "<<lf<<"<="<<(((*it).left+(*it).right)/2)<<"?"<<std::endl;
        if (lf <= (*it).left && (*it).left <= ((rt+lf)/2)) {
            // std::cout<<"yes!"<<std::endl;
            // I'm halfway overlapping in the x direction
            // new box is to left of old box
            // std::cout<<tp<<"<="<<(*it).top<<" && "<<(*it).top<<"<="<<((tp+bt)/2)<<"?"<<std::endl;
            // std::cout<<(*it).top<<"<="<<tp<<" && "<<tp<<"<="<<(((*it).top+(*it).bottom)/2)<<"?"<<std::endl;
            if (tp <= (*it).top && (*it).top <= ((tp+bt)/2)) {
                // std::cout<<"yes1.1!"<<std::endl;
                // new box is upper left of candidate box
                // (*it).top = tp;
                // (*it).left = lf;
                bt = (*it).bottom;
                rt = (*it).right;

                // printCandidates("printing candidates after merging 1");
                // return;
                // does this start at the second one actually?
                candidates.erase(it);
                it = candidates.begin()-1; // loop through them all again
            } else if ((*it).top <= tp && tp <= (((*it).top+(*it).bottom)/2)) {
                // std::cout<<"yes1.2!"<<std::endl;
                // new box is to lower left of candidate box
                // (*it).bottom = bt;
                // (*it).left = lf;
                tp = (*it).top;
                rt = (*it).right;
                // printCandidates("printing candidates after merging 2");
                // return;
                // does this start at the second one actually?
                candidates.erase(it);
                it = candidates.begin()-1; // loop through them all again
            }
        } else if ((*it).left <= lf && lf <= (((*it).left+(*it).right)/2)) {
            // std::cout<<"yes2!"<<std::endl;
            // I'm halfway overlapping in the x direction
            // new box is to left of old box
            // std::cout<<tp<<"<="<<(*it).top<<" && "<<(*it).top<<"<="<<((tp+bt)/2)<<"?"<<std::endl;
            // std::cout<<(*it).top<<"<="<<tp<<" && "<<tp<<"<="<<(((*it).top+(*it).bottom)/2)<<"?"<<std::endl;
            if (tp <= (*it).top && (*it).top <= ((tp+bt)/2)) {
                // std::cout<<"yes2.1"<<std::endl;
                // new box is upper right of candidate box
                // (*it).top = tp;
                // (*it).right = rt;
                bt = (*it).bottom;
                lf = (*it).left;

                // printCandidates("printing candidates after merging 3");
                // return;
                // does this start at the second one actually?
                candidates.erase(it);
                it = candidates.begin()-1; // loop through them all again
            } else if ((*it).top <= tp && tp <= (((*it).top+(*it).bottom)/2)) {
                // std::cout<<"yes2.2"<<std::endl;
                // new box is to lower right of candidate box
                // (*it).bottom = bt;
                // (*it).right = rt;
                tp = (*it).top;
                lf = (*it).left;
                // return;
                // printCandidates("printing candidates after merging 4");
                // does this start at the second one actually?
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

    int left_barrier = img_wd / 4;
    int right_barrier = (img_wd / 4)*3;
    int left_halfway = img_wd / 8;
    int middle_halfway = img_wd / 2;
    int right_halfway = (img_wd / 8)*7;

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

    // // ignore "NONE" direction, start at 1
    // for (int i = 1; i < 9; i++)
    // {
    //     if (detectedObstacles[i]==0) { continue; } //no obstacle here

    //     messages::RobotObstacle::VRobot* temp = current.get()->add_obstacle();
    //     temp->set_position(obstaclesList[i]);
    // }

    // visualRobotOut.setMessage(current);
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
