/*
 *  ------------------------------
 * |  Visual Robot Detection 2016 |
 *  ------------------------------
 */

#include "RobotDetector.h"

namespace man {
namespace vision {

RobotDetector::RobotDetector(int wd_, int ht_)
    : low_fuzzy_thresh(2),
      high_fuzzy_thresh(11)
{
    std::cout<<"[ ROBOT IMAGE] width = "<<wd_<<", height = "<<ht_<<std::endl;
    img_wd = wd_;
    img_ht = ht_;
}

// Run every frame from VisionModule.cpp
void RobotDetector::getWhiteGradImage(ImageLiteU8& WG, ImageLiteU8 whiteImage,
                                      EdgeDetector* ed, EdgeList& edges)
{
    uint8_t min = 255;
    uint8_t max = 0;

    // HACK: Do this better -> might impact other things
    ed->gradientThreshold(0); // set to lower fuzzy thresh

    for (int j = 1; j < img_ht-1; ++j) {
        for (int i = 1; i < img_wd-1; ++i) {
            uint8_t grad = ed->mag(i-1,j-1);
            uint8_t fuzzy = getFuzzyValue(grad);
            uint8_t whiteVal = *(whiteImage.pixelAddr(i,j));
            if (fuzzy < whiteVal) {
                *WG.pixelAddr(i,j) = fuzzy;
            } else {
                *WG.pixelAddr(i,j) = whiteVal;
            }

            if (grad < min) { min = grad; }
            if (grad > max) { max = grad; }
        }
    }

    removeHoughLines(WG, edges);

    // std::cout<<"Gradient: min = "<< min<<", max = "<< max<< std::endl;
}

uint8_t RobotDetector::getFuzzyValue(uint8_t gradientValue) {
    if (gradientValue <= low_fuzzy_thresh) { return 0; }
    if (gradientValue >= high_fuzzy_thresh) { return 255; }
    uint8_t top = gradientValue - low_fuzzy_thresh;
    uint8_t bot = high_fuzzy_thresh - low_fuzzy_thresh;
    return top * 255 / bot;
}

void RobotDetector::removeHoughLines(ImageLiteU8& WG, EdgeList& edges)
{
    // Get edges from vision
    AngleBinsIterator<Edge> abi(edges);
    for (Edge* e = *abi; e; e = *++abi){
        // If we are part of a hough line, we are not a robot
        if (e->memberOf()) {
            int xi = e->x();
            int yi = e->y();

            *WG.pixelAddr(xi + img_wd/2, img_ht/2 - yi) = 0;

            // remove all pixels within 1 of hough line pixel
            *WG.pixelAddr(xi + img_wd/2 + 1, img_ht/2 - yi) = 0;
            *WG.pixelAddr(xi + img_wd/2, img_ht/2 - yi + 1) = 0;
            *WG.pixelAddr(xi + img_wd/2 - 1, img_ht/2 - yi) = 0;
            *WG.pixelAddr(xi + img_wd/2, img_ht/2 - yi - 1) = 0;
            *WG.pixelAddr(xi + img_wd/2 + 1, img_ht/2 - yi + 1) = 0;
            *WG.pixelAddr(xi + img_wd/2 + 1, img_ht/2 - yi - 1) = 0;
            *WG.pixelAddr(xi + img_wd/2 - 1, img_ht/2 - yi + 1) = 0;
            *WG.pixelAddr(xi + img_wd/2 - 1, img_ht/2 - yi - 1) = 0;

            // remove all pixels within 1 of hough line pixel
            *WG.pixelAddr(xi + img_wd/2 + 2, img_ht/2 - yi) = 0;
            *WG.pixelAddr(xi + img_wd/2, img_ht/2 - yi + 2) = 0;
            *WG.pixelAddr(xi + img_wd/2 - 2, img_ht/2 - yi) = 0;
            *WG.pixelAddr(xi + img_wd/2, img_ht/2 - yi - 2) = 0;
            *WG.pixelAddr(xi + img_wd/2 + 2, img_ht/2 - yi + 2) = 0;
            *WG.pixelAddr(xi + img_wd/2 + 2, img_ht/2 - yi - 2) = 0;
            *WG.pixelAddr(xi + img_wd/2 - 2, img_ht/2 - yi + 2) = 0;
            *WG.pixelAddr(xi + img_wd/2 - 2, img_ht/2 - yi - 2) = 0;

            // remove all pixels within 1 of hough line pixel
            *WG.pixelAddr(xi + img_wd/2 + 3, img_ht/2 - yi) = 0;
            *WG.pixelAddr(xi + img_wd/2, img_ht/2 - yi + 3) = 0;
            *WG.pixelAddr(xi + img_wd/2 - 3, img_ht/2 - yi) = 0;
            *WG.pixelAddr(xi + img_wd/2, img_ht/2 - yi - 3) = 0;
            *WG.pixelAddr(xi + img_wd/2 + 3, img_ht/2 - yi + 3) = 0;
            *WG.pixelAddr(xi + img_wd/2 + 3, img_ht/2 - yi - 3) = 0;
            *WG.pixelAddr(xi + img_wd/2 - 3, img_ht/2 - yi + 3) = 0;
            *WG.pixelAddr(xi + img_wd/2 - 3, img_ht/2 - yi - 3) = 0;
        }
    }
}

} //namespace vision
} //namespace man
