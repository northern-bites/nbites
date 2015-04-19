#include "PostDetector.h"

#include <cmath>
#include <cstring>
#include <iostream>

#include "MathMorphology.h"
#include "DiffOfGaussianFilter.h"

namespace man {
namespace vision {

// TODO shrink post image by one
PostDetector::PostDetector(const Gradient& gradient,
                           const messages::PackedImage8& whiteImage)
    : wd(whiteImage.width()),
      postImage(wd, whiteImage.height()),
      candidates()
{
    unfilteredHistogram = new double[wd];
    filteredHistogram = new double[wd];

    memset(unfilteredHistogram, 0, wd*sizeof(double));
    memset(filteredHistogram, 0, wd*sizeof(double));

    buildPostImage(gradient, whiteImage);
    applyMathMorphology();
    buildHistogram();
    filterHistogram();
    findPeaks();
}

PostDetector::~PostDetector()
{
    delete[] unfilteredHistogram;
    delete[] filteredHistogram;
}

void PostDetector::buildPostImage(const Gradient& gradient,
                                  const messages::PackedImage8& whiteImage)
{
    int ht = postImage.height();

    // TODO post image should store doubles
    for (int y = 1; y < ht-1; y++) {
        unsigned char* whiteRow = whiteImage.pixelAddress(1, y);
        unsigned char* postRow = postImage.pixelAddress(0, y-1);
        for (int x = 1; x < wd-1; x++) {
            Fool gradScore(calculateGradScore(gradient.getX(y, x), gradient.getY(y, x)));
            Fool whiteScore(static_cast<double>(*whiteRow) / 255);
            *postRow = static_cast<unsigned char>(255*(gradScore & whiteScore).get());

            whiteRow += whiteImage.pixelPitch(); 
            postRow += postImage.pixelPitch();
        }
    }
}

inline Fool PostDetector::calculateGradScore(int16_t gradX, int16_t gradY) const {
    double x = static_cast<double>(gradX);
    double y = static_cast<double>(gradY);
    double cosSquaredAngle = x*x / (x*x + y*y);

    // TODO check magnitude of gradient vector
    return Fool(cosSquaredAngle);
}

void PostDetector::applyMathMorphology()
{
    // TODO more cache efficient structuring element
    std::pair<int, int> se[7];
    se[0].first = 0;
    se[0].second = 0;
    se[1].first = 0;
    se[1].second = 1;
    se[2].first = 0;
    se[2].second = -1;
    se[3].first = 0;
    se[3].second = -2;
    se[4].first = 0;
    se[4].second = 2;
    se[5].first = -1;
    se[5].second = 0;
    se[6].first = 1;
    se[6].second = 0;

    messages::PackedImage<unsigned char> postMorph;
    MathMorphology<unsigned char> morph(5, se);
    morph.opening(postImage, postMorph);
    postImage = postMorph;
}

void PostDetector::buildHistogram()
{
    int ht = postImage.height();

    for (int y = 0; y < ht; y++) {
        unsigned char* row = postImage.pixelAddress(0, y);
        for (int x = 0; x < wd; x++, row += postImage.pixelPitch()) {
            unfilteredHistogram[x] += static_cast<double>(*row) / 255;
        }
    }
}

void PostDetector::filterHistogram()
{
    // TODO constants should be static variables
    DiffOfGaussianFilter filter(81, 4, 40);
    filter.convolve(wd, unfilteredHistogram, filteredHistogram);
}

// TODO asymmetrical peak test?
void PostDetector::findPeaks()
{
    int leftLimit = 0;
    bool inPeak = false;
    for (int i = 0; i < wd; i++) {
        if (filteredHistogram[i] >= PostDetector::peakThreshold && !inPeak) {
            leftLimit = i;
            inPeak = true;
        } else if (filteredHistogram[i] < PostDetector::peakThreshold && inPeak) {
            candidates.push_back((leftLimit + i) / 2);
            inPeak = false;
        }
    }
}

}
}
