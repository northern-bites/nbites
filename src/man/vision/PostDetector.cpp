#include "PostDetector.h"

#include <cmath>
#include <cstring>
#include <iostream>

#include "MathMorphology.h"
#include "DiffOfGaussianFilter.h"

namespace man {
namespace vision {

// TODO shrink post image by one
PostDetector::PostDetector(const messages::PackedImage16& yImage, 
                           const messages::PackedImage8& whiteImage)
    : wd(whiteImage.width()),
      postImage(wd, whiteImage.height()),
      candidates()
{
    unfilteredHistogram = new double[wd];
    filteredHistogram = new double[wd];

    memset(unfilteredHistogram, 0, wd*sizeof(double));
    memset(filteredHistogram, 0, wd*sizeof(double));

    buildPostImage(yImage, whiteImage);
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

void PostDetector::buildPostImage(const messages::PackedImage16& yImage, 
                                  const messages::PackedImage8& whiteImage)
{
    int ht = postImage.height();

    // TODO post image should store doubles
    for (int y = 1; y < ht-1; y++) {
        uint16_t* yRow = yImage.pixelAddress(1, y);
        unsigned char* whiteRow = whiteImage.pixelAddress(1, y);
        unsigned char* postRow = postImage.pixelAddress(0, y-1);
        for (int x = 1; x < wd-1; x++) {
            Fool gradScore(calculateGradScore(yRow, yImage.rowPitch(), yImage.pixelPitch()));
            Fool whiteScore(static_cast<double>(*whiteRow) / 255);
            *postRow = static_cast<unsigned char>(255*(gradScore & whiteScore).get());
            // *postRow = static_cast<unsigned char>(255*(whiteScore).get());

            yRow += yImage.pixelPitch();
            whiteRow += whiteImage.pixelPitch(); 
            postRow += postImage.pixelPitch();
        }
    }
}

Fool PostDetector::calculateGradScore(uint16_t* pt, int rowPitch, int pixelPitch) {
    double hle = static_cast<double>(*(pt-rowPitch-pixelPitch));
    double h = static_cast<double>(*(pt-rowPitch));
    double hr = static_cast<double>(*(pt-rowPitch+pixelPitch)); 
    double le = static_cast<double>(*(pt-pixelPitch)); 
    double m = static_cast<double>(*(pt));
    double r = static_cast<double>(*(pt+pixelPitch)); 
    double lle = static_cast<double>(*(pt+rowPitch-pixelPitch)); 
    double l = static_cast<double>(*(pt+rowPitch)); 
    double lr = static_cast<double>(*(pt+rowPitch+pixelPitch));

    double gradX = -1*hle + 1*hr - 2*le + 2*r - 1*lle + 1*lr;
	double gradY = -1*hle - 2*h - 1*hr + 1*lle + 2*l + 1*lr;

    double magn = sqrt(gradX*gradX + gradY*gradY);

    double angle = 0;
    if (!(gradX == 0 && gradY == 0)) {
        if (gradX == 0) {
            angle = (gradY > 0 ? 90.0 : -90.0);
        } else {
            angle = (atan(fabs(gradY / gradX))) * 180 / M_PI;
            if (gradX >= 0 && gradY >= 0)
                angle = angle;
            else if (gradX <= 0 && gradY <= 0)
                angle = angle + 180;
            else if (gradX <= 0)
                angle = 180 - angle;
            else
                angle = -1*angle;
        }
    }

    if (angle > 90) 
        angle -= 180;
    if (angle > 0) 
        angle *= -1;
    
    // TODO constants should be static variables
    // TODO ignore gradient with high magnitude
    FuzzyThreshold sigmaMagnitudeLow(3, 7);
	FuzzyThreshold sigmaDirection(-60, -35);
		
    Fool magnLowScore(magn > sigmaMagnitudeLow);
    Fool dirScore(angle > sigmaDirection);
    
    return magnLowScore & dirScore;
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
