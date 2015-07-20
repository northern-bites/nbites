/*
 *  -----------------------
 * |  Robot Detection 2015 |
 *  -----------------------
 */

#include "RobotObstacle.h"

namespace man {
namespace vision {

RobotObstacle::RobotObstacle(int wd_, int ht_)
{
    std::cout<<"[ ROBOT IMAGE] width = "<<wd_<<", height = "<<ht_<<std::endl;
    img_wd = wd_;
    img_ht = ht_;

    bottom = new int[img_wd];
    top = new int[img_wd];
    maxbottom = new int[img_wd];
    mintop = new int[img_wd];
}

void RobotObstacle::printArray(int* array, int size, std::string name)
{
    std::cout<<name<<": ["<<array[0]<<"]";
    for (int i = 1; i < size; i++) {
        std::cout<<"  ["<<array[i]<<"]";
    }
    std::cout<<std::endl;
}

void RobotObstacle::printArray(bool* array, int size, std::string name)
{
    std::cout<<name<<": ["<<array[0]<<"]";
    for (int i = 1; i < size; i++) {
        if (array[i]){
            std::cout<<"  ["<<i<<"]";
            continue;
        }
        std::cout<<"  [0]";
    }
    std::cout<<std::endl;
}

// Run every frame from VisionModule.cpp
void RobotObstacle::updateVisionObstacle(ImageLiteU8 whiteImage, EdgeList& edges,
                                         float* obstacleBox, FieldHomography* hom)
{
    initAccumulators(obstacleBox);

    // Go through edges and determine which are "topmost" and "bottommost" ones
    getBottomAndTopEdges(edges);

    // CHINA 2015
    // Gets the first column of where we want to process obstacles
    // depending on azimuth: We don't want to pick up an obstacle in the
    // shoulder by accident
    // returns negative start column if "startCol" is actually the last column
    // we want to process (when shoulder is in left part of image)
    // return value of 0 = process entire image
    // return value of width = don't process any part of image
    int startCol = findAzimuthRestrictions(hom);

    // when we don't want to proess image
    if (startCol != img_wd) {
        // Find constricting box of obstacle in image coordinates
        // Returns column with the lowest edge point in the image
        int lowestCol = findObstacle(whiteImage, obstacleBox, startCol);

        // Convert the image coordinates in obstacleBox to robotCoordinates
        toFieldCoordinates(hom, obstacleBox, lowestCol);
    }
}

void RobotObstacle::initAccumulators(float* obstacleBox)
{
    for (int i = 0; i < img_wd; i++) {
        bottom[i] = img_ht;
        top[i] = 0;
        maxbottom[i] = 0;
        mintop[i] = img_ht;
    }

    resetObstacleBox(obstacleBox);
}

void RobotObstacle::resetObstacleBox(float* obstacleBox)
{
    // {| bottomCol | bottom | left | right |} coordinate values of box
    obstacleBox[0] = -1.f;
    obstacleBox[1] = -1.f;
    obstacleBox[2] = -1.f;
    obstacleBox[3] = -1.f;
}

void RobotObstacle::getBottomAndTopEdges(EdgeList& edges)
{
    // Get edges from vision
    AngleBinsIterator<Edge> abi(edges);
    for (Edge* e = *abi; e; e = *++abi){
        // If we are part of a hough line, we are not a robot edge
        if (e->memberOf()) { continue; }

        int x = e->x() + img_wd/2;
        int y = img_ht/2 - e->y();
        int ang = e->angle();
        // int mag = e->mag();      // magnitude - could be useful later?

        // don't want to get too high or low in the image
        if (y < BARRIER_TOP || y > img_ht - BARRIER_BOT ) { continue; }

        // binary angles, so 128 = pi radians
        if (ang < 128 && y > maxbottom[x]) {
            maxbottom[x] = y;
        } else if ( ang > 128 && y < mintop[x]) {
            mintop[x] = y;
        }
    }
}

int RobotObstacle::findAzimuthRestrictions(FieldHomography* hom)
{

    // A little hacky: TODO make function / calculate better values / use constants
    // HACK CHINA 2015 SORRY!!
    double az = hom->azimuth();
    int val = 0;

    // if abs(az) 1.3, too great to detect obstacle: return width
    if (az > 1.3 || az < -1.3) { return img_wd; }

    // if abs(az) is > 1.1 and <= 1.3, ignore 2/3
    if (az > 1.1 || az < -1.1) {
        val = (int)(.6666 * (double)img_wd);
    }

    // if abs(az) is > 1 and <= 1.1, ignore 1/2
    else if (az > 1.0 || az < -1.0) {
        val = (int)(.5 * (double)img_wd);
    }

    // if abs(az) is > 0.98 and <= 1, ignore 1/3
    else if (az > 0.98 || az < -0.98) {
        val = (int)(.3333 * (double)img_wd);
    }

    // if abs(az) is > 0.93 and <= 0.98, ignore 1/4
    else if (az > 0.93 || az < -0.93) {
        val = (int)(.25 * (double)img_wd);
    }

    // else, no obstacle! return 0 to detect everything
    else {
        return 0;
    }

    // if we've gotten to this point, we want to ignore part of the
    // image. Side of image depends on sign of azimuth.
    if (az > 0) {
        // right side
        return -1*(img_wd - val);
    } else {
        // left side (negative lets us know it is the "end col")
        return val;
    }

}

int RobotObstacle::findObstacle(ImageLiteU8 whiteImage, float* obstacleBox, int startCol)
{
    int maxLength = 0;          // max run length we've found so far
    int maxStart = -1;          // start of the max run
    int maxBot = 0;             // lowest point of the max run
    int maxCol = -1;            // column with the lowest point

    int currLength = 0;         // length of our current run
    int currStart = 0;          // start of first run, start at first index
    int currBot = 0;            // lowest edge of our current run
    int currCol = -1;           // column with lowest point in current run
    int topWhite = 0;           // how many of the topmost pixels in the run columns are white

    int blankCounter = 0;       // how many columns without evidence in a row we've found
    int endCol;                 // determined by azimuth, want to ignore parts of image to right/left

    // which part of the image do we want to process
    if (startCol == 0) {
        endCol = img_wd;
    } else if (startCol < 0) {
        endCol = -startCol;
        startCol = 0;
    } else {
        endCol = img_wd;
    }

    // Loop through each column to find a run of columns with evidence of an obstacle
    for (int i = 0; i < img_wd; i++) {
        if (i < startCol || i > endCol)
        {
            continue;
        }
        int w = 0;
        bool whiteTop = false;
        for (int j = 0; j < maxbottom[i]; j++) {
            // this is the key business, look for mostly white between edge and top
            if (*(whiteImage.pixelAddr(i,j)) > WHITE_CONFIDENCE_THRESH) {
                w++;

                // If I am one of the top 3 pixels, check to see if I am white
                if (j == 0 || j == 1 || j == 2) whiteTop = true;
            }
        }

        if (w > maxbottom[i] / 2) {
            // there is enough evidence for this column!
            if (currStart == -1) { currStart = i; }
            if (maxbottom[i] > currBot) {
                currBot = maxbottom[i];
                currCol = i;
            }
            if (whiteTop) { topWhite++; }
            blankCounter = 0;
            currLength++;
        } else if (currStart != -1) {   // ignore when we haven't started a run
            if (blankCounter >= MAX_DIST) {
                // no evidence and blank counter too high, reset params
                currLength -= blankCounter; // get rid of blanks we counted prematurely
                if (currLength > maxLength && (float)topWhite / (float)currLength > 0.8f) {
                    maxLength = currLength;
                    maxStart = currStart;
                    maxBot = currBot;
                    maxCol = currCol;
                }
                currLength = 0;
                currStart = -1;
                currBot = 0;
                blankCounter = 0;
                topWhite = 0;
            } else {
                // just a blank, let's increment the blank counter and continue
                blankCounter++;
                currLength++;
            }
        }
    }

    // check to see if we ended with our maximum length:
    if (currLength > maxLength && (float)topWhite / (float)currLength > 0.8f) {
        maxLength = currLength;
        maxStart = currStart;
        maxBot = currBot;
        maxCol = currCol;
    }

    if (maxLength > MIN_LENGTH) {
        // now update obstacle box
        obstacleBox[1] = (float)maxBot;                     // bottom
        obstacleBox[2] = (float)maxStart;                   // left
        obstacleBox[3] = (float)(maxStart + maxLength);     // right
    } else {
        maxCol = -1;
    }
    return maxCol;
}

void RobotObstacle::toFieldCoordinates(FieldHomography* hom, float* obstacleBox, int lowestCol)
{
    // If we haven't found an obstacle:
    if (lowestCol == -1) { return; }

    // Homography takes an image coordinate as a parameter, so we must
    // convert to that coordinate system
    //      FROM: y+ forward, x+ right, TO: x+ forward, y+ left

    // let's map the lowest point to field coordinates
    double x1, y1, xb, yb;
    x1 = (double)lowestCol - (double)img_wd/2;
    y1 = (double)img_ht/2 - (double)obstacleBox[1]; // bottom of obstacle box
    if (!hom->fieldCoords(x1, y1, xb, yb)) {
        // we were not successful in our first mapping
        resetObstacleBox(obstacleBox);
        return;
    }

    // now let's map the left point of the box
    double x2, y2, xl, yl;
    x2 = (double)obstacleBox[2] - (double)img_wd/2; // left side of obstacle box
    if (!hom->fieldCoords(x2, y1, xl, yl)) {
        // we were not successful in our second mapping
        resetObstacleBox(obstacleBox);
        return;
    }

    // finally let's map the right point of the box
    double x3, y3, xr, yr;
    x3 = (double)obstacleBox[3] - (double)img_wd/2; // left side of obstacle box
    if (!hom->fieldCoords(x3, y1, xr, yr)) {
        // we were not successful in our third mapping
        resetObstacleBox(obstacleBox);
        return;
    }

    // All the mapping was successful!
    // now that we have them in robot relative coordinates... we have to
    // convert them to the correct coordinate system, with same origin:
    // FROM: y+ forward, x+ right, TO: x+ forward, y+ to left
    obstacleBox[0] = -1.f*(float)xb;     // bottom x coordinate (now y)
    obstacleBox[1] = (float)yb;          // bottom y coordinate (now x)
    obstacleBox[2] = -1.f*(float)xl;     // left x coordinate (now y)
    obstacleBox[3] = -1.f*(float)xr;     // right x coordinate (now y)
}

} //namespace vision
} //namespace man
