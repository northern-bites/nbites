#include "PostDetector.h"

#include <cmath>
#include <cstring>
#include <iostream>

#include "DiffOfGaussianFilter.h"
#include "HighResTimer.h"

namespace man {
namespace vision {

PostDetector::PostDetector(const Gradient& gradient,
                           const messages::PackedImage8& whiteImage)
    : len(whiteImage.width() - 2),
#ifdef OFFLINE
      postImage(whiteImage.width(), whiteImage.height()),
#endif
      candidates()
{
    // HighResTimer timer("Constructor");
    unfilteredHistogram = new double[len];
    filteredHistogram = new double[len];

    memset(unfilteredHistogram, 0, len*sizeof(double));
    memset(filteredHistogram, 0, len*sizeof(double));

#ifdef OFFLINE
    buildPostImage(gradient, whiteImage);
#endif

    // timer.end("Histogram");
    buildHistogram(gradient, whiteImage);
    // timer.end("Filtering");
    filterHistogram();
    // timer.end("Peaks");
    findPeaks();
    // timer.lap();
}

PostDetector::~PostDetector()
{
    delete[] unfilteredHistogram;
    delete[] filteredHistogram;
}

inline Fool PostDetector::calculateGradScore(int16_t magnitude, int16_t gradX, int16_t gradY) const {
    double magn = static_cast<double>(magnitude);
    double x = static_cast<double>(gradX);
    double y = static_cast<double>(gradY);
    double cosSquaredAngle = x*x / (x*x + y*y);

    // TODO throw away low magn vectors?
    // TODO throw away high magn vectors?
    // FuzzyThreshold sigmaMagnHigh(80, 100);
    // Fool magnHighScore(magn < sigmaMagnHigh);

    Fool angleScore(cosSquaredAngle);

    return angleScore;
}

#ifdef OFFLINE
void PostDetector::buildPostImage(const Gradient& gradient,
                                  const messages::PackedImage8& whiteImage)
{
    int wd = postImage.width();
    int ht = postImage.height();

    // TODO shrink post image by one
    for (int y = 1; y < ht-1; y++) {
        unsigned char* whiteRow = whiteImage.pixelAddress(1, y);
        unsigned char* postRow = postImage.pixelAddress(0, y-1);
        for (int x = 1; x < wd-1; x++) {
            Fool gradScore(calculateGradScore(gradient.getMagnitude(y, x), gradient.getX(y, x), gradient.getY(y, x)));
            Fool whiteScore(static_cast<double>(*whiteRow) / 255);
            *postRow = static_cast<unsigned char>(255*(gradScore & whiteScore).get());

            whiteRow += whiteImage.pixelPitch(); 
            postRow += postImage.pixelPitch();
        }
    }
}
#endif

void PostDetector::buildHistogram(const Gradient& gradient,
                                  const messages::PackedImage8& whiteImage)
{
    int wd = whiteImage.width();
    int ht = whiteImage.height();

    // TODO avoid branching in fuzzy logic
    for (int y = 1; y < ht-1; y++) {
        unsigned char* whiteRow = whiteImage.pixelAddress(1, y);
        for (int x = 1; x < wd-1; x++, whiteRow += whiteImage.pixelPitch()) {
            Fool gradScore(calculateGradScore(gradient.getMagnitude(y, x), gradient.getX(y, x), gradient.getY(y, x)));
            Fool whiteScore(static_cast<double>(*whiteRow) / 255);
            unfilteredHistogram[x-1] += (gradScore & whiteScore).get();
        }
    }
}

void PostDetector::filterHistogram()
{
    // TODO constants should be static variables
    DiffOfGaussianFilter filter(81, 4, 40);
    filter.convolve(len, unfilteredHistogram, filteredHistogram);
}

// TODO asymmetrical peak test?
void PostDetector::findPeaks()
{
    int leftLimit = 0;
    bool inPeak = false;
    for (int i = 0; i < len; i++) {
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
