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
    // candidates.clear();
    uint8_t min = 255;
    uint8_t max = 0;
    uint8_t* pixels = new uint8_t[img_ht*img_wd];
    for (int j = 1; j < img_ht-1; ++j) {
        for (int i = 1; i < img_wd-1; ++i) {
            pixels[i + img_wd*j] = 0;
        }
    }
    ImageLiteU8 tempImage(0, 0, img_wd, img_ht, WG.pitch(), pixels);

    // HACK: Do this better -> might impact other things
    ed->gradientThreshold(0); // set to lower fuzzy thresh

    for (int j = 1; j < img_ht-1; ++j) {
        for (int i = 1; i < img_wd-1; ++i) {
            uint8_t grad = ed->mag(i-1,j-1);
            uint8_t fuzzy = getFuzzyValue(grad);
            uint8_t whiteVal = *(whiteImage.pixelAddr(i,j));
            if (fuzzy < whiteVal) {
                *tempImage.pixelAddr(i,j) = fuzzy;
            } else {
                *tempImage.pixelAddr(i,j) = whiteVal;
            }
            *tempImage.pixelAddr(i,j) = fuzzy;

            if (grad < min) { min = grad; }
            if (grad > max) { max = grad; }
        }
    }

    removeHoughLines(tempImage, edges);
    findCandidates(tempImage, WG);

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

            // remove all pixels within 2 of hough line pixel
            *WG.pixelAddr(xi + img_wd/2 + 2, img_ht/2 - yi) = 0;
            *WG.pixelAddr(xi + img_wd/2, img_ht/2 - yi + 2) = 0;
            *WG.pixelAddr(xi + img_wd/2 - 2, img_ht/2 - yi) = 0;
            *WG.pixelAddr(xi + img_wd/2, img_ht/2 - yi - 2) = 0;
            *WG.pixelAddr(xi + img_wd/2 + 2, img_ht/2 - yi + 2) = 0;
            *WG.pixelAddr(xi + img_wd/2 + 2, img_ht/2 - yi - 2) = 0;
            *WG.pixelAddr(xi + img_wd/2 - 2, img_ht/2 - yi + 2) = 0;
            *WG.pixelAddr(xi + img_wd/2 - 2, img_ht/2 - yi - 2) = 0;

            // remove all pixels within 3 of hough line pixel
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

void RobotDetector::findCandidates(ImageLiteU8& temp, ImageLiteU8& WG) {
    int boxW = 70;
    int boxH = 170;
    uint8_t brightnessThresh = 150;
    float percent = .7;
    int countThresh = ((float)boxW * (float)boxH * percent);

    for (int j = 1; j < img_ht-1; ++j) {
        for (int i = 1; i < img_wd-1; ++i) {
            *WG.pixelAddr(i,j) = 0;
        }
    }

    for (int j = 1; j < img_ht - boxH - 1; ++j) {
        for (int i = 1; i < img_wd - boxW - 1; ++i) {

            int count = 0;

            for (int n = j; n < j + boxH; n++) {
                for (int m = i; m < i + boxW; m++) {
                    if (*temp.pixelAddr(m,n) > brightnessThresh) {
                        count++;
                    }
                }
            }

            for (int n = j; n < j+boxH; n++) {
                for (int m = i; m < i + boxW; m++) {
                    uint8_t w = *temp.pixelAddr(m,n);
                    // std::cout<<"BR = "<<w<<std::endl;
                    if (w > brightnessThresh) {
                        // std::cout<<"Count++..............."<<count<<std::endl;
                        count++;
                    }
                }
            }

            // std::cout<<"Count = "<< count << std::endl;
            if (count > countThresh) {
                for (int n = j; n < j+boxH; n++) {
                    for (int m = i; m < i + boxW; m++) {
                        *WG.pixelAddr(m,n) = *temp.pixelAddr(m,n);
                    }
                }
                return;
            }
        }
    }

}

} //namespace vision
} //namespace man
